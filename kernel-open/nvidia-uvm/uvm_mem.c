/*******************************************************************************
    Copyright (c) 2016-2023 NVIDIA Corporation

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

#include "uvm_mem.h"
#include "uvm_hal_types.h"
#include "uvm_mmu.h"
#include "uvm_processors.h"
#include "uvm_va_space.h"
#include "uvm_gpu.h"
#include "uvm_global.h"
#include "uvm_kvmalloc.h"
#include "uvm_push.h"
#include "uvm_range_allocator.h"
#include "uvm_hal.h"
#include "uvm_linux.h"

static uvm_range_allocator_t g_free_ranges;
static bool g_mem_initialized;

NV_STATUS uvm_mem_global_init(void)
{
    NV_STATUS status = uvm_range_allocator_init(UVM_MEM_VA_SIZE, &g_free_ranges);
    if (status != NV_OK)
        return status;

    g_mem_initialized = true;

    return NV_OK;
}

void uvm_mem_global_exit(void)
{
    if (!g_mem_initialized)
        return;

    uvm_range_allocator_deinit(&g_free_ranges);
}

static bool vidmem_can_be_mapped(uvm_mem_t *vidmem, bool is_user_space)
{
    UVM_ASSERT(uvm_mem_is_vidmem(vidmem));

    // Mapping a vidmem allocation on a user VA space is currently unsupported,
    // because there is no use case.
    if (is_user_space)
        return false;

    return true;
}

static bool mem_can_be_mapped_on_cpu(uvm_mem_t *mem, bool is_user_space)
{
    if (uvm_mem_is_sysmem(mem))
        return true;

    if (!vidmem_can_be_mapped(mem, is_user_space))
        return false;

    return mem->backing_gpu->mem_info.numa.enabled && PAGE_ALIGNED(mem->chunk_size);
}

static bool mem_can_be_mapped_on_cpu_kernel(uvm_mem_t *mem)
{
    return mem_can_be_mapped_on_cpu(mem, false);
}

static bool mem_can_be_mapped_on_cpu_user(uvm_mem_t *mem)
{
    return mem_can_be_mapped_on_cpu(mem, true);
}

static bool sysmem_can_be_mapped_on_gpu(uvm_mem_t *sysmem)
{
    UVM_ASSERT(uvm_mem_is_sysmem(sysmem));

    // In Confidential Computing, only unprotected memory can be mapped on the
    // GPU
    if (g_uvm_global.conf_computing_enabled)
        return uvm_mem_is_sysmem_dma(sysmem);

    return true;
}

static bool mem_can_be_mapped_on_gpu(uvm_mem_t *mem, uvm_gpu_t *gpu, bool is_user_space)
{
    if (uvm_mem_is_sysmem(mem))
        return sysmem_can_be_mapped_on_gpu(mem);

    if (!vidmem_can_be_mapped(mem, is_user_space))
        return false;

    return uvm_mem_is_local_vidmem(mem, gpu);
}

static bool mem_can_be_mapped_on_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    return mem_can_be_mapped_on_gpu(mem, gpu, false);
}

static bool mem_can_be_mapped_on_gpu_user(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    return mem_can_be_mapped_on_gpu(mem, gpu, true);
}

bool uvm_mem_mapped_on_gpu_user(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    if (mem->user == NULL)
        return false;

    return uvm_processor_mask_test(&mem->user->mapped_on, gpu->id);
}

bool uvm_mem_mapped_on_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    return uvm_processor_mask_test(&mem->kernel.mapped_on, gpu->id);
}

bool uvm_mem_mapped_on_cpu_user(uvm_mem_t *mem)
{
    if (mem->user == NULL)
        return false;

    return uvm_processor_mask_test(&mem->user->mapped_on, UVM_ID_CPU);
}

bool uvm_mem_mapped_on_cpu_kernel(uvm_mem_t *mem)
{
    return uvm_processor_mask_test(&mem->kernel.mapped_on, UVM_ID_CPU);
}

static void mem_set_mapped_on_gpu_user(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(mem->user != NULL);
    UVM_ASSERT(mem_can_be_mapped_on_gpu_user(mem, gpu));
    UVM_ASSERT(!uvm_mem_mapped_on_gpu_user(mem, gpu));

    uvm_processor_mask_set(&mem->user->mapped_on, gpu->id);
}

static void mem_set_mapped_on_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(mem_can_be_mapped_on_gpu_kernel(mem, gpu));
    UVM_ASSERT(!uvm_mem_mapped_on_gpu_kernel(mem, gpu));

    uvm_processor_mask_set(&mem->kernel.mapped_on, gpu->id);
}

static void mem_set_mapped_on_cpu_user(uvm_mem_t *mem)
{
    UVM_ASSERT(mem->user != NULL);
    UVM_ASSERT(mem_can_be_mapped_on_cpu_user(mem));
    UVM_ASSERT(!uvm_mem_mapped_on_cpu_user(mem));

    uvm_processor_mask_set(&mem->user->mapped_on, UVM_ID_CPU);
}

static void mem_set_mapped_on_cpu_kernel(uvm_mem_t *mem)
{
    UVM_ASSERT(mem_can_be_mapped_on_cpu_kernel(mem));
    UVM_ASSERT(!uvm_mem_mapped_on_cpu_kernel(mem));

    uvm_processor_mask_set(&mem->kernel.mapped_on, UVM_ID_CPU);
}

static void mem_clear_mapped_on_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    uvm_processor_mask_clear(&mem->kernel.mapped_on, gpu->id);
}

static void mem_clear_mapped_on_gpu_user(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(mem->user != NULL);

    uvm_processor_mask_clear(&mem->user->mapped_on, gpu->id);
}

static void mem_clear_mapped_on_cpu_user(uvm_mem_t *mem)
{
    UVM_ASSERT(mem->user != NULL);

    uvm_processor_mask_clear(&mem->user->mapped_on, UVM_ID_CPU);
}

static void mem_clear_mapped_on_cpu_kernel(uvm_mem_t *mem)
{
    uvm_processor_mask_clear(&mem->kernel.mapped_on, UVM_ID_CPU);
}

static bool sysmem_mapped_on_gpu_phys(uvm_mem_t *sysmem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(uvm_mem_is_sysmem(sysmem));

    return uvm_processor_mask_test(&sysmem->sysmem.mapped_on_phys, gpu->id);
}

static void sysmem_set_mapped_on_gpu_phys(uvm_mem_t *sysmem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(uvm_mem_is_sysmem(sysmem));
    UVM_ASSERT(!sysmem_mapped_on_gpu_phys(sysmem, gpu));

    uvm_processor_mask_set(&sysmem->sysmem.mapped_on_phys, gpu->id);
}

static void sysmem_clear_mapped_on_gpu_phys(uvm_mem_t *sysmem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(uvm_mem_is_sysmem(sysmem));

    uvm_processor_mask_clear(&sysmem->sysmem.mapped_on_phys, gpu->id);
}

NV_STATUS uvm_mem_translate_gpu_attributes(const UvmGpuMappingAttributes *attrs,
                                           uvm_va_space_t *va_space,
                                           uvm_gpu_t **gpu_out,
                                           uvm_mem_gpu_mapping_attrs_t *attrs_out)
{
    uvm_gpu_t *gpu;

    switch (attrs->gpuMappingType) {
        case UvmGpuMappingTypeDefault:
            break;
        case UvmGpuMappingTypeReadWriteAtomic:
            attrs_out->protection = UVM_PROT_READ_WRITE_ATOMIC;
            break;
        case UvmGpuMappingTypeReadWrite:
            attrs_out->protection = UVM_PROT_READ_WRITE;
            break;
        case UvmGpuMappingTypeReadOnly:
            attrs_out->protection = UVM_PROT_READ_ONLY;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    switch (attrs->gpuCachingType) {
        case UvmGpuCachingTypeDefault:
            break;
        case UvmGpuCachingTypeForceUncached:
            attrs_out->is_cacheable = false;
            break;
        case UvmGpuCachingTypeForceCached:
            attrs_out->is_cacheable = true;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    gpu = uvm_va_space_get_gpu_by_uuid(va_space, &attrs->gpuUuid);
    if (!gpu)
        return NV_ERR_INVALID_DEVICE;

    if (gpu_out)
        *gpu_out = gpu;

    return NV_OK;
}

static struct page *uvm_virt_to_page(const void *addr)
{
    if (virt_addr_valid(addr))
        return virt_to_page(addr);

    if (is_vmalloc_addr(addr))
        return vmalloc_to_page(addr);

    return NULL;
}

uvm_chunk_sizes_mask_t uvm_mem_kernel_chunk_sizes(uvm_gpu_t *gpu)
{
    // Get the mmu mode hal directly as the internal address space tree has not
    // been created yet.
    uvm_mmu_mode_hal_t *hal = gpu->parent->arch_hal->mmu_mode_hal(gpu->big_page.internal_size);
    NvU64 page_sizes = hal->page_sizes();

    return (uvm_chunk_sizes_mask_t)(page_sizes & UVM_CHUNK_SIZES_MASK);
}

static NvU64 mem_pick_chunk_size(uvm_mem_t *mem)
{
    NvU64 biggest_page_size;
    NvU64 chunk_size;

    if (uvm_mem_is_sysmem(mem))
        return PAGE_SIZE;

    biggest_page_size = uvm_mmu_biggest_page_size_up_to(&mem->backing_gpu->address_space_tree, UVM_CHUNK_SIZE_MAX);

    if (mem->size < mem->backing_gpu->big_page.internal_size)
        chunk_size = UVM_PAGE_SIZE_4K;
    else if (mem->size < biggest_page_size)
        chunk_size = mem->backing_gpu->big_page.internal_size;
    else
        chunk_size = biggest_page_size;

    // When UVM_PAGE_SIZE_DEFAULT is used on NUMA-enabled GPUs, we force
    // chunk_size to be PAGE_SIZE at least, to allow CPU mappings.
    if (mem->backing_gpu->mem_info.numa.enabled)
        chunk_size = max(chunk_size, (NvU64)PAGE_SIZE);

    return chunk_size;
}

static NvU64 mem_pick_gpu_page_size(uvm_mem_t *mem, uvm_gpu_t *gpu, uvm_page_tree_t *gpu_page_tree)
{
    if (uvm_mem_is_vidmem(mem)) {
        // For vidmem allocations the chunk size is picked out of the supported
        // page sizes and can be used directly.
        return mem->chunk_size;
    }

    // For sysmem, check whether the GPU supports mapping it with large pages.
    if (gpu->parent->can_map_sysmem_with_large_pages) {
        // If it's supported, pick the largest page size not bigger than
        // the chunk size.
        return uvm_mmu_biggest_page_size_up_to(gpu_page_tree, mem->chunk_size);
    }

    // Otherwise just use 4K.
    return UVM_PAGE_SIZE_4K;
}

static void mem_free_vidmem_chunks(uvm_mem_t *mem)
{
    size_t i;

    UVM_ASSERT(uvm_mem_is_vidmem(mem));

    if (!mem->vidmem.chunks)
        return;

    for (i = 0; i < mem->chunks_count; ++i) {
        // On allocation error PMM guarantees the chunks array to be zeroed so
        // just check for NULL.
        if (mem->vidmem.chunks[i] == NULL)
            break;
        uvm_pmm_gpu_free(&mem->backing_gpu->pmm, mem->vidmem.chunks[i], NULL);
    }

    uvm_kvfree(mem->vidmem.chunks);
    mem->vidmem.chunks = NULL;
}

static void mem_free_sysmem_dma_chunks(uvm_mem_t *mem)
{
    size_t i;
    NvU32 gpu_index;

    UVM_ASSERT(uvm_mem_is_sysmem_dma(mem));
    gpu_index = uvm_id_gpu_index(mem->dma_owner->id);

    if (!mem->sysmem.pages || !mem->sysmem.va)
        goto end;

    for (i = 0; i < mem->chunks_count; ++i) {
        if (!mem->sysmem.va[i])
            break;

        uvm_parent_gpu_dma_free_page(mem->dma_owner->parent,
                                     mem->sysmem.va[i],
                                     mem->sysmem.dma_addrs[gpu_index][i]);
    }

end:
    sysmem_clear_mapped_on_gpu_phys(mem, mem->dma_owner);

    uvm_kvfree(mem->sysmem.dma_addrs[gpu_index]);
    mem->sysmem.dma_addrs[gpu_index] = NULL;

    uvm_kvfree(mem->sysmem.pages);
    mem->sysmem.pages = NULL;

    uvm_kvfree(mem->sysmem.va);
    mem->sysmem.va = NULL;
}

static void mem_free_sysmem_chunks(uvm_mem_t *mem)
{
    size_t i;

    UVM_ASSERT(uvm_mem_is_sysmem(mem));

    if (!mem->sysmem.pages)
        return;

    for (i = 0; i < mem->chunks_count; ++i) {
        if (!mem->sysmem.pages[i])
            break;
        __free_pages(mem->sysmem.pages[i], get_order(mem->chunk_size));
    }

    uvm_kvfree(mem->sysmem.pages);
    mem->sysmem.pages = NULL;
}

static void mem_free_chunks(uvm_mem_t *mem)
{
    if (uvm_mem_is_vidmem(mem))
        mem_free_vidmem_chunks(mem);
    else if (uvm_mem_is_sysmem_dma(mem))
        mem_free_sysmem_dma_chunks(mem);
    else
        mem_free_sysmem_chunks(mem);
}

static NV_STATUS mem_alloc_dma_addrs(uvm_mem_t *mem, const uvm_gpu_t *gpu)
{
    NvU64 *dma_addrs = NULL;
    NvU32 gpu_index = uvm_id_gpu_index(gpu->id);

    dma_addrs = uvm_kvmalloc_zero(sizeof(*dma_addrs) * mem->chunks_count);
    if (!dma_addrs)
        return NV_ERR_NO_MEMORY;

    mem->sysmem.dma_addrs[gpu_index] = dma_addrs;

    return NV_OK;
}

static gfp_t sysmem_allocation_gfp_flags(int order, bool zero)
{
    gfp_t gfp_flags = NV_UVM_GFP_FLAGS;

    if (zero)
        gfp_flags |= __GFP_ZERO;

    // High-order page allocations require the __GFP_COMP flag to work with
    // vm_insert_page.
    if (order > 0)
        gfp_flags |= __GFP_COMP;

    return gfp_flags;
}

// This allocation is a non-protected memory allocation under Confidential
// Computing.
//
// There is a tighter coupling between allocation and mapping because of the
// allocator UVM must use. Hence, this function does the equivalent of
// uvm_mem_map_gpu_phys().
//
// In case of failure, the caller is required to handle cleanup by calling
// uvm_mem_free
static NV_STATUS mem_alloc_sysmem_dma_chunks(uvm_mem_t *mem, gfp_t gfp_flags)
{
    size_t i;
    NV_STATUS status;
    NvU64 *dma_addrs;

    UVM_ASSERT_MSG(mem->chunk_size == PAGE_SIZE,
                   "mem->chunk_size is 0x%llx. PAGE_SIZE is only supported.",
                   mem->chunk_size);
    UVM_ASSERT(uvm_mem_is_sysmem_dma(mem));

    mem->sysmem.pages = uvm_kvmalloc_zero(sizeof(*mem->sysmem.pages) * mem->chunks_count);
    mem->sysmem.va = uvm_kvmalloc_zero(sizeof(*mem->sysmem.va) * mem->chunks_count);
    if (!mem->sysmem.pages || !mem->sysmem.va)
        goto err_no_mem;

    status = mem_alloc_dma_addrs(mem, mem->dma_owner);
    if (status != NV_OK)
        goto error;

    dma_addrs = mem->sysmem.dma_addrs[uvm_id_gpu_index(mem->dma_owner->id)];

    for (i = 0; i < mem->chunks_count; ++i) {
        mem->sysmem.va[i] = uvm_parent_gpu_dma_alloc_page(mem->dma_owner->parent, gfp_flags, &dma_addrs[i]);
        if (!mem->sysmem.va[i])
            goto err_no_mem;

        mem->sysmem.pages[i] = uvm_virt_to_page(mem->sysmem.va[i]);
        if (!mem->sysmem.pages[i])
            goto err_no_mem;
    }

    sysmem_set_mapped_on_gpu_phys(mem, mem->dma_owner);

    return NV_OK;

err_no_mem:
    status = NV_ERR_NO_MEMORY;
error:
    mem_free_sysmem_dma_chunks(mem);
    return status;
}

// In case of failure, the caller is required to handle cleanup by calling
// uvm_mem_free
static NV_STATUS mem_alloc_sysmem_chunks(uvm_mem_t *mem, gfp_t gfp_flags)
{
    size_t i;
    int order;

    UVM_ASSERT(uvm_mem_is_sysmem(mem) && !uvm_mem_is_sysmem_dma(mem));

    mem->sysmem.pages = uvm_kvmalloc_zero(sizeof(*mem->sysmem.pages) * mem->chunks_count);
    if (!mem->sysmem.pages)
        return NV_ERR_NO_MEMORY;

    order = get_order(mem->chunk_size);
    for (i = 0; i < mem->chunks_count; ++i) {
        mem->sysmem.pages[i] = alloc_pages(gfp_flags, order);
        if (!mem->sysmem.pages[i])
            return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

// In case of failure, the caller is required to handle cleanup by calling
// uvm_mem_free
static NV_STATUS mem_alloc_vidmem_chunks(uvm_mem_t *mem, bool zero)
{
    NV_STATUS status;

    UVM_ASSERT(uvm_mem_is_vidmem(mem));

    // TODO: Bug 2446832: A non-zeroing request may not be obeyed because PMM
    // does not support explicit allocation of non-zeroed (or zeroed) chunks.
    //
    // The zeroing case can be implemented even without resolving that bug, by
    // clearing the chunks after PMM allocation. But this functionality has not
    // been implemented, because the only expected use case is a memory that
    // gets mapped on user space, and vidmem never is.
    UVM_ASSERT(!zero);

    mem->vidmem.chunks = uvm_kvmalloc_zero(mem->chunks_count * sizeof(*mem->vidmem.chunks));
    if (!mem->vidmem.chunks)
        return NV_ERR_NO_MEMORY;

    status = uvm_pmm_gpu_alloc_kernel(&mem->backing_gpu->pmm,
                                      mem->chunks_count,
                                      mem->chunk_size,
                                      UVM_PMM_ALLOC_FLAGS_NONE,
                                      mem->vidmem.chunks,
                                      NULL);

    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_pmm_gpu_alloc_kernel (count=%zd, size=0x%llx) failed: %s\n",
                      mem->chunks_count,
                      mem->chunk_size,
                      nvstatusToString(status));
        return status;
    }

    return NV_OK;
}

static NV_STATUS mem_alloc_chunks(uvm_mem_t *mem, struct mm_struct *mm, bool zero)
{
    if (uvm_mem_is_sysmem(mem)) {
        gfp_t gfp_flags;
        uvm_memcg_context_t memcg_context;
        NV_STATUS status;

        UVM_ASSERT(PAGE_ALIGNED(mem->chunk_size));
        gfp_flags = sysmem_allocation_gfp_flags(get_order(mem->chunk_size), zero);
        if (UVM_CGROUP_ACCOUNTING_SUPPORTED() && mm)
            gfp_flags |= NV_UVM_GFP_FLAGS_ACCOUNT;

        uvm_memcg_context_start(&memcg_context, mm);
        if (uvm_mem_is_sysmem_dma(mem))
            status = mem_alloc_sysmem_dma_chunks(mem, gfp_flags);
        else
            status = mem_alloc_sysmem_chunks(mem, gfp_flags);

        uvm_memcg_context_end(&memcg_context);
        return status;
    }

    return mem_alloc_vidmem_chunks(mem, zero);
}

NV_STATUS uvm_mem_map_kernel(uvm_mem_t *mem, const uvm_processor_mask_t *mask)
{
    uvm_gpu_t *gpu;
    NV_STATUS status;

    if (!mask)
        return NV_OK;

    if (uvm_processor_mask_test(mask, UVM_ID_CPU)) {
        status = uvm_mem_map_cpu_kernel(mem);
        if (status != NV_OK)
            return status;
    }

    for_each_gpu_in_mask(gpu, mask) {
        status = uvm_mem_map_gpu_kernel(mem, gpu);
        if (status != NV_OK)
            return status;
    }
    return NV_OK;
}

NV_STATUS uvm_mem_alloc(const uvm_mem_alloc_params_t *params, uvm_mem_t **mem_out)
{
    NV_STATUS status;
    NvU64 physical_size;
    uvm_mem_t *mem = NULL;

    UVM_ASSERT(params->size > 0);

    mem = uvm_kvmalloc_zero(sizeof(*mem));
    if (mem == NULL)
        return NV_ERR_NO_MEMORY;

    mem->backing_gpu = params->backing_gpu;
    mem->dma_owner = params->dma_owner;
    UVM_ASSERT(!mem->dma_owner || !mem->backing_gpu);

    mem->size = params->size;
    mem->chunk_size = params->page_size;
    if (mem->chunk_size == UVM_PAGE_SIZE_DEFAULT)
        mem->chunk_size = mem_pick_chunk_size(mem);

    UVM_ASSERT(mem->chunk_size > 0);

    physical_size = UVM_ALIGN_UP(mem->size, mem->chunk_size);
    mem->chunks_count = physical_size / mem->chunk_size;

    status = mem_alloc_chunks(mem, params->mm, params->zero);
    if (status != NV_OK)
        goto error;

    *mem_out = mem;
    return NV_OK;

error:
    uvm_mem_free(mem);
    return status;
}

static NV_STATUS mem_init_user_mapping(uvm_mem_t *mem, uvm_va_space_t *user_va_space, void *user_addr)
{
    UVM_ASSERT(user_va_space);
    UVM_ASSERT(user_addr);

    // If the user structure exists, the VA space and address should match
    if (mem->user != NULL) {
        UVM_ASSERT(mem->user->va_space == user_va_space);
        UVM_ASSERT(mem->user->addr == user_addr);
        return NV_OK;
    }

    UVM_ASSERT(IS_ALIGNED((NvU64)user_addr, mem->chunk_size));
    UVM_ASSERT(uvm_mem_physical_size(mem) == mem->size);

    mem->user = uvm_kvmalloc_zero(sizeof(*mem->user));
    if (mem->user == NULL)
        return NV_ERR_NO_MEMORY;

    mem->user->va_space = user_va_space;
    mem->user->addr = user_addr;

    return NV_OK;
}

static void mem_deinit_user_mapping(uvm_mem_t *mem)
{
    if (mem->user == NULL)
        return;

    if (!uvm_processor_mask_empty(&mem->user->mapped_on))
        return;

    uvm_kvfree(mem->user);
    mem->user = NULL;
}

static NvU64 reserved_gpu_va(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(mem->kernel.range_alloc.aligned_start + uvm_mem_physical_size(mem) < gpu->parent->uvm_mem_va_size);

    return gpu->parent->uvm_mem_va_base + mem->kernel.range_alloc.aligned_start;
}

static struct page *mem_cpu_page(uvm_mem_t *mem, NvU64 offset)
{
    struct page *base_page = mem->sysmem.pages[offset / mem->chunk_size];

    UVM_ASSERT_MSG(PAGE_ALIGNED(offset), "offset 0x%llx\n", offset);

    offset = offset % mem->chunk_size;
    return pfn_to_page(page_to_pfn(base_page) + offset / PAGE_SIZE);
}

static NV_STATUS mem_map_cpu_to_sysmem_kernel(uvm_mem_t *mem)
{
    struct page **pages = mem->sysmem.pages;
    size_t num_pages = uvm_mem_physical_size(mem) / PAGE_SIZE;
    pgprot_t prot = PAGE_KERNEL;

    UVM_ASSERT(uvm_mem_is_sysmem(mem));

    // If chunk size is different than PAGE_SIZE then create a temporary array
    // of all the pages to map so that vmap() can be used.
    if (mem->chunk_size != PAGE_SIZE) {
        size_t page_index;
        pages = uvm_kvmalloc(sizeof(*pages) * num_pages);
        if (!pages)
            return NV_ERR_NO_MEMORY;
        for (page_index = 0; page_index < num_pages; ++page_index)
            pages[page_index] = mem_cpu_page(mem, page_index * PAGE_SIZE);
    }

    if (g_uvm_global.conf_computing_enabled && uvm_mem_is_sysmem_dma(mem))
        prot = uvm_pgprot_decrypted(PAGE_KERNEL_NOENC);

    mem->kernel.cpu_addr = vmap(pages, num_pages, VM_MAP, prot);

    if (mem->chunk_size != PAGE_SIZE)
        uvm_kvfree(pages);

    if (!mem->kernel.cpu_addr)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

static NV_STATUS mem_map_cpu_to_vidmem_kernel(uvm_mem_t *mem)
{
    struct page **pages;
    size_t num_chunk_pages = mem->chunk_size / PAGE_SIZE;
    size_t num_pages = uvm_mem_physical_size(mem) / PAGE_SIZE;
    size_t page_index;
    size_t chunk_index;

    UVM_ASSERT(uvm_mem_is_vidmem(mem));

    pages = uvm_kvmalloc(sizeof(*pages) * num_pages);
    if (!pages)
        return NV_ERR_NO_MEMORY;

    page_index = 0;

    for (chunk_index = 0; chunk_index < mem->chunks_count; ++chunk_index) {
        uvm_gpu_chunk_t *chunk = mem->vidmem.chunks[chunk_index];
        struct page *page = uvm_gpu_chunk_to_page(&mem->backing_gpu->pmm, chunk);
        size_t chunk_page_index;

        for (chunk_page_index = 0; chunk_page_index < num_chunk_pages; ++chunk_page_index)
            pages[page_index++] = page + chunk_page_index;
    }
    UVM_ASSERT(page_index == num_pages);

    mem->kernel.cpu_addr = vmap(pages, num_pages, VM_MAP, PAGE_KERNEL);

    uvm_kvfree(pages);

    if (!mem->kernel.cpu_addr)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_mem_unmap_cpu_kernel(uvm_mem_t *mem)
{
    if (!uvm_mem_mapped_on_cpu_kernel(mem))
        return;

    vunmap(mem->kernel.cpu_addr);
    mem->kernel.cpu_addr = NULL;
    mem_clear_mapped_on_cpu_kernel(mem);
}

static NV_STATUS mem_map_cpu_to_sysmem_user(uvm_mem_t *mem, struct vm_area_struct *vma)
{
    NV_STATUS status;
    NvU64 offset;

    UVM_ASSERT(mem->user != NULL);
    UVM_ASSERT(uvm_mem_is_sysmem(mem));
    uvm_assert_mmap_lock_locked(vma->vm_mm);

    // TODO: Bug 1995015: high-order page allocations need to be allocated as
    // compound pages in order to be able to use vm_insert_page on them. This
    // is not currently being exercised because the only allocations using this
    // are semaphore pools (which typically use a single page).
    for (offset = 0; offset < uvm_mem_physical_size(mem); offset += PAGE_SIZE) {
        int ret = vm_insert_page(vma, (unsigned long)mem->user->addr + offset, mem_cpu_page(mem, offset));
        if (ret) {
            UVM_ASSERT_MSG(ret == -ENOMEM, "ret: %d\n", ret);
            status = errno_to_nv_status(ret);
            goto error;
        }
    }

    return NV_OK;

error:
    unmap_mapping_range(mem->user->va_space->mapping, (size_t)mem->user->addr, uvm_mem_physical_size(mem), 1);
    return status;
}

void uvm_mem_unmap_cpu_user(uvm_mem_t *mem)
{
    if (!uvm_mem_mapped_on_cpu_user(mem))
        return;

    unmap_mapping_range(mem->user->va_space->mapping, (size_t)mem->user->addr, uvm_mem_physical_size(mem), 1);
    mem_clear_mapped_on_cpu_user(mem);
    mem_deinit_user_mapping(mem);
}

NV_STATUS uvm_mem_map_cpu_user(uvm_mem_t *mem, uvm_va_space_t *user_va_space, struct vm_area_struct *vma)
{
    NV_STATUS status;
    void *user_addr;

    UVM_ASSERT(mem);
    UVM_ASSERT(mem_can_be_mapped_on_cpu_user(mem));

    if (uvm_mem_mapped_on_cpu_user(mem))
        return NV_OK;

    UVM_ASSERT((vma->vm_end - vma->vm_start) == mem->size);

    user_addr = (void *) (uintptr_t)vma->vm_start;

    status = mem_init_user_mapping(mem, user_va_space, user_addr);
    if (status != NV_OK)
        return status;

    status = mem_map_cpu_to_sysmem_user(mem, vma);
    if (status != NV_OK)
        goto cleanup;

    mem_set_mapped_on_cpu_user(mem);

    return NV_OK;

cleanup:
    mem_deinit_user_mapping(mem);
    return status;
}

NV_STATUS uvm_mem_map_cpu_kernel(uvm_mem_t *mem)
{
    NV_STATUS status;

    UVM_ASSERT(mem);
    UVM_ASSERT(mem_can_be_mapped_on_cpu_kernel(mem));

    if (uvm_mem_mapped_on_cpu_kernel(mem))
        return NV_OK;

    if (uvm_mem_is_sysmem(mem))
        status = mem_map_cpu_to_sysmem_kernel(mem);
    else
        status = mem_map_cpu_to_vidmem_kernel(mem);

    if (status != NV_OK)
        return status;

    mem_set_mapped_on_cpu_kernel(mem);

    return NV_OK;
}

static void sysmem_unmap_gpu_phys(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    NvU64 *dma_addrs = mem->sysmem.dma_addrs[uvm_id_gpu_index(gpu->id)];
    NvU32 i;

    UVM_ASSERT(uvm_mem_is_sysmem(mem));
    UVM_ASSERT(gpu != mem->dma_owner);
    UVM_ASSERT(dma_addrs);

    for (i = 0; i < mem->chunks_count; ++i) {
        if (dma_addrs[i] == 0) {
            // The DMA address can only be 0 when cleaning up after a failed
            // partial map_gpu_sysmem_iommu() operation.
            break;
        }
        uvm_parent_gpu_unmap_cpu_pages(gpu->parent, dma_addrs[i], mem->chunk_size);
        dma_addrs[i] = 0;
    }

    uvm_kvfree(dma_addrs);
    mem->sysmem.dma_addrs[uvm_id_gpu_index(gpu->id)] = NULL;
}

static NV_STATUS sysmem_map_gpu_phys(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    NV_STATUS status;
    size_t i;

    UVM_ASSERT(uvm_mem_is_sysmem(mem));
    UVM_ASSERT(gpu != mem->dma_owner);

    status = mem_alloc_dma_addrs(mem, gpu);
    if (status != NV_OK)
        return status;

    for (i = 0; i < mem->chunks_count; ++i) {
        status = uvm_parent_gpu_map_cpu_pages(gpu->parent,
                                              mem->sysmem.pages[i],
                                              mem->chunk_size,
                                              &mem->sysmem.dma_addrs[uvm_id_gpu_index(gpu->id)][i]);
        if (status != NV_OK)
            goto error;
    }

    return NV_OK;

error:
    sysmem_unmap_gpu_phys(mem, gpu);
    return status;
}

static uvm_gpu_chunk_t *mem_get_chunk(uvm_mem_t *mem, size_t mem_offset, size_t *offset_in_chunk)
{
    size_t chunk_index = uvm_div_pow2_64(mem_offset, mem->chunk_size);

    if (offset_in_chunk)
        *offset_in_chunk = mem_offset & (mem->chunk_size - 1);

    UVM_ASSERT(uvm_mem_is_vidmem(mem));
    return mem->vidmem.chunks[chunk_index];
}

static uvm_gpu_phys_address_t mem_gpu_physical_vidmem(uvm_mem_t *mem, size_t offset)
{
    size_t chunk_offset;
    uvm_gpu_chunk_t *chunk = mem_get_chunk(mem, offset, &chunk_offset);
    return uvm_gpu_phys_address(UVM_APERTURE_VID, chunk->address + chunk_offset);
}

static uvm_gpu_phys_address_t mem_gpu_physical_sysmem(uvm_mem_t *mem, uvm_gpu_t *gpu, size_t offset)
{
    NvU64 *dma_addrs = mem->sysmem.dma_addrs[uvm_id_gpu_index(gpu->id)];
    NvU64 dma_addr = dma_addrs[offset / mem->chunk_size];

    UVM_ASSERT(uvm_mem_is_sysmem(mem));
    UVM_ASSERT(sysmem_mapped_on_gpu_phys(mem, gpu));

    return uvm_gpu_phys_address(UVM_APERTURE_SYS, dma_addr + offset % mem->chunk_size);
}

bool uvm_mem_is_physically_contiguous(uvm_mem_t *mem, NvU64 offset, NvU64 size)
{
    UVM_ASSERT(size != 0);
    UVM_ASSERT((offset + size) <= uvm_mem_physical_size(mem));

    return UVM_ALIGN_DOWN(offset, mem->chunk_size) == UVM_ALIGN_DOWN(offset + size - 1, mem->chunk_size);
}

uvm_gpu_phys_address_t uvm_mem_gpu_physical(uvm_mem_t *mem, uvm_gpu_t *gpu, NvU64 offset, NvU64 size)
{
    UVM_ASSERT(uvm_mem_is_physically_contiguous(mem, offset, size));

    if (uvm_mem_is_vidmem(mem)) {
        UVM_ASSERT(uvm_mem_is_local_vidmem(mem, gpu));

        return mem_gpu_physical_vidmem(mem, offset);
    }

    return mem_gpu_physical_sysmem(mem, gpu, offset);
}

uvm_gpu_address_t uvm_mem_gpu_address_copy(uvm_mem_t *mem, uvm_gpu_t *accessing_gpu, NvU64 offset, NvU64 size)
{
    uvm_gpu_address_t copy_addr;
    size_t chunk_offset;
    uvm_gpu_chunk_t *chunk;

    UVM_ASSERT(uvm_mem_is_physically_contiguous(mem, offset, size));

    if (uvm_mem_is_sysmem(mem) || uvm_mem_is_local_vidmem(mem, accessing_gpu))
        return uvm_gpu_address_copy(accessing_gpu, uvm_mem_gpu_physical(mem, accessing_gpu, offset, size));

    // Peer GPUs may need to use some form of translation (identity mappings,
    // indirect peers) to copy.
    chunk = mem_get_chunk(mem, offset, &chunk_offset);
    copy_addr = uvm_pmm_gpu_peer_copy_address(&mem->backing_gpu->pmm, chunk, accessing_gpu);
    copy_addr.address += chunk_offset;
    return copy_addr;
}

typedef struct uvm_mem_pte_maker_data_struct
{
    uvm_mem_t *mem;
    const uvm_mem_gpu_mapping_attrs_t *attrs;
} uvm_mem_pte_maker_data_t;

static NvU64 mem_pte_maker(uvm_page_table_range_vec_t *range_vec, NvU64 offset, void *vp_data)
{
    uvm_mem_pte_maker_data_t *data = (uvm_mem_pte_maker_data_t *)vp_data;
    uvm_page_tree_t *tree = range_vec->tree;
    uvm_gpu_t *gpu = tree->gpu;
    uvm_gpu_phys_address_t phys = uvm_mem_gpu_physical(data->mem, gpu, offset, range_vec->page_size);

    return tree->hal->make_pte(phys.aperture,
                               phys.address,
                               data->attrs->protection,
                               data->attrs->is_cacheable ? UVM_MMU_PTE_FLAGS_CACHED : UVM_MMU_PTE_FLAGS_NONE);
}

static void mem_unmap_gpu(uvm_mem_t *mem, uvm_gpu_t *gpu, uvm_page_table_range_vec_t **range_vec)
{
    uvm_membar_t tlb_membar = uvm_hal_downgrade_membar_type(gpu, uvm_mem_is_local_vidmem(mem, gpu));
    NV_STATUS status = uvm_page_table_range_vec_clear_ptes(*range_vec, tlb_membar);
    if (status != NV_OK)
        UVM_ERR_PRINT("Clearing PTEs failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));

    uvm_page_table_range_vec_destroy(*range_vec);
    *range_vec = NULL;
}

static NV_STATUS mem_map_gpu(uvm_mem_t *mem,
                             uvm_gpu_t *gpu,
                             NvU64 gpu_va,
                             uvm_page_tree_t *tree,
                             const uvm_mem_gpu_mapping_attrs_t *attrs,
                             uvm_page_table_range_vec_t **range_vec)
{
    NV_STATUS status;
    NvU64 page_size;
    uvm_pmm_alloc_flags_t pmm_flags = UVM_PMM_ALLOC_FLAGS_EVICT;

    uvm_mem_pte_maker_data_t pte_maker_data = {
            .mem = mem,
            .attrs = attrs
        };

    page_size = mem_pick_gpu_page_size(mem, gpu, tree);
    UVM_ASSERT_MSG(uvm_mmu_page_size_supported(tree, page_size), "page_size 0x%llx\n", page_size);

    // When the Confidential Computing feature is enabled, DMA allocations are
    // majoritarily allocated and managed by a per-GPU DMA buffer pool
    // (uvm_conf_computing_dma_buffer_pool_t). Because we would typically
    // already hold the DMA_BUFFER_POOL lock at this time, we cannot hold
    // the block lock. Allocate PTEs without eviction in this context.
    //
    // See uvm_pmm_gpu_alloc()
    if (uvm_mem_is_sysmem_dma(mem))
        pmm_flags = UVM_PMM_ALLOC_FLAGS_NONE;

    status = uvm_page_table_range_vec_create(tree,
                                             gpu_va,
                                             uvm_mem_physical_size(mem),
                                             page_size,
                                             pmm_flags,
                                             range_vec);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init page mapping at [0x%llx, 0x%llx): %s, GPU %s\n",
                      gpu_va,
                      gpu_va + uvm_mem_physical_size(mem),
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    status = uvm_page_table_range_vec_write_ptes(*range_vec, UVM_MEMBAR_NONE, mem_pte_maker, &pte_maker_data);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to write PTEs for mapping at [0x%llx, 0x%llx): %s, GPU %s\n",
                      gpu_va,
                      gpu_va + uvm_mem_physical_size(mem),
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        goto error;
    }

    return NV_OK;

error:
    mem_unmap_gpu(mem, gpu, range_vec);
    return status;
}

static NV_STATUS mem_init_gpu_kernel_range(uvm_mem_t *mem)
{
    if (mem->kernel.range_alloc.node != NULL)
        return NV_OK;

    return uvm_range_allocator_alloc(&g_free_ranges,
                                     uvm_mem_physical_size(mem),
                                     mem->chunk_size,
                                     &mem->kernel.range_alloc);
}

static void mem_deinit_gpu_kernel_range(uvm_mem_t *mem)
{
    if (mem->kernel.range_alloc.node == NULL)
        return;

    // Do not remove the range allocation if there is any GPU where the memory
    // is still mapped on kernel space.
    if (UVM_ID_IS_VALID(uvm_processor_mask_find_first_gpu_id(&mem->kernel.mapped_on)))
        return;

    uvm_range_allocator_free(&g_free_ranges, &mem->kernel.range_alloc);
}

NV_STATUS uvm_mem_map_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    NV_STATUS status;
    NvU64 gpu_va;
    uvm_page_table_range_vec_t **range_vec;
    uvm_mem_gpu_mapping_attrs_t attrs = {
            .protection = UVM_PROT_READ_WRITE_ATOMIC,
            .is_cacheable = uvm_mem_is_vidmem(mem)
        };

    UVM_ASSERT(mem_can_be_mapped_on_gpu_kernel(mem, gpu));

    if (uvm_mem_mapped_on_gpu_kernel(mem, gpu))
        return NV_OK;

    status = uvm_mem_map_gpu_phys(mem, gpu);
    if (status != NV_OK)
        return status;

    status = mem_init_gpu_kernel_range(mem);
    if (status != NV_OK)
        return status;

    gpu_va = reserved_gpu_va(mem, gpu);
    range_vec = &mem->kernel.range_vecs[uvm_id_gpu_index(gpu->id)];

    status = mem_map_gpu(mem, gpu, gpu_va, &gpu->address_space_tree, &attrs, range_vec);
    if (status != NV_OK)
        goto cleanup;

    mem_set_mapped_on_gpu_kernel(mem, gpu);

    return NV_OK;

cleanup:
    mem_deinit_gpu_kernel_range(mem);

    return status;
}

NV_STATUS uvm_mem_map_gpu_user(uvm_mem_t *mem,
                               uvm_gpu_t *gpu,
                               uvm_va_space_t *user_va_space,
                               void *user_addr,
                               const uvm_mem_gpu_mapping_attrs_t *attrs)
{
    NV_STATUS status;
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_page_table_range_vec_t **range_vec;
    NvU64 gpu_va;

    UVM_ASSERT(mem_can_be_mapped_on_gpu_user(mem, gpu));
    uvm_assert_rwsem_locked(&user_va_space->lock);

    if (uvm_mem_mapped_on_gpu_user(mem, gpu))
        return NV_OK;

    gpu_va = (NvU64)user_addr;
    if (!uvm_gpu_can_address(gpu, gpu_va, mem->size))
        return NV_ERR_OUT_OF_RANGE;

    status = uvm_mem_map_gpu_phys(mem, gpu);
    if (status != NV_OK)
        return status;

    status = mem_init_user_mapping(mem, user_va_space, user_addr);
    if (status != NV_OK)
        return status;

    gpu_va_space = uvm_gpu_va_space_get(mem->user->va_space, gpu);
    range_vec = &mem->user->range_vecs[uvm_id_gpu_index(gpu->id)];

    status = mem_map_gpu(mem, gpu, gpu_va, &gpu_va_space->page_tables, attrs, range_vec);
    if (status != NV_OK)
        goto cleanup;

    mem_set_mapped_on_gpu_user(mem, gpu);

    return NV_OK;

cleanup:
    mem_deinit_user_mapping(mem);
    return status;
}

void uvm_mem_unmap_gpu_user(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    if (!uvm_mem_mapped_on_gpu_user(mem, gpu))
        return;

    mem_unmap_gpu(mem, gpu, &mem->user->range_vecs[uvm_id_gpu_index(gpu->id)]);
    mem_clear_mapped_on_gpu_user(mem, gpu);
    mem_deinit_user_mapping(mem);
}

void uvm_mem_unmap_gpu_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    if (!uvm_mem_mapped_on_gpu_kernel(mem, gpu))
        return;

    mem_unmap_gpu(mem, gpu, &mem->kernel.range_vecs[uvm_id_gpu_index(gpu->id)]);
    mem_clear_mapped_on_gpu_kernel(mem, gpu);
    mem_deinit_gpu_kernel_range(mem);
}

static bool mem_can_be_phys_mapped_on_gpu(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    if (uvm_mem_is_sysmem(mem))
        return sysmem_can_be_mapped_on_gpu(mem);
    else
        return uvm_mem_is_local_vidmem(mem, gpu);
}

NV_STATUS uvm_mem_map_gpu_phys(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    NV_STATUS status;

    UVM_ASSERT(mem_can_be_phys_mapped_on_gpu(mem, gpu));

    if (uvm_mem_is_vidmem(mem))
        return NV_OK;

    if (gpu == mem->dma_owner)
        return NV_OK;

    if (sysmem_mapped_on_gpu_phys(mem, gpu))
        return NV_OK;

    status = sysmem_map_gpu_phys(mem, gpu);
    if (status != NV_OK)
        return status;

    sysmem_set_mapped_on_gpu_phys(mem, gpu);
    return NV_OK;
}

void uvm_mem_unmap_gpu_phys(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(mem);
    UVM_ASSERT(gpu);

    if (uvm_mem_is_vidmem(mem))
        return;

    // GPU for which the mapping is managed by the dma_alloc_coherent
    // API will be unmapped when the allocation is freed.
    if (gpu == mem->dma_owner)
        return;

    if (!sysmem_mapped_on_gpu_phys(mem, gpu))
        return;

    uvm_mem_unmap_gpu_user(mem, gpu);
    uvm_mem_unmap_gpu_kernel(mem, gpu);

    sysmem_unmap_gpu_phys(mem, gpu);
    sysmem_clear_mapped_on_gpu_phys(mem, gpu);
}

void uvm_mem_free(uvm_mem_t *mem)
{
    uvm_gpu_t *gpu;

    if (mem == NULL)
        return;

    uvm_mem_unmap_cpu_user(mem);
    uvm_mem_unmap_cpu_kernel(mem);

    if (mem->user != NULL) {
        for_each_gpu_in_mask(gpu, &mem->user->mapped_on) {
            uvm_mem_unmap_gpu_user(mem, gpu);

            // If we unmapped the last device, the user mapping is freed, so
            // exit the loop before the iterator accesses a non-existing mask.
            if (mem->user == NULL)
                break;
        }
    }

    for_each_gpu_in_mask(gpu, &mem->kernel.mapped_on)
        uvm_mem_unmap_gpu_kernel(mem, gpu);

    if (uvm_mem_is_sysmem(mem)) {
        for_each_gpu_in_mask(gpu, &mem->sysmem.mapped_on_phys)
            uvm_mem_unmap_gpu_phys(mem, gpu);
    }

    mem_free_chunks(mem);

    uvm_kvfree(mem);
}

void *uvm_mem_get_cpu_addr_kernel(uvm_mem_t *mem)
{
    UVM_ASSERT(uvm_mem_mapped_on_cpu_kernel(mem));

    return mem->kernel.cpu_addr;
}

NvU64 uvm_mem_get_gpu_va_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(uvm_mem_mapped_on_gpu_kernel(mem, gpu));

    return reserved_gpu_va(mem, gpu);
}

uvm_gpu_address_t uvm_mem_gpu_address_virtual_kernel(uvm_mem_t *mem, uvm_gpu_t *gpu)
{
    uvm_gpu_address_t addr = uvm_gpu_address_virtual(uvm_mem_get_gpu_va_kernel(mem, gpu));

    if (g_uvm_global.conf_computing_enabled && mem->dma_owner)
        addr.is_unprotected = true;

    return addr;
}

uvm_gpu_address_t uvm_mem_gpu_address_physical(uvm_mem_t *mem, uvm_gpu_t *gpu, NvU64 offset, NvU64 size)
{
    uvm_gpu_address_t addr = uvm_gpu_address_from_phys(uvm_mem_gpu_physical(mem, gpu, offset, size));

    if (g_uvm_global.conf_computing_enabled && mem->dma_owner)
        addr.is_unprotected = true;

    return addr;
}
