/*******************************************************************************
    Copyright (c) 2017-2024 NVIDIA Corporation

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

#include "uvm_gpu.h"
#include "uvm_pmm_sysmem.h"
#include "uvm_kvmalloc.h"
#include "uvm_va_block.h"
#include "uvm_va_space.h"

static int uvm_cpu_chunk_allocation_sizes = UVM_CPU_CHUNK_SIZES;
module_param(uvm_cpu_chunk_allocation_sizes, uint, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(uvm_cpu_chunk_allocation_sizes, "OR'ed value of all CPU chunk allocation sizes.");

static struct kmem_cache *g_reverse_page_map_cache __read_mostly;

NV_STATUS uvm_pmm_sysmem_init(void)
{
    g_reverse_page_map_cache = NV_KMEM_CACHE_CREATE("uvm_pmm_sysmem_page_reverse_map_t",
                                                    uvm_reverse_map_t);
    if (!g_reverse_page_map_cache)
        return NV_ERR_NO_MEMORY;

    // Ensure that only supported CPU chunk sizes are enabled.
    uvm_cpu_chunk_allocation_sizes &= UVM_CPU_CHUNK_SIZES;
    if (!uvm_cpu_chunk_allocation_sizes || !(uvm_cpu_chunk_allocation_sizes & PAGE_SIZE)) {
        pr_info("Invalid value for uvm_cpu_chunk_allocation_sizes = 0x%x, using 0x%llx instead\n",
                uvm_cpu_chunk_allocation_sizes,
                UVM_CPU_CHUNK_SIZES);
        uvm_cpu_chunk_allocation_sizes = UVM_CPU_CHUNK_SIZES;
    }

    return NV_OK;
}

void uvm_pmm_sysmem_exit(void)
{
    kmem_cache_destroy_safe(&g_reverse_page_map_cache);
}

NV_STATUS uvm_pmm_sysmem_mappings_init(uvm_gpu_t *gpu, uvm_pmm_sysmem_mappings_t *sysmem_mappings)
{
    memset(sysmem_mappings, 0, sizeof(*sysmem_mappings));

    sysmem_mappings->gpu = gpu;

    uvm_mutex_init(&sysmem_mappings->reverse_map_lock, UVM_LOCK_ORDER_LEAF);
    uvm_init_radix_tree_preloadable(&sysmem_mappings->reverse_map_tree);

    return NV_OK;
}

void uvm_pmm_sysmem_mappings_deinit(uvm_pmm_sysmem_mappings_t *sysmem_mappings)
{
    if (sysmem_mappings->gpu) {
        UVM_ASSERT_MSG(radix_tree_empty(&sysmem_mappings->reverse_map_tree),
                       "radix_tree not empty for GPU %s\n",
                       uvm_gpu_name(sysmem_mappings->gpu));
    }

    sysmem_mappings->gpu = NULL;
}

// TODO: Bug 1995015: use a more efficient data structure for
// physically-contiguous allocations.
NV_STATUS uvm_pmm_sysmem_mappings_add_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                  NvU64 dma_addr,
                                                  NvU64 virt_addr,
                                                  NvU64 region_size,
                                                  uvm_va_block_t *va_block,
                                                  uvm_processor_id_t owner)
{
    NV_STATUS status = NV_OK;
    uvm_reverse_map_t *new_reverse_map;
    NvU64 key;
    const NvU64 base_key = dma_addr / PAGE_SIZE;
    const NvU32 num_pages = region_size / PAGE_SIZE;
    uvm_page_index_t page_index;

    UVM_ASSERT(va_block);
    UVM_ASSERT(!uvm_va_block_is_dead(va_block));
    UVM_ASSERT(IS_ALIGNED(dma_addr, region_size));
    UVM_ASSERT(IS_ALIGNED(virt_addr, region_size));
    UVM_ASSERT(region_size <= UVM_VA_BLOCK_SIZE);
    UVM_ASSERT(is_power_of_2(region_size));
    UVM_ASSERT(uvm_va_block_contains_address(va_block, virt_addr));
    UVM_ASSERT(uvm_va_block_contains_address(va_block, virt_addr + region_size - 1));
    uvm_assert_mutex_locked(&va_block->lock);

    if (!sysmem_mappings->gpu->parent->access_counters_can_use_physical_addresses)
        return NV_OK;

    new_reverse_map = nv_kmem_cache_zalloc(g_reverse_page_map_cache, NV_UVM_GFP_FLAGS);
    if (!new_reverse_map)
        return NV_ERR_NO_MEMORY;

    page_index = uvm_va_block_cpu_page_index(va_block, virt_addr);

    new_reverse_map->va_block = va_block;
    new_reverse_map->region   = uvm_va_block_region(page_index, page_index + num_pages);
    new_reverse_map->owner    = owner;

    uvm_mutex_lock(&sysmem_mappings->reverse_map_lock);
    for (key = base_key; key < base_key + num_pages; ++key) {
        int ret = radix_tree_insert(&sysmem_mappings->reverse_map_tree, key, new_reverse_map);
        if (ret != 0) {
            NvU64 remove_key;

            for (remove_key = base_key; remove_key < key; ++remove_key)
                (void *)radix_tree_delete(&sysmem_mappings->reverse_map_tree, remove_key);

            kmem_cache_free(g_reverse_page_map_cache, new_reverse_map);
            status = errno_to_nv_status(ret);
            break;
        }
    }
    uvm_mutex_unlock(&sysmem_mappings->reverse_map_lock);

    // The assert is added for Coverity's sake. It is equivalent to adding
    // assert(num_pages > 0) before the loop. However, Coverity is not able to
    // deduce that the loop has to execute at least once from num_pages > 0.
    UVM_ASSERT(key != base_key || status != NV_OK);

    return status;
}

static void pmm_sysmem_mappings_remove_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                   NvU64 dma_addr,
                                                   bool check_mapping)
{
    uvm_reverse_map_t *reverse_map;
    NvU64 key;
    const NvU64 base_key = dma_addr / PAGE_SIZE;

    if (!sysmem_mappings->gpu->parent->access_counters_can_use_physical_addresses)
        return;

    uvm_mutex_lock(&sysmem_mappings->reverse_map_lock);

    reverse_map = radix_tree_delete(&sysmem_mappings->reverse_map_tree, base_key);
    if (check_mapping)
        UVM_ASSERT(reverse_map);

    if (!reverse_map) {
        uvm_mutex_unlock(&sysmem_mappings->reverse_map_lock);
        return;
    }

    uvm_assert_mutex_locked(&reverse_map->va_block->lock);

    for (key = base_key + 1; key < base_key + uvm_va_block_region_num_pages(reverse_map->region); ++key) {
        uvm_reverse_map_t *curr_reverse_map = radix_tree_delete(&sysmem_mappings->reverse_map_tree, key);
        UVM_ASSERT(curr_reverse_map == reverse_map);
    }

    uvm_mutex_unlock(&sysmem_mappings->reverse_map_lock);

    kmem_cache_free(g_reverse_page_map_cache, reverse_map);
}

void uvm_pmm_sysmem_mappings_remove_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings, NvU64 dma_addr)
{
    pmm_sysmem_mappings_remove_gpu_mapping(sysmem_mappings, dma_addr, true);
}

void uvm_pmm_sysmem_mappings_remove_gpu_mapping_on_eviction(uvm_pmm_sysmem_mappings_t *sysmem_mappings, NvU64 dma_addr)
{
    pmm_sysmem_mappings_remove_gpu_mapping(sysmem_mappings, dma_addr, false);
}

void uvm_pmm_sysmem_mappings_reparent_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                  NvU64 dma_addr,
                                                  uvm_va_block_t *va_block)
{
    NvU64 virt_addr;
    uvm_reverse_map_t *reverse_map;
    const NvU64 base_key = dma_addr / PAGE_SIZE;
    uvm_page_index_t new_start_page;

    UVM_ASSERT(PAGE_ALIGNED(dma_addr));
    UVM_ASSERT(va_block);
    UVM_ASSERT(!uvm_va_block_is_dead(va_block));

    if (!sysmem_mappings->gpu->parent->access_counters_can_use_physical_addresses)
        return;

    uvm_mutex_lock(&sysmem_mappings->reverse_map_lock);

    reverse_map = radix_tree_lookup(&sysmem_mappings->reverse_map_tree, base_key);
    UVM_ASSERT(reverse_map);

    // Compute virt address by hand since the old VA block may be messed up
    // during split
    virt_addr = reverse_map->va_block->start + reverse_map->region.first * PAGE_SIZE;
    new_start_page = uvm_va_block_cpu_page_index(va_block, virt_addr);

    reverse_map->region   = uvm_va_block_region(new_start_page,
                                                new_start_page + uvm_va_block_region_num_pages(reverse_map->region));
    reverse_map->va_block = va_block;

    UVM_ASSERT(uvm_va_block_contains_address(va_block, uvm_reverse_map_start(reverse_map)));
    UVM_ASSERT(uvm_va_block_contains_address(va_block, uvm_reverse_map_end(reverse_map)));

    uvm_mutex_unlock(&sysmem_mappings->reverse_map_lock);
}

NV_STATUS uvm_pmm_sysmem_mappings_split_gpu_mappings(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                     NvU64 dma_addr,
                                                     NvU64 new_region_size)
{
    uvm_reverse_map_t *orig_reverse_map;
    const NvU64 base_key = dma_addr / PAGE_SIZE;
    const size_t num_pages = new_region_size / PAGE_SIZE;
    size_t old_num_pages;
    size_t subregion, num_subregions;
    uvm_reverse_map_t **new_reverse_maps;

    UVM_ASSERT(IS_ALIGNED(dma_addr, new_region_size));
    UVM_ASSERT(new_region_size <= UVM_VA_BLOCK_SIZE);
    UVM_ASSERT(is_power_of_2(new_region_size));

    if (!sysmem_mappings->gpu->parent->access_counters_can_use_physical_addresses)
        return NV_OK;

    uvm_mutex_lock(&sysmem_mappings->reverse_map_lock);
    orig_reverse_map = radix_tree_lookup(&sysmem_mappings->reverse_map_tree, base_key);
    uvm_mutex_unlock(&sysmem_mappings->reverse_map_lock);

    // We can access orig_reverse_map outside the tree lock because we hold the
    // VA block lock so we cannot have concurrent modifications in the tree for
    // the mappings of the chunks that belong to that VA block.
    UVM_ASSERT(orig_reverse_map);
    UVM_ASSERT(orig_reverse_map->va_block);
    uvm_assert_mutex_locked(&orig_reverse_map->va_block->lock);
    old_num_pages = uvm_va_block_region_num_pages(orig_reverse_map->region);
    UVM_ASSERT(num_pages < old_num_pages);

    num_subregions = old_num_pages / num_pages;

    new_reverse_maps = uvm_kvmalloc_zero(sizeof(*new_reverse_maps) * (num_subregions - 1));
    if (!new_reverse_maps)
        return NV_ERR_NO_MEMORY;

    // Allocate the descriptors for the new subregions
    for (subregion = 1; subregion < num_subregions; ++subregion) {
        uvm_reverse_map_t *new_reverse_map = nv_kmem_cache_zalloc(g_reverse_page_map_cache, NV_UVM_GFP_FLAGS);
        uvm_page_index_t page_index = orig_reverse_map->region.first + num_pages * subregion;

        if (new_reverse_map == NULL) {
            // On error, free the previously-created descriptors
            while (--subregion != 0)
                kmem_cache_free(g_reverse_page_map_cache, new_reverse_maps[subregion - 1]);

            uvm_kvfree(new_reverse_maps);
            return NV_ERR_NO_MEMORY;
        }

        new_reverse_map->va_block = orig_reverse_map->va_block;
        new_reverse_map->region   = uvm_va_block_region(page_index, page_index + num_pages);
        new_reverse_map->owner    = orig_reverse_map->owner;

        new_reverse_maps[subregion - 1] = new_reverse_map;
    }

    uvm_mutex_lock(&sysmem_mappings->reverse_map_lock);

    for (subregion = 1; subregion < num_subregions; ++subregion) {
        NvU64 key;

        for (key = base_key + num_pages * subregion; key < base_key + num_pages * (subregion + 1); ++key) {
            void **slot = radix_tree_lookup_slot(&sysmem_mappings->reverse_map_tree, key);
            UVM_ASSERT(slot);
            UVM_ASSERT(radix_tree_deref_slot(slot) == orig_reverse_map);

            NV_RADIX_TREE_REPLACE_SLOT(&sysmem_mappings->reverse_map_tree, slot, new_reverse_maps[subregion - 1]);
        }
    }

    orig_reverse_map->region = uvm_va_block_region(orig_reverse_map->region.first,
                                                   orig_reverse_map->region.first + num_pages);

    uvm_mutex_unlock(&sysmem_mappings->reverse_map_lock);

    uvm_kvfree(new_reverse_maps);
    return NV_OK;
}

void uvm_pmm_sysmem_mappings_merge_gpu_mappings(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                NvU64 dma_addr,
                                                NvU64 new_region_size)
{
    uvm_reverse_map_t *first_reverse_map;
    uvm_page_index_t running_page_index;
    NvU64 key;
    const NvU64 base_key = dma_addr / PAGE_SIZE;
    const size_t num_pages = new_region_size / PAGE_SIZE;
    size_t num_mapping_pages;

    UVM_ASSERT(IS_ALIGNED(dma_addr, new_region_size));
    UVM_ASSERT(new_region_size <= UVM_VA_BLOCK_SIZE);
    UVM_ASSERT(is_power_of_2(new_region_size));

    if (!sysmem_mappings->gpu->parent->access_counters_can_use_physical_addresses)
        return;

    uvm_mutex_lock(&sysmem_mappings->reverse_map_lock);

    // Find the first mapping in the region
    first_reverse_map = radix_tree_lookup(&sysmem_mappings->reverse_map_tree, base_key);
    UVM_ASSERT(first_reverse_map);
    num_mapping_pages = uvm_va_block_region_num_pages(first_reverse_map->region);
    UVM_ASSERT(num_pages >= num_mapping_pages);
    UVM_ASSERT(IS_ALIGNED(base_key, num_mapping_pages));

    // The region in the tree matches the size of the merged region, just return
    if (num_pages == num_mapping_pages)
        goto unlock_no_update;

    // Otherwise update the rest of slots to point at the same reverse map
    // descriptor
    key = base_key + uvm_va_block_region_num_pages(first_reverse_map->region);
    running_page_index = first_reverse_map->region.outer;
    while (key < base_key + num_pages) {
        uvm_reverse_map_t *reverse_map = NULL;
        void **slot = radix_tree_lookup_slot(&sysmem_mappings->reverse_map_tree, key);
        size_t slot_index;
        UVM_ASSERT(slot);

        reverse_map = radix_tree_deref_slot(slot);
        UVM_ASSERT(reverse_map);
        UVM_ASSERT(reverse_map != first_reverse_map);
        UVM_ASSERT(reverse_map->va_block == first_reverse_map->va_block);
        UVM_ASSERT(uvm_id_equal(reverse_map->owner, first_reverse_map->owner));
        UVM_ASSERT(reverse_map->region.first == running_page_index);

        NV_RADIX_TREE_REPLACE_SLOT(&sysmem_mappings->reverse_map_tree, slot, first_reverse_map);

        num_mapping_pages = uvm_va_block_region_num_pages(reverse_map->region);
        UVM_ASSERT(IS_ALIGNED(key, num_mapping_pages));
        UVM_ASSERT(key + num_mapping_pages <= base_key + num_pages);

        for (slot_index = 1; slot_index < num_mapping_pages; ++slot_index) {
            slot = radix_tree_lookup_slot(&sysmem_mappings->reverse_map_tree, key + slot_index);
            UVM_ASSERT(slot);
            UVM_ASSERT(reverse_map == radix_tree_deref_slot(slot));

            NV_RADIX_TREE_REPLACE_SLOT(&sysmem_mappings->reverse_map_tree, slot, first_reverse_map);
        }

        key += num_mapping_pages;
        running_page_index = reverse_map->region.outer;

        kmem_cache_free(g_reverse_page_map_cache, reverse_map);
    }

    // Grow the first mapping to cover the whole region
    first_reverse_map->region.outer = first_reverse_map->region.first + num_pages;

unlock_no_update:
    uvm_mutex_unlock(&sysmem_mappings->reverse_map_lock);
}

size_t uvm_pmm_sysmem_mappings_dma_to_virt(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                           NvU64 dma_addr,
                                           NvU64 region_size,
                                           uvm_reverse_map_t *out_mappings,
                                           size_t max_out_mappings)
{
    NvU64 key;
    size_t num_mappings = 0;
    const NvU64 base_key = dma_addr / PAGE_SIZE;
    NvU32 num_pages = region_size / PAGE_SIZE;

    UVM_ASSERT(region_size >= PAGE_SIZE);
    UVM_ASSERT(PAGE_ALIGNED(region_size));
    UVM_ASSERT(sysmem_mappings->gpu->parent->access_counters_can_use_physical_addresses);
    UVM_ASSERT(max_out_mappings > 0);

    uvm_mutex_lock(&sysmem_mappings->reverse_map_lock);

    key = base_key;
    do {
        uvm_reverse_map_t *reverse_map = radix_tree_lookup(&sysmem_mappings->reverse_map_tree, key);

        if (reverse_map) {
            size_t num_chunk_pages = uvm_va_block_region_num_pages(reverse_map->region);
            NvU32 page_offset = key & (num_chunk_pages - 1);
            NvU32 num_mapping_pages = min(num_pages, (NvU32)num_chunk_pages - page_offset);

            // Sysmem mappings are removed during VA block destruction.
            // Therefore, we can safely retain the VA blocks as long as they
            // are in the reverse map and we hold the reverse map lock.
            uvm_va_block_retain(reverse_map->va_block);
            out_mappings[num_mappings]               = *reverse_map;
            out_mappings[num_mappings].region.first += page_offset;
            out_mappings[num_mappings].region.outer  = out_mappings[num_mappings].region.first + num_mapping_pages;

            if (++num_mappings == max_out_mappings)
                break;

            num_pages -= num_mapping_pages;
            key       += num_mapping_pages;
        }
        else {
            --num_pages;
            ++key;
        }
    }
    while (num_pages > 0);

    uvm_mutex_unlock(&sysmem_mappings->reverse_map_lock);

    return num_mappings;
}

uvm_chunk_sizes_mask_t uvm_cpu_chunk_get_allocation_sizes(void)
{
        return uvm_cpu_chunk_allocation_sizes & UVM_CPU_CHUNK_SIZES;
}

static void uvm_cpu_chunk_set_size(uvm_cpu_chunk_t *chunk, uvm_chunk_size_t size)
{
    chunk->log2_size = ilog2(size);
}

uvm_chunk_size_t uvm_cpu_chunk_get_size(uvm_cpu_chunk_t *chunk)
{
    return ((uvm_chunk_size_t)1) << chunk->log2_size;
}

static NvU32 compute_gpu_mappings_entry_index(uvm_parent_processor_mask_t *dma_addrs_mask, uvm_parent_gpu_id_t id)
{
    uvm_parent_processor_mask_t subset_mask;

    // Compute the array index for the given GPU ID by masking off all bits
    // above and including the id and then counting the number of bits
    // remaining.
    uvm_parent_processor_mask_zero(&subset_mask);
    bitmap_set(subset_mask.bitmap, UVM_PARENT_ID_GPU0_VALUE, uvm_parent_id_gpu_index(id));
    uvm_parent_processor_mask_and(&subset_mask, dma_addrs_mask, &subset_mask);

    return uvm_parent_processor_mask_get_gpu_count(&subset_mask);
}

static uvm_cpu_physical_chunk_t *get_physical_parent(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);
    UVM_ASSERT(chunk->page);

    while (uvm_cpu_chunk_is_logical(chunk))
        chunk = uvm_cpu_chunk_to_logical(chunk)->parent;

    return uvm_cpu_chunk_to_physical(chunk);
}

static uvm_page_index_t cpu_chunk_get_phys_index(uvm_cpu_logical_chunk_t *chunk)
{
    uvm_cpu_physical_chunk_t *phys_chunk = get_physical_parent(&chunk->common);

    UVM_ASSERT(phys_chunk->common.page);
    return (uvm_page_index_t)(chunk->common.page - phys_chunk->common.page);
}

static uvm_cpu_phys_mapping_t *chunk_phys_mapping_alloc(uvm_cpu_physical_chunk_t *chunk, uvm_parent_gpu_id_t id)
{
    NvU32 num_active_entries = uvm_parent_processor_mask_get_gpu_count(&chunk->gpu_mappings.dma_addrs_mask);
    uvm_cpu_phys_mapping_t *new_entries;
    NvU32 array_index;

    uvm_assert_mutex_locked(&chunk->lock);

    if (chunk->gpu_mappings.max_entries == 1 && num_active_entries == 0)
        return &chunk->gpu_mappings.static_entry;

    if (num_active_entries == chunk->gpu_mappings.max_entries) {
        NvU32 num_new_entries = chunk->gpu_mappings.max_entries * 2;

        if (chunk->gpu_mappings.max_entries == 1) {
            new_entries = uvm_kvmalloc(sizeof(*new_entries) * num_new_entries);
            if (new_entries)
                new_entries[0] = chunk->gpu_mappings.static_entry;
        }
        else {
            new_entries = uvm_kvrealloc(chunk->gpu_mappings.dynamic_entries,
                                        sizeof(*new_entries) * num_new_entries);
        }

        if (!new_entries)
            return NULL;

        chunk->gpu_mappings.max_entries = num_new_entries;
        chunk->gpu_mappings.dynamic_entries = new_entries;
    }

    array_index = compute_gpu_mappings_entry_index(&chunk->gpu_mappings.dma_addrs_mask, id);
    while (num_active_entries > array_index) {
        chunk->gpu_mappings.dynamic_entries[num_active_entries] =
            chunk->gpu_mappings.dynamic_entries[num_active_entries - 1];
        num_active_entries--;
    }

    return &chunk->gpu_mappings.dynamic_entries[array_index];
}

static uvm_cpu_phys_mapping_t *chunk_phys_mapping_get(uvm_cpu_physical_chunk_t *chunk, uvm_parent_gpu_id_t id)
{
    uvm_assert_mutex_locked(&chunk->lock);

    if (uvm_parent_processor_mask_test(&chunk->gpu_mappings.dma_addrs_mask, id)) {
        if (chunk->gpu_mappings.max_entries == 1) {
            return &chunk->gpu_mappings.static_entry;
        }
        else {
            NvU32 array_index = compute_gpu_mappings_entry_index(&chunk->gpu_mappings.dma_addrs_mask, id);
            return &chunk->gpu_mappings.dynamic_entries[array_index];
        }
    }

    return NULL;
}

static void chunk_inc_gpu_mapping(uvm_cpu_physical_chunk_t *chunk, uvm_parent_gpu_id_t id)
{
    uvm_cpu_phys_mapping_t *mapping;

    mapping = chunk_phys_mapping_get(chunk, id);
    UVM_ASSERT(mapping);
    mapping->map_count++;
}

static void chunk_dec_gpu_mapping(uvm_cpu_physical_chunk_t *chunk, uvm_parent_gpu_id_t id)
{
    uvm_cpu_phys_mapping_t *mapping;

    mapping = chunk_phys_mapping_get(chunk, id);
    UVM_ASSERT(mapping);
    UVM_ASSERT(mapping->dma_addr && mapping->map_count);
    mapping->map_count--;
    if (mapping->map_count == 0) {
        uvm_parent_gpu_t *parent_gpu = uvm_parent_gpu_get(id);

        UVM_ASSERT(uvm_sub_processor_mask_empty(&mapping->sub_processors));

        uvm_parent_gpu_unmap_cpu_pages(parent_gpu, mapping->dma_addr, uvm_cpu_chunk_get_size(&chunk->common));
        mapping->dma_addr = 0;
        if (chunk->gpu_mappings.max_entries > 1) {
            NvU32 num_active_entries = uvm_parent_processor_mask_get_gpu_count(&chunk->gpu_mappings.dma_addrs_mask);
            NvU32 array_index = compute_gpu_mappings_entry_index(&chunk->gpu_mappings.dma_addrs_mask, id);

            // Shift any GPU mappings above this one down in the mappings array.
            for (; array_index < num_active_entries - 1; array_index++)
                chunk->gpu_mappings.dynamic_entries[array_index] = chunk->gpu_mappings.dynamic_entries[array_index+1];
        }

        uvm_parent_processor_mask_clear(&chunk->gpu_mappings.dma_addrs_mask, id);
    }
}

NvU64 uvm_cpu_chunk_get_gpu_phys_addr(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu)
{
    uvm_cpu_physical_chunk_t *phys_chunk = get_physical_parent(chunk);
    uvm_cpu_phys_mapping_t *mapping;
    uvm_page_index_t parent_offset = 0;
    NvU64 dma_addr = 0;

    if (uvm_cpu_chunk_is_logical(chunk)) {
        uvm_cpu_logical_chunk_t *logical_chunk = uvm_cpu_chunk_to_logical(chunk);

        if (!uvm_processor_mask_test(&logical_chunk->mapped_gpus, gpu->id))
            return 0;

        parent_offset = cpu_chunk_get_phys_index(logical_chunk);
    }

    uvm_mutex_lock(&phys_chunk->lock);
    mapping = chunk_phys_mapping_get(phys_chunk, gpu->parent->id);
    if (mapping &&
        (uvm_cpu_chunk_is_logical(chunk) ||
         uvm_sub_processor_mask_test(&mapping->sub_processors, uvm_id_sub_processor_index(gpu->id))))
        dma_addr = mapping->dma_addr + (parent_offset * PAGE_SIZE);
    uvm_mutex_unlock(&phys_chunk->lock);

    return dma_addr;
}

// Create a DMA mapping for the chunk on the given GPU. This will map the
// entire physical chunk on the parent GPU and record that a given MIG
// partition is using the mapping.
//
// Returns NV_OK on success. On error, any of the errors returned by
// uvm_parent_gpu_map_cpu_pages() can be returned. In the case that the DMA
// mapping structure could not be allocated, NV_ERR_NO_MEMORY is returned.
static NV_STATUS cpu_chunk_map_gpu_phys(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu)
{
    uvm_parent_gpu_t *parent_gpu = gpu->parent;
    uvm_cpu_physical_chunk_t *phys_chunk;
    uvm_cpu_logical_chunk_t *logical_chunk = NULL;
    uvm_cpu_phys_mapping_t *mapping;
    NV_STATUS status = NV_OK;

    if (uvm_cpu_chunk_is_logical(chunk)) {
        logical_chunk = uvm_cpu_chunk_to_logical(chunk);
        if (uvm_processor_mask_test(&logical_chunk->mapped_gpus, gpu->id))
            return status;
    }

    phys_chunk = get_physical_parent(chunk);
    uvm_mutex_lock(&phys_chunk->lock);

    if (!uvm_parent_processor_mask_test(&phys_chunk->gpu_mappings.dma_addrs_mask, parent_gpu->id)) {
        uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(&phys_chunk->common);
        NvU64 dma_addr;

        status = uvm_parent_gpu_map_cpu_pages(parent_gpu, phys_chunk->common.page, chunk_size, &dma_addr);
        if (status != NV_OK)
            goto done;

        mapping = chunk_phys_mapping_alloc(phys_chunk, parent_gpu->id);
        if (!mapping) {
            uvm_parent_gpu_unmap_cpu_pages(parent_gpu, dma_addr, chunk_size);
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        mapping->dma_addr = dma_addr;
        mapping->map_count = 1;
        uvm_sub_processor_mask_zero(&mapping->sub_processors);
        if (!logical_chunk)
            uvm_sub_processor_mask_set(&mapping->sub_processors, uvm_id_sub_processor_index(gpu->id));

        uvm_parent_processor_mask_set(&phys_chunk->gpu_mappings.dma_addrs_mask, parent_gpu->id);
    }
    else {
        mapping = chunk_phys_mapping_get(phys_chunk, parent_gpu->id);
        UVM_ASSERT(mapping);

        // Increment the map_count for logical chunks or the first time a
        // MIG partition is sharing a physical chunk.
        if (logical_chunk ||
            !uvm_sub_processor_mask_test_and_set(&mapping->sub_processors, uvm_id_sub_processor_index(gpu->id)))
            mapping->map_count++;
    }

    if (logical_chunk) {
        uvm_processor_mask_set(&logical_chunk->mapped_gpus, gpu->id);
        UVM_ASSERT(uvm_sub_processor_mask_empty(&mapping->sub_processors));
    }
    else {
        UVM_ASSERT(!uvm_sub_processor_mask_empty(&mapping->sub_processors));
        UVM_ASSERT(uvm_sub_processor_mask_get_count(&mapping->sub_processors) == mapping->map_count);
    }

done:
    uvm_mutex_unlock(&phys_chunk->lock);

    return status;
}

static void cpu_chunk_unmap_gpu_phys(uvm_cpu_chunk_t *chunk, uvm_gpu_id_t gpu_id)
{
    uvm_cpu_physical_chunk_t *phys_chunk = get_physical_parent(chunk);
    uvm_parent_gpu_id_t id = uvm_parent_gpu_id_from_gpu_id(gpu_id);

    uvm_mutex_lock(&phys_chunk->lock);

    if (uvm_cpu_chunk_is_logical(chunk)) {
        uvm_processor_mask_t *mapping_mask = &uvm_cpu_chunk_to_logical(chunk)->mapped_gpus;

        if (uvm_processor_mask_test_and_clear(mapping_mask, gpu_id))
            chunk_dec_gpu_mapping(phys_chunk, id);
    }
    else {
        if (uvm_parent_processor_mask_test(&phys_chunk->gpu_mappings.dma_addrs_mask, id)) {
            uvm_cpu_phys_mapping_t *mapping = chunk_phys_mapping_get(phys_chunk, id);

            if (uvm_sub_processor_mask_test_and_clear(&mapping->sub_processors, uvm_id_sub_processor_index(gpu_id)))
                chunk_dec_gpu_mapping(phys_chunk, id);
        }
    }

    uvm_mutex_unlock(&phys_chunk->lock);
}

NV_STATUS uvm_cpu_chunk_map_gpu(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu)
{
    NV_STATUS status;
    uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(chunk);

    status = cpu_chunk_map_gpu_phys(chunk, gpu);
    if (status != NV_OK)
        return status;

    status = uvm_mmu_sysmem_map(gpu, uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu), chunk_size);
    if (status != NV_OK)
        cpu_chunk_unmap_gpu_phys(chunk, gpu->id);

    return status;
}

void uvm_cpu_chunk_unmap_gpu(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu)
{
    cpu_chunk_unmap_gpu_phys(chunk, gpu->id);

    // Note: there is no corresponding uvm_mmu_sysmem_unmap() for
    // uvm_mmu_sysmem_map().
}

static void cpu_logical_chunk_release(uvm_cpu_logical_chunk_t *logical_chunk)
{
    uvm_cpu_physical_chunk_t *phys_chunk = get_physical_parent(logical_chunk->parent);
    uvm_processor_id_t gpu_id;

    uvm_mutex_lock(&phys_chunk->lock);

    for_each_id_in_mask(gpu_id, &logical_chunk->mapped_gpus)
        chunk_dec_gpu_mapping(phys_chunk, uvm_parent_gpu_id_from_gpu_id(gpu_id));

    uvm_mutex_unlock(&phys_chunk->lock);

    uvm_cpu_chunk_free(logical_chunk->parent);
}

static void cpu_physical_chunk_release(uvm_cpu_chunk_t *chunk)
{
    uvm_cpu_physical_chunk_t *phys_chunk = uvm_cpu_chunk_to_physical(chunk);
    uvm_parent_processor_id_t id;

    uvm_assert_mutex_unlocked(&phys_chunk->lock);

    // There should be no other threads using this chunk but we lock it because
    // of assertions in chunk_phys_mapping_get() and chunk_dec_gpu_mapping().
    uvm_mutex_lock(&phys_chunk->lock);

    for_each_parent_id_in_mask(id, &phys_chunk->gpu_mappings.dma_addrs_mask) {
        uvm_cpu_phys_mapping_t *mapping = chunk_phys_mapping_get(phys_chunk, id);
        NvU32 count;

        UVM_ASSERT(mapping);
        UVM_ASSERT(!uvm_sub_processor_mask_empty(&mapping->sub_processors));

        // Get a count of set bits in the sub_processors mask then clear it so
        // that chunk_dec_gpu_mapping() sees an empty mask when map_count == 0.
        // Using for_each_sub_processor_in_mask could try to dereference
        // mapping after map_count == 0 in the loop below.
        count = uvm_sub_processor_mask_get_count(&mapping->sub_processors);
        uvm_sub_processor_mask_zero(&mapping->sub_processors);

        for (; count; count--)
            chunk_dec_gpu_mapping(phys_chunk, id);
    }

    uvm_mutex_unlock(&phys_chunk->lock);

    UVM_ASSERT(uvm_parent_processor_mask_empty(&phys_chunk->gpu_mappings.dma_addrs_mask));

    if (phys_chunk->gpu_mappings.max_entries > 1)
        uvm_kvfree(phys_chunk->gpu_mappings.dynamic_entries);

    if (uvm_cpu_chunk_get_size(chunk) > PAGE_SIZE &&
        !bitmap_empty(phys_chunk->dirty_bitmap, uvm_cpu_chunk_num_pages(chunk)))
        SetPageDirty(chunk->page);

    uvm_kvfree(phys_chunk->dirty_bitmap);

    if (chunk->type != UVM_CPU_CHUNK_TYPE_HMM)
        put_page(chunk->page);
}

static void cpu_chunk_release(nv_kref_t *kref)
{
    uvm_cpu_chunk_t *chunk = container_of(kref, uvm_cpu_chunk_t, refcount);

    if (uvm_cpu_chunk_is_logical(chunk))
        cpu_logical_chunk_release(uvm_cpu_chunk_to_logical(chunk));
    else
        cpu_physical_chunk_release(chunk);

    uvm_kvfree(chunk);
}

static void uvm_cpu_chunk_get(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);
    nv_kref_get(&chunk->refcount);
}

void uvm_cpu_chunk_free(uvm_cpu_chunk_t *chunk)
{
    if (!chunk)
        return;

    nv_kref_put(&chunk->refcount, cpu_chunk_release);
}

static struct page *uvm_cpu_chunk_alloc_page(uvm_chunk_size_t alloc_size,
                                             int nid,
                                             uvm_cpu_chunk_alloc_flags_t alloc_flags)
{
    gfp_t kernel_alloc_flags;
    struct page *page;

    UVM_ASSERT(is_power_of_2(alloc_size));
    UVM_ASSERT(alloc_size & uvm_cpu_chunk_get_allocation_sizes());

    if (alloc_flags & UVM_CPU_CHUNK_ALLOC_FLAGS_ACCOUNT)
        kernel_alloc_flags = NV_UVM_GFP_FLAGS_ACCOUNT;
    else
        kernel_alloc_flags = NV_UVM_GFP_FLAGS;

    if ((alloc_flags & UVM_CPU_CHUNK_ALLOC_FLAGS_STRICT) && nid != NUMA_NO_NODE)
        kernel_alloc_flags |= __GFP_THISNODE;

    if (alloc_flags & UVM_CPU_CHUNK_ALLOC_FLAGS_ALLOW_MOVABLE)
        kernel_alloc_flags |= GFP_HIGHUSER_MOVABLE;
    else
        kernel_alloc_flags |= GFP_HIGHUSER;

    // For allocation sizes higher than PAGE_SIZE, use __GFP_NORETRY in order
    // to avoid higher allocation latency from the kernel compacting memory to
    // satisfy the request.
    // Use __GFP_NOWARN to avoid printing allocation failure to the kernel log.
    // High order allocation failures are handled gracefully by the caller.
    if (alloc_size > PAGE_SIZE)
        kernel_alloc_flags |= __GFP_COMP | __GFP_NORETRY | __GFP_NOWARN;

    if (alloc_flags & UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO)
        kernel_alloc_flags |= __GFP_ZERO;

    if (nid == NUMA_NO_NODE) {
        page = alloc_pages(kernel_alloc_flags, get_order(alloc_size));
    }
    else {
        UVM_ASSERT(node_isset(nid, node_online_map));
        page = alloc_pages_node(nid, kernel_alloc_flags, get_order(alloc_size));
    }

    if (page) {
        if (alloc_flags & UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO)
            SetPageDirty(page);

        if (alloc_flags & UVM_CPU_CHUNK_ALLOC_FLAGS_STRICT)
            UVM_ASSERT(page_to_nid(page) == nid);
    }

    return page;
}

static uvm_cpu_physical_chunk_t *uvm_cpu_chunk_create(uvm_chunk_size_t alloc_size)
{
    uvm_cpu_physical_chunk_t *chunk;

    chunk = uvm_kvmalloc_zero(sizeof(*chunk));
    if (!chunk)
        return NULL;

    uvm_cpu_chunk_set_size(&chunk->common, alloc_size);
    nv_kref_init(&chunk->common.refcount);
    uvm_mutex_init(&chunk->lock, UVM_LOCK_ORDER_LEAF);
    chunk->gpu_mappings.max_entries = 1;
    if (alloc_size > PAGE_SIZE) {
        chunk->dirty_bitmap = uvm_kvmalloc_zero(BITS_TO_LONGS(alloc_size / PAGE_SIZE) * sizeof(*chunk->dirty_bitmap));
        if (!chunk->dirty_bitmap) {
            uvm_kvfree(chunk);
            return NULL;
        }
    }

    return chunk;
}

NV_STATUS uvm_cpu_chunk_alloc(uvm_chunk_size_t alloc_size,
                              uvm_cpu_chunk_alloc_flags_t alloc_flags,
                              int nid,
                              uvm_cpu_chunk_t **new_chunk)
{
    uvm_cpu_physical_chunk_t *chunk;
    struct page *page;

    UVM_ASSERT(new_chunk);

    page = uvm_cpu_chunk_alloc_page(alloc_size, nid, alloc_flags);
    if (!page)
        return NV_ERR_NO_MEMORY;

    chunk = uvm_cpu_chunk_create(alloc_size);
    if (!chunk) {
        __free_pages(page, get_order(alloc_size));
        return NV_ERR_NO_MEMORY;
    }

    chunk->common.type = UVM_CPU_CHUNK_TYPE_PHYSICAL;
    chunk->common.page = page;

    *new_chunk = &chunk->common;
    return NV_OK;
}

NV_STATUS uvm_cpu_chunk_alloc_hmm(struct page *page,
                                  uvm_cpu_chunk_t **new_chunk)
{
    uvm_cpu_physical_chunk_t *chunk;

    UVM_ASSERT(new_chunk);

    chunk = uvm_cpu_chunk_create(PAGE_SIZE);
    if (!chunk)
        return NV_ERR_NO_MEMORY;

    chunk->common.type = UVM_CPU_CHUNK_TYPE_HMM;
    chunk->common.page = page;

    *new_chunk = &chunk->common;
    return NV_OK;
}

int uvm_cpu_chunk_get_numa_node(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);
    UVM_ASSERT(chunk->page);
    return page_to_nid(chunk->page);
}

// Convert the mask of DMA mapped parent GPUs and the sub-processor mask into
// one uvm_processor_mask_t in 'dma_map_mask'.
static void get_dma_map_mask(uvm_cpu_physical_chunk_t *chunk, uvm_processor_mask_t *dma_map_mask)
{
    uvm_parent_processor_id_t id;
    NvU32 sub_index;

    uvm_assert_mutex_locked(&chunk->lock);

    for_each_parent_id_in_mask(id, &chunk->gpu_mappings.dma_addrs_mask) {
        uvm_cpu_phys_mapping_t *mapping = chunk_phys_mapping_get(chunk, id);

        for_each_sub_processor_index_in_mask(sub_index, &mapping->sub_processors) {
            uvm_processor_id_t gpu_id = uvm_gpu_id_from_sub_processor(id, sub_index);

            uvm_sub_processor_mask_clear(&mapping->sub_processors, sub_index);
            uvm_processor_mask_set(dma_map_mask, gpu_id);
        }

        UVM_ASSERT(uvm_sub_processor_mask_empty(&mapping->sub_processors));
    }
}

NV_STATUS uvm_cpu_chunk_split(uvm_cpu_chunk_t *chunk, uvm_cpu_chunk_t **new_chunks)
{
    NV_STATUS status = NV_OK;
    uvm_cpu_logical_chunk_t *new_chunk;
    uvm_cpu_physical_chunk_t *phys_chunk = get_physical_parent(chunk);
    uvm_cpu_logical_chunk_t *logical_chunk = NULL;
    uvm_processor_id_t gpu_id;
    uvm_processor_mask_t *dma_map_mask = NULL;
    uvm_chunk_size_t new_size;
    size_t num_new_chunks;
    size_t num_subchunk_pages;
    size_t i;

    UVM_ASSERT(chunk);
    UVM_ASSERT(new_chunks);
    UVM_ASSERT(chunk->type != UVM_CPU_CHUNK_TYPE_HMM);

    // This should never be true as HMM chunks are currently all of size
    // PAGE_SIZE and can't be split and attempting to won't work as it requires
    // kernel interaction.
    // TODO: Bug 3368756: add support for transparent huge page (THP)
    if (chunk->type == UVM_CPU_CHUNK_TYPE_HMM)
        return NV_ERR_INVALID_ARGUMENT;

    // Get the largest size below the size of the input chunk.
    new_size = uvm_chunk_find_prev_size(uvm_cpu_chunk_get_allocation_sizes(), uvm_cpu_chunk_get_size(chunk));
    UVM_ASSERT(new_size);
    UVM_ASSERT(new_size != UVM_CHUNK_SIZE_INVALID);
    num_new_chunks = uvm_cpu_chunk_get_size(chunk) / new_size;
    num_subchunk_pages = new_size / PAGE_SIZE;

    if (uvm_cpu_chunk_is_logical(chunk)) {
        logical_chunk = uvm_cpu_chunk_to_logical(chunk);
        dma_map_mask = &logical_chunk->mapped_gpus;
    }

    uvm_mutex_lock(&phys_chunk->lock);

    for (i = 0; i < num_new_chunks; i++) {
        new_chunk = uvm_kvmalloc_zero(sizeof(*new_chunk));
        if (!new_chunk) {
            uvm_mutex_unlock(&phys_chunk->lock);
            status = NV_ERR_NO_MEMORY;
            goto error;
        }

        new_chunk->common.type = UVM_CPU_CHUNK_TYPE_LOGICAL;
        new_chunk->common.page = chunk->page + (i * num_subchunk_pages);
        uvm_cpu_chunk_set_size(&new_chunk->common, new_size);
        nv_kref_init(&new_chunk->common.refcount);
        new_chunk->parent = chunk;
        uvm_cpu_chunk_get(new_chunk->parent);
        if (i == 0 && !logical_chunk) {
            dma_map_mask = &new_chunk->mapped_gpus;
            get_dma_map_mask(phys_chunk, dma_map_mask);
        }
        else {
            uvm_processor_mask_copy(&new_chunk->mapped_gpus, dma_map_mask);
        }
        for_each_id_in_mask(gpu_id, dma_map_mask)
            chunk_inc_gpu_mapping(phys_chunk, uvm_parent_gpu_id_from_gpu_id(gpu_id));
        new_chunks[i] = &new_chunk->common;
    }

    // Release the references that are held by the chunk being split.
    for_each_id_in_mask(gpu_id, dma_map_mask)
        chunk_dec_gpu_mapping(phys_chunk, uvm_parent_gpu_id_from_gpu_id(gpu_id));

    // If the chunk being split is a logical chunk clear it's mapped_gpus mask.
    if (logical_chunk)
        uvm_processor_mask_zero(&logical_chunk->mapped_gpus);

    uvm_mutex_unlock(&phys_chunk->lock);

    // Drop the original reference count on the parent (from its creation). This
    // is done so the parent's reference count goes to 0 when all the children
    // are released.
    uvm_cpu_chunk_free(chunk);

error:
    if (status != NV_OK) {
        while (i--)
            uvm_cpu_chunk_free(new_chunks[i]);
    }

    return status;
}

static bool verify_merging_chunks(uvm_cpu_chunk_t **chunks, size_t num_chunks)
{
    uvm_cpu_logical_chunk_t *logical_chunk;
    uvm_cpu_chunk_t *first_chunk_parent;
    uvm_processor_mask_t *first_chunk_mapped_gpus;
    uvm_chunk_size_t first_chunk_size;
    size_t i;

    logical_chunk = uvm_cpu_chunk_to_logical(chunks[0]);
    first_chunk_size = uvm_cpu_chunk_get_size(chunks[0]);
    first_chunk_parent = logical_chunk->parent;
    first_chunk_mapped_gpus = &logical_chunk->mapped_gpus;

    // Only chunks with the same size and parent can be merged.
    for (i = 1; i < num_chunks; i++) {
        UVM_ASSERT(uvm_cpu_chunk_is_logical(chunks[i]));

        logical_chunk = uvm_cpu_chunk_to_logical(chunks[i]);
        UVM_ASSERT(logical_chunk->parent);
        UVM_ASSERT(logical_chunk->parent == first_chunk_parent);
        UVM_ASSERT(uvm_cpu_chunk_get_size(&logical_chunk->common) == first_chunk_size);
        UVM_ASSERT(nv_kref_read(&logical_chunk->common.refcount) == 1);

        // For now, we require that all logical chunks to be merged have to be
        // mapped on the same set of processors in order to be merged.
        //
        // If this requirement is relaxed in the future, the process to handle
        // GPU mappings would be:
        //    1. If mapped_gpus matches for all child chunks, the parent chunk's
        //       mapped_gpus mask is set to
        //            (child_mapped_gpus | parent_mapped_gpus).
        //    2. If the mapped_gpus masks for the child chunks don't match:
        //       2.1 All mappings to GPUs in each of child chunks' masks that are
        //           not also present in the parent chunk's mask are destroyed.
        //       2.2 mapped_gpus mask of the parent chunk remains unmodified.
        UVM_ASSERT(uvm_processor_mask_equal(&logical_chunk->mapped_gpus, first_chunk_mapped_gpus));
    }

    return true;
}

uvm_cpu_chunk_t *uvm_cpu_chunk_merge(uvm_cpu_chunk_t **chunks)
{
    uvm_cpu_chunk_t *parent;
    uvm_cpu_logical_chunk_t *logical_chunk;
    uvm_cpu_physical_chunk_t *phys_chunk;
    uvm_processor_id_t gpu_id;
    uvm_chunk_size_t chunk_size;
    uvm_chunk_size_t parent_chunk_size;
    size_t num_merge_chunks;
    size_t i;

    UVM_ASSERT(chunks);
    UVM_ASSERT(uvm_cpu_chunk_is_logical(chunks[0]));

    logical_chunk = uvm_cpu_chunk_to_logical(chunks[0]);
    parent = logical_chunk->parent;
    UVM_ASSERT(parent);

    chunk_size = uvm_cpu_chunk_get_size(chunks[0]);
    parent_chunk_size = uvm_cpu_chunk_get_size(parent);
    UVM_ASSERT(IS_ALIGNED(parent_chunk_size, chunk_size));
    num_merge_chunks = parent_chunk_size / chunk_size;

    // This assert will never trigger since verify_merging_chunks() always
    // returns true. However, it will eliminate the call on release builds.
    UVM_ASSERT(verify_merging_chunks(chunks, num_merge_chunks));

    // Take a reference on the parent chunk so it doesn't get released when all
    // of the children are released below.
    uvm_cpu_chunk_get(parent);
    phys_chunk = get_physical_parent(chunks[0]);

    uvm_mutex_lock(&phys_chunk->lock);

    for_each_id_in_mask(gpu_id, &logical_chunk->mapped_gpus)
        chunk_inc_gpu_mapping(phys_chunk, uvm_parent_gpu_id_from_gpu_id(gpu_id));

    if (uvm_cpu_chunk_is_logical(parent)) {
        uvm_processor_mask_copy(&uvm_cpu_chunk_to_logical(parent)->mapped_gpus, &logical_chunk->mapped_gpus);
    }
    else {
        // Restore the mapping->sub_processors mask for each mapped GPU.
        for_each_id_in_mask(gpu_id, &logical_chunk->mapped_gpus) {
            uvm_cpu_phys_mapping_t *mapping = chunk_phys_mapping_get(phys_chunk, uvm_parent_gpu_id_from_gpu_id(gpu_id));

            UVM_ASSERT(mapping);
            uvm_sub_processor_mask_set(&mapping->sub_processors, uvm_id_sub_processor_index(gpu_id));
        }
    }

    uvm_mutex_unlock(&phys_chunk->lock);

    for (i = 0; i < num_merge_chunks; i++)
        uvm_cpu_chunk_free(chunks[i]);

    return parent;
}

// Check the CPU PTE dirty bit and if set, clear it and fill the
// physical chunk's dirty bitmap.
static void check_cpu_dirty_flag(uvm_cpu_physical_chunk_t *chunk, uvm_page_index_t page_index)
{
    struct page *page;

    uvm_assert_mutex_locked(&chunk->lock);

    // Kernels prior to v4.5 used the flags within the individual pages even for
    // compound pages. For those kernels, we don't necessarily need the bitmap
    // but using it allows for a single implementation.
    page = chunk->common.page + page_index;
    if (TestClearPageDirty(page))
        bitmap_fill(chunk->dirty_bitmap, uvm_cpu_chunk_num_pages(&chunk->common));
}

void uvm_cpu_chunk_mark_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_cpu_physical_chunk_t *parent;

    UVM_ASSERT(page_index < uvm_cpu_chunk_num_pages(chunk));
    UVM_ASSERT(!uvm_cpu_chunk_is_hmm(chunk));

    if (uvm_cpu_chunk_is_physical(chunk) && uvm_cpu_chunk_get_size(chunk) == PAGE_SIZE) {
        SetPageDirty(chunk->page);
        return;
    }

    parent = get_physical_parent(chunk);
    if (uvm_cpu_chunk_is_logical(chunk)) {
        uvm_cpu_logical_chunk_t *logical_chunk = uvm_cpu_chunk_to_logical(chunk);
        page_index += cpu_chunk_get_phys_index(logical_chunk);
    }

    uvm_mutex_lock(&parent->lock);
    set_bit(page_index, parent->dirty_bitmap);
    uvm_mutex_unlock(&parent->lock);
}

void uvm_cpu_chunk_mark_clean(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_cpu_physical_chunk_t *parent;

    UVM_ASSERT(page_index < uvm_cpu_chunk_num_pages(chunk));
    UVM_ASSERT(!uvm_cpu_chunk_is_hmm(chunk));

    if (uvm_cpu_chunk_is_physical(chunk) && uvm_cpu_chunk_get_size(chunk) == PAGE_SIZE) {
        ClearPageDirty(chunk->page);
        return;
    }

    parent = get_physical_parent(chunk);
    if (uvm_cpu_chunk_is_logical(chunk)) {
        uvm_cpu_logical_chunk_t *logical_chunk = uvm_cpu_chunk_to_logical(chunk);
        page_index += cpu_chunk_get_phys_index(logical_chunk);
    }

    uvm_mutex_lock(&parent->lock);
    check_cpu_dirty_flag(parent, page_index);
    clear_bit(page_index, parent->dirty_bitmap);
    uvm_mutex_unlock(&parent->lock);
}

bool uvm_cpu_chunk_is_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_cpu_physical_chunk_t *parent;
    bool dirty;

    UVM_ASSERT(page_index < uvm_cpu_chunk_num_pages(chunk));
    UVM_ASSERT(!uvm_cpu_chunk_is_hmm(chunk));

    if (uvm_cpu_chunk_is_physical(chunk) && uvm_cpu_chunk_get_size(chunk) == PAGE_SIZE)
        return PageDirty(chunk->page);

    parent = get_physical_parent(chunk);
    if (uvm_cpu_chunk_is_logical(chunk)) {
        uvm_cpu_logical_chunk_t *logical_chunk = uvm_cpu_chunk_to_logical(chunk);
        page_index += cpu_chunk_get_phys_index(logical_chunk);
    }

    uvm_mutex_lock(&parent->lock);
    check_cpu_dirty_flag(parent, page_index);
    dirty = test_bit(page_index, parent->dirty_bitmap);
    uvm_mutex_unlock(&parent->lock);

    return dirty;
}
