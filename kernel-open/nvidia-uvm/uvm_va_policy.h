/*******************************************************************************
    Copyright (c) 2022-2023 NVIDIA Corporation

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

#ifndef __UVM_VA_POLICY_H__
#define __UVM_VA_POLICY_H__

#include <linux/numa.h>
#include "uvm_linux.h"
#include "uvm_forward_decl.h"
#include "uvm_processors.h"
#include "uvm_range_tree.h"
#include "uvm_va_block_types.h"

// This enum must be kept in sync with UVM_TEST_READ_DUPLICATION_POLICY in
// uvm_test_ioctl.h
typedef enum
{
    UVM_READ_DUPLICATION_UNSET = 0,
    UVM_READ_DUPLICATION_ENABLED,
    UVM_READ_DUPLICATION_DISABLED,
    UVM_READ_DUPLICATION_MAX
} uvm_read_duplication_policy_t;

typedef enum
{
    UVM_VA_POLICY_PREFERRED_LOCATION = 0,
    UVM_VA_POLICY_ACCESSED_BY,
    UVM_VA_POLICY_READ_DUPLICATION,
} uvm_va_policy_type_t;

//
// A policy covers one or more contiguous Linux VMAs or portion of a VMA and
// does not cover non-existant VMAs.
// The VA range is determined from either the uvm_va_range_t for managed
// allocations or the uvm_va_policy_node_t for HMM allocations.
//
struct uvm_va_policy_struct
{
    // Read duplication policy for this VA range (unset, enabled, or disabled).
    uvm_read_duplication_policy_t read_duplication;

    // Processor ID of the preferred location for this VA range.
    // This is set to UVM_ID_INVALID if no preferred location is set.
    uvm_processor_id_t preferred_location;

    // If the preferred location is the CPU, this is either the preferred NUMA
    // node ID or NUMA_NO_NODE to indicate that there is no preference among
    // nodes.
    // If preferred_location is a GPU, preferred_nid will be used if CPU
    // pages have to be allocated for any staging copies. Otherwise, it is
    // not used.
    //
    // TODO: Bug 4148100 - Preferred_location and preferred_nid should be
    //       combined into a new type that combines the processor and NUMA node
    //       ID.
    int preferred_nid;

    // Mask of processors that are accessing this VA range and should have
    // their page tables updated to access the (possibly remote) pages.
    uvm_processor_mask_t accessed_by;

};

// Policy nodes are used for storing policies in HMM va_blocks.
// The va_block lock protects the tree so that invalidation callbacks can
// update the VA policy tree.
typedef struct uvm_va_policy_node_struct
{
    // Storage for the policy tree node. It also contains the range start and
    // end. Start and end + 1 have to be PAGE_SIZED aligned.
    uvm_range_tree_node_t node;

    uvm_va_policy_t policy;

} uvm_va_policy_node_t;

// Function pointer prototype for uvm_hmm_split_as_needed() callback.
typedef bool (*uvm_va_policy_is_split_needed_t)(const uvm_va_policy_t *policy, void *data);

// Default policy to save uvm_va_policy_node_t space in HMM va_blocks.
extern const uvm_va_policy_t uvm_va_policy_default;

// Return true if policy is the default policy.
static bool uvm_va_policy_is_default(const uvm_va_policy_t *policy)
{
    return policy == &uvm_va_policy_default;
}

bool uvm_va_policy_is_read_duplicate(const uvm_va_policy_t *policy, uvm_va_space_t *va_space);

// Returns the uvm_va_policy_t containing addr or default policy if not found.
// The va_block can be either a UVM or HMM va_block.
// Locking: The va_block lock must be held.
const uvm_va_policy_t *uvm_va_policy_get(uvm_va_block_t *va_block, NvU64 addr);

// Same as above but asserts the policy covers the whole region
const uvm_va_policy_t *uvm_va_policy_get_region(uvm_va_block_t *va_block, uvm_va_block_region_t region);

// Return a uvm_va_policy_node_t given a uvm_va_policy_t pointer.
static const uvm_va_policy_node_t *uvm_va_policy_node_from_policy(const uvm_va_policy_t *policy)
{
    return container_of(policy, uvm_va_policy_node_t, policy);
}

// Compare the preferred location and preferred nid from the policy
// with the input processor and CPU node ID.
// For GPUs, only the processors are compared. For the CPU, the
// NUMA node IDs are also compared.
bool uvm_va_policy_preferred_location_equal(const uvm_va_policy_t *policy, uvm_processor_id_t proc, int cpu_node_id);

#if UVM_IS_CONFIG_HMM()

// Module load/exit
NV_STATUS uvm_va_policy_init(void);
void uvm_va_policy_exit(void);

// Returns the uvm_va_policy_node_t containing addr or NULL if not found.
// The va_block must be a HMM va_block.
// Locking: The va_block lock must be held.
uvm_va_policy_node_t *uvm_va_policy_node_find(uvm_va_block_t *va_block, NvU64 addr);

// Split the old node. The old node will end at 'new_end' and the new node will
// start at 'new_end' + 1 and end at old end.
// The va_block must be a HMM va_block.
// Locking: The va_block lock must be held.
NV_STATUS uvm_va_policy_node_split(uvm_va_block_t *va_block,
                                   uvm_va_policy_node_t *old,
                                   NvU64 new_end,
                                   uvm_va_policy_node_t **new_ptr);

// Move hints from 'old' to 'new' which must both be HMM va_blocks.
// The old va_block policies should have been pre-split and since we don't
// merge policy ranges, they should still be split after locking/unlocking
// 'old'. This should be called after splitting a block.
// TODO: Bug 1707562: Add support for merging policy ranges.
// Locking: The va_block lock must be held for both old and new.
void uvm_va_policy_node_split_move(uvm_va_block_t *old_va_block,
                                   uvm_va_block_t *new_va_block);

// Remove all policy in the given range start/end where 'end' is inclusive.
// This function may clear a range larger than start/end if clearing the range
// requires memory allocation and the memory allocation fails.
// The va_block must be a HMM va_block.
// Locking: The va_block lock must be held.
void uvm_va_policy_clear(uvm_va_block_t *va_block, NvU64 start, NvU64 end);

// Fill in any missing policy nodes for the given range and set the policy
// to the given value. The caller is expected to split any policy nodes
// before calling this function so the range being set does not.
// The va_block must be a HMM va_block.
// Note that start and end + 1 must be page aligned, 'end' is inclusive.
// TODO: Bug 1707562: Add support for merging policy ranges.
// Locking: The va_block lock must be held.
NV_STATUS uvm_va_policy_set_range(uvm_va_block_t *va_block,
                                  NvU64 start,
                                  NvU64 end,
                                  uvm_va_policy_type_t which,
                                  bool is_default,
                                  uvm_processor_id_t processor_id,
                                  int cpu_node_id,
                                  uvm_read_duplication_policy_t new_policy);

// This is an optimized version of uvm_va_policy_set_range() where the caller
// guarantees that the the processor_id is not the same as the existing
// policy for the given region and that the region doesn't require splitting
// the existing policy node 'old_policy'.
// Returns the updated policy or NULL if memory could not be allocated.
// Locking: The va_block lock must be held.
const uvm_va_policy_t *uvm_va_policy_set_preferred_location(uvm_va_block_t *va_block,
                                                            uvm_va_block_region_t region,
                                                            uvm_processor_id_t processor_id,
                                                            int cpu_node_id,
                                                            const uvm_va_policy_t *old_policy);

// Iterators for specific VA policy ranges.

// Returns the first policy node in the range [start, end], if any.
// The va_block must be a HMM va_block.
// Locking: The va_block lock must be held.
uvm_va_policy_node_t *uvm_va_policy_node_iter_first(uvm_va_block_t *va_block, NvU64 start, NvU64 end);

// Returns the next VA policy following the provided policy in address order,
// if that policy's start <= the provided end.
// The va_block must be a HMM va_block.
// Locking: The va_block lock must be held.
uvm_va_policy_node_t *uvm_va_policy_node_iter_next(uvm_va_block_t *va_block, uvm_va_policy_node_t *node, NvU64 end);

#define uvm_for_each_va_policy_node_in(node, va_block, start, end)            \
    for ((node) = uvm_va_policy_node_iter_first((va_block), (start), (end));  \
         (node);                                                              \
         (node) = uvm_va_policy_node_iter_next((va_block), (node), (end)))

#define uvm_for_each_va_policy_node_in_safe(node, next, va_block, start, end) \
    for ((node) = uvm_va_policy_node_iter_first((va_block), (start), (end)),  \
         (next) = uvm_va_policy_node_iter_next((va_block), (node), (end));    \
         (node);                                                              \
         (node) = (next),                                                     \
         (next) = uvm_va_policy_node_iter_next((va_block), (node), (end)))

// Returns the first policy in the range [start, end], if any.
// Locking: The va_block lock must be held.
const uvm_va_policy_t *uvm_va_policy_iter_first(uvm_va_block_t *va_block,
                                                NvU64 start,
                                                NvU64 end,
                                                uvm_va_policy_node_t **out_node,
                                                uvm_va_block_region_t *out_region);

// Returns the next VA policy following the provided policy in address order,
// if that policy's start <= the provided end.
// Locking: The va_block lock must be held.
const uvm_va_policy_t *uvm_va_policy_iter_next(uvm_va_block_t *va_block,
                                               const uvm_va_policy_t *policy,
                                               NvU64 end,
                                               uvm_va_policy_node_t **inout_node,
                                               uvm_va_block_region_t *inout_region);

// Note that policy and region are set and usable in the loop body.
// The 'node' variable is used to retain loop state and 'policy' doesn't
// necessarily match &node->policy.
#define uvm_for_each_va_policy_in(policy, va_block, start, end, node, region) \
    for ((policy) = uvm_va_policy_iter_first((va_block), (start), (end), &(node), &(region)); \
         (policy);                                                              \
         (policy) = uvm_va_policy_iter_next((va_block), (policy), (end), &(node), &(region)))

#else // UVM_IS_CONFIG_HMM()

static NV_STATUS uvm_va_policy_init(void)
{
    return NV_OK;
}

static void uvm_va_policy_exit(void)
{
}

static uvm_va_policy_node_t *uvm_va_policy_node_find(uvm_va_block_t *va_block, NvU64 addr)
{
    UVM_ASSERT(0);
    return NULL;
}

static NV_STATUS uvm_va_policy_node_split(uvm_va_block_t *va_block,
                                          uvm_va_policy_node_t *old,
                                          NvU64 new_end,
                                          uvm_va_policy_node_t **new_ptr)
{
    return NV_OK;
}

static void uvm_va_policy_node_split_move(uvm_va_block_t *old_va_block,
                                          uvm_va_block_t *new_va_block)
{
}

static void uvm_va_policy_clear(uvm_va_block_t *va_block, NvU64 start, NvU64 end)
{
}

static NV_STATUS uvm_va_policy_set_range(uvm_va_block_t *va_block,
                                         NvU64 start,
                                         NvU64 end,
                                         uvm_va_policy_type_t which,
                                         bool is_default,
                                         uvm_processor_id_t processor_id,
                                         uvm_read_duplication_policy_t new_policy)
{
    return NV_OK;
}

static uvm_va_policy_node_t *uvm_va_policy_node_iter_first(uvm_va_block_t *va_block, NvU64 start, NvU64 end)
{
    return NULL;
}

#endif // UVM_IS_CONFIG_HMM()

#endif // __UVM_VA_POLICY_H__
