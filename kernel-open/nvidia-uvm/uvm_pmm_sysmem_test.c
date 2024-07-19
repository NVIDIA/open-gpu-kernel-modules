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

#include "uvm_test.h"
#include "uvm_test_ioctl.h"

#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_pmm_sysmem.h"
#include "uvm_va_block.h"
#include "uvm_va_range.h"
#include "uvm_va_space.h"
#include "uvm_kvmalloc.h"
#include "uvm_hal.h"
#include "uvm_push.h"
#include "uvm_processors.h"

// Pre-allocated array used for dma-to-virt translations
static uvm_reverse_map_t g_sysmem_translations[PAGES_PER_UVM_VA_BLOCK];

// We use our own separate reverse map to easily specify contiguous DMA
// address ranges
static uvm_pmm_sysmem_mappings_t g_reverse_map;

// Check that the DMA addresses in the range defined by
// [base_dma_addr:base_dma_addr + uvm_va_block_size(va_block)] and page_mask
// are registered in the reverse map, using one call per entry. The returned
// virtual addresses must belong to va_block. The function assumes a 1:1
// dma-to-virt mapping for the whole VA block
static NV_STATUS check_reverse_map_block_page(uvm_va_block_t *va_block,
                                              NvU64 base_dma_addr,
                                              const uvm_page_mask_t *page_mask)
{
    uvm_page_index_t page_index;

    for_each_va_block_page(page_index, va_block) {
        size_t num_pages;

        memset(g_sysmem_translations, 0, sizeof(g_sysmem_translations));
        num_pages = uvm_pmm_sysmem_mappings_dma_to_virt(&g_reverse_map,
                                                        base_dma_addr + page_index * PAGE_SIZE,
                                                        PAGE_SIZE,
                                                        g_sysmem_translations,
                                                        PAGES_PER_UVM_VA_BLOCK);
        if (!page_mask || uvm_page_mask_test(page_mask, page_index)) {
            TEST_CHECK_RET(num_pages == 1);
            TEST_CHECK_RET(g_sysmem_translations[0].va_block == va_block);
            TEST_CHECK_RET(nv_kref_read(&va_block->kref) >= 2);
            TEST_CHECK_RET(uvm_reverse_map_start(&g_sysmem_translations[0]) == uvm_va_block_cpu_page_address(va_block, page_index));
            TEST_CHECK_RET(uvm_va_block_region_num_pages(g_sysmem_translations[0].region) == 1);
            TEST_CHECK_RET(UVM_ID_IS_CPU(g_sysmem_translations[0].owner));
            uvm_va_block_release(g_sysmem_translations[0].va_block);
        }
        else {
            TEST_CHECK_RET(num_pages == 0);
        }
    }

    return NV_OK;
}

// Check that the DMA addresses in the range defined by
// [base_dma_addr:base_dma_addr + uvm_va_block_size(va_block)] and page_mask
// are registered in the reverse map, using a single translation call. The
// returned virtual addresses must belong to va_block. The function assumes a
// 1:1 dma-to-virt mapping for the whole VA block
static NV_STATUS check_reverse_map_block_batch(uvm_va_block_t *va_block,
                                               NvU64 base_dma_addr,
                                               const uvm_page_mask_t *page_mask)
{
    size_t num_translations;
    size_t num_pages;
    size_t reverse_map_index;

    memset(g_sysmem_translations, 0, sizeof(g_sysmem_translations));
    num_translations = uvm_pmm_sysmem_mappings_dma_to_virt(&g_reverse_map,
                                                           base_dma_addr,
                                                           uvm_va_block_size(va_block),
                                                           g_sysmem_translations,
                                                           PAGES_PER_UVM_VA_BLOCK);
    if (num_translations == 0 && page_mask)
        TEST_CHECK_RET(uvm_page_mask_empty(page_mask));

    num_pages = 0;
    for (reverse_map_index = 0; reverse_map_index < num_translations; ++reverse_map_index) {
        uvm_reverse_map_t *reverse_map = &g_sysmem_translations[reverse_map_index];
        size_t num_reverse_map_pages = uvm_va_block_region_num_pages(reverse_map->region);

        num_pages += num_reverse_map_pages;

        TEST_CHECK_RET(reverse_map->va_block == va_block);
        TEST_CHECK_RET(nv_kref_read(&va_block->kref) >= 2);
        uvm_va_block_release(reverse_map->va_block);
        TEST_CHECK_RET(UVM_ID_IS_CPU(reverse_map->owner));
    }

    if (page_mask)
        TEST_CHECK_RET(num_pages == uvm_page_mask_weight(page_mask));
    else
        TEST_CHECK_RET(num_pages == uvm_va_block_num_cpu_pages(va_block));

    return NV_OK;
}

// Check that the DMA addresses for all the CPU pages of the two given VA blocks
// are registered in the reverse map, using a single translation call. The
// returned virtual addresses must belong to one of the blocks. The function
// assumes a 1:1 dma-to-virt mapping for each VA block and that va_block1 is
// mapped behind va_block0.
static NV_STATUS check_reverse_map_two_blocks_batch(NvU64 base_dma_addr,
                                                    uvm_va_block_t *va_block0,
                                                    uvm_va_block_t *va_block1)
{
    size_t num_pages;
    size_t num_translations;
    size_t reverse_map_index;

    memset(g_sysmem_translations, 0, sizeof(g_sysmem_translations));
    num_translations = uvm_pmm_sysmem_mappings_dma_to_virt(&g_reverse_map,
                                                           base_dma_addr,
                                                           UVM_VA_BLOCK_SIZE,
                                                           g_sysmem_translations,
                                                           PAGES_PER_UVM_VA_BLOCK);
    TEST_CHECK_RET(num_translations == 2);

    num_pages = 0;
    for (reverse_map_index = 0; reverse_map_index < num_translations; ++reverse_map_index) {
        uvm_va_block_t *block;
        uvm_reverse_map_t *reverse_map = &g_sysmem_translations[reverse_map_index];
        NvU64 virt_addr = uvm_reverse_map_start(reverse_map);
        size_t num_reverse_map_pages = uvm_va_block_region_num_pages(reverse_map->region);

        if (reverse_map_index == 0)
            block = va_block0;
        else
            block = va_block1;

        TEST_CHECK_RET(reverse_map->va_block == block);
        TEST_CHECK_RET(nv_kref_read(&block->kref) >= 2);
        uvm_va_block_release(reverse_map->va_block);
        TEST_CHECK_RET(num_reverse_map_pages == uvm_va_block_num_cpu_pages(block));
        TEST_CHECK_RET(virt_addr == block->start);
        TEST_CHECK_RET(UVM_ID_IS_CPU(reverse_map->owner));

        num_pages += num_reverse_map_pages;
    }

    TEST_CHECK_RET(num_pages == uvm_va_block_num_cpu_pages(va_block0) + uvm_va_block_num_cpu_pages(va_block1));

    return NV_OK;
}

static const NvU64 g_base_dma_addr = UVM_VA_BLOCK_SIZE;

// This function adds the mappings for all the subregions in va_block defined
// by page_mask. g_base_dma_addr is used as the base DMA address for the whole
// VA block.
static NV_STATUS test_pmm_sysmem_reverse_map_single(uvm_va_block_t *va_block,
                                                    uvm_page_mask_t *page_mask,
                                                    uvm_chunk_size_t split_size,
                                                    bool merge)
{
    NV_STATUS status = NV_OK;
    uvm_va_block_region_t subregion;

    TEST_CHECK_RET(is_power_of_2(split_size));
    TEST_CHECK_RET(split_size >= PAGE_SIZE);

    for_each_va_block_subregion_in_mask(subregion, page_mask, uvm_va_block_region_from_block(va_block)) {
        TEST_CHECK_RET(is_power_of_2(uvm_va_block_region_size(subregion)));
        uvm_mutex_lock(&va_block->lock);
        status = uvm_pmm_sysmem_mappings_add_gpu_mapping(&g_reverse_map,
                                                         g_base_dma_addr + subregion.first * PAGE_SIZE,
                                                         va_block->start + subregion.first * PAGE_SIZE,
                                                         uvm_va_block_region_size(subregion),
                                                         va_block,
                                                         UVM_ID_CPU);
        uvm_mutex_unlock(&va_block->lock);
        if (status != NV_OK)
            return status;
    }

    TEST_CHECK_RET(check_reverse_map_block_page(va_block, g_base_dma_addr, page_mask) == NV_OK);
    TEST_CHECK_RET(check_reverse_map_block_batch(va_block, g_base_dma_addr, page_mask) == NV_OK);

    if (split_size != UVM_CHUNK_SIZE_MAX) {
        for_each_va_block_subregion_in_mask(subregion, page_mask, uvm_va_block_region_from_block(va_block)) {
            TEST_CHECK_RET(uvm_va_block_region_size(subregion) > split_size);

            uvm_mutex_lock(&va_block->lock);
            status = uvm_pmm_sysmem_mappings_split_gpu_mappings(&g_reverse_map,
                                                                g_base_dma_addr + subregion.first * PAGE_SIZE,
                                                                split_size);
            uvm_mutex_unlock(&va_block->lock);
            TEST_CHECK_RET(status == NV_OK);
        }

        TEST_CHECK_RET(check_reverse_map_block_page(va_block, g_base_dma_addr, page_mask) == NV_OK);
        TEST_CHECK_RET(check_reverse_map_block_batch(va_block, g_base_dma_addr, page_mask) == NV_OK);
    }

    if (split_size != UVM_CHUNK_SIZE_MAX && merge) {
        for_each_va_block_subregion_in_mask(subregion, page_mask, uvm_va_block_region_from_block(va_block)) {
            uvm_pmm_sysmem_mappings_merge_gpu_mappings(&g_reverse_map,
                                                       g_base_dma_addr + subregion.first * PAGE_SIZE,
                                                       uvm_va_block_region_size(subregion));
        }

        TEST_CHECK_RET(check_reverse_map_block_page(va_block, g_base_dma_addr, page_mask) == NV_OK);
        TEST_CHECK_RET(check_reverse_map_block_batch(va_block, g_base_dma_addr, page_mask) == NV_OK);
    }

    for_each_va_block_subregion_in_mask(subregion, page_mask, uvm_va_block_region_from_block(va_block)) {
        NvU64 subregion_dma_addr = g_base_dma_addr + subregion.first * PAGE_SIZE;

        if (split_size == UVM_CHUNK_SIZE_MAX || merge) {
            uvm_mutex_lock(&va_block->lock);
            uvm_pmm_sysmem_mappings_remove_gpu_mapping(&g_reverse_map, subregion_dma_addr);
            uvm_mutex_unlock(&va_block->lock);
        }
        else {
            size_t chunk;
            size_t num_chunks = uvm_va_block_region_size(subregion) / split_size;
            TEST_CHECK_RET(num_chunks > 1);

            uvm_mutex_lock(&va_block->lock);

            for (chunk = 0; chunk < num_chunks; ++chunk)
                uvm_pmm_sysmem_mappings_remove_gpu_mapping(&g_reverse_map, subregion_dma_addr + chunk * split_size);

            uvm_mutex_unlock(&va_block->lock);
        }
    }

    uvm_page_mask_zero(page_mask);

    TEST_CHECK_RET(check_reverse_map_block_page(va_block, g_base_dma_addr, page_mask) == NV_OK);
    TEST_CHECK_RET(check_reverse_map_block_batch(va_block, g_base_dma_addr, page_mask) == NV_OK);

    return status;
}

static uvm_page_mask_t g_page_mask;

static NV_STATUS test_pmm_sysmem_reverse_map_single_whole(uvm_va_space_t *va_space, NvU64 addr)
{
    NV_STATUS status;
    uvm_va_block_t *va_block;
    const bool merge_array[] = {false, true};
    const uvm_chunk_size_t chunk_split_array[] = { UVM_CHUNK_SIZE_4K, UVM_CHUNK_SIZE_64K, UVM_CHUNK_SIZE_MAX };
    unsigned merge_index;
    unsigned chunk_split_index;

    status = uvm_va_block_find(va_space, addr, &va_block);
    if (status != NV_OK)
        return status;

    TEST_CHECK_RET(is_power_of_2(uvm_va_block_size(va_block)));

    for (merge_index = 0; merge_index < ARRAY_SIZE(merge_array); ++merge_index) {
        for (chunk_split_index = 0; chunk_split_index < ARRAY_SIZE(chunk_split_array); ++chunk_split_index) {
            // The reverse map has PAGE_SIZE granularity
            if (chunk_split_array[chunk_split_index] < PAGE_SIZE)
                continue;

            uvm_page_mask_region_fill(&g_page_mask, uvm_va_block_region_from_block(va_block));

            TEST_CHECK_RET(test_pmm_sysmem_reverse_map_single(va_block,
                                                              &g_page_mask,
                                                              chunk_split_array[chunk_split_index],
                                                              merge_array[merge_index]) == NV_OK);
        }
    }

    return status;
}

static NV_STATUS test_pmm_sysmem_reverse_map_single_pattern(uvm_va_space_t *va_space, NvU64 addr)
{
    NV_STATUS status;
    uvm_va_block_t *va_block;
    uvm_page_index_t page_index;

    status = uvm_va_block_find(va_space, addr, &va_block);
    if (status != NV_OK)
        return status;

    uvm_page_mask_zero(&g_page_mask);

    for_each_va_block_page(page_index, va_block) {
        if (page_index % 2 == 0)
            uvm_page_mask_set(&g_page_mask, page_index);
    }

    return test_pmm_sysmem_reverse_map_single(va_block, &g_page_mask, UVM_CHUNK_SIZE_MAX, false);
}

// This function assumes that addr points at a VA range with 4 sized VA blocks
// with size UVM_VA_BLOCK_SIZE / 4.
static NV_STATUS test_pmm_sysmem_reverse_map_many_blocks(uvm_va_space_t *va_space, NvU64 addr)
{
    NV_STATUS status;
    uvm_va_block_t *va_block0;
    uvm_va_block_t *va_block1;
    NvU64 base_dma_addr0;
    NvU64 base_dma_addr1;

    status = uvm_va_block_find(va_space, addr + UVM_VA_BLOCK_SIZE / 4, &va_block0);
    if (status != NV_OK)
        return status;

    status = uvm_va_block_find(va_space, addr + 3 * UVM_VA_BLOCK_SIZE / 4, &va_block1);
    if (status != NV_OK)
        return status;

    TEST_CHECK_RET(va_block0 != va_block1);

    base_dma_addr0 = g_base_dma_addr + uvm_va_block_size(va_block0);
    base_dma_addr1 = base_dma_addr0 + uvm_va_block_size(va_block0);

    TEST_CHECK_RET(is_power_of_2(uvm_va_block_size(va_block0)));
    TEST_CHECK_RET(is_power_of_2(uvm_va_block_size(va_block1)));

    uvm_mutex_lock(&va_block0->lock);
    status = uvm_pmm_sysmem_mappings_add_gpu_mapping(&g_reverse_map,
                                                     base_dma_addr0,
                                                     va_block0->start,
                                                     uvm_va_block_size(va_block0),
                                                     va_block0,
                                                     UVM_ID_CPU);
    uvm_mutex_unlock(&va_block0->lock);
    TEST_CHECK_RET(status == NV_OK);

    uvm_mutex_lock(&va_block1->lock);
    status = uvm_pmm_sysmem_mappings_add_gpu_mapping(&g_reverse_map,
                                                     base_dma_addr1,
                                                     va_block1->start,
                                                     uvm_va_block_size(va_block1),
                                                     va_block1,
                                                     UVM_ID_CPU);
    uvm_mutex_unlock(&va_block1->lock);

    // Check each VA block individually
    if (status == NV_OK) {
        TEST_CHECK_GOTO(check_reverse_map_block_page(va_block0, base_dma_addr0, NULL) == NV_OK, error);
        TEST_CHECK_GOTO(check_reverse_map_block_batch(va_block0, base_dma_addr0, NULL) == NV_OK, error);
        TEST_CHECK_GOTO(check_reverse_map_block_page(va_block1, base_dma_addr1, NULL) == NV_OK, error);
        TEST_CHECK_GOTO(check_reverse_map_block_batch(va_block1, base_dma_addr1, NULL) == NV_OK, error);

        // Check both VA blocks at the same time
        TEST_CHECK_GOTO(check_reverse_map_two_blocks_batch(g_base_dma_addr, va_block0, va_block1) == NV_OK, error);

error:
        uvm_mutex_lock(&va_block1->lock);
        uvm_pmm_sysmem_mappings_remove_gpu_mapping(&g_reverse_map, base_dma_addr1);
        uvm_mutex_unlock(&va_block1->lock);
    }

    uvm_mutex_lock(&va_block0->lock);
    uvm_pmm_sysmem_mappings_remove_gpu_mapping(&g_reverse_map, base_dma_addr0);
    uvm_mutex_unlock(&va_block0->lock);

    return status;
}

// This function registers a non-uniform distribution of chunks (mixing 4K and 64K chunks)
// and merges them back to verify that the logic is working.
static NV_STATUS test_pmm_sysmem_reverse_map_merge(uvm_va_space_t *va_space, NvU64 addr)
{
    NV_STATUS status = NV_OK;
    uvm_va_block_t *va_block;
    const unsigned chunks_64k_pos[] =
    {
        16,
        64,
        96,
        192,
        208,
        224,
        288,
        320,
        384,
        480
    };
    uvm_page_index_t page_index;
    unsigned i;

    if (PAGE_SIZE != UVM_PAGE_SIZE_4K)
        return NV_OK;

    status = uvm_va_block_find(va_space, addr, &va_block);
    if (status != NV_OK)
        return status;

    TEST_CHECK_RET(uvm_va_block_size(va_block) == UVM_VA_BLOCK_SIZE);

    page_index = 0;
    for (i = 0; i < ARRAY_SIZE(chunks_64k_pos); ++i) {
        // Fill with 4K mappings until the next 64K mapping
        while (page_index < chunks_64k_pos[i]) {
            uvm_mutex_lock(&va_block->lock);
            status = uvm_pmm_sysmem_mappings_add_gpu_mapping(&g_reverse_map,
                                                             g_base_dma_addr + page_index * PAGE_SIZE,
                                                             uvm_va_block_cpu_page_address(va_block, page_index),
                                                             PAGE_SIZE,
                                                             va_block,
                                                             UVM_ID_CPU);
            uvm_mutex_unlock(&va_block->lock);
            TEST_CHECK_RET(status == NV_OK);

            ++page_index;
        }

        // Register the 64K mapping
        uvm_mutex_lock(&va_block->lock);
        status = uvm_pmm_sysmem_mappings_add_gpu_mapping(&g_reverse_map,
                                                         g_base_dma_addr + page_index * PAGE_SIZE,
                                                         uvm_va_block_cpu_page_address(va_block, page_index),
                                                         UVM_CHUNK_SIZE_64K,
                                                         va_block,
                                                         UVM_ID_CPU);
        uvm_mutex_unlock(&va_block->lock);
        TEST_CHECK_RET(status == NV_OK);

        page_index += UVM_PAGE_SIZE_64K / PAGE_SIZE;
    }

    // Fill the tail with 4K mappings, too
    while (page_index < PAGES_PER_UVM_VA_BLOCK) {
        uvm_mutex_lock(&va_block->lock);
        status = uvm_pmm_sysmem_mappings_add_gpu_mapping(&g_reverse_map,
                                                         g_base_dma_addr + page_index * PAGE_SIZE,
                                                         uvm_va_block_cpu_page_address(va_block, page_index),
                                                         PAGE_SIZE,
                                                         va_block,
                                                         UVM_ID_CPU);
        uvm_mutex_unlock(&va_block->lock);
        TEST_CHECK_RET(status == NV_OK);

        ++page_index;
    }

    TEST_CHECK_RET(check_reverse_map_block_page(va_block, g_base_dma_addr, NULL) == NV_OK);
    TEST_CHECK_RET(check_reverse_map_block_batch(va_block, g_base_dma_addr, NULL) == NV_OK);

    uvm_mutex_lock(&va_block->lock);
    uvm_pmm_sysmem_mappings_merge_gpu_mappings(&g_reverse_map,
                                               g_base_dma_addr,
                                               uvm_va_block_size(va_block));
    uvm_mutex_unlock(&va_block->lock);

    TEST_CHECK_RET(check_reverse_map_block_page(va_block, g_base_dma_addr, NULL) == NV_OK);
    TEST_CHECK_RET(check_reverse_map_block_batch(va_block, g_base_dma_addr, NULL) == NV_OK);

    uvm_mutex_lock(&va_block->lock);
    uvm_pmm_sysmem_mappings_remove_gpu_mapping(&g_reverse_map, g_base_dma_addr);
    uvm_mutex_unlock(&va_block->lock);

    return status;
}

static NV_STATUS test_pmm_sysmem_reverse_map_remove_on_eviction(uvm_va_space_t *va_space, NvU64 addr)
{
    uvm_va_block_t *va_block;
    NV_STATUS status = uvm_va_block_find(va_space, addr, &va_block);

    if (status != NV_OK)
        return status;

    TEST_CHECK_RET(is_power_of_2(uvm_va_block_size(va_block)));

    uvm_mutex_lock(&va_block->lock);
    status = uvm_pmm_sysmem_mappings_add_gpu_mapping(&g_reverse_map,
                                                     g_base_dma_addr,
                                                     addr,
                                                     uvm_va_block_size(va_block),
                                                     va_block,
                                                     UVM_ID_CPU);
    uvm_mutex_unlock(&va_block->lock);

    uvm_mutex_lock(&va_block->lock);
    uvm_pmm_sysmem_mappings_remove_gpu_mapping(&g_reverse_map, g_base_dma_addr);
    uvm_mutex_unlock(&va_block->lock);

    TEST_CHECK_RET(status == NV_OK);

    uvm_pmm_sysmem_mappings_remove_gpu_mapping_on_eviction(&g_reverse_map, g_base_dma_addr);
    uvm_pmm_sysmem_mappings_remove_gpu_mapping_on_eviction(&g_reverse_map, g_base_dma_addr);

    return NV_OK;
}

static NV_STATUS test_pmm_sysmem_reverse_map(uvm_va_space_t *va_space, NvU64 addr1, NvU64 addr2)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_t *volta_gpu = NULL;
    uvm_gpu_t *gpu;

    // Find a GPU with support for access counters with physical address
    // notifications, since it is required to add or remove entries to the
    // reverse map.
    for_each_va_space_gpu(gpu, va_space) {
        if (gpu->parent->access_counters_can_use_physical_addresses) {
            // Initialize the reverse map.
            status = uvm_pmm_sysmem_mappings_init(gpu, &g_reverse_map);
            if (status != NV_OK)
                return status;

            volta_gpu = gpu;
            break;
        }
    }

    if (!volta_gpu)
        return NV_ERR_INVALID_DEVICE;

    status = test_pmm_sysmem_reverse_map_single_whole(va_space, addr1);

    if (status == NV_OK)
        status = test_pmm_sysmem_reverse_map_single_pattern(va_space, addr1);

    if (status == NV_OK)
        status = test_pmm_sysmem_reverse_map_many_blocks(va_space, addr2);

    if (status == NV_OK)
        status = test_pmm_sysmem_reverse_map_merge(va_space, addr1);

    if (status == NV_OK)
        status = test_pmm_sysmem_reverse_map_remove_on_eviction(va_space, addr1);

    uvm_pmm_sysmem_mappings_deinit(&g_reverse_map);

    return status;
}

NV_STATUS uvm_test_pmm_sysmem(UVM_TEST_PMM_SYSMEM_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space;

    va_space = uvm_va_space_get(filp);

    // Take the global lock to void interferences from different instances of
    // the test, since we use a bunch of global variables
    uvm_mutex_lock(&g_uvm_global.global_lock);
    uvm_va_space_down_write(va_space);

    status = test_pmm_sysmem_reverse_map(va_space, params->range_address1, params->range_address2);

    uvm_va_space_up_write(va_space);
    uvm_mutex_unlock(&g_uvm_global.global_lock);

    return status;
}

static NV_STATUS cpu_chunk_map_on_cpu(uvm_cpu_chunk_t *chunk, void **cpu_addr)
{
    struct page **pages;
    uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(chunk);
    size_t num_pages = uvm_cpu_chunk_num_pages(chunk);
    NV_STATUS status = NV_OK;

    UVM_ASSERT(cpu_addr);

    // Map the CPU chunk on the CPU.
    if (chunk_size > PAGE_SIZE) {
        size_t i;

        pages = uvm_kvmalloc(num_pages * sizeof(*pages));
        if (!pages)
            return NV_ERR_NO_MEMORY;

        for (i = 0; i < num_pages; i++)
            pages[i] = chunk->page + i;
    }
    else {
        pages = &chunk->page;
    }

    *cpu_addr = vmap(pages, num_pages, VM_MAP, PAGE_KERNEL);
    if (!*cpu_addr)
        status = NV_ERR_NO_MEMORY;

    if (chunk_size > PAGE_SIZE)
        uvm_kvfree(pages);

    return status;
}

static NV_STATUS test_cpu_chunk_mapping_access(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu)
{
    NvU64 dma_addr;
    uvm_gpu_address_t gpu_addr;
    uvm_push_t push;
    NvU32 *cpu_addr;
    uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(chunk);
    size_t i;
    NV_STATUS status = NV_OK;

    TEST_NV_CHECK_RET(cpu_chunk_map_on_cpu(chunk, (void **)&cpu_addr));
    memset(cpu_addr, 0, chunk_size);

    dma_addr = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu);
    gpu_addr = uvm_gpu_address_copy(gpu, uvm_gpu_phys_address(UVM_APERTURE_SYS, dma_addr));

    TEST_NV_CHECK_GOTO(uvm_push_begin_acquire(gpu->channel_manager,
                                              UVM_CHANNEL_TYPE_GPU_TO_CPU,
                                              NULL,
                                              &push,
                                              "GPU -> CPU {%s, %llx} %u bytes",
                                              uvm_gpu_address_aperture_string(gpu_addr),
                                              gpu_addr.address,
                                              chunk_size),
                       done);
    gpu->parent->ce_hal->memset_4(&push, gpu_addr, 0xdeadc0de, chunk_size);
    TEST_NV_CHECK_GOTO(uvm_push_end_and_wait(&push), done);

    for (i = 0; i < chunk_size / sizeof(*cpu_addr); i++) {
        if (cpu_addr[i] != 0xdeadc0de) {
            UVM_TEST_PRINT("GPU write of {%s, 0x%llx} %u bytes expected pattern 0x%08x, but offset %zu is 0x%08x\n",
                           uvm_gpu_address_aperture_string(gpu_addr),
                           gpu_addr.address,
                           chunk_size,
                           0xdeadc0de,
                           i * sizeof(*cpu_addr),
                           cpu_addr[i]);
            status = NV_ERR_INVALID_STATE;
            break;
        }
    }

done:
    vunmap(cpu_addr);
    return status;
}

static NV_STATUS test_cpu_chunk_alloc(uvm_chunk_size_t size,
                                      uvm_cpu_chunk_alloc_flags_t flags,
                                      int nid,
                                      uvm_cpu_chunk_t **out_chunk)
{
    uvm_cpu_chunk_t *chunk;
    NV_STATUS status = NV_OK;
    size_t i;

    UVM_ASSERT(out_chunk);

    // It is possible that the allocation fails due to lack of large pages
    // rather than an API issue, which will result in a false negative.
    // However, that should be very rare.
    TEST_NV_CHECK_RET(uvm_cpu_chunk_alloc(size, flags, nid, &chunk));

    // Check general state of the chunk:
    //   - chunk should be a physical chunk,
    //   - chunk should have the correct size,
    //   - chunk should have the correct number of base pages, and
    TEST_CHECK_GOTO(uvm_cpu_chunk_is_physical(chunk), done);
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_size(chunk) == size, done);
    TEST_CHECK_GOTO(uvm_cpu_chunk_num_pages(chunk) == size / PAGE_SIZE, done);

    // It is possible for the kernel to allocate a chunk on a NUMA node other
    // than the one requested. However, that should not be an issue with
    // sufficient memory on each NUMA node.
    if (nid != NUMA_NO_NODE)
        TEST_CHECK_GOTO(uvm_cpu_chunk_get_numa_node(chunk) == nid, done);

    if (flags & UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO) {
        NvU64 *cpu_addr;

        TEST_NV_CHECK_GOTO(cpu_chunk_map_on_cpu(chunk, (void **)&cpu_addr), done);
        for (i = 0; i < size / sizeof(*cpu_addr); i++)
            TEST_CHECK_GOTO(cpu_addr[i] == 0, done);
        vunmap(cpu_addr);
    }

    for (i = 0; i < size / PAGE_SIZE; i++) {
        if (flags & UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO)
            TEST_CHECK_GOTO(uvm_cpu_chunk_is_dirty(chunk, i), done);
        else
            TEST_CHECK_GOTO(!uvm_cpu_chunk_is_dirty(chunk, i), done);
    }

done:
    if (status == NV_OK)
        *out_chunk = chunk;
    else
        uvm_cpu_chunk_free(chunk);

    return status;
}

static NV_STATUS test_cpu_chunk_mapping_basic_verify(uvm_gpu_t *gpu,
                                                     uvm_cpu_chunk_alloc_flags_t flags,
                                                     uvm_chunk_size_t size)
{
    uvm_cpu_chunk_t *chunk;
    uvm_cpu_physical_chunk_t *phys_chunk;
    NvU64 dma_addr;
    NV_STATUS status = NV_OK;

    TEST_NV_CHECK_RET(test_cpu_chunk_alloc(size, flags, NUMA_NO_NODE, &chunk));
    phys_chunk = uvm_cpu_chunk_to_physical(chunk);

    // Check state of the physical chunk:
    //   - gpu_mappings.max_entries should be 1 (for the static entry),
    //   - gpu_mappings.dma_addrs_mask should be 0.
    //   - no GPU mapping address.
    TEST_CHECK_GOTO(phys_chunk->gpu_mappings.max_entries == 1, done);
    TEST_CHECK_GOTO(uvm_parent_processor_mask_get_gpu_count(&phys_chunk->gpu_mappings.dma_addrs_mask) == 0, done);
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu) == 0, done);
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu), done);

    // Test basic access.
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu), done);

    // Test double map is harmless.
    dma_addr = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu);
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu), done);
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu) == dma_addr, done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu), done);

    // Test unmap, remap.
    uvm_cpu_chunk_unmap_gpu(chunk, gpu);
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu) == 0, done);
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu), done);

done:
    // Test free with mapped GPUs still works.
    uvm_cpu_chunk_free(chunk);
    return status;;
}

static NV_STATUS test_cpu_chunk_mapping_basic(uvm_gpu_t *gpu, uvm_cpu_chunk_alloc_flags_t flags)
{
    uvm_chunk_sizes_mask_t chunk_sizes = uvm_cpu_chunk_get_allocation_sizes();
    uvm_chunk_size_t size;

    for_each_chunk_size(size, chunk_sizes)
        TEST_NV_CHECK_RET(test_cpu_chunk_mapping_basic_verify(gpu, flags, size));

    return NV_OK;
}

// TODO: Bug 4351121: This won't actually test anything until uvm_test
// enumerates multiple MIG instances.
static NV_STATUS test_cpu_chunk_mig(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    NV_STATUS status = NV_OK;
    uvm_cpu_chunk_t *chunk;
    uvm_cpu_physical_chunk_t *phys_chunk;
    NvU64 dma_addr_gpu0;

    UVM_ASSERT(gpu0->parent == gpu1->parent);

    TEST_NV_CHECK_RET(test_cpu_chunk_alloc(PAGE_SIZE, UVM_CPU_CHUNK_ALLOC_FLAGS_NONE, NUMA_NO_NODE, &chunk));
    phys_chunk = uvm_cpu_chunk_to_physical(chunk);

    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu0), done);
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu1), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu0), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu1), done);

    // MIG instances in the same physical GPU share the same DMA addresses.
    dma_addr_gpu0 = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu0);
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu1) == dma_addr_gpu0, done);

    // Unmapping one GPU shouldn't affect the other.
    uvm_cpu_chunk_unmap_gpu(chunk, gpu0);
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu0) == 0, done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu1), done);

done:
    uvm_cpu_chunk_free(chunk);
    return status;
}

static NV_STATUS test_cpu_chunk_mapping_array(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1, uvm_gpu_t *gpu2)
{
    NV_STATUS status = NV_OK;
    uvm_cpu_chunk_t *chunk;
    uvm_cpu_physical_chunk_t *phys_chunk;
    NvU64 dma_addr_gpu1;

    TEST_NV_CHECK_RET(test_cpu_chunk_alloc(PAGE_SIZE, UVM_CPU_CHUNK_ALLOC_FLAGS_NONE, NUMA_NO_NODE, &chunk));
    phys_chunk = uvm_cpu_chunk_to_physical(chunk);

    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu1), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu1), done);
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu2), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu1), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu2), done);
    dma_addr_gpu1 = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu1);
    uvm_cpu_chunk_unmap_gpu(chunk, gpu2);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu1), done);
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu0), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu0), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu1), done);

    // DMA mapping addresses for different GPUs live in different IOMMU spaces,
    // so it would be perfectly legal for them to have the same IOVA, and even
    // if they lived in the same space we freed GPU3's address so it would be
    // available for reuse.
    // What we need to ensure is that GPU2's address didn't change after we map
    // GPU1. It's true that we may get a false negative if both addresses
    // happened to alias and we had a bug in how the addresses are shifted in
    // the dense array, but that's better than intermittent failure.
    // Also note that multiple MIG instances in the same physical GPU share the
    // parent's physical DMA mapping.
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu1) == dma_addr_gpu1, done);

done:
    uvm_cpu_chunk_free(chunk);
    return status;
}

static NV_STATUS do_test_cpu_chunk_split_and_merge(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu)
{
    NV_STATUS status = NV_OK;
    uvm_chunk_size_t size = uvm_cpu_chunk_get_size(chunk);
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    size_t num_split_chunks;
    uvm_cpu_chunk_t **split_chunks;
    uvm_cpu_chunk_t *merged_chunk;
    uvm_chunk_size_t split_size;
    NvU64 phys_dma_addr;
    size_t map_chunk;
    size_t i;

    split_size = uvm_chunk_find_prev_size(alloc_sizes, size);
    UVM_ASSERT(split_size != UVM_CHUNK_SIZE_INVALID);
    num_split_chunks = size / split_size;
    split_chunks = uvm_kvmalloc_zero(num_split_chunks * sizeof(*split_chunks));

    if (!split_chunks)
        return NV_ERR_NO_MEMORY;

    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu), done_free);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu), done_free);
    uvm_cpu_chunk_unmap_gpu(chunk, gpu);

    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_split(chunk, split_chunks), done_free);
    TEST_CHECK_GOTO(nv_kref_read(&chunk->refcount) == num_split_chunks, done);

    for (i = 0; i < num_split_chunks; i++) {
        TEST_CHECK_GOTO(split_chunks[i], done);
        TEST_CHECK_GOTO(uvm_cpu_chunk_is_logical(split_chunks[i]), done);
        TEST_CHECK_GOTO(uvm_cpu_chunk_get_size(split_chunks[i]) == split_size, done);
        TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(split_chunks[i], gpu), done);
        TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(split_chunks[i], gpu), done);
    }

    // Test CPU chunk merging.
    merged_chunk = uvm_cpu_chunk_merge(split_chunks);
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_size(merged_chunk) == size, done_free);
    TEST_CHECK_GOTO(merged_chunk == chunk, done_free);
    TEST_CHECK_GOTO(nv_kref_read(&chunk->refcount) == 1, done_free);

    // Since all logical chunks were mapped, the entire merged chunk should
    // be accessible without needing to map it.
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(merged_chunk, gpu), done_free);

    // Test that GPU mappings are transferred after a split
    phys_dma_addr = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu);

    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_split(chunk, split_chunks), done_free);

    for (i = 0; i < num_split_chunks; i++) {
        NvU64 dma_addr;

        TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(split_chunks[i], gpu), done);
        dma_addr = uvm_cpu_chunk_get_gpu_phys_addr(split_chunks[i], gpu);
        TEST_CHECK_GOTO(dma_addr == phys_dma_addr + (i * split_size), done);
        uvm_cpu_chunk_unmap_gpu(split_chunks[i], gpu);
    }

    // Test that mapping one logical chunk does not affect others.
    map_chunk = num_split_chunks / 2;
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(split_chunks[map_chunk], gpu), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(split_chunks[map_chunk], gpu), done);

    for (i = 0; i < num_split_chunks; i++) {
        if (i != map_chunk)
            TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(split_chunks[i], gpu) == 0, done);
    }

    if (split_size > PAGE_SIZE) {
        for (i = 0; i < num_split_chunks; i++)
            TEST_NV_CHECK_GOTO(do_test_cpu_chunk_split_and_merge(split_chunks[i], gpu), done);
    }

    // Map all chunks before merging.
    for (i = 0; i < num_split_chunks; i++)
        TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(split_chunks[i], gpu), done);

    // Test CPU chunk merging.
    merged_chunk = uvm_cpu_chunk_merge(split_chunks);

    // At this point, all split chunks have been merged.
    num_split_chunks = 0;

    TEST_CHECK_GOTO(uvm_cpu_chunk_get_size(merged_chunk) == size, done_free);
    TEST_CHECK_GOTO(merged_chunk == chunk, done_free);

    // Since all logical chunks were mapped, the entire merged chunk should
    // be accessible without needing to map it.
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(merged_chunk, gpu), done_free);

done:
    for (i = 0; i < num_split_chunks; i++)
        uvm_cpu_chunk_free(split_chunks[i]);

done_free:
    uvm_kvfree(split_chunks);

    return status;
}

static NV_STATUS test_cpu_chunk_split_and_merge(uvm_gpu_t *gpu)
{
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    uvm_chunk_size_t size;

    size = uvm_chunk_find_next_size(alloc_sizes, PAGE_SIZE);
    for_each_chunk_size_from(size, alloc_sizes) {
        uvm_cpu_chunk_t *chunk;
        NV_STATUS status;

        TEST_NV_CHECK_RET(test_cpu_chunk_alloc(size, UVM_CPU_CHUNK_ALLOC_FLAGS_NONE, NUMA_NO_NODE, &chunk));
        status = do_test_cpu_chunk_split_and_merge(chunk, gpu);
        uvm_cpu_chunk_free(chunk);

        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static NV_STATUS do_test_cpu_chunk_split_and_merge_2(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    NV_STATUS status = NV_OK;
    uvm_chunk_size_t size = uvm_cpu_chunk_get_size(chunk);
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    size_t num_split_chunks;
    uvm_cpu_chunk_t **split_chunks;
    uvm_cpu_chunk_t *merged_chunk;
    uvm_chunk_size_t split_size;
    size_t i;

    split_size = uvm_chunk_find_prev_size(alloc_sizes, size);
    UVM_ASSERT(split_size != UVM_CHUNK_SIZE_INVALID);
    num_split_chunks = size / split_size;
    split_chunks = uvm_kvmalloc_zero(num_split_chunks * sizeof(*split_chunks));

    if (!split_chunks)
        return NV_ERR_NO_MEMORY;

    // Map both GPUs.
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu0), done_free);
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(chunk, gpu1), done_free);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu0), done_free);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(chunk, gpu1), done_free);

    // Then split.
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_split(chunk, split_chunks), done_free);
    TEST_CHECK_GOTO(nv_kref_read(&chunk->refcount) == num_split_chunks, done);

    // Unmap gpu0 from all split chunks.
    for (i = 0; i < num_split_chunks; i++) {
        TEST_CHECK_GOTO(split_chunks[i], done);
        TEST_CHECK_GOTO(uvm_cpu_chunk_is_logical(split_chunks[i]), done);
        TEST_CHECK_GOTO(uvm_cpu_chunk_get_size(split_chunks[i]) == split_size, done);
        uvm_cpu_chunk_unmap_gpu(split_chunks[i], gpu0);
        TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(split_chunks[i], gpu0) == 0, done);

        // Test that gpu1 still has access.
        TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(split_chunks[i], gpu1), done);
    }

    // Test CPU chunk merging.
    merged_chunk = uvm_cpu_chunk_merge(split_chunks);
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_size(merged_chunk) == size, done_free);
    TEST_CHECK_GOTO(merged_chunk == chunk, done_free);
    TEST_CHECK_GOTO(nv_kref_read(&chunk->refcount) == 1, done_free);

    // Since all logical chunks were mapped, the entire merged chunk should
    // be accessible without needing to map it.
    TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(merged_chunk, gpu0) == 0, done_free);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(merged_chunk, gpu1), done_free);

    // Unmap gpu1 so we start with a fully unmapped physical chunk.
    uvm_cpu_chunk_unmap_gpu(chunk, gpu1);

    // Split the physical chunk.
    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_split(chunk, split_chunks), done_free);

    // Now map everything.
    for (i = 0; i < num_split_chunks; i++) {
        TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(split_chunks[i], gpu0), done);
        TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(split_chunks[i], gpu1), done);
    }

    // Test CPU chunk merging with everything mapped.
    merged_chunk = uvm_cpu_chunk_merge(split_chunks);

    // At this point, all split chunks have been merged.
    num_split_chunks = 0;

    TEST_CHECK_GOTO(uvm_cpu_chunk_get_size(merged_chunk) == size, done_free);
    TEST_CHECK_GOTO(merged_chunk == chunk, done_free);

    // Since all logical chunks were mapped, the entire merged chunk should
    // be accessible without needing to map it.
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(merged_chunk, gpu0), done_free);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_access(merged_chunk, gpu1), done_free);

done:
    for (i = 0; i < num_split_chunks; i++)
        uvm_cpu_chunk_free(split_chunks[i]);

done_free:
    uvm_kvfree(split_chunks);

    return status;
}

static NV_STATUS test_cpu_chunk_split_and_merge_2(uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    uvm_chunk_size_t size;

    size = uvm_chunk_find_next_size(alloc_sizes, PAGE_SIZE);
    for_each_chunk_size_from(size, alloc_sizes) {
        uvm_cpu_chunk_t *chunk;
        NV_STATUS status;

        // It is possible that the allocation fails due to lack of large pages
        // rather than an API issue, which will result in a false negative.
        // However, that should be very rare.
        TEST_NV_CHECK_RET(test_cpu_chunk_alloc(size, UVM_CPU_CHUNK_ALLOC_FLAGS_NONE, NUMA_NO_NODE, &chunk));
        status = do_test_cpu_chunk_split_and_merge_2(chunk, gpu0, gpu1);
        uvm_cpu_chunk_free(chunk);

        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static NV_STATUS test_cpu_chunk_dirty_split(uvm_cpu_chunk_t *chunk)
{
    uvm_chunk_size_t size = uvm_cpu_chunk_get_size(chunk);
    uvm_chunk_size_t split_size;
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    uvm_cpu_chunk_t **split_chunks;
    uvm_cpu_chunk_t *merged_chunk;
    size_t num_pages = size / PAGE_SIZE;
    size_t num_split_chunks;
    size_t num_split_chunk_pages;
    size_t i;
    NV_STATUS status = NV_OK;

    split_size = uvm_chunk_find_prev_size(alloc_sizes, size);
    UVM_ASSERT(split_size != UVM_CHUNK_SIZE_INVALID);
    num_split_chunks = size / split_size;
    num_split_chunk_pages = split_size / PAGE_SIZE;
    split_chunks = uvm_kvmalloc_zero(num_split_chunks * sizeof(*split_chunks));
    if (!split_chunks)
        return NV_ERR_NO_MEMORY;

    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_split(chunk, split_chunks), done_free);

    // The parent chunk had only the even pages set as dirty. Make sure
    // that's still the case after the split.
    for (i = 0; i < num_split_chunks; i++) {
        uvm_page_index_t chunk_page;

        for (chunk_page = 0; chunk_page < num_split_chunk_pages; chunk_page++) {
            if (((i * num_split_chunk_pages) + chunk_page) % 2)
                TEST_CHECK_GOTO(!uvm_cpu_chunk_is_dirty(split_chunks[i], chunk_page), done);
            else
                TEST_CHECK_GOTO(uvm_cpu_chunk_is_dirty(split_chunks[i], chunk_page), done);
        }
    }

    if (split_size > PAGE_SIZE) {
        for (i = 0; i < num_split_chunks; i++)
            TEST_NV_CHECK_GOTO(test_cpu_chunk_dirty_split(split_chunks[i]), done);
    }

    merged_chunk = uvm_cpu_chunk_merge(split_chunks);
    num_split_chunks = 0;
    for (i = 0; i < num_pages; i++) {
        if (i % 2)
            TEST_CHECK_GOTO(!uvm_cpu_chunk_is_dirty(merged_chunk, i), done_free);
        else
            TEST_CHECK_GOTO(uvm_cpu_chunk_is_dirty(merged_chunk, i), done_free);
    }

done:
    for (i = 0; i < num_split_chunks; i++)
        uvm_cpu_chunk_free(split_chunks[i]);

done_free:
    uvm_kvfree(split_chunks);
    return status;
}

static NV_STATUS test_cpu_chunk_dirty(uvm_gpu_t *gpu)
{
    NV_STATUS status = NV_OK;
    uvm_cpu_chunk_t *chunk;
    uvm_chunk_size_t size;
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    size_t i;

    for_each_chunk_size(size, alloc_sizes) {
        uvm_cpu_physical_chunk_t *phys_chunk;
        size_t num_pages;

        TEST_NV_CHECK_RET(test_cpu_chunk_alloc(size, UVM_CPU_CHUNK_ALLOC_FLAGS_NONE, NUMA_NO_NODE, &chunk));
        phys_chunk = uvm_cpu_chunk_to_physical(chunk);
        num_pages = uvm_cpu_chunk_num_pages(chunk);

        for (i = 0; i < num_pages; i++)
            TEST_CHECK_GOTO(!uvm_cpu_chunk_is_dirty(chunk, i), done);

        if (size > PAGE_SIZE)
            TEST_CHECK_GOTO(bitmap_empty(phys_chunk->dirty_bitmap, num_pages), done);

        uvm_cpu_chunk_free(chunk);

        TEST_NV_CHECK_RET(test_cpu_chunk_alloc(size, UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO, NUMA_NO_NODE, &chunk));
        phys_chunk = uvm_cpu_chunk_to_physical(chunk);
        num_pages = uvm_cpu_chunk_num_pages(chunk);

        // Allocating the chunk with UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO will set the
        // entire chunk as dirty.
        for (i = 0; i < num_pages; i++)
            TEST_CHECK_GOTO(uvm_cpu_chunk_is_dirty(chunk, i), done);

        if (size > PAGE_SIZE)
            TEST_CHECK_GOTO(bitmap_full(phys_chunk->dirty_bitmap, num_pages), done);

        // For chunks larger than PAGE_SIZE, marking individual pages in a
        // physical chunk should not affect the entire chunk.
        for (i = 0; i < num_pages; i++) {
            uvm_cpu_chunk_mark_clean(chunk, i);
            TEST_CHECK_GOTO(!uvm_cpu_chunk_is_dirty(chunk, i), done);
            if (size > PAGE_SIZE) {
                TEST_CHECK_GOTO(bitmap_empty(phys_chunk->dirty_bitmap, i + 1), done);
                TEST_CHECK_GOTO(bitmap_weight(phys_chunk->dirty_bitmap, num_pages) == num_pages - (i + 1), done);
            }
        }

        for (i = 0; i < num_pages; i++) {
            uvm_cpu_chunk_mark_dirty(chunk, i);
            TEST_CHECK_GOTO(uvm_cpu_chunk_is_dirty(chunk, i), done);
            if (size > PAGE_SIZE) {
                TEST_CHECK_GOTO(bitmap_full(phys_chunk->dirty_bitmap, i + 1), done);
                TEST_CHECK_GOTO(bitmap_weight(phys_chunk->dirty_bitmap, num_pages) == i + 1, done);
            }
        }

        // Leave only even pages as dirty
        for (i = 1; i < num_pages; i += 2)
            uvm_cpu_chunk_mark_clean(chunk, i);

        for (i = 0; i < num_pages; i++) {
            if (i % 2) {
                TEST_CHECK_GOTO(!uvm_cpu_chunk_is_dirty(chunk, i), done);
                if (size > PAGE_SIZE)
                    TEST_CHECK_GOTO(!test_bit(i, phys_chunk->dirty_bitmap), done);
            }
            else {
                TEST_CHECK_GOTO(uvm_cpu_chunk_is_dirty(chunk, i), done);
                if (size > PAGE_SIZE)
                    TEST_CHECK_GOTO(test_bit(i, phys_chunk->dirty_bitmap), done);
            }
        }

        if (size > PAGE_SIZE)
            TEST_NV_CHECK_GOTO(test_cpu_chunk_dirty_split(chunk), done);

done:
        uvm_cpu_chunk_free(chunk);

        if (status != NV_OK)
            break;
    }

    return status;
}

static NV_STATUS do_test_cpu_chunk_free(uvm_cpu_chunk_t *chunk,
                                        uvm_va_space_t *va_space,
                                        const uvm_processor_mask_t *test_gpus)
{
    NV_STATUS status = NV_OK;
    uvm_cpu_chunk_t **split_chunks;
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    size_t size = uvm_cpu_chunk_get_size(chunk);
    uvm_chunk_size_t split_size = uvm_chunk_find_prev_size(alloc_sizes, size);
    size_t num_split_chunks = size / split_size;
    uvm_gpu_t *gpu;
    size_t i;
    size_t j;

    split_chunks = uvm_kvmalloc_zero(num_split_chunks * sizeof(*split_chunks));
    if (!split_chunks) {
        UVM_TEST_PRINT("Failed to allocate split chunk array memory");
        status = NV_ERR_NO_MEMORY;
        goto done_free;
    }

    TEST_NV_CHECK_GOTO(uvm_cpu_chunk_split(chunk, split_chunks), done_free);

    // The caller does not free the input chunk.
    // So, we have to do it in this function. However, beyond this point
    // the input chunk will be freed by freeing the split chunks.
    chunk = NULL;

    // Map every other chunk.
    // The call to uvm_cpu_chunk_unmap_gpu() is here in case this
    // is part of a double split (see below). In that case, the parent chunk
    // would be either mapped or unmapped.
    //
    // If it is mapped, we have to unmap the subchunks in
    // order for the mapping check below to succeed. If it is unmapped, the
    // calls are noops.
    for (i = 0; i < num_split_chunks; i++) {
        for_each_va_space_gpu_in_mask(gpu, va_space, test_gpus) {
            if (i & (1 << uvm_id_gpu_index(gpu->id)))
                TEST_NV_CHECK_GOTO(uvm_cpu_chunk_map_gpu(split_chunks[i], gpu), done);
            else
                uvm_cpu_chunk_unmap_gpu(split_chunks[i], gpu);
        }
    }

    // Do a double split if we can
    if (split_size > PAGE_SIZE) {
        size_t chunk_to_be_resplit;

        // Test an even (mapped) chunk.
        chunk_to_be_resplit = num_split_chunks / 2;
        TEST_NV_CHECK_GOTO(do_test_cpu_chunk_free(split_chunks[chunk_to_be_resplit], va_space, test_gpus), done);

        // The chunk would have been freed by do_test_cpu_chunk_free().
        split_chunks[chunk_to_be_resplit] = NULL;

        // Test an odd (unmapped) chunk.
        chunk_to_be_resplit += 1;
        TEST_NV_CHECK_GOTO(do_test_cpu_chunk_free(split_chunks[chunk_to_be_resplit], va_space, test_gpus), done);
        split_chunks[chunk_to_be_resplit] = NULL;
    }

    for (i = 0; i < num_split_chunks; i++) {
        if (!split_chunks[i])
            continue;

        uvm_cpu_chunk_free(split_chunks[i]);
        split_chunks[i] = NULL;

        for (j = i + 1; j < num_split_chunks; j++) {
            if (!split_chunks[j])
                continue;

            TEST_CHECK_GOTO(uvm_cpu_chunk_is_logical(split_chunks[j]), done);
            TEST_CHECK_GOTO(uvm_cpu_chunk_get_size(split_chunks[j]) == split_size, done);
            for_each_va_space_gpu_in_mask(gpu, va_space, test_gpus) {
                if (j & (1 << uvm_id_gpu_index(gpu->id)))
                    TEST_CHECK_GOTO(uvm_cpu_chunk_get_gpu_phys_addr(split_chunks[j], gpu), done);
                else
                    TEST_CHECK_GOTO(!uvm_cpu_chunk_get_gpu_phys_addr(split_chunks[j], gpu), done);
            }
        }
    }

done:
    for (i = 0; i < num_split_chunks; i++) {
        if (split_chunks[i])
            uvm_cpu_chunk_free(split_chunks[i]);
    }

done_free:
    if (chunk)
        uvm_cpu_chunk_free(chunk);

    uvm_kvfree(split_chunks);
    return status;
}

static NV_STATUS test_cpu_chunk_free(uvm_va_space_t *va_space,
                                     const uvm_processor_mask_t *test_gpus)
{
    uvm_cpu_chunk_t *chunk;
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    size_t size = uvm_chunk_find_next_size(alloc_sizes, PAGE_SIZE);

    for_each_chunk_size_from(size, alloc_sizes) {
        TEST_NV_CHECK_RET(test_cpu_chunk_alloc(size, UVM_CPU_CHUNK_ALLOC_FLAGS_NONE, NUMA_NO_NODE, &chunk));
        TEST_NV_CHECK_RET(do_test_cpu_chunk_free(chunk, va_space, test_gpus));
    }

    return NV_OK;
}

static NV_STATUS test_cpu_chunk_numa_alloc(uvm_va_space_t *va_space)
{
    uvm_cpu_chunk_t *chunk;
    uvm_chunk_sizes_mask_t alloc_sizes = uvm_cpu_chunk_get_allocation_sizes();
    size_t size;

    for_each_chunk_size(size, alloc_sizes) {
        int nid;

        for_each_possible_uvm_node(nid) {
            // Do not test CPU allocation on nodes that have no memory or CPU
            if (!node_state(nid, N_MEMORY) || !node_state(nid, N_CPU))
                continue;

            TEST_NV_CHECK_RET(test_cpu_chunk_alloc(size, UVM_CPU_CHUNK_ALLOC_FLAGS_NONE, nid, &chunk));
            uvm_cpu_chunk_free(chunk);
        }
    }

    return NV_OK;
}

static uvm_gpu_t *find_first_parent_gpu(const uvm_processor_mask_t *test_gpus,
                                        uvm_va_space_t *va_space)
{
    return uvm_processor_mask_find_first_va_space_gpu(test_gpus, va_space);
}

static uvm_gpu_t *find_next_parent_gpu(const uvm_processor_mask_t *test_gpus,
                                       uvm_va_space_t *va_space,
                                       uvm_gpu_t *gpu)
{
    uvm_gpu_t *next_gpu = gpu;

    while (next_gpu) {
        next_gpu = uvm_processor_mask_find_next_va_space_gpu(test_gpus, va_space, next_gpu);
        if (!next_gpu || next_gpu->parent != gpu->parent)
            break;
    }

    return next_gpu;
}

static void find_shared_gpu_pair(const uvm_processor_mask_t *test_gpus,
                                 uvm_va_space_t *va_space,
                                 uvm_gpu_t **out_gpu0,
                                 uvm_gpu_t **out_gpu1)
{
    uvm_gpu_t *gpu0 = uvm_processor_mask_find_first_va_space_gpu(test_gpus, va_space);
    uvm_gpu_t *gpu1 = uvm_processor_mask_find_next_va_space_gpu(test_gpus, va_space, gpu0);

    while (gpu1) {
        if (gpu0->parent == gpu1->parent) {
            *out_gpu0 = gpu0;
            *out_gpu1 = gpu1;
            return;
        }

        gpu0 = gpu1;
        gpu1 = uvm_processor_mask_find_next_va_space_gpu(test_gpus, va_space, gpu0);
    }

    *out_gpu0 = NULL;
    *out_gpu1 = NULL;
}

NV_STATUS uvm_test_cpu_chunk_api(UVM_TEST_CPU_CHUNK_API_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_processor_mask_t *test_gpus;
    uvm_gpu_t *gpu;
    NV_STATUS status = NV_OK;

    test_gpus = uvm_processor_mask_cache_alloc();
    if (!test_gpus)
        return NV_ERR_NO_MEMORY;

    uvm_va_space_down_read(va_space);
    uvm_processor_mask_and(test_gpus, &va_space->registered_gpus, &va_space->accessible_from[uvm_id_value(UVM_ID_CPU)]);

    for_each_va_space_gpu_in_mask(gpu, va_space, test_gpus) {
        TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_basic(gpu, UVM_CPU_CHUNK_ALLOC_FLAGS_NONE), done);
        TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_basic(gpu, UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO), done);
        TEST_NV_CHECK_GOTO(test_cpu_chunk_split_and_merge(gpu), done);
        TEST_NV_CHECK_GOTO(test_cpu_chunk_dirty(gpu), done);
    }

    TEST_NV_CHECK_GOTO(test_cpu_chunk_free(va_space, test_gpus), done);
    TEST_NV_CHECK_GOTO(test_cpu_chunk_numa_alloc(va_space), done);

    if (uvm_processor_mask_get_gpu_count(test_gpus) >= 2) {
        uvm_gpu_t *gpu2, *gpu3 = NULL;

        // Look for a pair of GPUs that don't share a common parent.
        gpu = find_first_parent_gpu(test_gpus, va_space);
        gpu2 = find_next_parent_gpu(test_gpus, va_space, gpu);
        if (gpu2) {
            TEST_NV_CHECK_GOTO(test_cpu_chunk_split_and_merge_2(gpu, gpu2), done);

            // Look for a third physical GPU.
            gpu3 = find_next_parent_gpu(test_gpus, va_space, gpu2);

            if (gpu3)
                TEST_NV_CHECK_GOTO(test_cpu_chunk_mapping_array(gpu, gpu2, gpu3), done);
        }

        // Look for a pair of GPUs that share a common parent.
        find_shared_gpu_pair(test_gpus, va_space, &gpu, &gpu2);
        if (gpu) {
            // Test MIG instances within the same parent GPU.
            TEST_NV_CHECK_GOTO(test_cpu_chunk_split_and_merge_2(gpu, gpu2), done);
            TEST_NV_CHECK_GOTO(test_cpu_chunk_mig(gpu, gpu2), done);
        }
    }

done:
    uvm_va_space_up_read(va_space);
    uvm_processor_mask_cache_free(test_gpus);
    return status;
}
