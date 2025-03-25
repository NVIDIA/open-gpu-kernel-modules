/*******************************************************************************
    Copyright (c) 2017-2025 NVIDIA Corporation

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

    UVM_ASSERT(uvm_gpus_are_smc_peers(gpu0, gpu1));

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
        if (uvm_gpus_are_smc_peers(gpu0, gpu1)) {
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
