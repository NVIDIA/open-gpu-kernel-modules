/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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

#include "uvm_linux.h"
#include "uvm_perf_events.h"
#include "uvm_perf_module.h"
#include "uvm_perf_prefetch.h"
#include "uvm_perf_utils.h"
#include "uvm_kvmalloc.h"
#include "uvm_va_block.h"
#include "uvm_va_range.h"
#include "uvm_test.h"

//
// Tunables for prefetch detection/prevention (configurable via module parameters)
//

// Enable/disable prefetch performance heuristics
static unsigned uvm_perf_prefetch_enable = 1;

// TODO: Bug 1778037: [uvm] Use adaptive threshold for page prefetching
#define UVM_PREFETCH_THRESHOLD_DEFAULT 51

// Percentage of children subregions that need to be resident in order to
// trigger prefetching of the remaining subregions
//
// Valid values 1-100
static unsigned uvm_perf_prefetch_threshold  = UVM_PREFETCH_THRESHOLD_DEFAULT;

#define UVM_PREFETCH_MIN_FAULTS_MIN     1
#define UVM_PREFETCH_MIN_FAULTS_DEFAULT 1
#define UVM_PREFETCH_MIN_FAULTS_MAX     20

// Minimum number of faults on a block in order to enable the prefetching
// logic
static unsigned uvm_perf_prefetch_min_faults = UVM_PREFETCH_MIN_FAULTS_DEFAULT;

// Module parameters for the tunables
module_param(uvm_perf_prefetch_enable, uint, S_IRUGO);
module_param(uvm_perf_prefetch_threshold, uint, S_IRUGO);
module_param(uvm_perf_prefetch_min_faults, uint, S_IRUGO);

static bool g_uvm_perf_prefetch_enable;
static unsigned g_uvm_perf_prefetch_threshold;
static unsigned g_uvm_perf_prefetch_min_faults;

void uvm_perf_prefetch_bitmap_tree_iter_init(const uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                             uvm_page_index_t page_index,
                                             uvm_perf_prefetch_bitmap_tree_iter_t *iter)
{
    UVM_ASSERT(bitmap_tree->level_count > 0);
    UVM_ASSERT_MSG(page_index < bitmap_tree->leaf_count,
                   "%zd vs %zd",
                   (size_t)page_index,
                   (size_t)bitmap_tree->leaf_count);

    iter->level_idx = bitmap_tree->level_count - 1;
    iter->node_idx  = page_index;
}

uvm_va_block_region_t uvm_perf_prefetch_bitmap_tree_iter_get_range(const uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                                                   const uvm_perf_prefetch_bitmap_tree_iter_t *iter)
{
    NvU16 range_leaves = uvm_perf_tree_iter_leaf_range(bitmap_tree, iter);
    NvU16 range_start = uvm_perf_tree_iter_leaf_range_start(bitmap_tree, iter);
    uvm_va_block_region_t subregion = uvm_va_block_region(range_start, range_start + range_leaves);

    UVM_ASSERT(iter->level_idx >= 0);
    UVM_ASSERT(iter->level_idx < bitmap_tree->level_count);

    return subregion;
}

NvU16 uvm_perf_prefetch_bitmap_tree_iter_get_count(const uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                                   const uvm_perf_prefetch_bitmap_tree_iter_t *iter)
{
    uvm_va_block_region_t subregion = uvm_perf_prefetch_bitmap_tree_iter_get_range(bitmap_tree, iter);

    return uvm_page_mask_region_weight(&bitmap_tree->pages, subregion);
}

static uvm_va_block_region_t compute_prefetch_region(uvm_page_index_t page_index,
                                                     uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                                     uvm_va_block_region_t max_prefetch_region)
{
    NvU16 counter;
    uvm_perf_prefetch_bitmap_tree_iter_t iter;
    uvm_va_block_region_t prefetch_region = uvm_va_block_region(0, 0);

    uvm_perf_prefetch_bitmap_tree_traverse_counters(counter,
                                                    bitmap_tree,
                                                    page_index - max_prefetch_region.first + bitmap_tree->offset,
                                                    &iter) {
        uvm_va_block_region_t subregion = uvm_perf_prefetch_bitmap_tree_iter_get_range(bitmap_tree, &iter);
        NvU16 subregion_pages = uvm_va_block_region_num_pages(subregion);

        UVM_ASSERT(counter <= subregion_pages);
        if (counter * 100 > subregion_pages * g_uvm_perf_prefetch_threshold)
            prefetch_region = subregion;
    }

    // Clamp prefetch region to actual pages
    if (prefetch_region.outer) {
        prefetch_region.first += max_prefetch_region.first;
        if (prefetch_region.first < bitmap_tree->offset) {
            prefetch_region.first = bitmap_tree->offset;
        }
        else {
            prefetch_region.first -= bitmap_tree->offset;
            if (prefetch_region.first < max_prefetch_region.first)
                prefetch_region.first = max_prefetch_region.first;
        }

        prefetch_region.outer += max_prefetch_region.first;
        if (prefetch_region.outer < bitmap_tree->offset) {
            prefetch_region.outer = bitmap_tree->offset;
        }
        else {
            prefetch_region.outer -= bitmap_tree->offset;
            if (prefetch_region.outer > max_prefetch_region.outer)
                prefetch_region.outer = max_prefetch_region.outer;
        }
    }

    return prefetch_region;
}

static void grow_fault_granularity_if_no_thrashing(uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                                   uvm_va_block_region_t region,
                                                   uvm_page_index_t first,
                                                   const uvm_page_mask_t *faulted_pages,
                                                   const uvm_page_mask_t *thrashing_pages)
{
    if (!uvm_page_mask_region_empty(faulted_pages, region) &&
        (!thrashing_pages || uvm_page_mask_region_empty(thrashing_pages, region))) {
        UVM_ASSERT(region.first >= first);
        region.first = region.first - first + bitmap_tree->offset;
        region.outer = region.outer - first + bitmap_tree->offset;
        UVM_ASSERT(region.outer <= bitmap_tree->leaf_count);
        uvm_page_mask_region_fill(&bitmap_tree->pages, region);
    }
}

static void grow_fault_granularity(uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                   NvU64 big_page_size,
                                   uvm_va_block_region_t big_pages_region,
                                   uvm_va_block_region_t max_prefetch_region,
                                   const uvm_page_mask_t *faulted_pages,
                                   const uvm_page_mask_t *thrashing_pages)
{
    uvm_page_index_t pages_per_big_page = big_page_size / PAGE_SIZE;
    uvm_page_index_t page_index;

    // Migrate whole block if no big pages and no page in it is thrashing
    if (!big_pages_region.outer) {
        grow_fault_granularity_if_no_thrashing(bitmap_tree,
                                               max_prefetch_region,
                                               max_prefetch_region.first,
                                               faulted_pages,
                                               thrashing_pages);
        return;
    }

    // Migrate whole "prefix" if no page in it is thrashing
    if (big_pages_region.first > max_prefetch_region.first) {
        uvm_va_block_region_t prefix_region = uvm_va_block_region(max_prefetch_region.first, big_pages_region.first);

        grow_fault_granularity_if_no_thrashing(bitmap_tree,
                                               prefix_region,
                                               max_prefetch_region.first,
                                               faulted_pages,
                                               thrashing_pages);
    }

    // Migrate whole big pages if they are not thrashing
    for (page_index = big_pages_region.first;
         page_index < big_pages_region.outer;
         page_index += pages_per_big_page) {
        uvm_va_block_region_t big_region = uvm_va_block_region(page_index,
                                                               page_index + pages_per_big_page);

        grow_fault_granularity_if_no_thrashing(bitmap_tree,
                                               big_region,
                                               max_prefetch_region.first,
                                               faulted_pages,
                                               thrashing_pages);
    }

    // Migrate whole "suffix" if no page in it is thrashing
    if (big_pages_region.outer < max_prefetch_region.outer) {
        uvm_va_block_region_t suffix_region = uvm_va_block_region(big_pages_region.outer,
                                                                  max_prefetch_region.outer);

        grow_fault_granularity_if_no_thrashing(bitmap_tree,
                                               suffix_region,
                                               max_prefetch_region.first,
                                               faulted_pages,
                                               thrashing_pages);
    }
}

static void init_bitmap_tree_from_region(uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                         uvm_va_block_region_t max_prefetch_region,
                                         const uvm_page_mask_t *resident_mask,
                                         const uvm_page_mask_t *faulted_pages)
{
    if (resident_mask)
        uvm_page_mask_or(&bitmap_tree->pages, resident_mask, faulted_pages);
    else
        uvm_page_mask_copy(&bitmap_tree->pages, faulted_pages);

    // If we are using a subregion of the va_block, align bitmap_tree
    uvm_page_mask_shift_right(&bitmap_tree->pages, &bitmap_tree->pages, max_prefetch_region.first);

    bitmap_tree->offset = 0;
    bitmap_tree->leaf_count = uvm_va_block_region_num_pages(max_prefetch_region);
    bitmap_tree->level_count = ilog2(roundup_pow_of_two(bitmap_tree->leaf_count)) + 1;
}

static void update_bitmap_tree_from_va_block(uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                             uvm_va_block_t *va_block,
                                             uvm_va_block_context_t *va_block_context,
                                             uvm_processor_id_t new_residency,
                                             const uvm_page_mask_t *faulted_pages,
                                             uvm_va_block_region_t max_prefetch_region)

{
    NvU64 big_page_size;
    uvm_va_block_region_t big_pages_region;
    uvm_va_space_t *va_space;
    const uvm_page_mask_t *thrashing_pages;

    UVM_ASSERT(va_block);
    UVM_ASSERT(va_block_context);

    va_space = uvm_va_block_get_va_space(va_block);

    // Get the big page size for the new residency.
    // Assume 64K size if the new residency is the CPU or no GPU va space is
    // registered in the current process for this GPU.
    if (UVM_ID_IS_GPU(new_residency) &&
        uvm_processor_mask_test(&va_space->registered_gpu_va_spaces, new_residency)) {
        uvm_gpu_t *gpu = uvm_gpu_get(new_residency);

        big_page_size = uvm_va_block_gpu_big_page_size(va_block, gpu);
    }
    else {
        big_page_size = UVM_PAGE_SIZE_64K;
    }

    big_pages_region = uvm_va_block_big_page_region_subset(va_block, max_prefetch_region, big_page_size);

    // Adjust the prefetch tree to big page granularity to make sure that we
    // get big page-friendly prefetching hints
    if (big_pages_region.first - max_prefetch_region.first > 0) {
        bitmap_tree->offset = big_page_size / PAGE_SIZE - (big_pages_region.first - max_prefetch_region.first);
        bitmap_tree->leaf_count = uvm_va_block_region_num_pages(max_prefetch_region) + bitmap_tree->offset;

        UVM_ASSERT(bitmap_tree->offset < big_page_size / PAGE_SIZE);
        UVM_ASSERT(bitmap_tree->leaf_count <= PAGES_PER_UVM_VA_BLOCK);

        uvm_page_mask_shift_left(&bitmap_tree->pages, &bitmap_tree->pages, bitmap_tree->offset);

        bitmap_tree->level_count = ilog2(roundup_pow_of_two(bitmap_tree->leaf_count)) + 1;
    }

    thrashing_pages = uvm_perf_thrashing_get_thrashing_pages(va_block);

    // Assume big pages by default. Prefetch the rest of 4KB subregions within
    // the big page region unless there is thrashing.
    grow_fault_granularity(bitmap_tree,
                           big_page_size,
                           big_pages_region,
                           max_prefetch_region,
                           faulted_pages,
                           thrashing_pages);
}

static void compute_prefetch_mask(uvm_va_block_region_t faulted_region,
                                  uvm_va_block_region_t max_prefetch_region,
                                  uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                  const uvm_page_mask_t *faulted_pages,
                                  uvm_page_mask_t *out_prefetch_mask)
{
    uvm_page_index_t page_index;

    uvm_page_mask_zero(out_prefetch_mask);

    // Update the tree using the faulted mask to compute the pages to prefetch.
    for_each_va_block_page_in_region_mask(page_index, faulted_pages, faulted_region) {
        uvm_va_block_region_t region = compute_prefetch_region(page_index, bitmap_tree, max_prefetch_region);

        uvm_page_mask_region_fill(out_prefetch_mask, region);

        // Early out if we have already prefetched until the end of the VA block
        if (region.outer == max_prefetch_region.outer)
            break;
    }
}

// Within a block we only allow prefetching to a single processor. Therefore,
// if two processors are accessing non-overlapping regions within the same
// block they won't benefit from prefetching.
//
// TODO: Bug 1778034: [uvm] Explore prefetching to different processors within
// a VA block.
static NvU32 uvm_perf_prefetch_prenotify_fault_migrations(uvm_va_block_t *va_block,
                                                          uvm_va_block_context_t *va_block_context,
                                                          uvm_processor_id_t new_residency,
                                                          const uvm_page_mask_t *faulted_pages,
                                                          uvm_va_block_region_t faulted_region,
                                                          uvm_page_mask_t *prefetch_pages,
                                                          uvm_perf_prefetch_bitmap_tree_t *bitmap_tree)
{
    const uvm_page_mask_t *resident_mask = NULL;
    const uvm_va_policy_t *policy = uvm_va_policy_get_region(va_block, faulted_region);
    uvm_va_block_region_t max_prefetch_region;
    const uvm_page_mask_t *thrashing_pages = uvm_perf_thrashing_get_thrashing_pages(va_block);

    if (!uvm_id_equal(va_block->prefetch_info.last_migration_proc_id, new_residency)) {
        va_block->prefetch_info.last_migration_proc_id = new_residency;
        va_block->prefetch_info.fault_migrations_to_last_proc = 0;
    }

    // Compute the expanded region that prefetching is allowed from.
    if (uvm_va_block_is_hmm(va_block)) {
        max_prefetch_region = uvm_hmm_get_prefetch_region(va_block,
                                                          va_block_context->hmm.vma,
                                                          policy,
                                                          uvm_va_block_region_start(va_block, faulted_region));
    }
    else {
        max_prefetch_region = uvm_va_block_region_from_block(va_block);
    }

    uvm_page_mask_zero(prefetch_pages);

    if (UVM_ID_IS_CPU(new_residency) || va_block->gpus[uvm_id_gpu_index(new_residency)] != NULL)
        resident_mask = uvm_va_block_resident_mask_get(va_block, new_residency, NUMA_NO_NODE);

    // If this is a first-touch fault and the destination processor is the
    // preferred location, populate the whole max_prefetch_region.
    if (uvm_processor_mask_empty(&va_block->resident) &&
        uvm_id_equal(new_residency, policy->preferred_location)) {
        uvm_page_mask_region_fill(prefetch_pages, max_prefetch_region);
    }
    else {
        init_bitmap_tree_from_region(bitmap_tree, max_prefetch_region, resident_mask, faulted_pages);

        update_bitmap_tree_from_va_block(bitmap_tree,
                                         va_block,
                                         va_block_context,
                                         new_residency,
                                         faulted_pages,
                                         max_prefetch_region);

        // Do not compute prefetch regions with faults on pages that are thrashing
        if (thrashing_pages)
            uvm_page_mask_andnot(&va_block_context->scratch_page_mask, faulted_pages, thrashing_pages);
        else
            uvm_page_mask_copy(&va_block_context->scratch_page_mask, faulted_pages);

        compute_prefetch_mask(faulted_region,
                              max_prefetch_region,
                              bitmap_tree,
                              &va_block_context->scratch_page_mask,
                              prefetch_pages);
    }

    // Do not prefetch pages that are going to be migrated/populated due to a
    // fault
    uvm_page_mask_andnot(prefetch_pages, prefetch_pages, faulted_pages);

    // TODO: Bug 1765432: prefetching pages that are already mapped on the CPU
    // would trigger a remap, which may cause a large overhead. Therefore,
    // exclude them from the mask.
    // For HMM, we don't know what pages are mapped by the CPU unless we try to
    // migrate them. Prefetch pages will only be opportunistically migrated.
    if (UVM_ID_IS_CPU(new_residency) && !uvm_va_block_is_hmm(va_block)) {
        uvm_page_mask_and(&va_block_context->scratch_page_mask,
                          resident_mask,
                          &va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ]);
        uvm_page_mask_andnot(prefetch_pages, prefetch_pages, &va_block_context->scratch_page_mask);
    }

    // Avoid prefetching pages that are thrashing
    if (thrashing_pages)
        uvm_page_mask_andnot(prefetch_pages, prefetch_pages, thrashing_pages);

    va_block->prefetch_info.fault_migrations_to_last_proc += uvm_page_mask_region_weight(faulted_pages, faulted_region);

    return uvm_page_mask_weight(prefetch_pages);
}

bool uvm_perf_prefetch_enabled(uvm_va_space_t *va_space)
{
    if (!g_uvm_perf_prefetch_enable)
        return false;

    UVM_ASSERT(va_space);

    return va_space->test.page_prefetch_enabled;
}

void uvm_perf_prefetch_compute_ats(uvm_va_space_t *va_space,
                                   const uvm_page_mask_t *faulted_pages,
                                   uvm_va_block_region_t faulted_region,
                                   uvm_va_block_region_t max_prefetch_region,
                                   const uvm_page_mask_t *residency_mask,
                                   uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                   uvm_page_mask_t *out_prefetch_mask)
{
    UVM_ASSERT(faulted_pages);
    UVM_ASSERT(bitmap_tree);
    UVM_ASSERT(out_prefetch_mask);

    uvm_page_mask_zero(out_prefetch_mask);

    if (!uvm_perf_prefetch_enabled(va_space))
        return;

    init_bitmap_tree_from_region(bitmap_tree, max_prefetch_region, residency_mask, faulted_pages);

    compute_prefetch_mask(faulted_region, max_prefetch_region, bitmap_tree, faulted_pages, out_prefetch_mask);
}

void uvm_perf_prefetch_get_hint_va_block(uvm_va_block_t *va_block,
                                         uvm_va_block_context_t *va_block_context,
                                         uvm_processor_id_t new_residency,
                                         const uvm_page_mask_t *faulted_pages,
                                         uvm_va_block_region_t faulted_region,
                                         uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                         uvm_perf_prefetch_hint_t *out_hint)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_page_mask_t *prefetch_pages = &out_hint->prefetch_pages_mask;
    NvU32 pending_prefetch_pages;

    uvm_assert_rwsem_locked(&va_space->lock);
    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(uvm_hmm_check_context_vma_is_valid(va_block, va_block_context->hmm.vma, faulted_region));

    out_hint->residency = UVM_ID_INVALID;
    uvm_page_mask_zero(prefetch_pages);

    if (!uvm_perf_prefetch_enabled(va_space))
        return;

    pending_prefetch_pages = uvm_perf_prefetch_prenotify_fault_migrations(va_block,
                                                                          va_block_context,
                                                                          new_residency,
                                                                          faulted_pages,
                                                                          faulted_region,
                                                                          prefetch_pages,
                                                                          bitmap_tree);

    if (va_block->prefetch_info.fault_migrations_to_last_proc >= g_uvm_perf_prefetch_min_faults &&
        pending_prefetch_pages > 0) {
        bool changed = false;
        uvm_range_group_range_t *rgr;

        // Only prefetch in range group ranges which have pages that need to
        // move.
        uvm_range_group_for_each_range_in(rgr, va_space, va_block->start, va_block->end) {
            uvm_va_block_region_t region = uvm_va_block_region_from_start_end(va_block,
                                                                              max(rgr->node.start, va_block->start),
                                                                              min(rgr->node.end, va_block->end));

            if (uvm_page_mask_region_empty(faulted_pages, region) &&
                !uvm_page_mask_region_empty(prefetch_pages, region)) {
                uvm_page_mask_region_clear(prefetch_pages, region);
                changed = true;
            }
        }

        if (changed)
            pending_prefetch_pages = uvm_page_mask_weight(prefetch_pages);

        if (pending_prefetch_pages > 0)
            out_hint->residency = va_block->prefetch_info.last_migration_proc_id;
    }
}

NV_STATUS uvm_perf_prefetch_init(void)
{
    g_uvm_perf_prefetch_enable = uvm_perf_prefetch_enable != 0;

    if (!g_uvm_perf_prefetch_enable)
        return NV_OK;

    if (uvm_perf_prefetch_threshold <= 100) {
        g_uvm_perf_prefetch_threshold = uvm_perf_prefetch_threshold;
    }
    else {
        UVM_INFO_PRINT("Invalid value %u for uvm_perf_prefetch_threshold. Using %u instead\n",
                       uvm_perf_prefetch_threshold,
                       UVM_PREFETCH_THRESHOLD_DEFAULT);

        g_uvm_perf_prefetch_threshold = UVM_PREFETCH_THRESHOLD_DEFAULT;
    }

    if (uvm_perf_prefetch_min_faults >= UVM_PREFETCH_MIN_FAULTS_MIN &&
        uvm_perf_prefetch_min_faults <= UVM_PREFETCH_MIN_FAULTS_MAX) {
        g_uvm_perf_prefetch_min_faults = uvm_perf_prefetch_min_faults;
    }
    else {
        UVM_INFO_PRINT("Invalid value %u for uvm_perf_prefetch_min_faults. Using %u instead\n",
                       uvm_perf_prefetch_min_faults,
                       UVM_PREFETCH_MIN_FAULTS_DEFAULT);

        g_uvm_perf_prefetch_min_faults = UVM_PREFETCH_MIN_FAULTS_DEFAULT;
    }

    return NV_OK;
}

NV_STATUS uvm_test_set_page_prefetch_policy(UVM_TEST_SET_PAGE_PREFETCH_POLICY_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    if (params->policy >= UVM_TEST_PAGE_PREFETCH_POLICY_MAX)
        return NV_ERR_INVALID_ARGUMENT;

    uvm_va_space_down_write(va_space);

    if (params->policy == UVM_TEST_PAGE_PREFETCH_POLICY_ENABLE)
        va_space->test.page_prefetch_enabled = true;
    else
        va_space->test.page_prefetch_enabled = false;

    uvm_va_space_up_write(va_space);

    return NV_OK;
}
