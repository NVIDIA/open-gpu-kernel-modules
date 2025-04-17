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
#include "uvm_hal.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_kvmalloc.h"
#include "uvm_mem.h"
#include "uvm_push.h"
#include "uvm_conf_computing.h"
#include "uvm_test.h"
#include "uvm_test_ioctl.h"
#include "uvm_va_space.h"

static const size_t sysmem_alloc_sizes[] = { 1, PAGE_SIZE - 1, PAGE_SIZE, 7 * PAGE_SIZE };

static NvU64 first_page_size(NvU64 page_sizes)
{
    return page_sizes & ~(page_sizes - 1);
}

#define for_each_page_size(page_size, page_sizes)                                   \
    for (page_size = first_page_size(page_sizes);                                   \
         page_size;                                                                 \
         page_size = first_page_size((page_sizes) & ~(page_size | (page_size - 1))))

static inline NV_STATUS mem_alloc_sysmem_and_map_cpu_kernel(NvU64 size, uvm_gpu_t *gpu, uvm_mem_t **sys_mem)
{
    if (g_uvm_global.conf_computing_enabled)
        return uvm_mem_alloc_sysmem_dma_and_map_cpu_kernel(size, gpu, current->mm, sys_mem);

    return uvm_mem_alloc_sysmem_and_map_cpu_kernel(size, current->mm, sys_mem);
}

static NV_STATUS check_accessible_from_gpu(uvm_gpu_t *gpu, uvm_mem_t *mem)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *sys_mem = NULL;
    uvm_push_t push;
    NvU64 *sys_verif;
    size_t i;
    NvU64 verif_size = mem->size;
    NvU64 offset;
    uvm_tracker_t tracker = UVM_TRACKER_INIT();

    verif_size = UVM_ALIGN_UP(verif_size, sizeof(*sys_verif));

    UVM_ASSERT(uvm_mem_physical_size(mem) >= verif_size);
    UVM_ASSERT(verif_size >= sizeof(*sys_verif));

    TEST_NV_CHECK_GOTO(mem_alloc_sysmem_and_map_cpu_kernel(verif_size, gpu, &sys_mem), done);
    TEST_NV_CHECK_GOTO(uvm_mem_map_gpu_kernel(sys_mem, gpu), done);

    sys_verif = (NvU64*)uvm_mem_get_cpu_addr_kernel(sys_mem);

    for (i = 0; i < verif_size / sizeof(*sys_verif); ++i)
        sys_verif[i] = mem->size + i;

    // Copy from sys_mem to mem (in mem->page_size chunks) using:
    //   - virtual access for sys_mem
    //   - physical access for mem, unless the channel only supports virtual
    //     addressing
    for (offset = 0; offset < verif_size; offset += mem->chunk_size) {
        uvm_gpu_address_t sys_mem_gpu_address, mem_gpu_address;
        size_t size_this_time = min((NvU64)mem->chunk_size, verif_size - offset);

        TEST_NV_CHECK_GOTO(uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_CPU_TO_GPU, &push, " "), done);

        sys_mem_gpu_address = uvm_mem_gpu_address_virtual_kernel(sys_mem, gpu);
        sys_mem_gpu_address.address += offset;

        if (uvm_channel_is_privileged(push.channel)) {
            mem_gpu_address = uvm_mem_gpu_address_copy(mem, gpu, offset, size_this_time);
        }
        else {
            mem_gpu_address = uvm_mem_gpu_address_virtual_kernel(mem, gpu);
            mem_gpu_address.address += offset;
        }

        uvm_push_set_description(&push,
                                 "Memcopy %zu bytes from virtual sys_mem 0x%llx to %s mem 0x%llx [mem loc: %s, page "
                                 "size: %llu]",
                                 size_this_time,
                                 sys_mem_gpu_address.address,
                                 mem_gpu_address.is_virtual ? "virtual" : "physical",
                                 mem_gpu_address.address,
                                 uvm_mem_is_sysmem(mem) ? "sys" : "vid",
                                 mem->chunk_size);

        gpu->parent->ce_hal->memcopy(&push, mem_gpu_address, sys_mem_gpu_address, size_this_time);

        uvm_push_end(&push);
        TEST_NV_CHECK_GOTO(uvm_tracker_add_push(&tracker, &push), done);
    }

    TEST_NV_CHECK_GOTO(uvm_tracker_wait(&tracker), done);

    memset(sys_verif, 0, verif_size);

    // Copy back to sys_mem from mem (in sys_mem->page_size chunks) using:
    //   - physical access for sys_mem, unless the channel only supports virtual
    //     addressing
    //   - virtual access for mem
    for (offset = 0; offset < verif_size; offset += sys_mem->chunk_size) {
        uvm_gpu_address_t mem_gpu_address, sys_mem_gpu_address;
        size_t size_this_time = min((NvU64)sys_mem->chunk_size, verif_size - offset);

        TEST_NV_CHECK_GOTO(uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_GPU_TO_CPU, &push, " "), done);

        mem_gpu_address = uvm_mem_gpu_address_virtual_kernel(mem, gpu);
        mem_gpu_address.address += offset;

        if (uvm_channel_is_privileged(push.channel)) {
            sys_mem_gpu_address = uvm_mem_gpu_address_copy(sys_mem, gpu, offset, size_this_time);
        }
        else {
            sys_mem_gpu_address = uvm_mem_gpu_address_virtual_kernel(sys_mem, gpu);
            sys_mem_gpu_address.address += offset;
        }

        uvm_push_set_description(&push,
                                 "Memcopy %zd bytes from virtual mem 0x%llx to %s sys_mem 0x%llx",
                                 size_this_time,
                                 mem_gpu_address.address,
                                 sys_mem_gpu_address.is_virtual ? "virtual" : "physical",
                                 sys_mem_gpu_address.address);

        gpu->parent->ce_hal->memcopy(&push, sys_mem_gpu_address, mem_gpu_address, size_this_time);

        uvm_push_end(&push);
        TEST_NV_CHECK_GOTO(uvm_tracker_add_push(&tracker, &push), done);
    }

    TEST_NV_CHECK_GOTO(uvm_tracker_wait(&tracker), done);

    for (i = 0; i < verif_size / sizeof(*sys_verif); ++i) {
        if (sys_verif[i] != mem->size + i) {
            UVM_TEST_PRINT("Verif failed for %zd = 0x%llx instead of 0x%llx, verif_size=0x%llx mem(size=0x%llx, page_size=%llu, processor=%u)\n",
                           i,
                           sys_verif[i],
                           (NvU64)(verif_size + i),
                           verif_size,
                           mem->size,
                           mem->chunk_size,
                           uvm_mem_is_vidmem(mem) ? uvm_id_value(mem->backing_gpu->id) : UVM_ID_CPU_VALUE);
            status = NV_ERR_INVALID_STATE;
            goto done;
        }
    }

done:
    (void)uvm_tracker_wait(&tracker);
    uvm_tracker_deinit(&tracker);
    uvm_mem_free(sys_mem);

    return status;
}

static NV_STATUS test_map_gpu(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    NvU64 gpu_va;

    TEST_NV_CHECK_RET(uvm_mem_map_gpu_kernel(mem, gpu));
    TEST_CHECK_RET(uvm_mem_mapped_on_gpu_kernel(mem, gpu));
    TEST_CHECK_RET(!uvm_mem_mapped_on_gpu_user(mem, gpu));

    gpu_va = uvm_mem_get_gpu_va_kernel(mem, gpu);
    TEST_CHECK_RET(gpu_va >= gpu->parent->uvm_mem_va_base);
    TEST_CHECK_RET(gpu_va + uvm_mem_physical_size(mem) <= gpu->parent->uvm_mem_va_base + gpu->parent->uvm_mem_va_size);

    // Mapping if already mapped is OK
    TEST_NV_CHECK_RET(uvm_mem_map_gpu_kernel(mem, gpu));

    // Unmap
    uvm_mem_unmap_gpu_kernel(mem, gpu);
    TEST_CHECK_RET(!uvm_mem_mapped_on_gpu_kernel(mem, gpu));

    // Unmapping an unmapped memory is OK
    uvm_mem_unmap_gpu_kernel(mem, gpu);
    uvm_mem_unmap_gpu_user(mem, gpu);

    // Map again
    TEST_NV_CHECK_RET(uvm_mem_map_gpu_kernel(mem, gpu));

    // Should get the same VA
    TEST_CHECK_RET(gpu_va == uvm_mem_get_gpu_va_kernel(mem, gpu));

    return check_accessible_from_gpu(gpu, mem);
}

static NV_STATUS test_map_cpu(uvm_mem_t *mem)
{
    char *cpu_addr;

    if (uvm_mem_is_vidmem(mem))
        UVM_ASSERT(mem->backing_gpu->mem_info.numa.enabled);

    // Map
    TEST_NV_CHECK_RET(uvm_mem_map_cpu_kernel(mem));
    TEST_CHECK_RET(uvm_mem_mapped_on_cpu_kernel(mem));
    TEST_CHECK_RET(!uvm_mem_mapped_on_cpu_user(mem));
    TEST_CHECK_RET(uvm_mem_get_cpu_addr_kernel(mem) != NULL);

    // Mapping if already mapped is OK
    TEST_NV_CHECK_RET(uvm_mem_map_cpu_kernel(mem));

    // Unmap
    uvm_mem_unmap_cpu_kernel(mem);
    TEST_CHECK_RET(!uvm_mem_mapped_on_cpu_kernel(mem));

    // Unmapping an unmapped memory is OK
    uvm_mem_unmap_cpu_kernel(mem);
    uvm_mem_unmap_cpu_user(mem);

    // Map again
    TEST_NV_CHECK_RET(uvm_mem_map_cpu_kernel(mem));

    cpu_addr = uvm_mem_get_cpu_addr_kernel(mem);
    TEST_CHECK_RET(cpu_addr != NULL);

    memset(cpu_addr, 3, mem->size);

    return NV_OK;
}

static NV_STATUS test_alloc_sysmem(uvm_va_space_t *va_space, NvU64 page_size, size_t size, uvm_mem_t **mem_out)
{
    NV_STATUS status;
    uvm_mem_t *mem;
    uvm_gpu_t *gpu;
    uvm_mem_alloc_params_t params = { 0 };

    params.size = size;
    params.page_size = page_size;
    params.mm = current->mm;

    TEST_NV_CHECK_GOTO(uvm_mem_alloc(&params, &mem), error);

    TEST_NV_CHECK_GOTO(test_map_cpu(mem), error);

    for_each_va_space_gpu(gpu, va_space)
        TEST_NV_CHECK_GOTO(test_map_gpu(mem, gpu), error);

    *mem_out = mem;

    return NV_OK;

error:
    uvm_mem_free(mem);

    return status;
}

static NV_STATUS test_alloc_sysmem_dma(uvm_va_space_t *va_space, uvm_gpu_t *dma_owner, size_t size, uvm_mem_t **mem_out)
{
    NV_STATUS status;
    uvm_mem_t *mem;
    uvm_gpu_t *gpu;
    uvm_mem_alloc_params_t params = { 0 };

    params.size = size;
    params.page_size = PAGE_SIZE;
    params.dma_owner = dma_owner;
    params.mm = current->mm;

    status = uvm_mem_alloc(&params, &mem);
    TEST_CHECK_GOTO(status == NV_OK, error);

    TEST_CHECK_GOTO(test_map_cpu(mem) == NV_OK, error);

    // Mapping twice on the dma_owner is OK.
    for_each_va_space_gpu(gpu, va_space)
        TEST_NV_CHECK_GOTO(test_map_gpu(mem, gpu), error);

    *mem_out = mem;

    return NV_OK;

error:
    uvm_mem_free(mem);
    return status;
}

static NV_STATUS test_alloc_vidmem(uvm_gpu_t *gpu, NvU64 page_size, size_t size, uvm_mem_t **mem_out)
{
    NV_STATUS status;
    uvm_mem_t *mem;
    uvm_mem_alloc_params_t params = { 0 };

    params.backing_gpu = gpu;
    params.page_size = page_size;
    params.size = size;
    params.mm = current->mm;

    status = uvm_mem_alloc(&params, &mem);
    TEST_CHECK_GOTO(status == NV_OK, error);

    if (page_size == UVM_PAGE_SIZE_DEFAULT) {
        if (gpu->mem_info.numa.enabled)
            TEST_CHECK_GOTO(mem->chunk_size >= PAGE_SIZE && mem->chunk_size <= max(size, (size_t)PAGE_SIZE), error);
        else
            TEST_CHECK_GOTO(mem->chunk_size == UVM_PAGE_SIZE_4K || mem->chunk_size <= size, error);
    }

    TEST_NV_CHECK_GOTO(test_map_gpu(mem, gpu), error);

    if (gpu->mem_info.numa.enabled && (page_size == UVM_PAGE_SIZE_DEFAULT || page_size >= PAGE_SIZE))
        TEST_CHECK_GOTO(test_map_cpu(mem) == NV_OK, error);

    *mem_out = mem;

    return NV_OK;

error:
    uvm_mem_free(mem);
    return status;
}

static bool should_test_page_size(size_t alloc_size, NvU64 page_size)
{
    if (g_uvm_global.num_simulated_devices == 0)
        return true;

    return alloc_size <= UVM_PAGE_SIZE_2M || page_size == UVM_PAGE_SIZE_2M;
}

static NV_STATUS test_all(uvm_va_space_t *va_space)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu;
    NvU32 gpu_count;
    uvm_mem_t **all_mem = NULL;
    NvU32 allocation_count;
    NvU32 current_alloc = 0;

    // Create allocations of these sizes
    static const size_t sizes[] = { 1, 4, 16, 1024, 4096, 1024 * 1024, 7 * 1024 * 1024 + 17 };

    // Pascal+ can map sysmem with 4K, 64K and 2M PTEs, other GPUs can only use
    // 4K. Test all of the sizes supported by Pascal+ and 128K to match big page
    // size on pre-Pascal GPUs with 128K big page size.
    // Ampere+ supports 512M PTEs and Blackwell+ supports 256G PTEs, but since
    // UVM's maximum chunk size is 2M, we don't test for these page sizes.
    static const NvU64 cpu_chunk_sizes = PAGE_SIZE | UVM_PAGE_SIZE_64K | UVM_PAGE_SIZE_128K | UVM_PAGE_SIZE_2M;

    // All supported page sizes will be tested, CPU has the most with 4 and +1
    // for the default.
    static const int max_supported_page_sizes = 4 + 1;
    int i;

    // TODO: Bug 3839176: the test is waived on Confidential Computing because
    // it assumes that GPU can access system memory without using encryption.
    if (g_uvm_global.conf_computing_enabled)
        return NV_OK;

    gpu_count = uvm_processor_mask_get_gpu_count(&va_space->registered_gpus);

    // +1 for the CPU
    allocation_count = (gpu_count + 1) * max_supported_page_sizes * ARRAY_SIZE(sizes);

    // For the DMA allocations per GPU
    allocation_count += gpu_count * ARRAY_SIZE(sizes);

    all_mem = uvm_kvmalloc_zero(sizeof(*all_mem) * allocation_count);

    if (all_mem == NULL)
        return NV_ERR_NO_MEMORY;

    for (i = 0; i < ARRAY_SIZE(sizes); ++i) {
        NvU64 page_size = 0;
        uvm_mem_t *mem;

        if (should_test_page_size(sizes[i], UVM_PAGE_SIZE_DEFAULT)) {
            status = test_alloc_sysmem(va_space, UVM_PAGE_SIZE_DEFAULT, sizes[i], &mem);
            if (status != NV_OK) {
                UVM_TEST_PRINT("Failed to alloc sysmem size %zd, page_size default\n", sizes[i]);
                goto cleanup;
            }
            all_mem[current_alloc++] = mem;
        }

        for_each_page_size(page_size, cpu_chunk_sizes) {
            if (!should_test_page_size(sizes[i], page_size))
                continue;

            status = test_alloc_sysmem(va_space, page_size, sizes[i], &mem);
            if (status != NV_OK) {
                UVM_TEST_PRINT("Failed to alloc sysmem size %zd, page_size %llu\n", sizes[i], page_size);
                goto cleanup;
            }
            all_mem[current_alloc++] = mem;
        }

        for_each_va_space_gpu(gpu, va_space) {
            NvU64 page_sizes = gpu->address_space_tree.hal->page_sizes();

            if (gpu->mem_info.size == 0)
                continue;

            UVM_ASSERT(max_supported_page_sizes >= hweight_long(page_sizes));

            status = test_alloc_vidmem(gpu, UVM_PAGE_SIZE_DEFAULT, sizes[i], &mem);
            if (status != NV_OK) {
                UVM_TEST_PRINT("Test alloc vidmem failed, page_size default size %zd GPU %s\n",
                               sizes[i],
                               uvm_gpu_name(gpu));
                goto cleanup;
            }
            all_mem[current_alloc++] = mem;

            page_sizes &= UVM_CHUNK_SIZES_MASK;
            for_each_page_size(page_size, page_sizes) {
                status = test_alloc_vidmem(gpu, page_size, sizes[i], &mem);
                if (status != NV_OK) {
                    UVM_TEST_PRINT("Test alloc vidmem failed, page_size %llu size %zd GPU %s\n",
                                   page_size,
                                   sizes[i],
                                   uvm_gpu_name(gpu));
                    goto cleanup;
                }
                all_mem[current_alloc++] = mem;

            }
        }

        for_each_va_space_gpu(gpu, va_space) {
            status = test_alloc_sysmem_dma(va_space, gpu, sizes[i], &mem);
            if (status != NV_OK) {
                UVM_TEST_PRINT("Test alloc sysmem DMA failed, size %zd GPU %s\n",
                               sizes[i],
                               uvm_gpu_name(gpu));
                goto cleanup;
            }
            all_mem[current_alloc++] = mem;
        }
    }

cleanup:
    for (i = 0; i < current_alloc; ++i)
        uvm_mem_free(all_mem[i]);

    uvm_kvfree(all_mem);

    return status;
}

static NV_STATUS test_basic_vidmem(uvm_gpu_t *gpu)
{
    NV_STATUS status = NV_OK;
    NvU64 page_size;
    NvU64 page_sizes = gpu->address_space_tree.hal->page_sizes();
    NvU64 biggest_page_size = uvm_mmu_biggest_page_size_up_to(&gpu->address_space_tree, UVM_CHUNK_SIZE_MAX);
    NvU64 smallest_page_size = page_sizes & ~(page_sizes - 1);
    uvm_mem_t *mem = NULL;

    if (gpu->mem_info.size == 0)
        return NV_OK;

    page_sizes &= UVM_CHUNK_SIZES_MASK;
    for_each_page_size(page_size, page_sizes) {
        TEST_CHECK_GOTO(uvm_mem_alloc_vidmem(page_size - 1, gpu, &mem) == NV_OK, done);
        if (gpu->mem_info.numa.enabled)
            TEST_CHECK_GOTO(mem->chunk_size >= PAGE_SIZE && mem->chunk_size <= max(page_size, (NvU64)PAGE_SIZE), done);
        else
            TEST_CHECK_GOTO(mem->chunk_size < page_size || page_size == smallest_page_size, done);
        uvm_mem_free(mem);
        mem = NULL;

        TEST_CHECK_GOTO(uvm_mem_alloc_vidmem(page_size, gpu, &mem) == NV_OK, done);
        if (gpu->mem_info.numa.enabled)
            TEST_CHECK_GOTO(mem->chunk_size == max(page_size, (NvU64)PAGE_SIZE), done);
        else
            TEST_CHECK_GOTO(mem->chunk_size == page_size, done);
        uvm_mem_free(mem);
        mem = NULL;
    }

    TEST_CHECK_GOTO(uvm_mem_alloc_vidmem(5 * biggest_page_size - 1, gpu, &mem) == NV_OK, done);
    TEST_CHECK_GOTO(mem->chunk_size == biggest_page_size, done);

done:
    uvm_mem_free(mem);
    return status;
}

static NV_STATUS test_basic_sysmem(void)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *mem = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(sysmem_alloc_sizes); ++i) {
        size_t size = sysmem_alloc_sizes[i];
        TEST_NV_CHECK_GOTO(uvm_mem_alloc_sysmem(size, current->mm, &mem), done);
        TEST_CHECK_GOTO(mem->chunk_size == PAGE_SIZE, done);
        uvm_mem_free(mem);
        mem = NULL;
    }

done:
    uvm_mem_free(mem);
    return status;
}

static NV_STATUS test_basic_sysmem_dma(uvm_gpu_t *gpu)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *mem = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(sysmem_alloc_sizes); ++i) {
        size_t size = sysmem_alloc_sizes[i];
        TEST_NV_CHECK_GOTO(uvm_mem_alloc_sysmem_dma(size, gpu, current->mm, &mem), done);
        TEST_CHECK_GOTO(mem->chunk_size == PAGE_SIZE, done);
        uvm_mem_free(mem);
        mem = NULL;
    }

done:
    uvm_mem_free(mem);
    return status;
}

static NV_STATUS test_basic_dma_pool(uvm_gpu_t *gpu)
{
    size_t i, j;
    size_t num_buffers;
    size_t status = NV_OK;
    uvm_conf_computing_dma_buffer_t **dma_buffers;

    // If the Confidential Computing feature is disabled, the DMA buffers
    // pool is not initialized.
    if (!g_uvm_global.conf_computing_enabled)
        return NV_OK;

    // We're going to reclaim one more chunks that the pool have. Triggerring
    // one expansion.
    num_buffers = gpu->conf_computing.dma_buffer_pool.num_dma_buffers + 1;
    dma_buffers = uvm_kvmalloc_zero(sizeof(*dma_buffers) * num_buffers);
    if (dma_buffers == NULL)
        return NV_ERR_NO_MEMORY;

    for (i = 0; i < num_buffers; ++i) {
        status = uvm_conf_computing_dma_buffer_alloc(&gpu->conf_computing.dma_buffer_pool, &dma_buffers[i], NULL);
        if (status != NV_OK)
            break;
    }

    TEST_CHECK_GOTO(gpu->conf_computing.dma_buffer_pool.num_dma_buffers >= num_buffers, done);
    TEST_CHECK_GOTO(i == num_buffers, done);

done:
    j = i;
    for (i = 0; i < j; ++i)
        uvm_conf_computing_dma_buffer_free(&gpu->conf_computing.dma_buffer_pool, dma_buffers[i], NULL);

    uvm_kvfree(dma_buffers);
    return status;
}

static NV_STATUS check_huge_page_from_gpu(uvm_gpu_t *gpu, uvm_mem_t *mem, NvU64 offset)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *sys_mem = NULL;
    uvm_push_t push;
    NvU64 *sys_verif;
    NvU64 *expected_value;
    NvU64 verif_size = mem->size;
    uvm_gpu_address_t mem_gpu_address, sys_mem_gpu_address;

    UVM_ASSERT(uvm_mem_physical_size(mem) >= verif_size);

    TEST_NV_CHECK_GOTO(mem_alloc_sysmem_and_map_cpu_kernel(verif_size, gpu, &sys_mem), done);
    sys_verif = uvm_mem_get_cpu_addr_kernel(sys_mem);
    memset(sys_verif, 0x0, mem->size);

    TEST_NV_CHECK_GOTO(uvm_mem_map_gpu_kernel(sys_mem, gpu), done);

    mem_gpu_address = uvm_gpu_address_virtual(offset);
    sys_mem_gpu_address = uvm_mem_gpu_address_virtual_kernel(sys_mem, gpu);

    TEST_NV_CHECK_GOTO(uvm_push_begin(gpu->channel_manager,
                                      UVM_CHANNEL_TYPE_GPU_TO_CPU,
                                      &push,
                                      "Memcopy %llu bytes from virtual mem 0x%llx to virtual sys_mem 0x%llx",
                                      verif_size,
                                      mem_gpu_address.address,
                                      sys_mem_gpu_address.address),
                       done);

    gpu->parent->ce_hal->memcopy(&push, sys_mem_gpu_address, mem_gpu_address, verif_size);
    TEST_NV_CHECK_GOTO(uvm_push_end_and_wait(&push), done);

    expected_value = uvm_mem_get_cpu_addr_kernel(mem);
    TEST_CHECK_GOTO(memcmp(sys_verif, expected_value, verif_size) == 0, done);

done:
    uvm_mem_free(sys_mem);

    return status;
}

static NvU64 test_pte_maker(uvm_page_table_range_vec_t *range_vec, NvU64 offset, void *phys_addr)
{
    uvm_page_tree_t *tree = range_vec->tree;
    uvm_gpu_phys_address_t phys = uvm_gpu_phys_address(UVM_APERTURE_SYS, (NvU64)phys_addr);

    return tree->hal->make_pte(phys.aperture, phys.address, UVM_PROT_READ_ONLY, UVM_MMU_PTE_FLAGS_NONE);
}

static NV_STATUS test_huge_page_size(uvm_va_space_t *va_space, uvm_gpu_t *gpu, NvU64 page_size)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *mem = NULL;
    size_t size = PAGE_SIZE;
    NvU64 *cpu_addr;
    NvU64 huge_gpu_va;
    NvU64 gpu_phys_addr;
    uvm_page_table_range_vec_t *range_vec;
    NvU8 value = 0xA5;

    // TODO: Bug 3839176: the test is waived on Confidential Computing because
    // it assumes that GPU can access system memory without using encryption.
    if (g_uvm_global.conf_computing_enabled)
        return NV_OK;

    TEST_NV_CHECK_GOTO(mem_alloc_sysmem_and_map_cpu_kernel(size, gpu, &mem), cleanup);
    cpu_addr = uvm_mem_get_cpu_addr_kernel(mem);
    memset(cpu_addr, value, mem->size);

    // Map it on the GPU (uvm_mem base area), it creates GPU physical address
    // for the sysmem mapping.
    TEST_NV_CHECK_GOTO(uvm_mem_map_gpu_phys(mem, gpu), cleanup);

    huge_gpu_va = UVM_ALIGN_UP(gpu->parent->uvm_mem_va_base + gpu->parent->uvm_mem_va_size, page_size);
    TEST_CHECK_GOTO(IS_ALIGNED(huge_gpu_va, page_size), cleanup);
    TEST_CHECK_GOTO((huge_gpu_va + page_size) < (1ull << gpu->address_space_tree.hal->num_va_bits()), cleanup);

    // Manually mapping huge_gpu_va because page_size is larger than the largest
    // uvm_mem_t chunk/page size, so we don't use uvm_mem_gpu_kernel() helper.
    TEST_NV_CHECK_GOTO(uvm_page_table_range_vec_create(&gpu->address_space_tree,
                                                       huge_gpu_va,
                                                       page_size,
                                                       page_size,
                                                       UVM_PMM_ALLOC_FLAGS_NONE,
                                                       &range_vec), cleanup);

    gpu_phys_addr = uvm_mem_gpu_physical(mem, gpu, 0, size).address;

    TEST_NV_CHECK_GOTO(uvm_page_table_range_vec_write_ptes(range_vec,
                                                           UVM_MEMBAR_NONE,
                                                           test_pte_maker,
                                                           (void *)gpu_phys_addr), cleanup_range);

    // Despite the huge page_size mapping, only PAGE_SIZE is backed by an
    // allocation "own" by the test. We compute the offset within the huge page
    // to verify only this segment.
    TEST_NV_CHECK_GOTO(check_huge_page_from_gpu(gpu, mem, huge_gpu_va + (gpu_phys_addr % page_size)),
                       cleanup_range);

cleanup_range:
    uvm_page_table_range_vec_destroy(range_vec);
    range_vec = NULL;

cleanup:
    uvm_mem_free(mem);

    return status;
}

// Check the GPU access to memory from a 512MB+ page size mapping.
// The test allocates a PAGE_SIZE sysmem page, but uses the GMMU to map a huge
// page size area. It maps the allocated page to this area, and uses the CE to
// access it, thus, exercising a memory access using a huge page.
static NV_STATUS test_huge_pages(uvm_va_space_t *va_space, uvm_gpu_t *gpu)
{
    NvU64 page_sizes = gpu->address_space_tree.hal->page_sizes();
    NvU64 page_size = 0;

    for_each_page_size(page_size, page_sizes) {
        if (page_size < UVM_PAGE_SIZE_512M)
            continue;

        TEST_NV_CHECK_RET(test_huge_page_size(va_space, gpu, page_size));
    }

    return NV_OK;
}

static NV_STATUS test_basic(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;

    TEST_NV_CHECK_RET(test_basic_sysmem());

    for_each_va_space_gpu(gpu, va_space) {
        TEST_NV_CHECK_RET(test_basic_vidmem(gpu));
        TEST_NV_CHECK_RET(test_basic_sysmem_dma(gpu));
        TEST_NV_CHECK_RET(test_basic_dma_pool(gpu));
        TEST_NV_CHECK_RET(test_huge_pages(va_space, gpu));
    }

    return NV_OK;
}

static NV_STATUS tests(uvm_va_space_t *va_space)
{
    TEST_NV_CHECK_RET(test_basic(va_space));
    TEST_NV_CHECK_RET(test_all(va_space));

    return NV_OK;
}

NV_STATUS uvm_test_mem_sanity(UVM_TEST_MEM_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_va_space_down_read(va_space);

    status = tests(va_space);

    uvm_va_space_up_read(va_space);

    return status;
}
