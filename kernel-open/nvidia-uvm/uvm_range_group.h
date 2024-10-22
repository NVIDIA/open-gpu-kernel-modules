/*******************************************************************************
    Copyright (c) 2015-2024 NVIDIA Corporation

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

#ifndef __UVM_RANGE_GROUP_H__
#define __UVM_RANGE_GROUP_H__

#include "nvtypes.h"
#include "uvm_range_tree.h"
#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_va_block_types.h"

typedef struct uvm_range_group_struct
{
    // Group ID
    NvU64 id;

    // Does this group allow migration?
    // This is not protected by a lock, which is okay since it is valid
    // for a range group to be set as migratable while a migration of its
    // VA ranges to their preferred location has not been completed yet.
    // Using atomic_t rather than bool here to emphasize the fact that this
    // field is special
    atomic_t allow_migration;

    // range group ranges in this group
    struct list_head ranges;

    // Range group ranges in this group that have been migrated from their preferred location. This should always be
    // a subset of the ranges list. Any time this list is modified the migrated_ranges_lock should be acquired.
    struct list_head migrated_ranges;
    uvm_spinlock_t migrated_ranges_lock;
} uvm_range_group_t;

typedef struct uvm_range_group_range_struct
{
    uvm_range_tree_node_t node;
    struct list_head range_group_list_node;
    struct list_head range_group_migrated_list_node;
    uvm_range_group_t *range_group;
} uvm_range_group_range_t;

// This structure can represent gaps in the tree too
typedef struct uvm_range_group_range_iter_struct
{
    NvU64 start;
    NvU64 end;

    // Whether this range is migratable.
    bool migratable;

    // Whether the iterator is pointing to a valid element.
    // Set to false when iteration moves past the last element.
    bool valid;

    // Whether start and end lie within node.
    bool is_current;

    // When valid and is_current are true, the node that corresponds to the position of the iterator
    // Also see uvm_range_group_range_iter_all_first()
    uvm_range_group_range_t *node;
} uvm_range_group_range_iter_t;

static inline bool uvm_range_group_migratable(uvm_range_group_t *range_group)
{
    // A NULL range group is always migratable
    return !range_group || atomic_read(&range_group->allow_migration);
}

// Causes [start, end] to have a range group of range_group.
// This function may split or overwrite existing range group ranges to accomplish this; merging when possible.
// If range_group is NULL, this function will clear all range group ranges for the given range.
// It is not necessary to clear a range before overwriting it with a new range_group association.
NV_STATUS uvm_range_group_assign_range(uvm_va_space_t *va_space, uvm_range_group_t *range_group, NvU64 start, NvU64 end);

// False when the address belongs to a range group range of a non-migratable range group.
// True otherwise, even if the address is invalid.
bool uvm_range_group_address_migratable(uvm_va_space_t *va_space, NvU64 address);

// False if all addresses in the range belong to range group ranges of non-migratable range groups.
// True otherwise, even if the range is invalid.
bool uvm_range_group_any_migratable(uvm_va_space_t *va_space, NvU64 start, NvU64 end);

// False if any address in the range belongs to a range group range of a non-migratable range group.
// True otherwise, even if the range is invalid.
bool uvm_range_group_all_migratable(uvm_va_space_t *va_space, NvU64 start, NvU64 end);

// These iterators return range group ranges, skipping over addresses that have no associated range group.
uvm_range_group_range_t *uvm_range_group_range_find(uvm_va_space_t *va_space, NvU64 addr);
uvm_range_group_range_t *uvm_range_group_range_iter_first(uvm_va_space_t *va_space, NvU64 start, NvU64 end);
uvm_range_group_range_t *uvm_range_group_range_iter_next(uvm_va_space_t *va_space,
                                                         uvm_range_group_range_t *range,
                                                         NvU64 end);

// These iterators return range group ranges one at a time, but also yield gaps in between range group ranges.
// When iter refers to a range_group_range, iter->is_current will be true and iter->node will be the range group range.
// When iter refers to a gap, iter->is_current will be false.
// This function will always return a valid iterator, as long as start <= end, which is required.
// iter->valid is set when iter refers to a range within the caller-specified range.  It is cleared to indicate the end of iteration.
// It does not reflect the validity of those virtual addresses in the va_space.
// iter_all_first and iter_all_next clamp iter->start and iter->end to the start and end values passed by the caller.
void uvm_range_group_range_iter_all_first(uvm_va_space_t *va_space,
                                          NvU64 start,
                                          NvU64 end,
                                          uvm_range_group_range_iter_t *iter);

// Advance an iterator that yields range group ranges as well as any gaps in between them.
// Returns iter->valid.
bool uvm_range_group_range_iter_all_next(uvm_va_space_t *va_space,
                                         uvm_range_group_range_iter_t *iter,
                                         NvU64 end);

// These iterators return the largest possible range of addresses that are either all migratable or not.
// To determine which is the case, check iter->migratable.
// This function will always return a valid iterator, as long as start <= end, which is required.
// migratability_iter_first and migratability_iter_next clamp iter->start and iter->end
// to the start and end values passed by the caller.
void uvm_range_group_range_migratability_iter_first(uvm_va_space_t *va_space,
                                                    NvU64 start,
                                                    NvU64 end,
                                                    uvm_range_group_range_iter_t *iter);

// Advance a migratability iterator.  Check iter->valid to determine whether iter refers to a valid region.
void uvm_range_group_range_migratability_iter_next(uvm_va_space_t *va_space,
                                                   uvm_range_group_range_iter_t *iter,
                                                   NvU64 end);

// This iterator is like uvm_range_group_range_migratability_iter_next except that it allows the caller to
// modify the range group range tree during iteration, and is slightly less efficient.
static void uvm_range_group_range_migratability_iter_next_safe(uvm_va_space_t *va_space,
                                                               uvm_range_group_range_iter_t *iter,
                                                               NvU64 end)
{
    iter->valid = iter->end < end;
    if (!iter->valid)
        return;
    uvm_range_group_range_migratability_iter_first(va_space, iter->end + 1, end, iter);
}

// Fill mask_out with all migratable pages in region. The mask is guaranteed to
// be zero outside the region.
void uvm_range_group_migratable_page_mask(uvm_va_block_t *va_block,
                                          uvm_va_block_region_t region,
                                          uvm_page_mask_t *mask_out);

NV_STATUS uvm_range_group_init(void);
void uvm_range_group_exit(void);
void uvm_range_group_radix_tree_destroy(uvm_va_space_t *va_space);

// Move a non-migratable VA range to its preferred location and add
// mappings for processors in the accessed by mask and for the preferred
// location (with the exception of CPU which never gets any mapping).
//
// This does not wait for the migration to complete. The work is added to the
// output tracker.
NV_STATUS uvm_range_group_va_range_migrate(uvm_va_range_managed_t *managed_range,
                                           NvU64 start,
                                           NvU64 end,
                                           uvm_tracker_t *out_tracker);

#define uvm_range_group_for_each_range_in(node, va_space, start, end)           \
    for ((node) = uvm_range_group_range_iter_first((va_space), (start), (end)); \
         (node);                                                                \
         (node) = uvm_range_group_range_iter_next((va_space), (node), (end)))

#define uvm_range_group_for_all_ranges_in(iter, va_space, start, end) \
    for (uvm_range_group_range_iter_all_first((va_space), (start), (end), (iter)); \
         (iter)->valid; \
         uvm_range_group_range_iter_all_next((va_space), (iter), (end)))

#define uvm_range_group_for_each_migratability_in(iter, va_space, start, end) \
    for (uvm_range_group_range_migratability_iter_first((va_space), (start), (end), (iter)); \
         (iter)->valid; \
         uvm_range_group_range_migratability_iter_next((va_space), (iter), (end)))

#define uvm_range_group_for_each_migratability_in_safe(iter, va_space, start, end) \
    for (uvm_range_group_range_migratability_iter_first((va_space), (start), (end), (iter)); \
         (iter)->valid; \
         uvm_range_group_range_migratability_iter_next_safe((va_space), (iter), (end)))


#endif // __UVM_RANGE_GROUP_H__
