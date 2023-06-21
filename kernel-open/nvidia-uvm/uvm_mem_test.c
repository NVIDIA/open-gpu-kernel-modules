/*******************************************************************************
    Copyright (c) 2016-2021 NVIDIA Corporation

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
#include "uvm_gpu.h"
#include "uvm_kvmalloc.h"
#include "uvm_mem.h"
#include "uvm_push.h"
#include "uvm_conf_computing.h"
#include "uvm_test.h"
#include "uvm_test_ioctl.h"
#include "uvm_va_space.h"

static const size_t sysmem_alloc_sizes[] = { 1, PAGE_SIZE - 1, PAGE_SIZE, 7 * PAGE_SIZE };

static NvU32 first_page_size(NvU32 page_sizes)
{
    return page_sizes & ~(page_sizes - 1);
}

#define for_each_page_size(page_size, page_sizes)                                   \
    for (page_size = first_page_size(page_sizes);                                   \
         page_size;                                                                 \
         page_size = first_page_size((page_sizes) & ~(page_size | (page_size - 1))))

static inline NV_STATUS __alloc_map_sysmem(NvU64 size, uvm_gpu_t *gpu, uvm_mem_t **sys_mem)
{
    if (g_uvm_global.sev_enabled)
        return uvm_mem_alloc_sysmem_dma_and_map_cpu_kernel(size, gpu, current->mm, sys_mem);
    else
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

    TEST_NV_CHECK_GOTO(__alloc_map_sysmem(verif_size, gpu, &sys_mem), done);
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
                                 "Memcopy %zd bytes from virtual sys_mem 0x%llx to %s mem 0x%llx [mem loc: %s, page size: %u]",
                                 size_this_time,
                                 sys_mem_gpu_address.address,
                                 mem_gpu_address.is_virtual? "virtual" : "physical",
                                 mem_gpu_address.address,
                                 uvm_mem_is_sysmem(mem)? "sys" : "vid",
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
                                 sys_mem_gpu_address.is_virtual? "virtual" : "physical",
                                 sys_mem_gpu_address.address);

        gpu->parent->ce_hal->memcopy(&push, sys_mem_gpu_address, mem_gpu_address, size_this_time);

        uvm_push_end(&push);
        TEST_NV_CHECK_GOTO(uvm_tracker_add_push(&tracker, &push), done);
    }

    TEST_NV_CHECK_GOTO(uvm_tracker_wait(&tracker), done);

    for (i = 0; i < verif_size / sizeof(*sys_verif); ++i) {
        if (sys_verif[i] != mem->size + i) {
            UVM_TEST_PRINT("Verif failed for %zd = 0x%llx instead of 0x%llx, verif_size=0x%llx mem(size=0x%llx, page_size=%u, processor=%u)\n",
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

static NV_STATUS test_alloc_sysmem(uvm_va_space_t *va_space, NvU32 page_size, size_t size, uvm_mem_t **mem_out)
{
    NV_STATUS status;
    uvm_mem_t *mem;
    uvm_gpu_t *gpu;
    uvm_mem_alloc_params_t params = { 0 };

    params.size = size;
    params.page_size = page_size;
    params.mm = current->mm;

    status = uvm_mem_alloc(&params, &mem);
    TEST_CHECK_GOTO(status == NV_OK, error);

    TEST_CHECK_GOTO(test_map_cpu(mem) == NV_OK, error);

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

static NV_STATUS test_alloc_vidmem(uvm_gpu_t *gpu, NvU32 page_size, size_t size, uvm_mem_t **mem_out)
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

static bool should_test_page_size(size_t alloc_size, NvU32 page_size)
{
    if (g_uvm_global.sev_enabled)
        return false;

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
    static const size_t sizes[] = {1, 4, 16, 1024, 4096, 1024 * 1024, 7 * 1024 * 1024 + 17 };

    // Pascal+ can map sysmem with 4K, 64K and 2M PTEs, other GPUs can only use
    // 4K. Test all of the sizes supported by Pascal+ and 128K to match big page
    // size on pre-Pascal GPUs with 128K big page size.
    // Ampere+ also supports 512M PTEs, but since UVM's maximum chunk size is
    // 2M, we don't test for this page size.
    static const NvU32 cpu_chunk_sizes = PAGE_SIZE | UVM_PAGE_SIZE_64K | UVM_PAGE_SIZE_128K | UVM_PAGE_SIZE_2M;

    // All supported page sizes will be tested, CPU has the most with 4 and +1
    // for the default.
    static const int max_supported_page_sizes = 4 + 1;
    int i;


    // TODO: Bug 3839176: the test is waived on Confidential Computing because
    // it assumes that GPU can access system memory without using encryption.
    if (uvm_conf_computing_mode_enabled(uvm_va_space_find_first_gpu(va_space)))
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
        NvU32 page_size = 0;
        uvm_mem_t *mem;

        if (should_test_page_size(sizes[i], UVM_PAGE_SIZE_DEFAULT)) {
            status = test_alloc_sysmem(va_space, UVM_PAGE_SIZE_DEFAULT, sizes[i], &mem);
            if (status != NV_OK) {
                UVM_TEST_PRINT("Failed to alloc sysmem size %zd, page_size default\n", sizes[i], page_size);
                goto cleanup;
            }
            all_mem[current_alloc++] = mem;
        }

        for_each_page_size(page_size, cpu_chunk_sizes) {
            if (!should_test_page_size(sizes[i], page_size))
                continue;

            status = test_alloc_sysmem(va_space, page_size, sizes[i], &mem);
            if (status != NV_OK) {
                UVM_TEST_PRINT("Failed to alloc sysmem size %zd, page_size %u\n", sizes[i], page_size);
                goto cleanup;
            }
            all_mem[current_alloc++] = mem;
        }

        for_each_va_space_gpu(gpu, va_space) {
            NvU32 page_sizes = gpu->address_space_tree.hal->page_sizes();

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
                    UVM_TEST_PRINT("Test alloc vidmem failed, page_size %u size %zd GPU %s\n",
                                   page_size,
                                   sizes[i],
                                   uvm_gpu_name(gpu));
                    goto cleanup;
                }
                all_mem[current_alloc++] = mem;

            }

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
    NvU32 page_size;
    NvU32 page_sizes = gpu->address_space_tree.hal->page_sizes();
    NvU32 biggest_page_size = uvm_mmu_biggest_page_size_up_to(&gpu->address_space_tree, UVM_CHUNK_SIZE_MAX);
    NvU32 smallest_page_size = page_sizes & ~(page_sizes - 1);
    uvm_mem_t *mem = NULL;

    page_sizes &= UVM_CHUNK_SIZES_MASK;
    for_each_page_size(page_size, page_sizes) {
        TEST_CHECK_GOTO(uvm_mem_alloc_vidmem(page_size - 1, gpu, &mem) == NV_OK, done);
        if (gpu->mem_info.numa.enabled)
            TEST_CHECK_GOTO(mem->chunk_size >= PAGE_SIZE && mem->chunk_size <= max(page_size, (NvU32)PAGE_SIZE), done);
        else
            TEST_CHECK_GOTO(mem->chunk_size < page_size || page_size == smallest_page_size, done);
        uvm_mem_free(mem);
        mem = NULL;

        TEST_CHECK_GOTO(uvm_mem_alloc_vidmem(page_size, gpu, &mem) == NV_OK, done);
        if (gpu->mem_info.numa.enabled)
            TEST_CHECK_GOTO(mem->chunk_size == max(page_size, (NvU32)PAGE_SIZE), done);
        else
            TEST_CHECK_GOTO(mem->chunk_size == page_size, done);
        uvm_mem_free(mem);
        mem = NULL;
    }

    TEST_CHECK_GOTO(uvm_mem_alloc_vidmem(5 * ((NvU64)biggest_page_size) - 1, gpu, &mem) == NV_OK, done);
    TEST_CHECK_GOTO(mem->chunk_size == biggest_page_size, done);

done:
    uvm_mem_free(mem);
    return status;
}

static NV_STATUS test_basic_vidmem_unprotected(uvm_gpu_t *gpu)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *mem = NULL;

    uvm_mem_alloc_params_t params = { 0 };
    params.size = UVM_PAGE_SIZE_4K;
    params.backing_gpu = gpu;
    params.page_size = UVM_PAGE_SIZE_4K;

    // If CC is enabled, the protection flag is observed. Because currently all
    // vidmem is in the protected region, the allocation should succeed.
    //
    // If CC is disabled, the protection flag is ignored.
    params.is_unprotected = false;
    TEST_NV_CHECK_RET(uvm_mem_alloc(&params, &mem));

    uvm_mem_free(mem);
    mem = NULL;

    // If CC is enabled, the allocation should fail because currently the
    // unprotected region is empty.
    //
    // If CC is disabled, the behavior should be identical to that of a
    // protected allocation.
    params.is_unprotected = true;
    if (uvm_conf_computing_mode_enabled(gpu))
        TEST_CHECK_RET(uvm_mem_alloc(&params, &mem) == NV_ERR_NO_MEMORY);
    else
        TEST_NV_CHECK_RET(uvm_mem_alloc(&params, &mem));

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
    if (!uvm_conf_computing_mode_enabled(gpu))
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

static NV_STATUS test_basic(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;

    TEST_NV_CHECK_RET(test_basic_sysmem());

    for_each_va_space_gpu(gpu, va_space) {
        TEST_NV_CHECK_RET(test_basic_vidmem(gpu));
        TEST_NV_CHECK_RET(test_basic_sysmem_dma(gpu));
        TEST_NV_CHECK_RET(test_basic_vidmem_unprotected(gpu));
        TEST_NV_CHECK_RET(test_basic_dma_pool(gpu));
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
