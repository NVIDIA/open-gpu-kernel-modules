/*******************************************************************************
    Copyright (c) 2017-2019 NVIDIA Corporation

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

// Pre-allocated array used for dma-to-virt translations
static uvm_reverse_map_t g_sysmem_translations[PAGES_PER_UVM_VA_BLOCK];

// We use our own separate reverse map to easily specify contiguous DMA
// address ranges
static uvm_pmm_sysmem_mappings_t g_reverse_map;

static uvm_gpu_t *g_volta_plus_gpu;

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
    uvm_gpu_t *gpu;

    g_volta_plus_gpu = NULL;

    // Find a GPU with support for access counters, since it is required to add
    // or remove entries to the reverse map.
    for_each_va_space_gpu(gpu, va_space) {
        if (gpu->parent->access_counters_supported) {
            // Initialize the reverse map.
            status = uvm_pmm_sysmem_mappings_init(gpu, &g_reverse_map);
            if (status != NV_OK)
                return status;

            g_volta_plus_gpu = gpu;
            break;
        }
    }

    if (!g_volta_plus_gpu)
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

    if (uvm_pmm_sysmem_mappings_indirect_supported()) {
        status = test_pmm_sysmem_reverse_map(va_space, params->range_address1, params->range_address2);
    }
    else {
        UVM_TEST_PRINT("Skipping kernel_driver_pmm_sysmem test due to lack of support for radix_tree_replace_slot in Linux 4.10");
        status = NV_OK;
    }

    uvm_va_space_up_write(va_space);
    uvm_mutex_unlock(&g_uvm_global.global_lock);

    return status;
}
