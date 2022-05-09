/*******************************************************************************
    Copyright (c) 2017-2021 NVIDIA Corporation

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

    if (!sysmem_mappings->gpu->parent->access_counters_supported)
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

    if (!sysmem_mappings->gpu->parent->access_counters_supported)
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

    if (!sysmem_mappings->gpu->parent->access_counters_supported)
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

    if (!sysmem_mappings->gpu->parent->access_counters_supported)
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

    if (!sysmem_mappings->gpu->parent->access_counters_supported)
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
    UVM_ASSERT(sysmem_mappings->gpu->parent->access_counters_supported);
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

static void uvm_cpu_chunk_set_phys_size(uvm_cpu_chunk_t *chunk, uvm_chunk_size_t size)
{
#if !UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE()
    chunk->log2_phys_size = ilog2(size);
#endif
}

uvm_chunk_size_t uvm_cpu_chunk_get_size(uvm_cpu_chunk_t *chunk)
{
#if UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE()
    return PAGE_SIZE;
#else
    uvm_chunk_size_t chunk_size;

    UVM_ASSERT(chunk);
    UVM_ASSERT(uvm_cpu_chunk_get_phys_size(chunk));
    chunk_size = uvm_va_block_region_size(chunk->region);
    UVM_ASSERT(uvm_cpu_chunk_get_phys_size(chunk) >= chunk_size);
    return chunk_size;
#endif
}

#if UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE()
struct page *uvm_cpu_chunk_get_cpu_page(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    UVM_ASSERT(chunk);
    return chunk;
}

void uvm_cpu_chunk_put(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);
    put_page(chunk);
}

NV_STATUS uvm_cpu_chunk_gpu_mapping_alloc(uvm_va_block_t *va_block, uvm_gpu_id_t id)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, id);
    size_t num_pages = uvm_va_block_num_cpu_pages(va_block);

    UVM_ASSERT(gpu_state);
    gpu_state->cpu_chunks_dma_addrs = uvm_kvmalloc_zero(num_pages * sizeof(gpu_state->cpu_chunks_dma_addrs[0]));
    if (!gpu_state->cpu_chunks_dma_addrs)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_cpu_chunk_gpu_mapping_split(uvm_va_block_t *existing, uvm_va_block_t *new, uvm_gpu_id_t id)
{
    uvm_va_block_gpu_state_t *existing_state = uvm_va_block_gpu_state_get(existing, id);
    uvm_va_block_gpu_state_t *new_state = uvm_va_block_gpu_state_get(new, id);
    size_t new_pages = uvm_va_block_num_cpu_pages(new);

    memcpy(&new_state->cpu_chunks_dma_addrs[0],
           &existing_state->cpu_chunks_dma_addrs[uvm_va_block_num_cpu_pages(existing) - new_pages],
           new_pages * sizeof(new_state->cpu_chunks_dma_addrs[0]));
}

void uvm_cpu_chunk_gpu_mapping_free(uvm_va_block_t *va_block, uvm_gpu_id_t id)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, id);

    if (gpu_state)
        uvm_kvfree(gpu_state->cpu_chunks_dma_addrs);
}

NV_STATUS uvm_cpu_chunk_set_gpu_mapping_addr(uvm_va_block_t *va_block,
                                             uvm_page_index_t page_index,
                                             uvm_cpu_chunk_t *chunk,
                                             uvm_gpu_id_t id,
                                             NvU64 dma_addr)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, id);

    gpu_state->cpu_chunks_dma_addrs[page_index] = dma_addr;
    return NV_OK;
}

NvU64 uvm_cpu_chunk_get_gpu_mapping_addr(uvm_va_block_t *va_block,
                                         uvm_page_index_t page_index,
                                         uvm_cpu_chunk_t *chunk,
                                         uvm_gpu_id_t id)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, id);

    return gpu_state->cpu_chunks_dma_addrs[page_index];
}

NV_STATUS uvm_cpu_chunk_insert_in_block(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    if (!va_block->cpu.chunks) {
        va_block->cpu.chunks = (unsigned long)uvm_kvmalloc_zero(uvm_va_block_num_cpu_pages(va_block) *
                                                                sizeof(uvm_cpu_chunk_t *));
        if (!va_block->cpu.chunks)
            return NV_ERR_NO_MEMORY;
    }

    UVM_ASSERT(!uvm_page_mask_test(&va_block->cpu.allocated, page_index));
    UVM_ASSERT(((uvm_cpu_chunk_t **)va_block->cpu.chunks)[page_index] == NULL);
    ((uvm_cpu_chunk_t **)va_block->cpu.chunks)[page_index] = chunk;
    uvm_page_mask_set(&va_block->cpu.allocated, page_index);
    return NV_OK;
}

void uvm_cpu_chunk_remove_from_block(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    UVM_ASSERT(uvm_page_mask_test(&va_block->cpu.allocated, page_index));
    UVM_ASSERT(((uvm_cpu_chunk_t **)va_block->cpu.chunks)[page_index] != NULL);
    ((uvm_cpu_chunk_t **)va_block->cpu.chunks)[page_index] = NULL;
    uvm_page_mask_clear(&va_block->cpu.allocated, page_index);
}

uvm_cpu_chunk_t *uvm_cpu_chunk_get_chunk_for_page(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    UVM_ASSERT(page_index < uvm_va_block_num_cpu_pages(va_block));
    if (!uvm_page_mask_test(&va_block->cpu.allocated, page_index))
        return NULL;

    return ((uvm_cpu_chunk_t **)va_block->cpu.chunks)[page_index];
}

NV_STATUS uvm_cpu_chunk_alloc(uvm_va_block_t *va_block,
                              uvm_page_index_t page_index,
                              struct mm_struct *mm,
                              uvm_cpu_chunk_t **new_chunk)
{
    uvm_cpu_chunk_t *chunk = NULL;
    gfp_t alloc_flags;
    NV_STATUS status;

    UVM_ASSERT(!uvm_page_mask_test(&va_block->cpu.allocated, page_index));
    UVM_ASSERT(new_chunk);

    alloc_flags = (mm ? NV_UVM_GFP_FLAGS_ACCOUNT : NV_UVM_GFP_FLAGS) | GFP_HIGHUSER;

    if (!uvm_va_block_page_resident_processors_count(va_block, page_index))
        alloc_flags |= __GFP_ZERO;

    chunk = alloc_pages(alloc_flags, 0);
    if (!chunk)
        return NV_ERR_NO_MEMORY;

    if (alloc_flags & __GFP_ZERO)
        SetPageDirty(chunk);

    status = uvm_cpu_chunk_insert_in_block(va_block, chunk, page_index);
    if (status != NV_OK) {
        uvm_cpu_chunk_put(chunk);
        return status;
    }

    *new_chunk = chunk;
    return NV_OK;
}

#else

struct page *uvm_cpu_chunk_get_cpu_page(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_va_block_region_t chunk_region;

    UVM_ASSERT(chunk);
    UVM_ASSERT(chunk->page);
    chunk_region = uvm_va_block_chunk_region(va_block, uvm_cpu_chunk_get_size(chunk), page_index);
    return chunk->page + (page_index - chunk_region.first);
}

static NvU64 uvm_cpu_chunk_get_virt_addr(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);
    UVM_ASSERT(chunk->region.first < chunk->region.outer);
    return uvm_va_block_cpu_page_address(va_block, chunk->region.first);
}

static void cpu_chunk_release(nv_kref_t *kref)
{
    uvm_cpu_chunk_t *chunk = container_of(kref, uvm_cpu_chunk_t, refcount);
    uvm_cpu_chunk_t *parent = chunk->parent;

    if (uvm_processor_mask_get_gpu_count(&chunk->gpu_mappings.dma_addrs_mask) > 1)
        uvm_kvfree(chunk->gpu_mappings.dynamic_entries);

    if (!parent) {
        uvm_assert_spinlock_unlocked(&chunk->lock);
        uvm_kvfree(chunk->dirty_bitmap);
        put_page(chunk->page);
    }
    else {
        uvm_cpu_chunk_put(parent);
    }

    uvm_kvfree(chunk);
}

void uvm_cpu_chunk_get(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);
    nv_kref_get(&chunk->refcount);
}

void uvm_cpu_chunk_put(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);

    nv_kref_put(&chunk->refcount, cpu_chunk_release);
}

NV_STATUS uvm_cpu_chunk_gpu_mapping_alloc(uvm_va_block_t *va_block, uvm_gpu_id_t id)
{
    return NV_OK;
}

void uvm_cpu_chunk_gpu_mapping_split(uvm_va_block_t *existing, uvm_va_block_t *va_block, uvm_gpu_id_t id)
{
    return;
}

void uvm_cpu_chunk_gpu_mapping_free(uvm_va_block_t *va_block, uvm_gpu_id_t id)
{
    return;
}

static NvU32 compute_gpu_mappings_entry_index(uvm_processor_mask_t dma_addrs_mask, uvm_gpu_id_t id)
{
    uvm_processor_mask_t subset_mask;

    // Compute the array index for the given GPU ID by masking off all bits
    // above the id and then counting the number of bits remaining.
    uvm_processor_mask_zero(&subset_mask);
    bitmap_set(subset_mask.bitmap, 0, uvm_id_value(id) + 1);
    uvm_processor_mask_and(&subset_mask, &dma_addrs_mask, &subset_mask);

    if (uvm_processor_mask_empty(&subset_mask))
        return 0;

    return uvm_processor_mask_get_gpu_count(&subset_mask) - 1;
}

NV_STATUS uvm_cpu_chunk_set_gpu_mapping_addr(uvm_va_block_t *va_block,
                                             uvm_page_index_t page_index,
                                             uvm_cpu_chunk_t *chunk,
                                             uvm_gpu_id_t id,
                                             NvU64 dma_addr)
{
    NvU32 num_existing_entries = uvm_processor_mask_get_gpu_count(&chunk->gpu_mappings.dma_addrs_mask);
    NvU32 num_new_entries;
    NvU32 array_index;
    NvU64 *new_entries;

    if (uvm_processor_mask_empty(&chunk->gpu_mappings.dma_addrs_mask)) {
        uvm_processor_mask_set(&chunk->gpu_mappings.dma_addrs_mask, id);
        chunk->gpu_mappings.static_entry = dma_addr;
        return NV_OK;
    }

    if (uvm_processor_mask_test(&chunk->gpu_mappings.dma_addrs_mask, id)) {
        if (num_existing_entries == 1) {
            chunk->gpu_mappings.static_entry = dma_addr;
        }
        else {
            array_index = compute_gpu_mappings_entry_index(chunk->gpu_mappings.dma_addrs_mask, id);
            chunk->gpu_mappings.dynamic_entries[array_index] = dma_addr;
        }
        return NV_OK;
    }

    num_new_entries = num_existing_entries + 1;
    if (num_existing_entries == 1) {
        new_entries = uvm_kvmalloc(sizeof(*new_entries) * num_new_entries);

        if (new_entries) {
            uvm_processor_id_t first = uvm_processor_mask_find_first_id(&chunk->gpu_mappings.dma_addrs_mask);

            if (uvm_id_value(first) < uvm_id_value(id))
                new_entries[0] = chunk->gpu_mappings.static_entry;
            else
                new_entries[1] = chunk->gpu_mappings.static_entry;
        }
    }
    else {
        new_entries = uvm_kvrealloc(chunk->gpu_mappings.dynamic_entries,
                                    sizeof(*new_entries) * num_new_entries);
        if (new_entries) {
            // Get the number of bits set below the input id.
            num_existing_entries = compute_gpu_mappings_entry_index(chunk->gpu_mappings.dma_addrs_mask, id);
            for (; num_existing_entries < num_new_entries - 1; num_existing_entries++)
                new_entries[num_existing_entries + 1] = new_entries[num_existing_entries];
        }
    }

    if (!new_entries)
        return NV_ERR_NO_MEMORY;

    chunk->gpu_mappings.dynamic_entries = new_entries;
    uvm_processor_mask_set(&chunk->gpu_mappings.dma_addrs_mask, id);
    array_index = compute_gpu_mappings_entry_index(chunk->gpu_mappings.dma_addrs_mask, id);
    chunk->gpu_mappings.dynamic_entries[array_index] = dma_addr;

    return NV_OK;
}

NvU64 uvm_cpu_chunk_get_gpu_mapping_addr(uvm_va_block_t *va_block,
                                         uvm_page_index_t page_index,
                                         uvm_cpu_chunk_t *chunk,
                                         uvm_gpu_id_t id)
{
    NvU64 dma_addr;

    if (!uvm_processor_mask_test(&chunk->gpu_mappings.dma_addrs_mask, id))
        return 0;

    if (uvm_processor_mask_get_gpu_count(&chunk->gpu_mappings.dma_addrs_mask) == 1) {
        dma_addr = chunk->gpu_mappings.static_entry;
    }
    else {
        NvU32 array_index = compute_gpu_mappings_entry_index(chunk->gpu_mappings.dma_addrs_mask, id);

        dma_addr = chunk->gpu_mappings.dynamic_entries[array_index];
    }

    return dma_addr;
}

// The bottom two bits of uvm_va_block_t::chunks is used to indicate how
// CPU chunks are stored.
//
// CPU chunk storage is handled in three different ways depending on the
// type of chunks the VA block owns. This is done to minimize the memory
// required to hold metadata.
typedef enum
{
    // The uvm_va_block_t::chunk pointer points to a single 2MB
    // CPU chunk.
    UVM_CPU_CHUNK_STORAGE_CHUNK = 0,

    // The uvm_va_block_t::chunks pointer points to an array of
    // pointers to CPU chunks.
    UVM_CPU_CHUNK_STORAGE_ARRAY,

    // The uvm_va_block_t::chunks pointer points to a
    // structure of mixed (64K and 4K) chunks.
    UVM_CPU_CHUNK_STORAGE_MIXED,
    UVM_CPU_CHUNK_STORAGE_COUNT,
} uvm_cpu_chunk_storage_type_t;

#define UVM_CPU_CHUNK_STORAGE_MASK 0x3

#define UVM_CPU_STORAGE_GET_PTR(block) ((void *)((block)->cpu.chunks & ~UVM_CPU_CHUNK_STORAGE_MASK))
#define UVM_CPU_STORAGE_GET_TYPE(block) \
    ((uvm_cpu_chunk_storage_type_t)((block)->cpu.chunks & UVM_CPU_CHUNK_STORAGE_MASK))

// The maximum number of slots in the mixed chunk mode (64K + 4K chunks) is one
// more than MAX_BIG_PAGES_PER_UVM_VA_BLOCK to account for misaligned VA blocks.
#define MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK (MAX_BIG_PAGES_PER_UVM_VA_BLOCK + 1)

#define MAX_SMALL_CHUNK_PER_BIG_SLOT (UVM_CHUNK_SIZE_64K / PAGE_SIZE)

// This structure is used when a VA block contains 64K or a mix of 64K and 4K
// CPU chunks.
// For every 64K CPU chunks, big_chunks will have its corresponding bit set
// and the corresponding index in slots will point directly to the
// uvm_cpu_chunk_t structure.
//
// For 4K CPU chunks, the corresponding bit in big_chunks will be clear and
// the element in slots will point to an array of 16 uvm_cpu_chunk_t pointers.
typedef struct {
    DECLARE_BITMAP(big_chunks, MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK);
    void *slots[MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK];
} uvm_cpu_chunk_storage_mixed_t;

static uvm_page_index_t compute_slot_index(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    uvm_va_block_region_t block_region = uvm_va_block_region_from_block(va_block);
    size_t prefix;
    uvm_page_index_t big_page_index;

    if (page_index < block_region.first || page_index >= block_region.outer)
        return MAX_BIG_PAGES_PER_UVM_VA_BLOCK;

    prefix = (UVM_ALIGN_UP(va_block->start, UVM_CHUNK_SIZE_64K) - va_block->start) / PAGE_SIZE;

    if (page_index < prefix)
        return 0;

    big_page_index = ((page_index - prefix) / MAX_SMALL_CHUNK_PER_BIG_SLOT) + !!prefix;
    UVM_ASSERT(big_page_index < MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK);

    return big_page_index;
}

static size_t compute_small_index(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    size_t prefix = (UVM_ALIGN_UP(va_block->start, UVM_CHUNK_SIZE_64K) - va_block->start) / PAGE_SIZE;

    if (page_index < prefix)
        return page_index;

    return (page_index - prefix) % MAX_SMALL_CHUNK_PER_BIG_SLOT;
}

NV_STATUS uvm_cpu_chunk_insert_in_block(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(chunk);
    uvm_page_index_t big_page_index;
    uvm_cpu_chunk_storage_mixed_t *mixed;
    uvm_cpu_chunk_t **chunks = NULL;

    // We only want to use the bottom two bits of a pointer.
    BUILD_BUG_ON(UVM_CPU_CHUNK_STORAGE_COUNT > 4);

    chunk->region = uvm_va_block_region(page_index, page_index + uvm_cpu_chunk_num_pages(chunk));
    UVM_ASSERT(chunk->region.outer <= PAGES_PER_UVM_VA_BLOCK);

    // We want to protect against two threads manipulating the VA block's CPU
    // chunks at the same time. However, when a block is split, the new block's
    // lock is locked without tracking. So, we can't use
    // uvm_assert_mutex_locked().
    UVM_ASSERT(mutex_is_locked(&va_block->lock.m));

    if (!va_block->cpu.chunks) {
        switch (chunk_size) {
            case UVM_CHUNK_SIZE_2M:
                break;
            case UVM_CHUNK_SIZE_64K:
                mixed = uvm_kvmalloc_zero(sizeof(*mixed));
                if (!mixed)
                    return NV_ERR_NO_MEMORY;

                va_block->cpu.chunks = (unsigned long)mixed | UVM_CPU_CHUNK_STORAGE_MIXED;
                break;
            case UVM_CHUNK_SIZE_4K:
                chunks = uvm_kvmalloc_zero(sizeof(*chunks) * uvm_va_block_num_cpu_pages(va_block));
                if (!chunks)
                    return NV_ERR_NO_MEMORY;

                va_block->cpu.chunks = (unsigned long)chunks | UVM_CPU_CHUNK_STORAGE_ARRAY;
                break;
            default:
                return NV_ERR_INVALID_ARGUMENT;
        }
    }

    switch (UVM_CPU_STORAGE_GET_TYPE(va_block)) {
        case UVM_CPU_CHUNK_STORAGE_CHUNK:
            if (va_block->cpu.chunks)
                return NV_ERR_INVALID_STATE;
            UVM_ASSERT(chunk_size == UVM_CHUNK_SIZE_2M);
            va_block->cpu.chunks = (unsigned long)chunk | UVM_CPU_CHUNK_STORAGE_CHUNK;
            break;
        case UVM_CPU_CHUNK_STORAGE_MIXED:
            mixed = UVM_CPU_STORAGE_GET_PTR(va_block);
            big_page_index = compute_slot_index(va_block, page_index);
            UVM_ASSERT(big_page_index != MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
            UVM_ASSERT(compute_slot_index(va_block, page_index + uvm_cpu_chunk_num_pages(chunk) - 1) == big_page_index);

            if (test_bit(big_page_index, mixed->big_chunks))
                return NV_ERR_INVALID_STATE;

            if (chunk_size == UVM_CHUNK_SIZE_64K) {
                mixed->slots[big_page_index] = chunk;
                set_bit(big_page_index, mixed->big_chunks);
            }
            else {
                size_t slot_index;

                UVM_ASSERT(chunk_size == UVM_CHUNK_SIZE_4K);
                chunks = mixed->slots[big_page_index];

                if (!chunks) {
                    chunks = uvm_kvmalloc_zero(sizeof(*chunks) * MAX_SMALL_CHUNK_PER_BIG_SLOT);
                    if (!chunks)
                        return NV_ERR_NO_MEMORY;
                    mixed->slots[big_page_index] = chunks;
                }

                slot_index = compute_small_index(va_block, page_index);
                chunks[slot_index] = chunk;
            }
            break;
        case UVM_CPU_CHUNK_STORAGE_ARRAY:
            chunks = UVM_CPU_STORAGE_GET_PTR(va_block);
            if (chunk_size == UVM_CHUNK_SIZE_64K) {
                uvm_cpu_chunk_t **subchunks = NULL;
                uvm_page_index_t sub_page_index;

                mixed = uvm_kvmalloc_zero(sizeof(*mixed));
                if (!mixed)
                    return NV_ERR_NO_MEMORY;

                big_page_index = compute_slot_index(va_block, page_index);
                UVM_ASSERT(big_page_index != MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
                UVM_ASSERT(compute_slot_index(va_block, page_index + uvm_cpu_chunk_num_pages(chunk) - 1) ==
                           big_page_index);
                mixed->slots[big_page_index] = chunk;
                set_bit(big_page_index, mixed->big_chunks);

                for (sub_page_index = 0; sub_page_index < uvm_va_block_num_cpu_pages(va_block); sub_page_index++) {
                    uvm_cpu_chunk_t *subchunk = chunks[sub_page_index];
                    size_t subchunk_index = compute_small_index(va_block, sub_page_index);

                    if (!subchunk)
                        continue;

                    if (!subchunks || compute_slot_index(va_block, sub_page_index) != big_page_index) {
                        subchunks = uvm_kvmalloc_zero(sizeof(*subchunks) * MAX_SMALL_CHUNK_PER_BIG_SLOT);
                        if (!subchunks) {
                            size_t i;

                            for (i = 0; i < MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK; i++) {
                                if (!test_bit(i, mixed->big_chunks) && mixed->slots[i])
                                    uvm_kvfree(mixed->slots[i]);
                            }

                            uvm_kvfree(mixed);
                            return NV_ERR_NO_MEMORY;
                        }

                        big_page_index = compute_slot_index(va_block, sub_page_index);
                        UVM_ASSERT(mixed->slots[big_page_index] == NULL);
                        mixed->slots[big_page_index] = subchunks;
                    }

                    subchunks[subchunk_index] = subchunk;
                    if (subchunk_index == MAX_SMALL_CHUNK_PER_BIG_SLOT - 1)
                        subchunks = NULL;
                }

                va_block->cpu.chunks = (unsigned long)mixed | UVM_CPU_CHUNK_STORAGE_MIXED;
                uvm_kvfree(chunks);
            }
            else {
                chunks[page_index] = chunk;
            }

       default:
           break;
    }

    uvm_page_mask_region_fill(&va_block->cpu.allocated,
                              uvm_va_block_region(page_index, page_index + uvm_cpu_chunk_num_pages(chunk)));

    return NV_OK;
}

void uvm_cpu_chunk_remove_from_block(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_cpu_chunk_storage_mixed_t *mixed;
    uvm_page_index_t big_page_index;
    uvm_cpu_chunk_t **chunks;

    // We want to protect against two threads manipulating the VA block's CPU
    // chunks at the same time. However, when a block is split, the new block's
    // lock is locked without tracking. So, we can't use
    // uvm_assert_mutex_locked().
    UVM_ASSERT(mutex_is_locked(&va_block->lock.m));
    UVM_ASSERT(va_block->cpu.chunks);

    switch (UVM_CPU_STORAGE_GET_TYPE(va_block)) {
        case UVM_CPU_CHUNK_STORAGE_CHUNK:
            UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_2M);
            UVM_ASSERT(UVM_CPU_STORAGE_GET_PTR(va_block) == chunk);
            va_block->cpu.chunks = 0;
            break;
        case UVM_CPU_CHUNK_STORAGE_MIXED:
            UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) != UVM_CHUNK_SIZE_2M);
            mixed = UVM_CPU_STORAGE_GET_PTR(va_block);
            big_page_index = compute_slot_index(va_block, page_index);
            UVM_ASSERT(big_page_index != MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
            UVM_ASSERT(mixed->slots[big_page_index] != NULL);

            if (test_bit(big_page_index, mixed->big_chunks)) {
                UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_64K);
                UVM_ASSERT(mixed->slots[big_page_index] == chunk);
                mixed->slots[big_page_index] = NULL;
                clear_bit(big_page_index, mixed->big_chunks);
            }
            else {
                size_t slot_index;

                UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_4K);
                chunks = mixed->slots[big_page_index];
                slot_index = compute_small_index(va_block, page_index);
                UVM_ASSERT(chunks[slot_index] == chunk);
                chunks[slot_index] = NULL;

                for (slot_index = 0; slot_index < MAX_SMALL_CHUNK_PER_BIG_SLOT; slot_index++) {
                    if (chunks[slot_index])
                        break;
                }

                if (slot_index == MAX_SMALL_CHUNK_PER_BIG_SLOT) {
                    uvm_kvfree(chunks);
                    mixed->slots[big_page_index] = NULL;
                }
            }

            break;
        case UVM_CPU_CHUNK_STORAGE_ARRAY:
            UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_4K);
            chunks = UVM_CPU_STORAGE_GET_PTR(va_block);
            UVM_ASSERT(chunks[page_index] == chunk);
            chunks[page_index] = NULL;
            break;
        default:
            return;
    };

    uvm_page_mask_region_clear(&va_block->cpu.allocated,
                               uvm_va_block_region(page_index, page_index + uvm_cpu_chunk_num_pages(chunk)));

    if (uvm_page_mask_empty(&va_block->cpu.allocated)) {
        if (UVM_CPU_STORAGE_GET_TYPE(va_block) != UVM_CPU_CHUNK_STORAGE_CHUNK)
            uvm_kvfree(UVM_CPU_STORAGE_GET_PTR(va_block));
        va_block->cpu.chunks = 0;
    }
}

uvm_cpu_chunk_t *uvm_cpu_chunk_get_chunk_for_page(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    uvm_cpu_chunk_storage_mixed_t *mixed;
    uvm_cpu_chunk_t *chunk;
    uvm_cpu_chunk_t **chunks;
    uvm_page_index_t big_page_index;
    size_t slot_index;

    if (page_index >= uvm_va_block_num_cpu_pages(va_block) || !uvm_page_mask_test(&va_block->cpu.allocated, page_index))
        return NULL;

    UVM_ASSERT(va_block->cpu.chunks);

    switch (UVM_CPU_STORAGE_GET_TYPE(va_block)) {
        case UVM_CPU_CHUNK_STORAGE_CHUNK:
            return UVM_CPU_STORAGE_GET_PTR(va_block);
        case UVM_CPU_CHUNK_STORAGE_MIXED:
            mixed = UVM_CPU_STORAGE_GET_PTR(va_block);
            big_page_index = compute_slot_index(va_block, page_index);
            UVM_ASSERT(big_page_index != MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
            UVM_ASSERT(mixed->slots[big_page_index] != NULL);
            if (test_bit(big_page_index, mixed->big_chunks))
                return mixed->slots[big_page_index];

            chunks = mixed->slots[big_page_index];
            slot_index = compute_small_index(va_block, page_index);
            chunk = chunks[slot_index];
            break;
        case UVM_CPU_CHUNK_STORAGE_ARRAY:
            chunks = UVM_CPU_STORAGE_GET_PTR(va_block);
            chunk = chunks[page_index];
            break;
        default:
            return NULL;
    }

    UVM_ASSERT(chunk);
    return chunk;
}

NV_STATUS uvm_cpu_chunk_alloc(uvm_va_block_t *va_block,
                              uvm_page_index_t page_index,
                              struct mm_struct *mm,
                              uvm_cpu_chunk_t **new_chunk)
{
    uvm_va_block_test_t *block_test = uvm_va_block_get_test(va_block);
    uvm_cpu_chunk_t *chunk = NULL;
    NvU32 cpu_allocation_sizes;
    uvm_page_mask_t zero_page_mask;
    uvm_gpu_id_t id;
    struct page *page = NULL;
    uvm_chunk_size_t alloc_size;
    uvm_va_block_region_t region;
    uvm_va_space_t *va_space;
    uvm_processor_mask_t uvm_lite_gpus;
    gfp_t base_alloc_flags;
    NV_STATUS status;

    UVM_ASSERT(new_chunk);

    // Limit the allocation sizes only to the ones supported.
    cpu_allocation_sizes = uvm_cpu_chunk_get_allocation_sizes();

    if (block_test && block_test->cpu_chunk_allocation_size_mask)
        cpu_allocation_sizes &= block_test->cpu_chunk_allocation_size_mask;

    // Get a mask of all the block pages that are resident somewhere.
    uvm_page_mask_zero(&zero_page_mask);
    for_each_id_in_mask(id, &va_block->resident)
        uvm_page_mask_or(&zero_page_mask, &zero_page_mask, uvm_va_block_resident_mask_get(va_block, id));

    // If the VA space has a UVM-Lite GPU registered, only PAGE_SIZE allocations
    // should be used in order to avoid extra copies due to dirty compound
    // pages.
    va_space = uvm_va_block_get_va_space(va_block);
    uvm_processor_mask_andnot(&uvm_lite_gpus, &va_space->registered_gpus, &va_space->faultable_processors);
    if (!uvm_processor_mask_empty(&uvm_lite_gpus))
        cpu_allocation_sizes = PAGE_SIZE;

    base_alloc_flags = (mm ? NV_UVM_GFP_FLAGS_ACCOUNT : NV_UVM_GFP_FLAGS) | GFP_HIGHUSER;

    // Attempt to allocate CPU pages with the largest physically contiguous
    // size from the set of CPU chunk sizes that we can.
    // This is accomplished by:
    //   1. Aligning the CPU page address down to the allocation size.
    //   2. Ensuring that the entire allocation region fits withing the VA
    //      block.
    //   3. Ensuring that the region covered by the allocation is empty.
    for_each_chunk_size_rev(alloc_size, cpu_allocation_sizes) {
        NvU64 alloc_virt_addr;
        uvm_page_mask_t scratch_page_mask;
        uvm_page_index_t alloc_page_index;
        gfp_t alloc_flags = base_alloc_flags;

        if (alloc_size < PAGE_SIZE)
            break;

        alloc_virt_addr = UVM_ALIGN_DOWN(uvm_va_block_cpu_page_address(va_block, page_index), alloc_size);

        if (!uvm_va_block_contains_address(va_block, alloc_virt_addr) ||
            !uvm_va_block_contains_address(va_block, alloc_virt_addr + alloc_size - 1))
            continue;

        alloc_page_index = uvm_va_block_cpu_page_index(va_block, alloc_virt_addr);
        region = uvm_va_block_region(alloc_page_index, alloc_page_index + (alloc_size / PAGE_SIZE));
        uvm_page_mask_init_from_region(&scratch_page_mask, region, NULL);
        uvm_page_mask_and(&scratch_page_mask, &va_block->cpu.allocated, &scratch_page_mask);

        if (!uvm_page_mask_empty(&scratch_page_mask))
            continue;

        // For allocation sizes higher than PAGE_SIZE, use __GFP_NORETRY in
        // order to avoid higher allocation latency from the kernel compacting
        // memory to satisfy the request.
        if (alloc_size > PAGE_SIZE)
            alloc_flags |= __GFP_COMP | __GFP_NORETRY;

        // If not all pages in the allocation region are resident somewhere,
        // zero out the allocated page.
        // This could be wasteful if only a few pages in high-order allocation
        // need to be zero'ed out but the alternative is to map single sub-
        // pages one-by-one.
        if (!uvm_page_mask_region_full(&zero_page_mask, region))
            alloc_flags |= __GFP_ZERO;

        page = alloc_pages(alloc_flags, get_order(alloc_size));
        if (page) {
            if (alloc_flags & __GFP_ZERO)
                SetPageDirty(page);
            break;
        }
    }

    if (!page) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    chunk = uvm_kvmalloc_zero(sizeof(*chunk));
    if (!chunk) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    chunk->page = page;
    uvm_cpu_chunk_set_phys_size(chunk, alloc_size);
    chunk->region = region;
    nv_kref_init(&chunk->refcount);
    uvm_spin_lock_init(&chunk->lock, UVM_LOCK_ORDER_LEAF);
    if (alloc_size > PAGE_SIZE) {
        chunk->dirty_bitmap = uvm_kvmalloc_zero(BITS_TO_LONGS(alloc_size / PAGE_SIZE) * sizeof(*chunk->dirty_bitmap));
        if (!chunk->dirty_bitmap) {
            status = NV_ERR_NO_MEMORY;
            goto error;
        }
    }

    status = uvm_cpu_chunk_insert_in_block(va_block, chunk, chunk->region.first);
    if (status != NV_OK)
        goto error;

    if (new_chunk)
        *new_chunk = chunk;

    return NV_OK;

error:

    // If chunk has been allocated, uvm_cpu_chunk_put() will release the chunk
    // and the page. Otherwise, only release the page.
    if (chunk)
        uvm_cpu_chunk_put(chunk);
    else if (page)
        __free_pages(page, get_order(alloc_size));

    return status;
}

NV_STATUS uvm_cpu_chunk_split(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_chunk_size_t new_size)
{
    NV_STATUS status = NV_OK;
    NV_STATUS insert_status;
    uvm_cpu_chunk_t *new_chunk;
    uvm_page_index_t running_page_index = chunk->region.first;
    uvm_page_index_t next_page_index;
    size_t num_new_chunks;
    size_t num_subchunk_pages;
    size_t i;

    UVM_ASSERT(chunk);
    UVM_ASSERT(is_power_of_2(new_size));
    UVM_ASSERT(new_size < uvm_cpu_chunk_get_size(chunk));

    // We subtract 1 from the computed number of subchunks because we always
    // keep the original chunk as the first in the block's list. This is so we
    // don't lose the physical chunk.
    // All new subchunks will point to the original chunk as their parent.
    num_new_chunks = (uvm_cpu_chunk_get_size(chunk) / new_size) - 1;
    num_subchunk_pages = new_size / PAGE_SIZE;
    running_page_index += num_subchunk_pages;

    // Remove the existing chunk from the block first. We re-insert it after
    // the split.
    uvm_cpu_chunk_remove_from_block(va_block, chunk, chunk->region.first);

    for (i = 0; i < num_new_chunks; i++) {
        uvm_page_index_t relative_page_index = running_page_index - chunk->region.first;
        uvm_gpu_id_t id;

        new_chunk = uvm_kvmalloc_zero(sizeof(*new_chunk));
        if (!new_chunk) {
            status = NV_ERR_NO_MEMORY;
            goto error;
        }

        new_chunk->page = chunk->page + relative_page_index;
        new_chunk->offset = chunk->offset + relative_page_index;
        new_chunk->region = uvm_va_block_region(running_page_index, running_page_index + num_subchunk_pages);
        uvm_cpu_chunk_set_phys_size(new_chunk, new_size);
        nv_kref_init(&new_chunk->refcount);

        // This lock is unused for logical blocks but initialize it for
        // consistency.
        uvm_spin_lock_init(&new_chunk->lock, UVM_LOCK_ORDER_LEAF);
        new_chunk->parent = chunk;
        uvm_cpu_chunk_get(new_chunk->parent);

        for_each_gpu_id(id) {
            NvU64 parent_dma_addr = uvm_cpu_chunk_get_gpu_mapping_addr(va_block, running_page_index, chunk, id);

            if (!parent_dma_addr)
                continue;

            uvm_cpu_chunk_set_gpu_mapping_addr(va_block,
                                               relative_page_index,
                                               new_chunk,
                                               id,
                                               parent_dma_addr + (relative_page_index * PAGE_SIZE));
        }

        status = uvm_cpu_chunk_insert_in_block(va_block, new_chunk, new_chunk->region.first);
        if (status != NV_OK) {
            uvm_cpu_chunk_put(new_chunk);
            goto error;
        }

        running_page_index += num_subchunk_pages;
    }

    chunk->region = uvm_va_block_region(chunk->region.first, chunk->region.first + num_subchunk_pages);

error:
    // Re-insert the split chunk. This is done unconditionally in both the
    // success and error paths. The difference is that on the success path,
    // the chunk's region has been updated.
    // This operation should never fail with NV_ERR_NO_MEMORY since all
    // state memory should already be allocated. Failing with other errors
    // is a programmer error.
    insert_status = uvm_cpu_chunk_insert_in_block(va_block, chunk, chunk->region.first);
    UVM_ASSERT(insert_status != NV_ERR_INVALID_ARGUMENT && insert_status != NV_ERR_INVALID_STATE);

    if (status != NV_OK) {
        for_each_cpu_chunk_in_block_region_safe(new_chunk,
                                                running_page_index,
                                                next_page_index,
                                                va_block,
                                                chunk->region) {
            uvm_cpu_chunk_remove_from_block(va_block, new_chunk, new_chunk->region.first);
            uvm_cpu_chunk_put(new_chunk);
        }
    }

    return status;
}

uvm_cpu_chunk_t *uvm_cpu_chunk_merge(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk)
{
    uvm_cpu_chunk_t *parent;
    uvm_cpu_chunk_t *subchunk;
    uvm_chunk_sizes_mask_t merge_sizes = uvm_cpu_chunk_get_allocation_sizes();
    uvm_chunk_size_t merge_chunk_size;
    uvm_chunk_size_t parent_phys_size;
    uvm_chunk_size_t chunk_size;
    uvm_va_block_region_t subchunk_region;
    uvm_page_index_t page_index;
    uvm_page_index_t next_page_index;
    NV_STATUS insert_status;

    UVM_ASSERT(chunk);
    parent = chunk->parent;

    // If the chunk does not have a parent, a merge cannot be done.
    if (!parent)
        return NULL;

    chunk_size = uvm_cpu_chunk_get_size(chunk);
    parent_phys_size = uvm_cpu_chunk_get_phys_size(parent);

    // Remove all sizes above the parent's physical size.
    merge_sizes &= parent_phys_size | (parent_phys_size - 1);

    // Remove all sizes including and below the chunk's current size.
    merge_sizes &= ~(chunk_size | (chunk_size - 1));

    // Find the largest size that is fully contained within the VA block.
    for_each_chunk_size_rev(merge_chunk_size, merge_sizes) {
        NvU64 parent_start = uvm_cpu_chunk_get_virt_addr(va_block, parent);
        NvU64 parent_end = parent_start + parent_phys_size - 1;

        if (uvm_va_block_contains_address(va_block, parent_start) &&
            uvm_va_block_contains_address(va_block, parent_start + merge_chunk_size - 1) &&
            IS_ALIGNED(parent_start, merge_chunk_size) &&
            IS_ALIGNED(parent_end + 1, merge_chunk_size))
            break;
    }

    if (merge_chunk_size == UVM_CHUNK_SIZE_INVALID)
        return NULL;

    if (uvm_cpu_chunk_get_size(parent) == merge_chunk_size)
        return NULL;

    UVM_ASSERT(chunk_size == uvm_cpu_chunk_get_size(parent));
    UVM_ASSERT(IS_ALIGNED(merge_chunk_size, chunk_size));

    subchunk_region = uvm_va_block_region(parent->region.first + uvm_cpu_chunk_num_pages(parent),
                                          parent->region.first + (merge_chunk_size / PAGE_SIZE));

    // Remove the first (parent) subchunk. It will be re-inserted later with an
    // updated region.
    uvm_cpu_chunk_remove_from_block(va_block, parent, parent->region.first);

    for_each_cpu_chunk_in_block_region_safe(subchunk, page_index, next_page_index, va_block, subchunk_region) {
        UVM_ASSERT(subchunk);
        uvm_cpu_chunk_remove_from_block(va_block, subchunk, subchunk->region.first);
        uvm_cpu_chunk_put(subchunk);
    }

    parent->region = uvm_va_block_region(parent->region.first, parent->region.first + (merge_chunk_size / PAGE_SIZE));
    insert_status = uvm_cpu_chunk_insert_in_block(va_block, parent, parent->region.first);
    UVM_ASSERT(insert_status != NV_ERR_INVALID_ARGUMENT && insert_status != NV_ERR_INVALID_STATE);

    return parent;
}

static uvm_cpu_chunk_t *get_parent_cpu_chunk(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);

    while (chunk->parent)
        chunk = chunk->parent;

    return chunk;
}

// Check the CPU PTE dirty bit and if set, clear it and fill the
// physical chunk's dirty bitmap.
static void check_cpu_dirty_flag(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    struct page *page;

    UVM_ASSERT(!chunk->parent);
    uvm_assert_spinlock_locked(&chunk->lock);

    // Kernels prior to v4.5 used the flags within the individual pages even for
    // compound pages.
    page = chunk->page + page_index;
    if (PageDirty(page)) {
        bitmap_fill(chunk->dirty_bitmap, uvm_cpu_chunk_get_phys_size(chunk) / PAGE_SIZE);
        ClearPageDirty(page);
    }
}

static uvm_cpu_chunk_t *get_parent_and_page_index(uvm_cpu_chunk_t *chunk, uvm_page_index_t *out_page_index)
{
    uvm_cpu_chunk_t *parent;
    uvm_page_index_t page_index;

    UVM_ASSERT(chunk);
    UVM_ASSERT(chunk->page);
    UVM_ASSERT(out_page_index);
    page_index = *out_page_index;
    UVM_ASSERT(chunk->region.first <= page_index && page_index < chunk->region.outer);

    page_index = chunk->offset + (page_index - chunk->region.first);
    parent = get_parent_cpu_chunk(chunk);
    UVM_ASSERT(page_index < uvm_cpu_chunk_get_phys_size(parent) / PAGE_SIZE);
    *out_page_index = page_index;
    return parent;
}

void uvm_cpu_chunk_mark_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_cpu_chunk_t *parent;

    parent = get_parent_and_page_index(chunk, &page_index);
    if (uvm_cpu_chunk_get_phys_size(parent) == PAGE_SIZE) {
        SetPageDirty(parent->page);
        return;
    }

    uvm_spin_lock(&parent->lock);
    set_bit(page_index, parent->dirty_bitmap);
    uvm_spin_unlock(&parent->lock);
}

void uvm_cpu_chunk_mark_clean(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_cpu_chunk_t *parent;

    parent = get_parent_and_page_index(chunk, &page_index);
    if (uvm_cpu_chunk_get_phys_size(parent) == PAGE_SIZE) {
        ClearPageDirty(parent->page);
        return;
    }

    uvm_spin_lock(&parent->lock);
    check_cpu_dirty_flag(parent, page_index);
    clear_bit(page_index, parent->dirty_bitmap);
    uvm_spin_unlock(&parent->lock);
}

bool uvm_cpu_chunk_is_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_cpu_chunk_t *parent;
    bool dirty;

    parent = get_parent_and_page_index(chunk, &page_index);
    if (uvm_cpu_chunk_get_phys_size(parent) == PAGE_SIZE)
        return PageDirty(parent->page);

    uvm_spin_lock(&parent->lock);
    check_cpu_dirty_flag(parent, page_index);
    dirty = test_bit(page_index, parent->dirty_bitmap);
    uvm_spin_unlock(&parent->lock);

    return dirty;
}
#endif // !UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE()

uvm_cpu_chunk_t *uvm_cpu_chunk_first_in_block(uvm_va_block_t *va_block, uvm_page_index_t *out_page_index)
{
    uvm_cpu_chunk_t *chunk = NULL;
    uvm_page_index_t page_index;
    uvm_va_block_region_t block_region = uvm_va_block_region_from_block(va_block);

    page_index = uvm_va_block_first_page_in_mask(block_region, &va_block->cpu.allocated);
    if (page_index < block_region.outer)
        chunk = uvm_cpu_chunk_get_chunk_for_page(va_block, page_index);

    if (out_page_index)
        *out_page_index = page_index;

    return chunk;
}

uvm_cpu_chunk_t *uvm_cpu_chunk_next(uvm_va_block_t *va_block, uvm_page_index_t *previous_page_index)
{
    uvm_va_block_region_t block_region;

    UVM_ASSERT(va_block);
    UVM_ASSERT(previous_page_index);

    block_region = uvm_va_block_region_from_block(va_block);
    *previous_page_index = uvm_va_block_next_page_in_mask(block_region, &va_block->cpu.allocated, *previous_page_index);
    if (*previous_page_index == block_region.outer)
        return NULL;

    return uvm_cpu_chunk_get_chunk_for_page(va_block, *previous_page_index);
}
