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

#include "uvm_range_group.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_va_block.h"
#include "uvm_tools.h"
#include "uvm_ioctl.h"
#include "uvm_types.h"
#include "uvm_api.h"
#include "uvm_test.h"

static struct kmem_cache *g_uvm_range_group_cache __read_mostly;
static struct kmem_cache *g_uvm_range_group_range_cache __read_mostly;

NV_STATUS uvm_range_group_init(void)
{
    g_uvm_range_group_cache = NV_KMEM_CACHE_CREATE("uvm_range_group_t", uvm_range_group_t);
    if (!g_uvm_range_group_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_range_group_range_cache = NV_KMEM_CACHE_CREATE("uvm_range_group_range_t", uvm_range_group_range_t);
    if (!g_uvm_range_group_range_cache)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_range_group_exit(void)
{
    kmem_cache_destroy_safe(&g_uvm_range_group_cache);
    kmem_cache_destroy_safe(&g_uvm_range_group_range_cache);
}

static uvm_range_group_range_t *range_group_range_create(uvm_range_group_t *range_group,
                                                         NvU64 start,
                                                         NvU64 end)
{
    uvm_range_group_range_t *rgr = nv_kmem_cache_zalloc(g_uvm_range_group_range_cache, NV_UVM_GFP_FLAGS);
    if (rgr == NULL)
        return NULL;

    UVM_ASSERT(range_group != NULL);
    INIT_LIST_HEAD(&rgr->range_group_migrated_list_node);
    list_add(&rgr->range_group_list_node, &range_group->ranges);
    rgr->range_group = range_group;

    rgr->node.start = start;
    rgr->node.end = end;

    return rgr;
}

static void uvm_range_group_range_destroy(uvm_range_group_range_t *rgr)
{
    if (rgr == NULL)
        return;

    list_del(&rgr->range_group_list_node);

    // The VA space should be locked in write mode when this function is called,
    // so we don't need to acquire the migrated_list lock.
    list_del(&rgr->range_group_migrated_list_node);
    kmem_cache_free(g_uvm_range_group_range_cache, rgr);
}

NV_STATUS uvm_api_create_range_group(UVM_CREATE_RANGE_GROUP_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_range_group_t *range_group = NULL;
    NV_STATUS status = NV_OK;
    int ret;

    range_group = nv_kmem_cache_zalloc(g_uvm_range_group_cache, NV_UVM_GFP_FLAGS);
    if (!range_group)
        return NV_ERR_NO_MEMORY;

    range_group->id = atomic64_inc_return(&va_space->range_group_id_counter);
    UVM_ASSERT(range_group->id != UVM_RANGE_GROUP_ID_NONE);

    atomic_set(&range_group->allow_migration, 1);
    INIT_LIST_HEAD(&range_group->ranges);
    INIT_LIST_HEAD(&range_group->migrated_ranges);
    uvm_spin_lock_init(&range_group->migrated_ranges_lock, UVM_LOCK_ORDER_LEAF);

    uvm_va_space_down_write(va_space);

    ret = radix_tree_insert(&va_space->range_groups, range_group->id, range_group);
    status = errno_to_nv_status(ret);
    if (status != NV_OK) {
        kmem_cache_free(g_uvm_range_group_cache, range_group);
        goto done;
    }

    params->rangeGroupId = range_group->id;

done:
    uvm_va_space_up_write(va_space);

    return status;
}

static void uvm_range_group_destroy(uvm_va_space_t *va_space, uvm_range_group_t *range_group)
{
    uvm_range_group_range_t *rgr, *tmp;

    list_for_each_entry_safe(rgr, tmp, &range_group->ranges, range_group_list_node) {
        UVM_ASSERT(rgr->range_group == range_group);

        uvm_range_tree_remove(&va_space->range_group_ranges, &rgr->node);
        uvm_range_group_range_destroy(rgr);
    }

    UVM_ASSERT(list_empty(&range_group->migrated_ranges));

    kmem_cache_free(g_uvm_range_group_cache, range_group);
}

NV_STATUS uvm_api_destroy_range_group(UVM_DESTROY_RANGE_GROUP_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_range_group_t *range_group = NULL;
    NV_STATUS status = NV_OK;

    uvm_va_space_down_write(va_space);

    range_group = radix_tree_delete(&va_space->range_groups, params->rangeGroupId);
    if (!range_group) {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    uvm_range_group_destroy(va_space, range_group);

done:
    uvm_va_space_up_write(va_space);
    return status;
}

void uvm_range_group_radix_tree_destroy(uvm_va_space_t *va_space)
{
    uvm_range_group_t *range_group = NULL;
    struct radix_tree_root *root = &va_space->range_groups;
    NvU64 index = 1;

    while (radix_tree_gang_lookup(root, (void**)&range_group, index, 1)) {
        UVM_ASSERT(range_group);
        radix_tree_delete(root, range_group->id);
        index = range_group->id + 1;
        uvm_range_group_destroy(va_space, range_group);
    }
}

static NV_STATUS uvm_range_group_va_range_migrate_block_locked(uvm_va_range_managed_t *managed_range,
                                                               uvm_va_block_t *va_block,
                                                               uvm_va_block_retry_t *va_block_retry,
                                                               uvm_va_block_context_t *va_block_context,
                                                               uvm_va_block_region_t region,
                                                               uvm_tracker_t *tracker)
{
    NV_STATUS status;
    NV_STATUS tracker_status;
    uvm_gpu_id_t gpu_id;
    uvm_processor_mask_t *map_mask = &va_block_context->caller_processor_mask;
    uvm_va_policy_t *policy = &managed_range->policy;
    // Set the migration CPU NUMA node from the policy.
    va_block_context->make_resident.dest_nid = policy->preferred_nid;

    // Unmapping UVM_ID_CPU is guaranteed to never fail
    status = uvm_va_block_unmap(va_block, va_block_context, UVM_ID_CPU, region, NULL, NULL);
    UVM_ASSERT(status == NV_OK);

    if (uvm_va_policy_is_read_duplicate(&managed_range->policy, managed_range->va_range.va_space)) {
        status = uvm_va_block_make_resident_read_duplicate(va_block,
                                                           va_block_retry,
                                                           va_block_context,
                                                           policy->preferred_location,
                                                           region,
                                                           NULL,
                                                           NULL,
                                                           UVM_MAKE_RESIDENT_CAUSE_API_SET_RANGE_GROUP);
    }
    else {
        status = uvm_va_block_make_resident(va_block,
                                            va_block_retry,
                                            va_block_context,
                                            policy->preferred_location,
                                            region,
                                            NULL,
                                            NULL,
                                            UVM_MAKE_RESIDENT_CAUSE_API_SET_RANGE_GROUP);
    }

    if (status != NV_OK)
        return status;

    // 1- Map all UVM-Lite SetAccessedBy GPUs and the preferred location with
    // RWA permission
    status = uvm_va_block_map_mask(va_block,
                                   va_block_context,
                                   &managed_range->va_range.uvm_lite_gpus,
                                   region,
                                   NULL,
                                   UVM_PROT_READ_WRITE_ATOMIC,
                                   UvmEventMapRemoteCauseCoherence);
    if (status != NV_OK)
        goto out;

    // 2- Map faultable SetAccessedBy GPUs.
    uvm_processor_mask_and(map_mask,
                           &managed_range->policy.accessed_by,
                           &managed_range->va_range.va_space->can_access[uvm_id_value(policy->preferred_location)]);
    uvm_processor_mask_andnot(map_mask, map_mask, &managed_range->va_range.uvm_lite_gpus);

    for_each_gpu_id_in_mask(gpu_id, map_mask) {
        status = uvm_va_block_add_mappings(va_block,
                                           va_block_context,
                                           gpu_id,
                                           region,
                                           NULL,
                                           UvmEventMapRemoteCausePolicy);
        if (status != NV_OK)
            goto out;
    }

out:
    tracker_status = uvm_tracker_add_tracker_safe(tracker, &va_block->tracker);

    return status == NV_OK ? tracker_status : status;
}

NV_STATUS uvm_range_group_va_range_migrate(uvm_va_range_managed_t *managed_range,
                                           NvU64 start,
                                           NvU64 end,
                                           uvm_tracker_t *out_tracker)
{
    uvm_va_block_t *va_block = NULL;
    size_t i = 0;
    NV_STATUS status = NV_OK;
    uvm_va_block_retry_t va_block_retry;
    uvm_va_block_context_t *va_block_context;

    // This path is only called for non-migratable range groups so it never
    // creates CPU mappings, meaning no mm is needed.
    va_block_context = uvm_va_block_context_alloc(NULL);
    if (!va_block_context)
        return NV_ERR_NO_MEMORY;

    uvm_assert_rwsem_locked(&managed_range->va_range.va_space->lock);

    // Iterate over blocks, populating them if necessary
    for (i = uvm_va_range_block_index(managed_range, start); i <= uvm_va_range_block_index(managed_range, end); ++i) {
        uvm_va_block_region_t region;
        status = uvm_va_range_block_create(managed_range, i, &va_block);
        if (status != NV_OK)
            break;

        region = uvm_va_block_region_from_start_end(va_block,
                                                    max(start, va_block->start),
                                                    min(end, va_block->end));

        status = UVM_VA_BLOCK_LOCK_RETRY(va_block, &va_block_retry,
                uvm_range_group_va_range_migrate_block_locked(managed_range,
                                                              va_block,
                                                              &va_block_retry,
                                                              va_block_context,
                                                              region,
                                                              out_tracker));
        if (status != NV_OK)
            break;
    }

    uvm_va_block_context_free(va_block_context);

    return status;
}

NV_STATUS uvm_api_set_range_group(UVM_SET_RANGE_GROUP_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_range_group_t *range_group = NULL;
    uvm_va_range_managed_t *managed_range, *managed_range_last;
    unsigned long long last_address = params->requestedBase + params->length - 1;
    uvm_tracker_t local_tracker;
    NV_STATUS tracker_status;
    NV_STATUS status = NV_OK;
    bool has_va_space_write_lock;
    bool migratable;

    UVM_ASSERT(va_space);

    // Check address and length alignment
    if (uvm_api_range_invalid(params->requestedBase, params->length))
        return NV_ERR_INVALID_ADDRESS;

    uvm_tracker_init(&local_tracker);

    uvm_va_space_down_write(va_space);
    has_va_space_write_lock = true;

    // Check that range group exists
    range_group = radix_tree_lookup(&va_space->range_groups, params->rangeGroupId);
    if (!range_group && (params->rangeGroupId != UVM_RANGE_GROUP_ID_NONE)) {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    // If the desired range group is not migratable, any overlapping managed
    // ranges must have a preferred location
    migratable = uvm_range_group_migratable(range_group);
    managed_range_last = NULL;
    uvm_for_each_va_range_managed_in_contig(managed_range, va_space, params->requestedBase, last_address) {
        managed_range_last = managed_range;
        if (!migratable && UVM_ID_IS_INVALID(managed_range->policy.preferred_location)) {
            status = NV_ERR_INVALID_ADDRESS;
            goto done;
        }
    }

    // Check that we were able to iterate over the entire range without any gaps
    if (!managed_range_last || managed_range_last->va_range.node.end < last_address) {
        status = NV_ERR_INVALID_ADDRESS;
        goto done;
    }

    status = uvm_range_group_assign_range(va_space, range_group, params->requestedBase, last_address);
    if (status != NV_OK)
        goto done;

    // Early exit if no need for migration
    if (uvm_range_group_migratable(range_group))
        goto done;

    uvm_va_space_downgrade_write(va_space);
    has_va_space_write_lock = false;

    // Already checked for gaps above
    uvm_for_each_va_range_managed_in(managed_range, va_space, params->requestedBase, last_address) {
        status = uvm_range_group_va_range_migrate(managed_range,
                                                  max(managed_range->va_range.node.start, params->requestedBase),
                                                  min(managed_range->va_range.node.end, last_address),
                                                  &local_tracker);
        if (status != NV_OK)
            goto done;
    }

done:
    tracker_status = uvm_tracker_wait_deinit(&local_tracker);

    if (has_va_space_write_lock)
        uvm_va_space_up_write(va_space);
    else
        uvm_va_space_up_read(va_space);

    return status == NV_OK ? tracker_status : status;
}

static NV_STATUS uvm_range_group_prevent_migration(uvm_range_group_t *range_group,
                                                   uvm_va_space_t *va_space)
{
    uvm_range_group_range_t *rgr = NULL;
    uvm_va_range_managed_t *managed_range;
    uvm_processor_id_t preferred_location;
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    NV_STATUS tracker_status;
    NV_STATUS status = NV_OK;

    LIST_HEAD(local_migrated_ranges);

    UVM_ASSERT(range_group);
    UVM_ASSERT(va_space);

    uvm_assert_rwsem_locked(&va_space->lock);

    // Move the range group's migrated_ranges list to the local_migrated_ranges
    // list and process it from there.
    uvm_spin_lock(&range_group->migrated_ranges_lock);
    list_replace_init(&range_group->migrated_ranges, &local_migrated_ranges);
    uvm_spin_unlock(&range_group->migrated_ranges_lock);

    while (true) {
        // Delete each item from the beginning of the list.
        uvm_spin_lock(&range_group->migrated_ranges_lock);
        rgr = list_first_entry_or_null(&local_migrated_ranges,
                                       uvm_range_group_range_t,
                                       range_group_migrated_list_node);
        if (rgr)
            list_del_init(&rgr->range_group_migrated_list_node);
        uvm_spin_unlock(&range_group->migrated_ranges_lock);

        if (!rgr)
            break;

        uvm_for_each_va_range_managed_in(managed_range, va_space, rgr->node.start, rgr->node.end) {
            // Managed ranges need to have a preferred location set in order for
            // their range group to be set to non-migratable.
            preferred_location = managed_range->policy.preferred_location;
            if (UVM_ID_IS_INVALID(preferred_location)) {
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }

            // If the preferred location is a GPU, check that it's not
            // fault-capable
            if (UVM_ID_IS_GPU(preferred_location) &&
                uvm_processor_mask_test(&va_space->faultable_processors, preferred_location)) {
                status = NV_ERR_INVALID_DEVICE;
                goto done;
            }

            // Check that all UVM-Lite GPUs are able to access the
            // preferred location
            if (!uvm_processor_mask_subset(&managed_range->va_range.uvm_lite_gpus,
                                           &va_space->accessible_from[uvm_id_value(preferred_location)])) {
                status = NV_ERR_INVALID_DEVICE;
                goto done;
            }

            // Perform the migration of the managed range.
            status = uvm_range_group_va_range_migrate(managed_range,
                                                      max(managed_range->va_range.node.start, rgr->node.start),
                                                      min(managed_range->va_range.node.end, rgr->node.end),
                                                      &local_tracker);
            if (status != NV_OK)
                goto done;
        }
    }

done:
    tracker_status = uvm_tracker_wait_deinit(&local_tracker);
    if (status == NV_OK)
        status = tracker_status;

    // We may have exited early, in which case rgr may be unprocessed and
    // local_migrated_ranges may not be empty. These entries should be put back
    // on range_group's migrated_ranges list.
    if (status != NV_OK) {
        uvm_spin_lock(&range_group->migrated_ranges_lock);
        if (rgr)
            list_move_tail(&rgr->range_group_migrated_list_node, &range_group->migrated_ranges);
        list_splice_tail(&local_migrated_ranges, &range_group->migrated_ranges);
        uvm_spin_unlock(&range_group->migrated_ranges_lock);
    }

    return status;
}

static NV_STATUS uvm_range_groups_set_migration_policy(uvm_va_space_t *va_space,
                                                       NvU64 *range_group_ids,
                                                       NvU64 num_group_ids,
                                                       bool allow_migration)
{
    NV_STATUS status = NV_OK;
    NvU64 i;
    uvm_range_group_t *range_groups[UVM_MAX_RANGE_GROUPS_PER_IOCTL_CALL];

    UVM_ASSERT(va_space);

    if (!range_group_ids || num_group_ids == 0 || num_group_ids > UVM_MAX_RANGE_GROUPS_PER_IOCTL_CALL)
        return NV_ERR_INVALID_ARGUMENT;

    if (!allow_migration) {
        // We take the VA space in write mode to perform range group lookup and
        // change the migratability to false. This will wait for any concurrent
        // fault/migration to finish.
        //
        // TODO: Bug 1878225: Use a finer grain synchronization mechanism to
        // prevent races with concurrent fault handling/migration operations.
        uvm_va_space_down_write(va_space);
    }
    else {
        // It is safe to allow migration with the VA space in read mode since
        // it just flips the value of allow_migration.
        uvm_va_space_down_read(va_space);
    }

    for (i = 0; i < num_group_ids; ++i) {
        range_groups[i] = radix_tree_lookup(&va_space->range_groups, range_group_ids[i]);
        if (!range_groups[i]) {
            if (!allow_migration)
                uvm_va_space_up_write(va_space);
            else
                uvm_va_space_up_read(va_space);

            return NV_ERR_OBJECT_NOT_FOUND;
        }

        atomic_set(&range_groups[i]->allow_migration, !!allow_migration);
    }

    // If we are allowing migration, we are done.
    if (!allow_migration) {
        // Any fault handler/migration that executes after downgrading the lock
        // mode will see migratability disabled.
        uvm_va_space_downgrade_write(va_space);

        for (i = 0; i < num_group_ids; ++i) {
            status = uvm_range_group_prevent_migration(range_groups[i], va_space);
            if (status != NV_OK)
                break;
        }
    }

    uvm_va_space_up_read(va_space);
    return status;
}

static uvm_range_group_range_t *range_group_range_container(uvm_range_tree_node_t *node)
{
    if (node == NULL)
        return NULL;
    return container_of(node, uvm_range_group_range_t, node);
}

static uvm_range_group_range_t *range_group_range_prev(uvm_va_space_t *va_space, uvm_range_group_range_t *range)
{
    uvm_range_tree_node_t *node = uvm_range_tree_prev(&va_space->range_group_ranges, &range->node);
    return range_group_range_container(node);
}

static uvm_range_group_range_t *range_group_range_next(uvm_va_space_t *va_space, uvm_range_group_range_t *range)
{
    uvm_range_tree_node_t *node = uvm_range_tree_next(&va_space->range_group_ranges, &range->node);
    return range_group_range_container(node);
}

NV_STATUS uvm_range_group_assign_range(uvm_va_space_t *va_space, uvm_range_group_t *range_group, NvU64 start, NvU64 end)
{
    NV_STATUS status;
    uvm_range_group_range_t *rgr;
    uvm_range_group_range_t *temp;
    uvm_range_group_range_t *next;
    uvm_range_group_range_t *prev;
    uvm_range_group_range_t *new_rgr = NULL;
    LIST_HEAD(internal_nodes);

    uvm_assert_rwsem_locked_write(&va_space->lock);

    if (range_group != NULL) {
        new_rgr = range_group_range_create(range_group, start, end);
        if (new_rgr == NULL)
            return NV_ERR_NO_MEMORY;
    }

    uvm_range_group_for_each_range_in(rgr, va_space, start, end) {
        if (rgr->node.start < start && rgr->node.end > end) {
            // The region described by [start, end] lies entirely within rgr
            // and does not sit on rgr's boundary.
            NvU64 orig_end = rgr->node.end;

            // Check if the rgr is already part of the specified range group and,
            // if so, do nothing.
            if (rgr->range_group == range_group) {
                uvm_range_group_range_destroy(new_rgr);
                return NV_OK;
            }

            // rgr needs to be split to make room for the new range group range.
            // Do this by first creating a new range group range called "next"
            // for the region of memory just above new_rgr, then shrink rgr
            // down so that it fits just below new_rgr.
            next = range_group_range_create(rgr->range_group, end + 1, orig_end);
            if (next == NULL) {
                uvm_range_group_range_destroy(new_rgr);
                return NV_ERR_NO_MEMORY;
            }

            uvm_range_tree_shrink_node(&va_space->range_group_ranges, &rgr->node, rgr->node.start, start - 1);
            status = uvm_range_tree_add(&va_space->range_group_ranges, &next->node);
            UVM_ASSERT(status == NV_OK);

            // Check if we need to add "next" to the range group's migrated list.
            uvm_spin_lock(&rgr->range_group->migrated_ranges_lock);
            if (!list_empty(&rgr->range_group_migrated_list_node))
                list_move_tail(&next->range_group_migrated_list_node, &next->range_group->migrated_ranges);
            uvm_spin_unlock(&rgr->range_group->migrated_ranges_lock);
        }
        else if (rgr->node.start < start) {
            // The region [start, end] overlaps with the end of rgr.
            // Shrink rgr by moving its end downward.
            uvm_range_tree_shrink_node(&va_space->range_group_ranges, &rgr->node, rgr->node.start, start - 1);
        }
        else if (rgr->node.end > end) {
            // The region [start, end] overlaps with the begining of rgr.
            // Shrink rgr by moving its beginning upward.
            uvm_range_tree_shrink_node(&va_space->range_group_ranges, &rgr->node, end + 1, rgr->node.end);
        }
        else {
            // rgr lies entirely within [start, end]. rgr should be destroyed.
            list_move(&rgr->range_group_list_node, &internal_nodes);
        }
    }

    // Always add new_rgr to range_group's migrated list.
    if (new_rgr) {
        uvm_spin_lock(&new_rgr->range_group->migrated_ranges_lock);
        list_move_tail(&new_rgr->range_group_migrated_list_node, &new_rgr->range_group->migrated_ranges);
        uvm_spin_unlock(&new_rgr->range_group->migrated_ranges_lock);
    }

    list_for_each_entry_safe(rgr, temp, &internal_nodes, range_group_list_node) {
        uvm_range_tree_remove(&va_space->range_group_ranges, &rgr->node);
        uvm_range_group_range_destroy(rgr);
    }

    if (range_group == NULL)
        return NV_OK;

    status = uvm_range_tree_add(&va_space->range_group_ranges, &new_rgr->node);
    UVM_ASSERT(status == NV_OK);

    prev = range_group_range_prev(va_space, new_rgr);
    if (prev != NULL && prev->node.end + 1 == new_rgr->node.start && prev->range_group == new_rgr->range_group) {
        uvm_range_tree_merge_prev(&va_space->range_group_ranges, &new_rgr->node);
        uvm_range_group_range_destroy(prev);
    }

    next = range_group_range_next(va_space, new_rgr);
    if (next != NULL && next->node.start - 1 == new_rgr->node.end && next->range_group == new_rgr->range_group) {
        uvm_range_tree_merge_next(&va_space->range_group_ranges, &new_rgr->node);
        uvm_range_group_range_destroy(next);
    }

    return NV_OK;
}

bool uvm_range_group_address_migratable(uvm_va_space_t *va_space, NvU64 address)
{
    uvm_range_group_range_t *rgr = uvm_range_group_range_find(va_space, address);
    return rgr == NULL || uvm_range_group_migratable(rgr->range_group);
}

bool uvm_range_group_any_migratable(uvm_va_space_t *va_space, NvU64 start, NvU64 end)
{
    uvm_range_group_range_iter_t iter;
    uvm_range_group_for_all_ranges_in(&iter, va_space, start, end) {
        if (iter.migratable)
            return true;
    }

    return false;
}

bool uvm_range_group_all_migratable(uvm_va_space_t *va_space, NvU64 start, NvU64 end)
{
    uvm_range_group_range_t *rgr;
    uvm_range_group_for_each_range_in(rgr, va_space, start, end) {
        if (!uvm_range_group_migratable(rgr->range_group))
            return false;
    }

    return true;
}

uvm_range_group_range_t *uvm_range_group_range_find(uvm_va_space_t *va_space, NvU64 addr)
{
    uvm_range_tree_node_t *node;
    uvm_assert_rwsem_locked(&va_space->lock);

    node = uvm_range_tree_find(&va_space->range_group_ranges, addr);
    return range_group_range_container(node);
}

uvm_range_group_range_t *uvm_range_group_range_iter_first(uvm_va_space_t *va_space, NvU64 start, NvU64 end)
{
    uvm_range_tree_node_t *node = uvm_range_tree_iter_first(&va_space->range_group_ranges, start, end);
    return range_group_range_container(node);
}

uvm_range_group_range_t *uvm_range_group_range_iter_next(uvm_va_space_t *va_space,
                                                         uvm_range_group_range_t *range,
                                                         NvU64 end)
{
    uvm_range_tree_node_t *node = uvm_range_tree_iter_next(&va_space->range_group_ranges, &range->node, end);
    return range_group_range_container(node);
}

static void range_group_range_iter_advance(uvm_range_group_range_iter_t *iter, NvU64 end)
{
    if (iter->node == NULL) {
        iter->end = end;
    }
    else {
        iter->is_current = iter->start >= iter->node->node.start;
        if (iter->is_current)
            iter->end = min(iter->node->node.end, end);
        else
            iter->end = min(iter->node->node.start - 1, end);
    }
    iter->migratable = iter->node == NULL || !iter->is_current || uvm_range_group_migratable(iter->node->range_group);
}


void uvm_range_group_range_iter_all_first(uvm_va_space_t *va_space,
                                          NvU64 start,
                                          NvU64 end,
                                          uvm_range_group_range_iter_t *iter)
{
    iter->valid = true;
    iter->start = start;
    iter->node = uvm_range_group_range_iter_first(va_space, start, end);

    range_group_range_iter_advance(iter, end);
}

bool uvm_range_group_range_iter_all_next(uvm_va_space_t *va_space,
                                         uvm_range_group_range_iter_t *iter,
                                         NvU64 end)
{
    iter->valid = iter->end < end;
    if (!iter->valid)
        return false;

    iter->start = iter->end + 1;
    if (iter->is_current)
        iter->node = uvm_range_group_range_iter_next(va_space, iter->node, end);

    range_group_range_iter_advance(iter, end);
    return true;
}

void uvm_range_group_range_migratability_iter_first(uvm_va_space_t *va_space,
                                                    NvU64 start,
                                                    NvU64 end,
                                                    uvm_range_group_range_iter_t *iter)
{
    uvm_range_group_range_iter_t next;

    uvm_range_group_range_iter_all_first(va_space, start, end, iter);
    next = *iter;

    while (uvm_range_group_range_iter_all_next(va_space, &next, end) && next.migratable == iter->migratable)
        *iter = next;

    iter->start = start;
}

void uvm_range_group_range_migratability_iter_next(uvm_va_space_t *va_space,
                                                   uvm_range_group_range_iter_t *iter,
                                                   NvU64 end)
{
    uvm_range_group_range_iter_t next;
    NvU64 start;

    if (!uvm_range_group_range_iter_all_next(va_space, iter, end))
        return;

    start = iter->start;
    next = *iter;
    while (uvm_range_group_range_iter_all_next(va_space, &next, end) && next.migratable == iter->migratable)
        *iter = next;

    iter->start = start;
}

void uvm_range_group_migratable_page_mask(uvm_va_block_t *va_block,
                                          uvm_va_block_region_t region,
                                          uvm_page_mask_t *mask_out)
{
    uvm_range_group_range_iter_t iter;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    uvm_page_mask_zero(mask_out);

    uvm_range_group_for_each_migratability_in(&iter,
                                              va_space,
                                              uvm_va_block_region_start(va_block, region),
                                              uvm_va_block_region_end(va_block, region)) {
        if (iter.migratable)
            uvm_page_mask_region_fill(mask_out, uvm_va_block_region_from_start_end(va_block, iter.start, iter.end));
    }
}

NV_STATUS uvm_api_prevent_migration_range_groups(UVM_PREVENT_MIGRATION_RANGE_GROUPS_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    status = uvm_range_groups_set_migration_policy(va_space, params->rangeGroupIds, params->numGroupIds, false);
    if (status == NV_OK)
        uvm_tools_flush_events();

    return status;
}

NV_STATUS uvm_api_allow_migration_range_groups(UVM_ALLOW_MIGRATION_RANGE_GROUPS_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    return uvm_range_groups_set_migration_policy(va_space, params->rangeGroupIds, params->numGroupIds, true);
}

NV_STATUS uvm_test_range_group_range_info(UVM_TEST_RANGE_GROUP_RANGE_INFO_PARAMS *params, struct file *filp)
{
    uvm_range_group_range_t *rgr;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_va_space_down_read(va_space);

    rgr = uvm_range_group_range_iter_first(va_space, params->lookup_address, ULLONG_MAX);

    params->range_group_present = rgr != NULL && rgr->node.start <= params->lookup_address;
    if (params->range_group_present) {
        params->range_group_range_start = rgr->node.start;
        params->range_group_range_end = rgr->node.end;
        params->range_group_id = rgr->range_group->id;
    }
    else {
        uvm_range_group_range_t *prev;
        uvm_range_group_range_t *next = rgr;

        if (next) {
            params->range_group_range_end = next->node.start - 1;
            prev = range_group_range_prev(va_space, next);
        }
        else {
            params->range_group_range_end = ULLONG_MAX;
            prev = list_last_entry_or_null(&va_space->range_group_ranges.head, uvm_range_group_range_t, node.list);
        }

        if (prev)
            params->range_group_range_start = prev->node.end + 1;
        else
            params->range_group_range_start = 0;

        params->range_group_id = UVM_RANGE_GROUP_ID_NONE;
    }

    uvm_va_space_up_read(va_space);

    return NV_OK;
}

NV_STATUS uvm_test_range_group_range_count(UVM_TEST_RANGE_GROUP_RANGE_COUNT_PARAMS *params, struct file *filp)
{
    uvm_range_group_range_t *rgr;
    uvm_range_group_t *range_group;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    params->count = 0;
    uvm_va_space_down_read(va_space);

    range_group = radix_tree_lookup(&va_space->range_groups, params->rangeGroupId);
    if (range_group == NULL) {
        uvm_va_space_up_read(va_space);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    list_for_each_entry(rgr, &range_group->ranges, range_group_list_node) {
        UVM_ASSERT(rgr->range_group == range_group);
        params->count++;
    }

    uvm_va_space_up_read(va_space);

    return NV_OK;
}
