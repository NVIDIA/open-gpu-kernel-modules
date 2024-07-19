/*******************************************************************************
    Copyright (c) 2022 NVIDIA Corporation

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
#include "uvm_linux.h"
#include "uvm_types.h"
#include "uvm_va_policy.h"
#include "uvm_va_block.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"

const uvm_va_policy_t uvm_va_policy_default = {
    .preferred_location = UVM_ID_INVALID,
    .preferred_nid = NUMA_NO_NODE,
    .read_duplication = UVM_READ_DUPLICATION_UNSET,
};

bool uvm_va_policy_is_read_duplicate(const uvm_va_policy_t *policy, uvm_va_space_t *va_space)
{
    return policy->read_duplication == UVM_READ_DUPLICATION_ENABLED &&
           uvm_va_space_can_read_duplicate(va_space, NULL);
}

const uvm_va_policy_t *uvm_va_policy_get(uvm_va_block_t *va_block, NvU64 addr)
{
    uvm_assert_mutex_locked(&va_block->lock);

    if (uvm_va_block_is_hmm(va_block)) {
        const uvm_va_policy_node_t *node = uvm_va_policy_node_find(va_block, addr);

        return node ? &node->policy : &uvm_va_policy_default;
    }
    else {
        return uvm_va_range_get_policy(va_block->va_range);
    }
}

// HMM va_blocks can have different polices for different regions withing the
// va_block. This function checks the given region is covered by the same policy
// and asserts if the region is covered by different policies.
// This always returns true and is intended to only be used with UVM_ASSERT() to
// avoid calling it on release builds.
// Locking: the va_block lock must be held.
static bool uvm_hmm_va_block_assert_policy_is_valid(uvm_va_block_t *va_block,
                                                    const uvm_va_policy_t *policy,
                                                    uvm_va_block_region_t region)
{
    const uvm_va_policy_node_t *node;

    if (uvm_va_policy_is_default(policy)) {
        // There should only be the default policy within the region.
        node = uvm_va_policy_node_iter_first(va_block,
                                             uvm_va_block_region_start(va_block, region),
                                             uvm_va_block_region_end(va_block, region));
        UVM_ASSERT(!node);
    }
    else {
        // The policy node should cover the region.
        node = uvm_va_policy_node_from_policy(policy);
        UVM_ASSERT(node->node.start <= uvm_va_block_region_start(va_block, region));
        UVM_ASSERT(node->node.end >= uvm_va_block_region_end(va_block, region));
    }

    return true;
}

const uvm_va_policy_t *uvm_va_policy_get_region(uvm_va_block_t *va_block, uvm_va_block_region_t region)
{
    uvm_assert_mutex_locked(&va_block->lock);

    if (uvm_va_block_is_hmm(va_block)) {
        const uvm_va_policy_t *policy;
        const uvm_va_policy_node_t *node = uvm_va_policy_node_find(va_block, uvm_va_block_region_start(va_block, region));

        policy = node ? &node->policy : &uvm_va_policy_default;
        UVM_ASSERT(uvm_hmm_va_block_assert_policy_is_valid(va_block, policy, region));
        return policy;
    }
    else {
        return uvm_va_range_get_policy(va_block->va_range);
    }
}

bool uvm_va_policy_preferred_location_equal(const uvm_va_policy_t *policy, uvm_processor_id_t proc, int cpu_numa_id)
{
    bool equal = uvm_id_equal(policy->preferred_location, proc);

    if (!UVM_ID_IS_CPU(policy->preferred_location))
        UVM_ASSERT(policy->preferred_nid == NUMA_NO_NODE);

    if (!UVM_ID_IS_CPU(proc))
        UVM_ASSERT(cpu_numa_id == NUMA_NO_NODE);

    if (equal && UVM_ID_IS_CPU(policy->preferred_location))
        equal = uvm_numa_id_eq(policy->preferred_nid, cpu_numa_id);

    return equal;
}

#if UVM_IS_CONFIG_HMM()

static struct kmem_cache *g_uvm_va_policy_node_cache __read_mostly;

static uvm_va_policy_node_t *uvm_va_policy_node_container(uvm_range_tree_node_t *tree_node)
{
    return container_of(tree_node, uvm_va_policy_node_t, node);
}

NV_STATUS uvm_va_policy_init(void)
{
    g_uvm_va_policy_node_cache = NV_KMEM_CACHE_CREATE("uvm_va_policy_node_t", uvm_va_policy_node_t);
    if (!g_uvm_va_policy_node_cache)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_va_policy_exit(void)
{
    kmem_cache_destroy_safe(&g_uvm_va_policy_node_cache);
}

static uvm_va_policy_node_t *uvm_va_policy_node_alloc(NvU64 start, NvU64 end)
{
    uvm_va_policy_node_t *node;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(end + 1));

    node = nv_kmem_cache_zalloc(g_uvm_va_policy_node_cache, NV_UVM_GFP_FLAGS);
    if (!node)
        return NULL;

    node->node.start = start;
    node->node.end = end;

    return node;
}

static void uvm_va_policy_node_free(uvm_va_policy_node_t *node)
{
    kmem_cache_free(g_uvm_va_policy_node_cache, node);
}

static uvm_va_policy_node_t *uvm_va_policy_node_create(uvm_va_block_t *va_block, NvU64 start, NvU64 end)
{
    uvm_va_policy_node_t *node;
    NV_STATUS status;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(start >= va_block->start);
    UVM_ASSERT(end <= va_block->end);

    node = uvm_va_policy_node_alloc(start, end);
    if (!node)
        return NULL;

    node->policy = uvm_va_policy_default;

    status = uvm_range_tree_add(&va_block->hmm.va_policy_tree, &node->node);
    UVM_ASSERT(status == NV_OK);

    return node;
}

uvm_va_policy_node_t *uvm_va_policy_node_find(uvm_va_block_t *va_block, NvU64 addr)
{
    uvm_range_tree_node_t *tree_node;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);

    tree_node = uvm_range_tree_find(&va_block->hmm.va_policy_tree, addr);
    if (!tree_node)
        return NULL;

    return uvm_va_policy_node_container(tree_node);
}

uvm_va_policy_node_t *uvm_va_policy_node_iter_first(uvm_va_block_t *va_block, NvU64 start, NvU64 end)
{
    uvm_range_tree_node_t *tree_node;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);

    tree_node = uvm_range_tree_iter_first(&va_block->hmm.va_policy_tree, start, end);
    if (!tree_node)
        return NULL;

    return uvm_va_policy_node_container(tree_node);
}

uvm_va_policy_node_t *uvm_va_policy_node_iter_next(uvm_va_block_t *va_block,
                                                   uvm_va_policy_node_t *node,
                                                   NvU64 end)
{
    uvm_range_tree_node_t *tree_node;

    if (!node)
        return NULL;

    tree_node = uvm_range_tree_iter_next(&va_block->hmm.va_policy_tree, &node->node, end);
    if (!tree_node)
        return NULL;

    return uvm_va_policy_node_container(tree_node);
}

const uvm_va_policy_t *uvm_va_policy_iter_first(uvm_va_block_t *va_block,
                                                NvU64 start,
                                                NvU64 end,
                                                uvm_va_policy_node_t **out_node,
                                                uvm_va_block_region_t *out_region)
{
    uvm_range_tree_node_t *tree_node;
    uvm_va_policy_node_t *node;
    const uvm_va_policy_t *policy;
    uvm_va_block_region_t region;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(start >= va_block->start);
    UVM_ASSERT(end <= va_block->end);
    UVM_ASSERT(start < end);

    region.first = uvm_va_block_cpu_page_index(va_block, start);

    // Even if no policy is found, we return the default policy and loop
    // one time.
    tree_node = uvm_range_tree_iter_first(&va_block->hmm.va_policy_tree, start, end);
    if (tree_node) {
        node = uvm_va_policy_node_container(tree_node);
        if (node->node.start <= start) {
            policy = &node->policy;
            region.outer = uvm_va_block_cpu_page_index(va_block, min(end, node->node.end)) + 1;
        }
        else {
            // This node starts after the requested start so use the default,
            // then use this policy node.
            policy = &uvm_va_policy_default;
            region.outer = uvm_va_block_cpu_page_index(va_block, node->node.start - 1) + 1;
        }
    }
    else {
        node = NULL;
        policy = &uvm_va_policy_default;
        region.outer = uvm_va_block_cpu_page_index(va_block, end) + 1;
    }

    *out_node = node;
    *out_region = region;
    return policy;
}

const uvm_va_policy_t *uvm_va_policy_iter_next(uvm_va_block_t *va_block,
                                               const uvm_va_policy_t *policy,
                                               NvU64 end,
                                               uvm_va_policy_node_t **inout_node,
                                               uvm_va_block_region_t *inout_region)
{
    uvm_va_policy_node_t *node = *inout_node;
    uvm_va_policy_node_t *next;
    uvm_va_block_region_t region;

    if (!node)
        return NULL;

    next = uvm_va_policy_node_iter_next(va_block, node, end);

    if (uvm_va_policy_is_default(policy)) {
        // We haven't used the current policy node yet so use it now.
        next = node;
        policy = &node->policy;
        region = uvm_va_block_region_from_start_end(va_block,
                                                    node->node.start,
                                                    min(end, node->node.end));
    }
    else if (!next) {
        if (node->node.end >= end)
            return NULL;
        policy = &uvm_va_policy_default;
        region.first = inout_region->outer;
        region.outer = uvm_va_block_cpu_page_index(va_block, end) + 1;
    }
    else {
        region.first = inout_region->outer;

        if (next->node.start <= uvm_va_block_region_start(va_block, region)) {
            policy = &next->policy;
            region.outer = uvm_va_block_cpu_page_index(va_block, min(end, next->node.end)) + 1;
        }
        else {
            // There is a gap between the last node and next so use the
            // default policy.
            policy = &uvm_va_policy_default;
            region.outer = uvm_va_block_cpu_page_index(va_block, next->node.start - 1) + 1;
        }
    }

    *inout_node = next;
    *inout_region = region;
    return policy;
}

NV_STATUS uvm_va_policy_node_split(uvm_va_block_t *va_block,
                                   uvm_va_policy_node_t *old,
                                   NvU64 new_end,
                                   uvm_va_policy_node_t **new_ptr)
{
    uvm_va_policy_node_t *new;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);

    UVM_ASSERT(new_end > old->node.start);
    UVM_ASSERT(new_end < old->node.end);

    new = uvm_va_policy_node_alloc(new_end + 1, old->node.end);
    if (!new)
        return NV_ERR_NO_MEMORY;

    new->policy = old->policy;

    uvm_range_tree_split(&va_block->hmm.va_policy_tree, &old->node, &new->node);

    if (new_ptr)
        *new_ptr = new;

    return NV_OK;
}

void uvm_va_policy_node_split_move(uvm_va_block_t *old_va_block,
                                   uvm_va_block_t *new_va_block)
{
    uvm_va_policy_node_t *node, *next;
    NV_STATUS status;

    UVM_ASSERT(uvm_va_block_is_hmm(old_va_block));
    UVM_ASSERT(uvm_va_block_is_hmm(new_va_block));
    uvm_assert_mutex_locked(&old_va_block->lock);

    UVM_ASSERT(old_va_block->end + 1 == new_va_block->start);

    uvm_for_each_va_policy_node_in_safe(node, next, old_va_block, new_va_block->start, new_va_block->end) {
        uvm_range_tree_remove(&old_va_block->hmm.va_policy_tree, &node->node);
        UVM_ASSERT(node->node.start >= new_va_block->start);
        UVM_ASSERT(node->node.end <= new_va_block->end);
        status = uvm_range_tree_add(&new_va_block->hmm.va_policy_tree, &node->node);
        UVM_ASSERT(status == NV_OK);
    }
}

void uvm_va_policy_clear(uvm_va_block_t *va_block, NvU64 start, NvU64 end)
{
    uvm_va_policy_node_t *node, *new;
    uvm_range_tree_node_t *tree_node;
    NV_STATUS status;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);

    tree_node = uvm_range_tree_iter_first(&va_block->hmm.va_policy_tree, start, end);
    while (tree_node) {
        node = uvm_va_policy_node_container(tree_node);
        tree_node = uvm_range_tree_iter_next(&va_block->hmm.va_policy_tree, &node->node, end);

        if (node->node.start < start) {
            if (node->node.end <= end) {
                uvm_range_tree_shrink_node(&va_block->hmm.va_policy_tree, &node->node, node->node.start, start - 1);
                continue;
            }

            status = uvm_va_policy_node_split(va_block, node, start - 1, &new);
            // If we can't split, save the policy before the part being cleared
            // but forget the policy after the range to be cleared.
            // Since policy isn't a guarantee, at least this is safe.
            if (status != NV_OK) {
                uvm_range_tree_shrink_node(&va_block->hmm.va_policy_tree, &node->node, node->node.start, start - 1);
                continue;
            }

            node = new;
        }

        if (node->node.end > end) {
            uvm_range_tree_shrink_node(&va_block->hmm.va_policy_tree, &node->node, end + 1, node->node.end);
            continue;
        }

        uvm_range_tree_remove(&va_block->hmm.va_policy_tree, &node->node);
        uvm_va_policy_node_free(node);
    }
}

static void uvm_va_policy_node_set(uvm_va_policy_node_t *node,
                                   uvm_va_policy_type_t which,
                                   uvm_processor_id_t processor_id,
                                   int cpu_numa_nid,
                                   uvm_read_duplication_policy_t new_policy)
{
    switch (which) {
        case UVM_VA_POLICY_PREFERRED_LOCATION:
            UVM_ASSERT(!UVM_ID_IS_INVALID(processor_id));
            node->policy.preferred_location = processor_id;
            node->policy.preferred_nid = cpu_numa_nid;
            break;

        case UVM_VA_POLICY_ACCESSED_BY:
            UVM_ASSERT(!UVM_ID_IS_INVALID(processor_id));
            uvm_processor_mask_set(&node->policy.accessed_by, processor_id);
            break;

        case UVM_VA_POLICY_READ_DUPLICATION:
            UVM_ASSERT(new_policy == UVM_READ_DUPLICATION_ENABLED ||
                       new_policy == UVM_READ_DUPLICATION_DISABLED);
            node->policy.read_duplication = new_policy;
            break;

        default:
            UVM_ASSERT_MSG(0, "Unknown policy type %u\n", which);
            break;
    }
}

static void uvm_va_policy_node_clear(uvm_va_block_t *va_block,
                                     uvm_va_policy_node_t *node,
                                     uvm_va_policy_type_t which,
                                     uvm_processor_id_t processor_id,
                                     uvm_read_duplication_policy_t new_policy)
{
    switch (which) {
        case UVM_VA_POLICY_PREFERRED_LOCATION:
            UVM_ASSERT(UVM_ID_IS_INVALID(processor_id));
            node->policy.preferred_location = processor_id;
            node->policy.preferred_nid = NUMA_NO_NODE;
            break;

        case UVM_VA_POLICY_ACCESSED_BY:
            UVM_ASSERT(!UVM_ID_IS_INVALID(processor_id));
            uvm_processor_mask_clear(&node->policy.accessed_by, processor_id);
            break;

        case UVM_VA_POLICY_READ_DUPLICATION:
        default:
            // Read duplication is never set back to UVM_READ_DUPLICATION_UNSET.
            UVM_ASSERT(0);
            break;
    }

    // Check to see if the node is now the default and can be removed.
    if (UVM_ID_IS_INVALID(node->policy.preferred_location) &&
            uvm_processor_mask_empty(&node->policy.accessed_by) &&
            node->policy.read_duplication == UVM_READ_DUPLICATION_UNSET) {
        uvm_range_tree_remove(&va_block->hmm.va_policy_tree, &node->node);
        uvm_va_policy_node_free(node);
    }
}

static uvm_va_policy_node_t *create_node_and_set(uvm_va_block_t *va_block,
                                                 NvU64 start,
                                                 NvU64 end,
                                                 uvm_va_policy_type_t which,
                                                 uvm_processor_id_t processor_id,
                                                 int cpu_numa_nid,
                                                 uvm_read_duplication_policy_t new_policy)
{
    uvm_va_policy_node_t *node;

    // Create a new node for the missing range.
    node = uvm_va_policy_node_create(va_block, start, end);
    if (!node)
        return node;

    uvm_va_policy_node_set(node, which, processor_id, cpu_numa_nid, new_policy);

    return node;
}

static bool va_policy_node_split_needed(uvm_va_policy_node_t *node,
                                        NvU64 start,
                                        NvU64 end,
                                        uvm_va_policy_type_t which,
                                        bool is_default,
                                        uvm_processor_id_t processor_id,
                                        int cpu_numa_nid,
                                        uvm_read_duplication_policy_t new_policy)
{
    // If the node doesn't extend beyond the range being set, it doesn't need
    // to be split.
    if (node->node.start >= start && node->node.end <= end)
        return false;

    // If the new policy value doesn't match the old value, a split is needed.
    switch (which) {
        case UVM_VA_POLICY_PREFERRED_LOCATION:
            return !uvm_va_policy_preferred_location_equal(&node->policy, processor_id, cpu_numa_nid);

        case UVM_VA_POLICY_ACCESSED_BY:
            if (is_default)
                return uvm_processor_mask_test(&node->policy.accessed_by, processor_id);
            else
                return !uvm_processor_mask_test(&node->policy.accessed_by, processor_id);

        case UVM_VA_POLICY_READ_DUPLICATION:
            return node->policy.read_duplication != new_policy;

        default:
            UVM_ASSERT(0);
            return false;
    }
}

NV_STATUS uvm_va_policy_set_range(uvm_va_block_t *va_block,
                                  NvU64 start,
                                  NvU64 end,
                                  uvm_va_policy_type_t which,
                                  bool is_default,
                                  uvm_processor_id_t processor_id,
                                  int cpu_numa_nid,
                                  uvm_read_duplication_policy_t new_policy)
{
    uvm_va_policy_node_t *node, *next, *new;
    NvU64 addr;
    NvU64 node_start;
    NvU64 node_end;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(end + 1));
    UVM_ASSERT(start < end);
    UVM_ASSERT(start >= va_block->start);
    UVM_ASSERT(start < va_block->end);
    UVM_ASSERT(end <= va_block->end);

    // Note that the policy range ends have already been split so we only need
    // to fill in the middle or remove nodes.
    node = uvm_va_policy_node_iter_first(va_block, start, end);

    if (!node) {
        // There is no policy in the given range so it is already the default.
        if (is_default)
            return NV_OK;

        // Create a new node for the missing range.
        node = create_node_and_set(va_block,
                                   start,
                                   end,
                                   which,
                                   processor_id,
                                   cpu_numa_nid,
                                   new_policy);
        if (!node)
            return NV_ERR_NO_MEMORY;

        return NV_OK;
    }

    for (addr = start; node; addr = node_end + 1, node = next) {
        node_start = node->node.start;
        node_end = node->node.end;

        // Nodes should have been split before setting policy so verify that.
        UVM_ASSERT(!va_policy_node_split_needed(node,
                                                start,
                                                end,
                                                which,
                                                is_default,
                                                processor_id,
                                                cpu_numa_nid,
                                                new_policy));

        next = uvm_va_policy_node_iter_next(va_block, node, end);

        if (is_default) {
            uvm_va_policy_node_clear(va_block, node, which, processor_id, new_policy);
            // Note that node may have been deleted.
        }
        else {
            uvm_va_policy_node_set(node, which, processor_id, cpu_numa_nid, new_policy);

            // TODO: Bug 1707562: Add support for merging policy ranges.
        }

        if (!is_default && addr < node_start) {
            // Create a new node for the missing range on the left.
            new = create_node_and_set(va_block,
                                      addr,
                                      node_start - 1,
                                      which,
                                      processor_id,
                                      cpu_numa_nid,
                                      new_policy);
            if (!new)
                return NV_ERR_NO_MEMORY;
        }
        else if (!is_default && !next && node_end < end) {
            // Create a new node for the missing range on the right.
            new = create_node_and_set(va_block,
                                      node_end + 1,
                                      end,
                                      which,
                                      processor_id,
                                      cpu_numa_nid,
                                      new_policy);
            if (!new)
                return NV_ERR_NO_MEMORY;
            break;
        }
    }

    return NV_OK;
}

const uvm_va_policy_t *uvm_va_policy_set_preferred_location(uvm_va_block_t *va_block,
                                                            uvm_va_block_region_t region,
                                                            uvm_processor_id_t processor_id,
                                                            int cpu_node_id,
                                                            const uvm_va_policy_t *old_policy)
{
    NvU64 start = uvm_va_block_region_start(va_block, region);
    NvU64 end = uvm_va_block_region_end(va_block, region);
    uvm_va_policy_node_t *node;

    if (uvm_va_policy_is_default(old_policy)) {

        UVM_ASSERT(!UVM_ID_IS_INVALID(processor_id));
        UVM_ASSERT(!uvm_range_tree_iter_first(&va_block->hmm.va_policy_tree, start, end));

        node = uvm_va_policy_node_create(va_block, start, end);
        if (!node)
            return NULL;
    }
    else {
        // Since the old_policy isn't the constant default policy, we know it
        // is an allocated uvm_va_policy_node_t and can be cast.
        node = container_of((uvm_va_policy_t *)old_policy, uvm_va_policy_node_t, policy);

        // The caller guarantees that the policy node doesn't require splitting
        // and that the policy is changing.
        UVM_ASSERT(node->node.start >= start);
        UVM_ASSERT(node->node.end <= end);
        UVM_ASSERT(!uvm_va_policy_preferred_location_equal(&node->policy, processor_id, cpu_node_id));
    }

    node->policy.preferred_location = processor_id;
    node->policy.preferred_nid = cpu_node_id;

    return &node->policy;
}

#endif // UVM_IS_CONFIG_HMM()
