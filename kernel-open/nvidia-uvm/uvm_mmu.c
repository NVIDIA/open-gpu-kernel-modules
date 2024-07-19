/*******************************************************************************
    Copyright (c) 2015-2024 NVIDIA Corporation

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
#include "uvm_types.h"
#include "uvm_forward_decl.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_mmu.h"
#include "uvm_hal.h"
#include "uvm_kvmalloc.h"
#include "uvm_pte_batch.h"
#include "uvm_tlb_batch.h"
#include "uvm_push.h"
#include "uvm_mem.h"
#include "uvm_va_space.h"

#include <linux/mm.h>

// The page tree has 6 levels on Hopper+ GPUs, and the root is never freed by a
// normal 'put' operation which leaves a maximum of 5 levels.
#define MAX_OPERATION_DEPTH 5

// Wrappers for push begin handling channel_manager not being there when running
// the page tree unit test
//
// When in SR-IOV heavy, the push needs to be associated with the proxy channel
// (UVM_CHANNEL_TYPE_MEMOPS), because it is used to manipulate the page tree.
// In any other scenario using UVM_CHANNEL_TYPE_GPU_INTERNAL is preferred,
// because that type is normally associated with the LCE mapped to the most
// PCEs. The higher bandwidth is beneficial when doing bulk operations such as
// clearing PTEs, or initializing a page directory/table.
#define page_tree_begin_acquire(tree, tracker, push, format, ...) ({                                                        \
    NV_STATUS status;                                                                                                       \
    uvm_channel_manager_t *manager = (tree)->gpu->channel_manager;                                                          \
                                                                                                                            \
    if (manager == NULL)                                                                                                    \
        status = uvm_push_begin_fake((tree)->gpu, (push));                                                                  \
    else if (uvm_parent_gpu_is_virt_mode_sriov_heavy((tree)->gpu->parent))                                                  \
        status = uvm_push_begin_acquire(manager, UVM_CHANNEL_TYPE_MEMOPS, (tracker), (push), (format), ##__VA_ARGS__);      \
    else                                                                                                                    \
        status = uvm_push_begin_acquire(manager, UVM_CHANNEL_TYPE_GPU_INTERNAL, (tracker), (push), (format), ##__VA_ARGS__);\
                                                                                                                            \
    status;                                                                                                                 \
})

// Default location of page table allocations
static uvm_aperture_t page_table_aperture = UVM_APERTURE_DEFAULT;

static char *uvm_page_table_location;
module_param(uvm_page_table_location, charp, S_IRUGO);
MODULE_PARM_DESC(uvm_page_table_location,
                "Set the location for UVM-allocated page tables. Choices are: vid, sys.");

NV_STATUS uvm_mmu_init(void)
{
    UVM_ASSERT((page_table_aperture == UVM_APERTURE_VID) ||
               (page_table_aperture == UVM_APERTURE_SYS) ||
               (page_table_aperture == UVM_APERTURE_DEFAULT));

    if (!uvm_page_table_location)
        return NV_OK;

    // TODO: Bug 1766651: Add modes for testing, e.g. alternating vidmem and
    //       sysmem etc.
    if (strcmp(uvm_page_table_location, "vid") == 0) {
        page_table_aperture = UVM_APERTURE_VID;
    }
    else if (strcmp(uvm_page_table_location, "sys") == 0) {
        page_table_aperture = UVM_APERTURE_SYS;
    }
    else {
        pr_info("Invalid uvm_page_table_location %s. Using %s instead.\n",
                uvm_page_table_location,
                uvm_aperture_string(page_table_aperture));
    }

    return NV_OK;
}

static NV_STATUS phys_mem_allocate_sysmem(uvm_page_tree_t *tree, NvLength size, uvm_mmu_page_table_alloc_t *out)
{
    NV_STATUS status = NV_OK;
    NvU64 dma_addr;
    unsigned long flags = __GFP_ZERO;
    uvm_memcg_context_t memcg_context;
    uvm_va_space_t *va_space = NULL;
    struct mm_struct *mm = NULL;

    if (tree->type == UVM_PAGE_TREE_TYPE_USER && tree->gpu_va_space && UVM_CGROUP_ACCOUNTING_SUPPORTED()) {
        va_space = tree->gpu_va_space->va_space;
        mm = uvm_va_space_mm_retain(va_space);
        if (mm)
            uvm_memcg_context_start(&memcg_context, mm);
    }

    // If mm is not NULL, memcg context has been started and we can use
    // the account flags.
    if (mm)
        flags |= NV_UVM_GFP_FLAGS_ACCOUNT;
    else
        flags |= NV_UVM_GFP_FLAGS;

    out->handle.page = alloc_pages(flags, get_order(size));

    // va_space and mm will be set only if the memcg context has been started.
    if (mm) {
        uvm_memcg_context_end(&memcg_context);
        uvm_va_space_mm_release(va_space);
    }

    if (out->handle.page == NULL)
        return NV_ERR_NO_MEMORY;

    // Check for fake GPUs from the unit test
    if (tree->gpu->parent->pci_dev)
        status = uvm_parent_gpu_map_cpu_pages(tree->gpu->parent, out->handle.page, UVM_PAGE_ALIGN_UP(size), &dma_addr);
    else
        dma_addr = page_to_phys(out->handle.page);

    if (status != NV_OK) {
        __free_pages(out->handle.page, get_order(size));
        return status;
    }

    out->addr = uvm_gpu_phys_address(UVM_APERTURE_SYS, dma_addr);
    out->size = size;

    return NV_OK;
}

// The aperture may filter the biggest page size:
// - UVM_APERTURE_VID       biggest page size on vidmem mappings
// - UVM_APERTURE_SYS       biggest page size on sysmem mappings
// - UVM_APERTURE_PEER_0-7  biggest page size on peer mappings
static NvU64 mmu_biggest_page_size(uvm_page_tree_t *tree, uvm_aperture_t aperture)
{
    UVM_ASSERT(aperture < UVM_APERTURE_DEFAULT);

    // There may be scenarios where the GMMU must use a subset of the supported
    // page sizes, e.g., to comply with the vMMU supported page sizes due to
    // segmentation sizes.
    if (aperture == UVM_APERTURE_VID)
        return uvm_mmu_biggest_page_size_up_to(tree, tree->gpu->mem_info.max_vidmem_page_size);

    return 1ULL << __fls(tree->hal->page_sizes());
}

static NV_STATUS phys_mem_allocate_vidmem(uvm_page_tree_t *tree,
                                          NvLength size,
                                          uvm_pmm_alloc_flags_t pmm_flags,
                                          uvm_mmu_page_table_alloc_t *out)
{
    NV_STATUS status;
    uvm_gpu_t *gpu = tree->gpu;
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();

    status = uvm_pmm_gpu_alloc_kernel(&gpu->pmm, 1, size, pmm_flags, &out->handle.chunk, &local_tracker);
    if (status != NV_OK)
        return status;

    if (!uvm_tracker_is_empty(&local_tracker)) {
        uvm_mutex_lock(&tree->lock);
        status = uvm_tracker_add_tracker_safe(&tree->tracker, &local_tracker);
        uvm_mutex_unlock(&tree->lock);
    }

    uvm_tracker_deinit(&local_tracker);

    if (status != NV_OK) {
        uvm_pmm_gpu_free(&tree->gpu->pmm, out->handle.chunk, NULL);
        return status;
    }

    out->addr = uvm_gpu_phys_address(UVM_APERTURE_VID, out->handle.chunk->address);
    out->size = size;

    return status;
}

static NV_STATUS phys_mem_allocate(uvm_page_tree_t *tree,
                                   NvLength size,
                                   uvm_aperture_t location,
                                   uvm_pmm_alloc_flags_t pmm_flags,
                                   uvm_mmu_page_table_alloc_t *out)
{
    UVM_ASSERT((location == UVM_APERTURE_VID) || (location == UVM_APERTURE_SYS));

    memset(out, 0, sizeof(*out));

    if (location == UVM_APERTURE_SYS)
        return phys_mem_allocate_sysmem(tree, size, out);
    else
        return phys_mem_allocate_vidmem(tree, size, pmm_flags, out);
}

static void phys_mem_deallocate_vidmem(uvm_page_tree_t *tree, uvm_mmu_page_table_alloc_t *ptr)
{
    uvm_assert_mutex_locked(&tree->lock);
    UVM_ASSERT(ptr->addr.aperture == UVM_APERTURE_VID);

    uvm_pmm_gpu_free(&tree->gpu->pmm, ptr->handle.chunk, &tree->tracker);
}

static void phys_mem_deallocate_sysmem(uvm_page_tree_t *tree, uvm_mmu_page_table_alloc_t *ptr)
{
    NV_STATUS status;

    uvm_assert_mutex_locked(&tree->lock);

    // Synchronize any pending operations before freeing the memory that might
    // be used by them.
    status = uvm_tracker_wait(&tree->tracker);
    if (status != NV_OK)
        UVM_ASSERT(status == uvm_global_get_status());

    UVM_ASSERT(ptr->addr.aperture == UVM_APERTURE_SYS);
    if (tree->gpu->parent->pci_dev)
        uvm_parent_gpu_unmap_cpu_pages(tree->gpu->parent, ptr->addr.address, UVM_PAGE_ALIGN_UP(ptr->size));
    __free_pages(ptr->handle.page, get_order(ptr->size));
}

static void phys_mem_deallocate(uvm_page_tree_t *tree, uvm_mmu_page_table_alloc_t *ptr)
{
    if (ptr->addr.aperture == UVM_APERTURE_SYS)
        phys_mem_deallocate_sysmem(tree, ptr);
    else
        phys_mem_deallocate_vidmem(tree, ptr);

    memset(ptr, 0, sizeof(*ptr));
}

static void page_table_range_init(uvm_page_table_range_t *range,
                                 NvU64 page_size,
                                 uvm_page_directory_t *dir,
                                 NvU32 start_index,
                                 NvU32 end_index)
{
    range->table = dir;
    range->start_index = start_index;
    range->entry_count = 1 + end_index - start_index;
    range->page_size = page_size;
    dir->ref_count += range->entry_count;
}

static bool uvm_mmu_use_cpu(uvm_page_tree_t *tree)
{
    // When physical CE writes can't be used for vidmem we use a flat virtual
    // mapping instead. The GPU PTEs for that flat mapping have to be
    // bootstrapped using the CPU.
    return tree->location != UVM_APERTURE_SYS &&
           !tree->gpu->parent->ce_phys_vidmem_write_supported &&
           !tree->gpu->static_flat_mapping.ready;
}

// uvm_mmu_page_table_page() and the uvm_mmu_page_table_cpu_* family of
// functions can only be used when uvm_mmu_use_cpu() returns true, which implies
// a coherent system.

static struct page *uvm_mmu_page_table_page(uvm_gpu_t *gpu, uvm_mmu_page_table_alloc_t *phys_alloc)
{
    // All platforms that require CPU PTE writes for bootstrapping can fit
    // tables within a page.
    UVM_ASSERT(phys_alloc->size <= PAGE_SIZE);

    if (phys_alloc->addr.aperture == UVM_APERTURE_SYS)
        return phys_alloc->handle.page;

    return uvm_gpu_chunk_to_page(&gpu->pmm, phys_alloc->handle.chunk);
}

static void *uvm_mmu_page_table_cpu_map(uvm_gpu_t *gpu, uvm_mmu_page_table_alloc_t *phys_alloc)
{
    struct page *page = uvm_mmu_page_table_page(gpu, phys_alloc);
    NvU64 page_offset = offset_in_page(phys_alloc->addr.address);
    return (char *)kmap(page) + page_offset;
}

static void uvm_mmu_page_table_cpu_unmap(uvm_gpu_t *gpu, uvm_mmu_page_table_alloc_t *phys_alloc)
{
    kunmap(uvm_mmu_page_table_page(gpu, phys_alloc));
}

static void uvm_mmu_page_table_cpu_memset_8(uvm_gpu_t *gpu,
                                            uvm_mmu_page_table_alloc_t *phys_alloc,
                                            NvU32 start_index,
                                            NvU64 pattern,
                                            NvU32 num_entries)
{
    NvU64 *ptr = uvm_mmu_page_table_cpu_map(gpu, phys_alloc);
    size_t i;

    UVM_ASSERT(IS_ALIGNED((uintptr_t)ptr, sizeof(*ptr)));
    UVM_ASSERT((start_index + num_entries) * sizeof(*ptr) <= phys_alloc->size);

    for (i = 0; i < num_entries; i++)
        ptr[start_index + i] = pattern;

    uvm_mmu_page_table_cpu_unmap(gpu, phys_alloc);
}

static void uvm_mmu_page_table_cpu_memset_16(uvm_gpu_t *gpu,
                                             uvm_mmu_page_table_alloc_t *phys_alloc,
                                             NvU32 start_index,
                                             NvU64 *pattern,
                                             NvU32 num_entries)
{
    struct
    {
        NvU64 u0, u1;
    } *ptr;
    size_t i;

    ptr = uvm_mmu_page_table_cpu_map(gpu, phys_alloc);
    UVM_ASSERT(IS_ALIGNED((uintptr_t)ptr, sizeof(*ptr)));
    UVM_ASSERT((start_index + num_entries) * sizeof(*ptr) <= phys_alloc->size);

    for (i = 0; i < num_entries; i++)
        memcpy(&ptr[start_index + i], pattern, sizeof(*ptr));

    uvm_mmu_page_table_cpu_unmap(gpu, phys_alloc);
}

static void pde_fill_cpu(uvm_page_tree_t *tree,
                         uvm_page_directory_t *directory,
                         NvU32 start_index,
                         NvU32 pde_count,
                         uvm_mmu_page_table_alloc_t **phys_addr)
{
    NvU64 pde_data[2], entry_size;
    NvU32 i;

    UVM_ASSERT(uvm_mmu_use_cpu(tree));

    entry_size = tree->hal->entry_size(directory->depth);
    UVM_ASSERT(sizeof(pde_data) >= entry_size);

    for (i = 0; i < pde_count; i++) {
        tree->hal->make_pde(pde_data, phys_addr, directory, start_index + i);

        if (entry_size == sizeof(pde_data[0]))
            uvm_mmu_page_table_cpu_memset_8(tree->gpu, &directory->phys_alloc, start_index + i, pde_data[0], 1);
        else
            uvm_mmu_page_table_cpu_memset_16(tree->gpu, &directory->phys_alloc, start_index + i, pde_data, 1);
    }
}

static void pde_fill_gpu(uvm_page_tree_t *tree,
                         uvm_page_directory_t *directory,
                         NvU32 start_index,
                         NvU32 pde_count,
                         uvm_mmu_page_table_alloc_t **phys_addr,
                         uvm_push_t *push)
{
    NvU64 pde_data[2], entry_size;
    uvm_gpu_address_t pde_entry_addr = uvm_mmu_gpu_address(tree->gpu, directory->phys_alloc.addr);
    NvU32 max_inline_entries;
    uvm_push_flag_t push_membar_flag = UVM_PUSH_FLAG_COUNT;
    uvm_gpu_address_t inline_data_addr;
    uvm_push_inline_data_t inline_data;
    NvU32 entry_count, i, j;

    UVM_ASSERT(!uvm_mmu_use_cpu(tree));

    entry_size = tree->hal->entry_size(directory->depth);
    UVM_ASSERT(sizeof(pde_data) >= entry_size);

    max_inline_entries = UVM_PUSH_INLINE_DATA_MAX_SIZE / entry_size;

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE))
        push_membar_flag = UVM_PUSH_FLAG_NEXT_MEMBAR_NONE;
    else if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU))
        push_membar_flag = UVM_PUSH_FLAG_NEXT_MEMBAR_GPU;

    pde_entry_addr.address += start_index * entry_size;

    for (i = 0; i < pde_count;) {
        // All but the first memory operation can be pipelined. We respect the
        // caller's pipelining settings for the first push.
        if (i != 0)
            uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);

        entry_count = min(pde_count - i, max_inline_entries);

        // No membar is needed until the last memory operation. Otherwise,
        // use caller's membar flag.
        if ((i + entry_count) < pde_count)
            uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        else if (push_membar_flag != UVM_PUSH_FLAG_COUNT)
            uvm_push_set_flag(push, push_membar_flag);

        uvm_push_inline_data_begin(push, &inline_data);
        for (j = 0; j < entry_count; j++) {
            tree->hal->make_pde(pde_data, phys_addr, directory, start_index + i + j);
            uvm_push_inline_data_add(&inline_data, pde_data, entry_size);
        }
        inline_data_addr = uvm_push_inline_data_end(&inline_data);

        tree->gpu->parent->ce_hal->memcopy(push, pde_entry_addr, inline_data_addr, entry_count * entry_size);

        i += entry_count;
        pde_entry_addr.address += entry_size * entry_count;
    }
}

// pde_fill() populates pde_count PDE entries (starting at start_index) with
// the same mapping, i.e., with the same physical address (phys_addr).
// pde_fill() is optimized for pde_count == 1, which is the common case.
static void pde_fill(uvm_page_tree_t *tree,
                     uvm_page_directory_t *directory,
                     NvU32 start_index,
                     NvU32 pde_count,
                     uvm_mmu_page_table_alloc_t **phys_addr,
                     uvm_push_t *push)
{
    UVM_ASSERT(start_index + pde_count <= uvm_mmu_page_tree_entries(tree, directory->depth, UVM_PAGE_SIZE_AGNOSTIC));

    if (push)
        pde_fill_gpu(tree, directory, start_index, pde_count, phys_addr, push);
    else
        pde_fill_cpu(tree, directory, start_index, pde_count, phys_addr);
}

static void phys_mem_init(uvm_page_tree_t *tree, NvU64 page_size, uvm_page_directory_t *dir, uvm_push_t *push)
{
    NvU64 entries_count = uvm_mmu_page_tree_entries(tree, dir->depth, page_size);
    NvU8 max_pde_depth = tree->hal->page_table_depth(UVM_PAGE_SIZE_AGNOSTIC) - 1;

    // Passing in NULL for the phys_allocs will mark the child entries as
    // invalid.
    uvm_mmu_page_table_alloc_t *phys_allocs[2] = {NULL, NULL};

    // Init with an invalid PTE or clean PDE. Only Maxwell PDEs can have more
    // than 512 entries. In this case, we initialize them all with the same
    // clean PDE. ATS systems may require clean PDEs with
    // ATS_ALLOWED/ATS_NOT_ALLOWED bit settings based on the mapping VA.
    // We only clean_bits to 0 at the lowest page table level (PTE table), i.e.,
    // when depth is greater than the max_pde_depth.
    if ((dir->depth > max_pde_depth) || (entries_count > 512 && !g_uvm_global.ats.enabled)) {
        NvU64 clear_bits[2];

        // If it is not a PTE, make a clean PDE.
        if (dir->depth != tree->hal->page_table_depth(page_size)) {
            // make_pde() child index is zero/ignored, since it is only used in
            // PDEs on ATS-enabled systems where pde_fill() is preferred.
            tree->hal->make_pde(clear_bits, phys_allocs, dir, 0);

            // Make sure that using only clear_bits[0] will work.
            UVM_ASSERT(tree->hal->entry_size(dir->depth) == sizeof(clear_bits[0]) || clear_bits[0] == clear_bits[1]);
        }
        else {
            *clear_bits = 0;
        }

        // Initialize the memory to a reasonable value.
        if (push) {
            tree->gpu->parent->ce_hal->memset_8(push,
                                                uvm_mmu_gpu_address(tree->gpu, dir->phys_alloc.addr),
                                                *clear_bits,
                                                dir->phys_alloc.size);
        }
        else {
            uvm_mmu_page_table_cpu_memset_8(tree->gpu,
                                            &dir->phys_alloc,
                                            0,
                                            *clear_bits,
                                            dir->phys_alloc.size / sizeof(*clear_bits));
        }
    }
    else {
        pde_fill(tree, dir, 0, entries_count, phys_allocs, push);
    }

}

static uvm_page_directory_t *allocate_directory(uvm_page_tree_t *tree,
                                                NvU64 page_size,
                                                NvU32 depth,
                                                uvm_pmm_alloc_flags_t pmm_flags)
{
    NV_STATUS status;
    uvm_mmu_mode_hal_t *hal = tree->hal;
    NvU32 entry_count;
    NvLength phys_alloc_size = hal->allocation_size(depth, page_size);
    uvm_page_directory_t *dir;

    // The page tree doesn't cache PTEs so space is not allocated for entries
    // that are always PTEs.
    // 2M PTEs may later become PDEs so pass UVM_PAGE_SIZE_AGNOSTIC, not
    // page_size.
    if (depth == hal->page_table_depth(UVM_PAGE_SIZE_AGNOSTIC))
        entry_count = 0;
    else
        entry_count = hal->entries_per_index(depth) << hal->index_bits(depth, page_size);

    dir = uvm_kvmalloc_zero(sizeof(uvm_page_directory_t) + sizeof(dir->entries[0]) * entry_count);
    if (dir == NULL)
        return NULL;

    status = phys_mem_allocate(tree, phys_alloc_size, tree->location, pmm_flags, &dir->phys_alloc);

    // Fall back to sysmem if allocating page tables in vidmem with eviction
    // fails, and the fallback is allowed.
    if ((status == NV_ERR_NO_MEMORY) &&
        (tree->location == UVM_APERTURE_VID) &&
        (tree->location_sys_fallback) &&
        ((pmm_flags & UVM_PMM_ALLOC_FLAGS_EVICT) != 0)) {
        status = phys_mem_allocate(tree, phys_alloc_size, UVM_APERTURE_SYS, pmm_flags, &dir->phys_alloc);
    }

    if (status != NV_OK) {
        uvm_kvfree(dir);
        return NULL;
    }
    dir->depth = depth;

    return dir;
}

static inline NvU32 entry_index_from_vaddr(NvU64 vaddr, NvU32 addr_bit_shift, NvU32 bits)
{
    NvU64 mask = ((NvU64)1 << bits) - 1;
    return (NvU32)((vaddr >> addr_bit_shift) & mask);
}

static inline NvU32 index_to_entry(uvm_mmu_mode_hal_t *hal, NvU32 entry_index, NvU32 depth, NvU64 page_size)
{
    return hal->entries_per_index(depth) * entry_index + hal->entry_offset(depth, page_size);
}

static uvm_page_directory_t *host_pde_write(uvm_page_directory_t *dir,
                                            uvm_page_directory_t *parent,
                                            NvU32 index_in_parent)
{
    dir->host_parent = parent;
    dir->index_in_parent = index_in_parent;
    parent->ref_count++;
    return dir;
}

static void pde_write(uvm_page_tree_t *tree,
                      uvm_page_directory_t *dir,
                      NvU32 entry_index,
                      bool force_clear,
                      uvm_push_t *push)
{
    NvU32 i;
    uvm_mmu_page_table_alloc_t *phys_allocs[2];
    NvU32 entries_per_index = tree->hal->entries_per_index(dir->depth);

    // extract physical allocs from non-null entries.
    for (i = 0; i < entries_per_index; i++) {
        uvm_page_directory_t *entry = dir->entries[entries_per_index * entry_index + i];
        if (entry == NULL || force_clear)
            phys_allocs[i] = NULL;
        else
            phys_allocs[i] = &entry->phys_alloc;
    }

    pde_fill(tree, dir, entry_index, 1, phys_allocs, push);
}

static void host_pde_clear(uvm_page_tree_t *tree, uvm_page_directory_t *dir, NvU32 entry_index, NvU64 page_size)
{
    UVM_ASSERT(dir->ref_count > 0);

    dir->entries[index_to_entry(tree->hal, entry_index, dir->depth, page_size)] = NULL;
    dir->ref_count--;
}

static void pde_clear(uvm_page_tree_t *tree,
                      uvm_page_directory_t *dir,
                      NvU32 entry_index,
                      NvU64 page_size,
                      uvm_push_t *push)
{
    host_pde_clear(tree, dir, entry_index, page_size);
    pde_write(tree, dir, entry_index, false, push);
}

static uvm_chunk_sizes_mask_t allocation_sizes_for_big_page_size(uvm_parent_gpu_t *parent_gpu, NvU64 big_page_size)
{
    uvm_mmu_mode_hal_t *hal = parent_gpu->arch_hal->mmu_mode_hal(big_page_size);
    unsigned long page_sizes, page_size_log2;
    uvm_chunk_sizes_mask_t alloc_sizes;

    if (hal == NULL)
        return 0;

    page_sizes = hal->page_sizes();
    alloc_sizes = 0;

    BUILD_BUG_ON(sizeof(hal->page_sizes()) > sizeof(page_sizes));

    for_each_set_bit(page_size_log2, &page_sizes, BITS_PER_LONG) {
        NvU32 i;
        NvU64 page_size = 1ULL << page_size_log2;
        for (i = 0; i <= hal->page_table_depth(page_size); i++)
            alloc_sizes |= hal->allocation_size(i, page_size);
    }

    return alloc_sizes;
}

static NvU64 page_sizes_for_big_page_size(uvm_parent_gpu_t *parent_gpu, NvU64 big_page_size)
{
    uvm_mmu_mode_hal_t *hal = parent_gpu->arch_hal->mmu_mode_hal(big_page_size);

    if (hal != NULL)
        return hal->page_sizes();

    return 0;
}

static void page_tree_end(uvm_page_tree_t *tree, uvm_push_t *push)
{
    if (tree->gpu->channel_manager != NULL)
        uvm_push_end(push);
    else
        uvm_push_end_fake(push);
}

static void page_tree_tracker_overwrite_with_push(uvm_page_tree_t *tree, uvm_push_t *push)
{
    uvm_assert_mutex_locked(&tree->lock);

    // No GPU work to track for fake GPU testing
    if (tree->gpu->channel_manager == NULL)
        return;

    uvm_tracker_overwrite_with_push(&tree->tracker, push);
}

static NV_STATUS page_tree_end_and_wait(uvm_page_tree_t *tree, uvm_push_t *push)
{
    if (tree->gpu->channel_manager != NULL)
        return uvm_push_end_and_wait(push);
    else
        uvm_push_end_fake(push);

    return NV_OK;
}

static NV_STATUS write_gpu_state_cpu(uvm_page_tree_t *tree,
                                     NvU64 page_size,
                                     NvS32 invalidate_depth,
                                     NvU32 used_count,
                                     uvm_page_directory_t **dirs_used)
{
    NvS32 i;
    uvm_push_t push;
    NV_STATUS status;

    uvm_assert_mutex_locked(&tree->lock);
    UVM_ASSERT(uvm_mmu_use_cpu(tree));

    if (used_count == 0)
        return NV_OK;

    status = uvm_tracker_wait(&tree->tracker);
    if (status != NV_OK)
        return status;

    for (i = 0; i < used_count; i++)
        phys_mem_init(tree, page_size, dirs_used[i], NULL);

    // Only a single membar is needed between the memsets of the page tables
    // and the writes of the PDEs pointing to those page tables.
    mb();

    // write entries bottom up, so that they are valid once they're inserted
    // into the tree
    for (i = used_count - 1; i >= 0; i--)
        pde_write(tree, dirs_used[i]->host_parent, dirs_used[i]->index_in_parent, false, NULL);

    // A CPU membar is needed between the PDE writes and the subsequent TLB
    // invalidate. Work submission guarantees such a membar.
    status = page_tree_begin_acquire(tree, &tree->tracker, &push, "%u dirs", used_count);
    if (status != NV_OK)
        return status;

    UVM_ASSERT(invalidate_depth >= 0);

    // See the comments in write_gpu_state_gpu()
    tree->gpu->parent->host_hal->tlb_invalidate_all(&push,
                                                    uvm_page_tree_pdb(tree)->addr,
                                                    invalidate_depth,
                                                    UVM_MEMBAR_NONE);
    page_tree_end(tree, &push);
    page_tree_tracker_overwrite_with_push(tree, &push);

    return NV_OK;
}

static NV_STATUS write_gpu_state_gpu(uvm_page_tree_t *tree,
                                     NvU64 page_size,
                                     NvS32 invalidate_depth,
                                     NvU32 used_count,
                                     uvm_page_directory_t **dirs_used)
{
    NvS32 i;
    uvm_push_t push;
    NV_STATUS status;

    // The logic of what membar is needed when is pretty subtle, please refer to
    // the UVM Functional Spec (section 5.1) for all the details.
    uvm_membar_t membar_after_writes = UVM_MEMBAR_GPU;

    uvm_assert_mutex_locked(&tree->lock);
    UVM_ASSERT(!uvm_mmu_use_cpu(tree));

    if (used_count == 0)
        return NV_OK;

    status = page_tree_begin_acquire(tree, &tree->tracker, &push, "%u dirs", used_count);
    if (status != NV_OK)
        return status;

    // only do GPU work once all the allocations have succeeded
    // first, zero-out the new allocations
    for (i = 0; i < used_count; i++) {
        // Appropriate membar will be done after all the writes. Pipelining can
        // be enabled as they are all initializing newly allocated memory that
        // cannot have any writes pending.
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

        phys_mem_init(tree, page_size, dirs_used[i], &push);

        if (dirs_used[i]->phys_alloc.addr.aperture == UVM_APERTURE_SYS)
            membar_after_writes = UVM_MEMBAR_SYS;
    }

    // Only a single membar is needed between the memsets of the page tables
    // and the writes of the PDEs pointing to those page tables.
    // The membar can be local if all of the page tables and PDEs are in GPU
    // memory, but must be a sysmembar if any of them are in sysmem.
    uvm_hal_wfi_membar(&push, membar_after_writes);

    // Reset back to a local membar by default
    membar_after_writes = UVM_MEMBAR_GPU;

    // write entries bottom up, so that they are valid once they're inserted
    // into the tree
    for (i = used_count - 1; i >= 0; i--) {
        uvm_page_directory_t *dir = dirs_used[i];

        // Appropriate membar will be done after all the writes. Pipelining can
        // be enabled as they are all independent and we just did a WFI above.
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        pde_write(tree, dir->host_parent, dir->index_in_parent, false, &push);

        // If any of the written PDEs is in sysmem, a sysmembar is needed before
        // the TLB invalidate.
        // Notably sysmembar is needed even though the writer (CE) and reader
        // (MMU) are on the same GPU, because CE physical writes take the L2
        // bypass path.
        if (dir->host_parent->phys_alloc.addr.aperture == UVM_APERTURE_SYS)
            membar_after_writes = UVM_MEMBAR_SYS;
    }

    uvm_hal_wfi_membar(&push, membar_after_writes);

    UVM_ASSERT(invalidate_depth >= 0);

    // Upgrades don't have to flush out accesses, so no membar is needed on the
    // TLB invalidate.
    tree->gpu->parent->host_hal->tlb_invalidate_all(&push,
                                                    uvm_page_tree_pdb(tree)->addr,
                                                    invalidate_depth,
                                                    UVM_MEMBAR_NONE);

    // We just did the appropriate membar after the WFI, so no need for another
    // one in push_end().
    // At least currently as if the L2 bypass path changes to only require a GPU
    // membar between PDE write and TLB invalidate, we'll need to push a
    // sysmembar so the end-of-push semaphore is ordered behind the PDE writes.
    uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
    page_tree_end(tree, &push);
    page_tree_tracker_overwrite_with_push(tree, &push);

    return NV_OK;
}

// initialize new page tables and insert them into the tree
static NV_STATUS write_gpu_state(uvm_page_tree_t *tree,
                                 NvU64 page_size,
                                 NvS32 invalidate_depth,
                                 NvU32 used_count,
                                 uvm_page_directory_t **dirs_used)
{
    if (uvm_mmu_use_cpu(tree))
        return write_gpu_state_cpu(tree, page_size, invalidate_depth, used_count, dirs_used);
    else
        return write_gpu_state_gpu(tree, page_size, invalidate_depth, used_count, dirs_used);
}

static void free_unused_directories(uvm_page_tree_t *tree,
                                    NvU32 used_count,
                                    uvm_page_directory_t **dirs_used,
                                    uvm_page_directory_t **dir_cache)
{
    NvU32 i;

    // free unused entries
    for (i = 0; i < MAX_OPERATION_DEPTH; i++) {
        uvm_page_directory_t *dir = dir_cache[i];
        if (dir != NULL) {
            NvU32 j;

            for (j = 0; j < used_count; j++) {
                if (dir == dirs_used[j])
                    break;
            }

            if (j == used_count) {
                phys_mem_deallocate(tree, &dir->phys_alloc);
                uvm_kvfree(dir);
            }
        }
    }
}

static NV_STATUS allocate_page_table(uvm_page_tree_t *tree, NvU64 page_size, uvm_mmu_page_table_alloc_t *out)
{
    NvU32 depth = tree->hal->page_table_depth(page_size);
    NvLength alloc_size = tree->hal->allocation_size(depth, page_size);

    return phys_mem_allocate(tree, alloc_size, tree->location, UVM_PMM_ALLOC_FLAGS_EVICT, out);
}

static bool page_tree_ats_init_required(uvm_page_tree_t *tree)
{
    // We have full control of the kernel page tables mappings, no ATS address
    // aliases is expected.
    if (tree->type == UVM_PAGE_TREE_TYPE_KERNEL)
        return false;

    // Enable uvm_page_tree_init() from the page_tree test.
    if (uvm_enable_builtin_tests && tree->gpu_va_space == NULL)
        return false;

    if (!tree->gpu_va_space->ats.enabled)
        return false;

    return tree->gpu->parent->no_ats_range_required;
}

static NV_STATUS page_tree_ats_init(uvm_page_tree_t *tree)
{
    NV_STATUS status;
    NvU64 min_va_upper, max_va_lower;
    NvU64 page_size;

    if (!page_tree_ats_init_required(tree))
        return NV_OK;

    page_size = mmu_biggest_page_size(tree, UVM_APERTURE_VID);

    uvm_cpu_get_unaddressable_range(&max_va_lower, &min_va_upper);

    // Potential violation of the UVM internal get/put_ptes contract. get_ptes()
    // creates and initializes enough PTEs to populate all PDEs covering the
    // no_ats_ranges. We store the no_ats_ranges in the tree, so they can be
    // put_ptes()'ed on deinit(). It doesn't preclude the range to be used by a
    // future get_ptes(), since we don't write to the PTEs (range->table) from
    // the tree->no_ats_ranges.
    //
    // Lower half
    status = uvm_page_tree_get_ptes(tree,
                                    page_size,
                                    max_va_lower,
                                    page_size,
                                    UVM_PMM_ALLOC_FLAGS_EVICT,
                                    &tree->no_ats_ranges[0]);
    if (status != NV_OK)
        return status;

    UVM_ASSERT(tree->no_ats_ranges[0].entry_count == 1);

    if (uvm_platform_uses_canonical_form_address()) {
        // Upper half
        status = uvm_page_tree_get_ptes(tree,
                                        page_size,
                                        min_va_upper - page_size,
                                        page_size,
                                        UVM_PMM_ALLOC_FLAGS_EVICT,
                                        &tree->no_ats_ranges[1]);
        if (status != NV_OK)
            return status;

        UVM_ASSERT(tree->no_ats_ranges[1].entry_count == 1);
    }

    return NV_OK;
}

static void page_tree_ats_deinit(uvm_page_tree_t *tree)
{
    size_t i;

    if (page_tree_ats_init_required(tree)) {
        for (i = 0; i < ARRAY_SIZE(tree->no_ats_ranges); i++) {
            if (tree->no_ats_ranges[i].entry_count)
                uvm_page_tree_put_ptes(tree, &tree->no_ats_ranges[i]);
        }

        memset(tree->no_ats_ranges, 0, sizeof(tree->no_ats_ranges));
    }
}

static void map_remap_deinit(uvm_page_tree_t *tree)
{
    if (tree->map_remap.pde0) {
        phys_mem_deallocate(tree, &tree->map_remap.pde0->phys_alloc);
        uvm_kvfree(tree->map_remap.pde0);
        tree->map_remap.pde0 = NULL;
    }

    if (tree->map_remap.ptes_invalid_4k.size)
        phys_mem_deallocate(tree, &tree->map_remap.ptes_invalid_4k);
}

static NV_STATUS map_remap_init(uvm_page_tree_t *tree)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_pte_batch_t batch;
    NvU32 entry_size;

    UVM_ASSERT(!uvm_mmu_use_cpu(tree));

    // Allocate the ptes_invalid_4k.
    status = allocate_page_table(tree, UVM_PAGE_SIZE_4K, &tree->map_remap.ptes_invalid_4k);
    if (status != NV_OK)
        goto error;

    // Allocate the pde0 struct (pde0 depth = depth(UVM_PAGE_SIZE_2M))
    // This is only needed for the 512M page size, since it requires a
    // PDE1-depth(512M) PTE. We first map it to the pde0 directory, then we
    // return the PTE for the get_ptes()'s caller.
    if (tree->hal->page_sizes() & UVM_PAGE_SIZE_512M) {
        tree->map_remap.pde0 = allocate_directory(tree,
                                                  UVM_PAGE_SIZE_2M,
                                                  tree->hal->page_table_depth(UVM_PAGE_SIZE_2M),
                                                  UVM_PMM_ALLOC_FLAGS_EVICT);
        if (tree->map_remap.pde0 == NULL) {
            status = NV_ERR_NO_MEMORY;
            goto error;
        }
    }

    status = page_tree_begin_acquire(tree, &tree->tracker, &push, "map remap init");
    if (status != NV_OK)
        goto error;

    uvm_pte_batch_begin(&push, &batch);
    entry_size = uvm_mmu_pte_size(tree, UVM_PAGE_SIZE_4K);

    // Invalidate all entries in the ptes_invalid_4k page table.
    uvm_pte_batch_clear_ptes(&batch,
                             tree->map_remap.ptes_invalid_4k.addr,
                             0,
                             entry_size,
                             tree->map_remap.ptes_invalid_4k.size / entry_size);

    uvm_pte_batch_end(&batch);

    // Set pde0 entries to ptes_invalid_4k.
    if (tree->hal->page_sizes() & UVM_PAGE_SIZE_512M) {
        uvm_mmu_page_table_alloc_t *phys_allocs[2] = {NULL, NULL};
        NvU32 depth = tree->hal->page_table_depth(UVM_PAGE_SIZE_4K) - 1;
        size_t index_4k = tree->hal->entry_offset(depth, UVM_PAGE_SIZE_4K);
        NvU32 pde0_entries = tree->map_remap.pde0->phys_alloc.size / tree->hal->entry_size(tree->map_remap.pde0->depth);

        // The big-page entry is NULL which makes it an invalid entry.
        phys_allocs[index_4k] = &tree->map_remap.ptes_invalid_4k;

        // By default CE operations include a MEMBAR_SYS. MEMBAR_GPU is
        // sufficient when pde0 is allocated in VIDMEM.
        if (tree->map_remap.pde0->phys_alloc.addr.aperture == UVM_APERTURE_VID)
            uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);

        // This is an orphan directory, make_pde() requires a directory to
        // compute the VA. The UVM depth map_remap() operates on is not in the
        // range make_pde() must operate. We only need to supply the fields used
        // by make_pde() to not access invalid memory addresses.

        pde_fill(tree,
                 tree->map_remap.pde0,
                 0,
                 pde0_entries,
                 (uvm_mmu_page_table_alloc_t **)&phys_allocs,
                 &push);
    }

    return page_tree_end_and_wait(tree, &push);
error:
    map_remap_deinit(tree);

    return status;
}

// The location of the page tree backing storage depends on the aperture passed
// to the page tree initialization ("init location" in the table below), and the
// uvm_page_table_location module parameter. Only when the initialization
// aperture is UVM_APERTURE_DEFAULT, and the module parameter is not set,
// the pages can be in more than one location: vidmem is given priority, but if
// the allocation fails it will fallback to sysmem.
//
// Behavior outside of SR-IOV heavy (bare metal, SR-IOV standard, etc):
//             Inputs                                     Outputs
// init location | uvm_page_table_location || tree->location | tree->location_sys_fallback
// --------------|-------------------------||----------------|----------------
//    vidmem     |           -             ||    vidmem      |      false
//    sysmem     |           -             ||    sysmem      |      false
//    default    |        <not set>        ||    vidmem      |      true
//    default    |         vidmem          ||    vidmem      |      false
//    default    |         sysmem          ||    sysmem      |      false
//
//
// In SR-IOV heavy the the page tree must be in vidmem, to prevent guest drivers
// from updating GPU page tables without hypervisor knowledge.
// When the Confidential Computing feature is enabled, all kernel
// allocations must be made in the CPR of vidmem. This is a hardware security
// constraint.
//             Inputs                                     Outputs
// init location | uvm_page_table_location || tree->location | tree->location_sys_fallback
//  -------------|-------------------------||----------------|----------------
//    vidmem     |            -            ||     vidmem     |      false
//    sysmem     |            -            ||           <disallowed>
//    default    |            -            ||           <disallowed>
//
static void page_tree_set_location(uvm_page_tree_t *tree, uvm_aperture_t location)
{
    UVM_ASSERT(tree->gpu != NULL);
    UVM_ASSERT_MSG((location == UVM_APERTURE_VID) ||
                   (location == UVM_APERTURE_SYS) ||
                   (location == UVM_APERTURE_DEFAULT),
                   "Invalid location %s (%d)\n", uvm_aperture_string(location), (int)location);

    // The page tree of a "fake" GPU used during page tree testing can be in
    // sysmem in scenarios where a "real" GPU must be in vidmem. Fake GPUs can
    // be identified by having no channel manager.
    if (tree->gpu->channel_manager != NULL) {

        if (uvm_parent_gpu_is_virt_mode_sriov_heavy(tree->gpu->parent))
            UVM_ASSERT(location == UVM_APERTURE_VID);
        else if (g_uvm_global.conf_computing_enabled)
            UVM_ASSERT(location == UVM_APERTURE_VID);
    }

    if (location == UVM_APERTURE_DEFAULT) {
        if (page_table_aperture == UVM_APERTURE_DEFAULT) {
            tree->location = UVM_APERTURE_VID;
            tree->location_sys_fallback = true;
        }
        else {
            tree->location = page_table_aperture;
            tree->location_sys_fallback = false;
        }
    }
    else {
        tree->location = location;
        tree->location_sys_fallback = false;
    }
}

NV_STATUS uvm_page_tree_init(uvm_gpu_t *gpu,
                             uvm_gpu_va_space_t *gpu_va_space,
                             uvm_page_tree_type_t type,
                             NvU64 big_page_size,
                             uvm_aperture_t location,
                             uvm_page_tree_t *tree)
{
    uvm_push_t push;
    NV_STATUS status;
    BUILD_BUG_ON(sizeof(uvm_page_directory_t) != offsetof(uvm_page_directory_t, entries));

    UVM_ASSERT(type < UVM_PAGE_TREE_TYPE_COUNT);

    memset(tree, 0, sizeof(*tree));
    uvm_mutex_init(&tree->lock, UVM_LOCK_ORDER_PAGE_TREE);
    tree->hal = gpu->parent->arch_hal->mmu_mode_hal(big_page_size);
    UVM_ASSERT(tree->hal != NULL);
    UVM_ASSERT(MAX_OPERATION_DEPTH >= tree->hal->page_table_depth(UVM_PAGE_SIZE_AGNOSTIC));
    tree->gpu = gpu;
    tree->type = type;
    tree->gpu_va_space = gpu_va_space;
    tree->big_page_size = big_page_size;

    UVM_ASSERT(uvm_mmu_page_size_supported(tree, big_page_size));

    page_tree_set_location(tree, location);

    uvm_tracker_init(&tree->tracker);

    tree->root = allocate_directory(tree, UVM_PAGE_SIZE_AGNOSTIC, 0, UVM_PMM_ALLOC_FLAGS_EVICT);

    if (tree->root == NULL)
        return NV_ERR_NO_MEMORY;

    if (gpu->parent->map_remap_larger_page_promotion) {
        status = map_remap_init(tree);
        if (status != NV_OK)
            return status;
    }

    if (uvm_mmu_use_cpu(tree)) {
        phys_mem_init(tree, UVM_PAGE_SIZE_AGNOSTIC, tree->root, NULL);
        return NV_OK;
    }

    status = page_tree_begin_acquire(tree, &tree->tracker, &push, "init page tree");
    if (status != NV_OK)
        return status;

    phys_mem_init(tree, UVM_PAGE_SIZE_AGNOSTIC, tree->root, &push);

    status = page_tree_end_and_wait(tree, &push);
    if (status != NV_OK)
        return status;

    status = page_tree_ats_init(tree);
    if (status != NV_OK)
        return status;

    return NV_OK;
}

void uvm_page_tree_deinit(uvm_page_tree_t *tree)
{
    page_tree_ats_deinit(tree);

    UVM_ASSERT(tree->root->ref_count == 0);

    // Take the tree lock only to avoid assertions. It is not required for
    // thread safety during deinit.
    uvm_mutex_lock(&tree->lock);

    // Invalidate the entire PDB before destroying it. This is only required for
    // ATS-enabled PDBs, because we have already invalidated all GMMU entries
    // under this PDB. Stale ATS entries however might be present, and we need
    // to invalidate them to handle the unlikely event that this PASID gets
    // reused before those entries are evicted from the TLBs.
    //
    // While this is only required for ATS, we don't know at this point whether
    // the PDB had ATS enabled, so just do it for all user PDBs.
    if (g_uvm_global.ats.enabled && tree->type == UVM_PAGE_TREE_TYPE_USER) {
        uvm_push_t push;
        NV_STATUS status = page_tree_begin_acquire(tree, &tree->tracker, &push, "deinit inval all");

        // Failure to get a push can only happen if we've hit a fatal UVM channel
        // error. We can't perform the unmap, so just leave things in place for
        // debug.
        if (status == NV_OK) {
            tree->gpu->parent->host_hal->tlb_invalidate_all(&push, uvm_page_tree_pdb(tree)->addr, 0, UVM_MEMBAR_NONE);
            page_tree_end(tree, &push);
            page_tree_tracker_overwrite_with_push(tree, &push);
        }
        else {
            UVM_ASSERT(status == uvm_global_get_status());
        }
    }

    (void)uvm_tracker_wait(&tree->tracker);
    phys_mem_deallocate(tree, &tree->root->phys_alloc);

    if (tree->gpu->parent->map_remap_larger_page_promotion)
        map_remap_deinit(tree);

    uvm_mutex_unlock(&tree->lock);

    uvm_tracker_deinit(&tree->tracker);
    uvm_kvfree(tree->root);
}

void uvm_page_tree_put_ptes_async(uvm_page_tree_t *tree, uvm_page_table_range_t *range)
{
    NvU32 free_count = 0;
    NvU32 i;
    uvm_page_directory_t *free_queue[MAX_OPERATION_DEPTH];
    uvm_page_directory_t *dir = range->table;
    uvm_push_t push;
    NV_STATUS status = NV_OK;
    NvU32 invalidate_depth = 0;

    // The logic of what membar is needed when is pretty subtle, please refer to
    // the UVM Functional Spec (section 5.1) for all the details.
    uvm_membar_t membar_after_pde_clears = UVM_MEMBAR_GPU;
    uvm_membar_t membar_after_invalidate = UVM_MEMBAR_GPU;

    UVM_ASSERT(tree->hal->page_table_depth(range->page_size) <= MAX_OPERATION_DEPTH);

    uvm_mutex_lock(&tree->lock);

    // release the range
    UVM_ASSERT(dir->ref_count >= range->entry_count);
    dir->ref_count -= range->entry_count;

    // traverse until we hit an in-use page, or the root
    while (dir->host_parent != NULL && dir->ref_count == 0) {
        uvm_page_directory_t *parent = dir->host_parent;
        uvm_membar_t this_membar;

        if (free_count == 0) {
            if (uvm_mmu_use_cpu(tree))
                status = uvm_tracker_wait(&tree->tracker);

            if (status == NV_OK) {
                // Begin a push which will be submitted before the memory gets
                // freed.
                //
                // When writing with the CPU we don't strictly need to begin
                // this push until after the writes are done, but doing it here
                // doesn't hurt and makes the function's logic simpler.
                status = page_tree_begin_acquire(tree,
                                                 &tree->tracker,
                                                 &push,
                                                 "put ptes: start: %u, count: %u",
                                                 range->start_index,
                                                 range->entry_count);
            }

            // Failure to wait for a tracker or get a push can only happen if
            // we've hit a fatal UVM channel error. We can't perform the unmap,
            // so just leave things in place for debug.
            if (status != NV_OK) {
                UVM_ASSERT(status == uvm_global_get_status());
                dir->ref_count += range->entry_count;
                goto done;
            }
        }

        if (uvm_mmu_use_cpu(tree)) {
            pde_clear(tree, dir->host_parent, dir->index_in_parent, range->page_size, NULL);
        }
        else {
            // All writes can be pipelined as put_ptes() cannot be called with
            // any operations pending on the affected PTEs and PDEs.
            uvm_push_set_flag(&push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);

            // Don't issue any membars as part of the clear, a single membar
            // will be done below before the invalidate.
            uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
            pde_clear(tree, dir->host_parent, dir->index_in_parent, range->page_size, &push);
        }

        invalidate_depth = dir->host_parent->depth;

        // If we're using the CPU to do the write a SYS membar is required.
        // Otherwise, take the membar with the widest scope of any of the
        // pointed-to PDEs.
        if (uvm_mmu_use_cpu(tree))
            this_membar = UVM_MEMBAR_SYS;
        else
            this_membar = uvm_hal_downgrade_membar_type(tree->gpu, dir->phys_alloc.addr.aperture == UVM_APERTURE_VID);

        membar_after_invalidate = max(membar_after_invalidate, this_membar);

        // If any of the cleared PDEs were in sysmem then a SYS membar is
        // required after the clears and before the TLB invalidate.
        if (dir->host_parent->phys_alloc.addr.aperture == UVM_APERTURE_SYS)
            membar_after_pde_clears = UVM_MEMBAR_SYS;

        // Add this dir to the queue of directories that should be freed once
        // the tracker value of the associated PDE writes is known.
        UVM_ASSERT(free_count < tree->hal->page_table_depth(range->page_size));
        free_queue[free_count++] = dir;

        dir = parent;
    }

    if (free_count == 0)
        goto done;

    if (uvm_mmu_use_cpu(tree))
        mb();
    else
        uvm_hal_wfi_membar(&push, membar_after_pde_clears);

    tree->gpu->parent->host_hal->tlb_invalidate_all(&push,
                                                    uvm_page_tree_pdb(tree)->addr,
                                                    invalidate_depth,
                                                    membar_after_invalidate);

    if (!uvm_mmu_use_cpu(tree)) {
        // We just did the appropriate membar above, no need for another one in
        // push_end(). If the L2 bypass path changes to only require a GPU
        // membar between PDE write and TLB invalidate, we'll need to push a
        // sysmembar so the end-of-push semaphore is ordered behind the PDE
        // writes.
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
    }

    page_tree_end(tree, &push);
    page_tree_tracker_overwrite_with_push(tree, &push);

    // now that we've traversed all the way up the tree, free everything
    for (i = 0; i < free_count; i++) {
        phys_mem_deallocate(tree, &free_queue[i]->phys_alloc);
        uvm_kvfree(free_queue[i]);
    }

done:
    uvm_mutex_unlock(&tree->lock);
}

void uvm_page_tree_put_ptes(uvm_page_tree_t *tree, uvm_page_table_range_t *range)
{
    uvm_page_tree_put_ptes_async(tree, range);
    (void)uvm_page_tree_wait(tree);
}

NV_STATUS uvm_page_tree_wait(uvm_page_tree_t *tree)
{
    NV_STATUS status;

    uvm_mutex_lock(&tree->lock);

    status = uvm_tracker_wait(&tree->tracker);

    uvm_mutex_unlock(&tree->lock);

    return status;
}

static NV_STATUS try_get_ptes(uvm_page_tree_t *tree,
                              NvU64 page_size,
                              NvU64 start,
                              NvU64 size,
                              uvm_page_table_range_t *range,
                              NvU32 *cur_depth,
                              uvm_page_directory_t **dir_cache)
{
    uvm_mmu_mode_hal_t *hal = tree->hal;

    // bit index just beyond the most significant bit used to index the current
    // entry
    NvU32 addr_bit_shift = hal->num_va_bits();

    // track depth upon which the invalidate occured
    NvS32 invalidate_depth = -1;
    uvm_page_directory_t *dir = tree->root;

    // directories used in attempt
    NvU32 used_count = 0;
    NvU32 i;
    uvm_page_directory_t *dirs_used[MAX_OPERATION_DEPTH];

    uvm_assert_mutex_locked(&tree->lock);

    UVM_ASSERT(is_power_of_2(page_size));

    // ensure that the caller has specified a valid page size
    UVM_ASSERT((page_size & hal->page_sizes()) != 0);

    // This algorithm will work with unaligned ranges, but the caller's intent
    // is unclear
    UVM_ASSERT_MSG(start % page_size == 0 && size % page_size == 0,
                   "start 0x%llx size 0x%llx page_size 0x%llx\n",
                   start,
                   size,
                   page_size);

    // The GPU should be capable of addressing the passed range
    if (tree->type == UVM_PAGE_TREE_TYPE_USER)
        UVM_ASSERT(uvm_gpu_can_address(tree->gpu, start, size));
    else
        UVM_ASSERT(uvm_gpu_can_address_kernel(tree->gpu, start, size));

    while (true) {
        // index of the entry, for the first byte of the range, within its
        // containing directory
        NvU32 start_index;

        // index of the entry, for the last byte of the range, within its
        // containing directory
        NvU32 end_index;

        // pointer to PDE/PTE
        uvm_page_directory_t **entry;
        NvU32 index_bits = hal->index_bits(dir->depth, page_size);

        addr_bit_shift -= index_bits;
        start_index = entry_index_from_vaddr(start, addr_bit_shift, index_bits);
        end_index = entry_index_from_vaddr(start + size - 1, addr_bit_shift, index_bits);

        UVM_ASSERT(start_index <= end_index && end_index < (1 << index_bits));

        entry = dir->entries + index_to_entry(hal, start_index, dir->depth, page_size);

        if (dir->depth == hal->page_table_depth(page_size)) {
            page_table_range_init(range, page_size, dir, start_index, end_index);
            break;
        }
        else {
            UVM_ASSERT(start_index == end_index);

            if (*entry == NULL) {
                if (dir_cache[dir->depth] == NULL) {
                    *cur_depth = dir->depth;

                    // Undo the changes to the tree so that the dir cache
                    // remains private to the thread.
                    for (i = 0; i < used_count; i++)
                        host_pde_clear(tree, dirs_used[i]->host_parent, dirs_used[i]->index_in_parent, page_size);

                    return NV_ERR_MORE_PROCESSING_REQUIRED;
                }

                *entry = host_pde_write(dir_cache[dir->depth], dir, start_index);
                dirs_used[used_count++] = *entry;

                if (invalidate_depth == -1)
                    invalidate_depth = dir->depth;
            }
        }
        dir = *entry;
    }

    free_unused_directories(tree, used_count, dirs_used, dir_cache);
    return write_gpu_state(tree, page_size, invalidate_depth, used_count, dirs_used);
}

static NV_STATUS map_remap(uvm_page_tree_t *tree, NvU64 start, NvU64 size, uvm_page_table_range_t *range)
{
    NV_STATUS status;
    uvm_push_t push;
    NvU64 page_sizes;
    uvm_mmu_page_table_alloc_t *phys_alloc[1];

    // TODO: Bug 2734399
    if (range->page_size != UVM_PAGE_SIZE_512M)
        return NV_OK;

    UVM_ASSERT(tree->hal->entries_per_index(range->table->depth) == 1);

    status = page_tree_begin_acquire(tree,
                                     &tree->tracker,
                                     &push,
                                     "map remap: [0x%llx, 0x%llx), page_size: %lld",
                                     start,
                                     start + size,
                                     range->page_size);
    if (status != NV_OK)
        return status;

    // By default CE operations include a MEMBAR_SYS. MEMBAR_GPU is
    // sufficient when the range is allocated in VIDMEM. We must enforce the
    // following ordering between operations:
    // PDE write -> TLB invalidate -> MMU fills.
    if (uvm_page_table_range_aperture(range) == UVM_APERTURE_VID)
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);

    phys_alloc[0] = &tree->map_remap.pde0->phys_alloc;
    pde_fill(tree,
             range->table,
             range->start_index,
             range->entry_count,
             (uvm_mmu_page_table_alloc_t **)&phys_alloc,
             &push);

    tree->gpu->parent->host_hal->wait_for_idle(&push);

    // Invalidate all the supported page sizes smaller than or equal to
    // range->page_size, because the GPU TLBs may cache invalid entries using
    // any page size they decide, including the smallest one.
    page_sizes = (range->page_size | (range->page_size - 1)) & tree->hal->page_sizes();

    // No membar is needed, any in-flight access to this range may fault and a
    // lazy or delayed invalidate will evict the potential stale/invalid TLB
    // entry.
    uvm_tlb_batch_single_invalidate(tree, &push, start, size, page_sizes, UVM_MEMBAR_NONE);

    page_tree_end(tree, &push);
    page_tree_tracker_overwrite_with_push(tree, &push);
    return NV_OK;
}

NV_STATUS uvm_page_tree_get_ptes_async(uvm_page_tree_t *tree,
                                       NvU64 page_size,
                                       NvU64 start,
                                       NvU64 size,
                                       uvm_pmm_alloc_flags_t pmm_flags,
                                       uvm_page_table_range_t *range)
{
    NV_STATUS status;
    NvU32 cur_depth = 0;
    uvm_page_directory_t *dir_cache[MAX_OPERATION_DEPTH];
    memset(dir_cache, 0, sizeof(dir_cache));

    uvm_mutex_lock(&tree->lock);
    while ((status = try_get_ptes(tree,
                                  page_size,
                                  start,
                                  size,
                                  range,
                                  &cur_depth,
                                  dir_cache)) == NV_ERR_MORE_PROCESSING_REQUIRED) {
        uvm_mutex_unlock(&tree->lock);

        // try_get_ptes never needs depth 0, so store a directory at its
        // parent's depth.
        // TODO: Bug 1766655: Allocate everything below cur_depth instead of
        //       retrying for every level.
        dir_cache[cur_depth] = allocate_directory(tree, page_size, cur_depth + 1, pmm_flags);
        if (dir_cache[cur_depth] == NULL) {
            uvm_mutex_lock(&tree->lock);
            free_unused_directories(tree, 0, NULL, dir_cache);
            uvm_mutex_unlock(&tree->lock);
            return NV_ERR_NO_MEMORY;
        }

        uvm_mutex_lock(&tree->lock);
    }

    if ((status == NV_OK) && tree->gpu->parent->map_remap_larger_page_promotion)
        status = map_remap(tree, start, size, range);

    uvm_mutex_unlock(&tree->lock);

    return status;
}

NV_STATUS uvm_page_tree_get_ptes(uvm_page_tree_t *tree,
                                 NvU64 page_size,
                                 NvU64 start,
                                 NvU64 size,
                                 uvm_pmm_alloc_flags_t pmm_flags,
                                 uvm_page_table_range_t *range)
{
    NV_STATUS status = uvm_page_tree_get_ptes_async(tree, page_size, start, size, pmm_flags, range);
    if (status != NV_OK)
        return status;

    return uvm_page_tree_wait(tree);
}

void uvm_page_table_range_get_upper(uvm_page_tree_t *tree,
                                    uvm_page_table_range_t *existing,
                                    uvm_page_table_range_t *upper,
                                    NvU32 num_upper_pages)
{
    NvU32 upper_start_index = existing->start_index + (existing->entry_count - num_upper_pages);
    NvU32 upper_end_index = upper_start_index + num_upper_pages - 1;

    UVM_ASSERT(num_upper_pages);
    UVM_ASSERT(num_upper_pages <= existing->entry_count);

    uvm_mutex_lock(&tree->lock);
    page_table_range_init(upper, existing->page_size, existing->table, upper_start_index, upper_end_index);
    uvm_mutex_unlock(&tree->lock);
}

void uvm_page_table_range_shrink(uvm_page_tree_t *tree, uvm_page_table_range_t *range, NvU32 new_page_count)
{
    UVM_ASSERT(range->entry_count >= new_page_count);

    if (new_page_count > 0) {
        // Take a ref count on the smaller portion of the PTEs, then drop the
        // entire old range.
        uvm_mutex_lock(&tree->lock);

        UVM_ASSERT(range->table->ref_count >= range->entry_count);
        range->table->ref_count -= (range->entry_count - new_page_count);

        uvm_mutex_unlock(&tree->lock);

        range->entry_count = new_page_count;
    }
    else {
        uvm_page_tree_put_ptes(tree, range);
    }
}

NV_STATUS uvm_page_tree_get_entry(uvm_page_tree_t *tree,
                                  NvU64 page_size,
                                  NvU64 start,
                                  uvm_pmm_alloc_flags_t pmm_flags,
                                  uvm_page_table_range_t *single)
{
    NV_STATUS status = uvm_page_tree_get_ptes(tree, page_size, start, page_size, pmm_flags, single);
    UVM_ASSERT(single->entry_count == 1);
    return status;
}

void uvm_page_tree_write_pde(uvm_page_tree_t *tree, uvm_page_table_range_t *single, uvm_push_t *push)
{
    UVM_ASSERT(single->entry_count == 1);
    pde_write(tree, single->table, single->start_index, false, push);
}

void uvm_page_tree_clear_pde(uvm_page_tree_t *tree, uvm_page_table_range_t *single, uvm_push_t *push)
{
    UVM_ASSERT(single->entry_count == 1);
    pde_write(tree, single->table, single->start_index, true, push);
}

static NV_STATUS poison_ptes(uvm_page_tree_t *tree,
                             uvm_page_directory_t *pte_dir,
                             uvm_page_directory_t *parent,
                             NvU64 page_size)
{
    NV_STATUS status;
    uvm_push_t push;

    uvm_assert_mutex_locked(&tree->lock);

    UVM_ASSERT(pte_dir->depth == tree->hal->page_table_depth(page_size));

    // The flat mappings should always be set up when executing this path
    UVM_ASSERT(!uvm_mmu_use_cpu(tree));

    status = page_tree_begin_acquire(tree, &tree->tracker, &push, "Poisoning child table of page size %llu", page_size);
    if (status != NV_OK)
        return status;

    tree->gpu->parent->ce_hal->memset_8(&push,
                                        uvm_mmu_gpu_address(tree->gpu, pte_dir->phys_alloc.addr),
                                        tree->hal->poisoned_pte(),
                                        pte_dir->phys_alloc.size);

    // If both the new PTEs and the parent PDE are in vidmem, then a GPU-
    // local membar is enough to keep the memset of the PTEs ordered with
    // any later write of the PDE. Otherwise we need a sysmembar. See the
    // comments in write_gpu_state_gpu.
    if (pte_dir->phys_alloc.addr.aperture == UVM_APERTURE_VID &&
        parent->phys_alloc.addr.aperture == UVM_APERTURE_VID)
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);

    page_tree_end(tree, &push);

    // The push acquired the tracker so it's ok to just overwrite it with
    // the entry tracking the push.
    page_tree_tracker_overwrite_with_push(tree, &push);

    return NV_OK;
}

NV_STATUS uvm_page_tree_alloc_table(uvm_page_tree_t *tree,
                                    NvU64 page_size,
                                    uvm_pmm_alloc_flags_t pmm_flags,
                                    uvm_page_table_range_t *single,
                                    uvm_page_table_range_t *children)
{
    bool should_free = false;
    uvm_page_directory_t **entry;
    uvm_page_directory_t *dir;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(single->entry_count == 1);

    entry = single->table->entries + index_to_entry(tree->hal,
                                                    single->start_index,
                                                    single->table->depth,
                                                    page_size);

    dir = allocate_directory(tree, page_size, single->table->depth + 1, pmm_flags);
    if (dir == NULL)
        return NV_ERR_NO_MEMORY;

    uvm_mutex_lock(&tree->lock);

    // The caller is responsible for initializing this table, so enforce that on
    // debug builds.
    if (UVM_IS_DEBUG()) {
        status = poison_ptes(tree, dir, single->table, page_size);
        if (status != NV_OK)
            goto out;
    }

    status = uvm_tracker_wait(&tree->tracker);
    if (status != NV_OK)
        goto out;

    // the range always refers to the entire page table
    children->start_index = 0;
    children->entry_count = 1 << tree->hal->index_bits(dir->depth, page_size);
    children->page_size = page_size;

    // is this entry currently unassigned?
    if (*entry == NULL) {
        children->table = dir;
        *entry = dir;
        host_pde_write(dir, single->table, single->start_index);
    }
    else {
        should_free = true;
        children->table = *entry;
    }
    children->table->ref_count += children->entry_count;

out:
    if (should_free || status != NV_OK) {
        phys_mem_deallocate(tree, &dir->phys_alloc);
        uvm_kvfree(dir);
    }
    uvm_mutex_unlock(&tree->lock);

    return status;
}

static size_t range_vec_calc_range_count(uvm_page_table_range_vec_t *range_vec)
{
    NvU64 pde_coverage = uvm_mmu_pde_coverage(range_vec->tree, range_vec->page_size);
    NvU64 aligned_start = UVM_ALIGN_DOWN(range_vec->start, pde_coverage);
    NvU64 aligned_end = UVM_ALIGN_UP(range_vec->start + range_vec->size, pde_coverage);
    size_t count = uvm_div_pow2_64(aligned_end - aligned_start, pde_coverage);

    UVM_ASSERT(count != 0);

    return count;
}

static NvU64 range_vec_calc_range_start(uvm_page_table_range_vec_t *range_vec, size_t i)
{
    NvU64 pde_coverage = uvm_mmu_pde_coverage(range_vec->tree, range_vec->page_size);
    NvU64 aligned_start = UVM_ALIGN_DOWN(range_vec->start, pde_coverage);
    NvU64 range_start = aligned_start + i * pde_coverage;
    return max(range_vec->start, range_start);
}

static NvU64 range_vec_calc_range_end(uvm_page_table_range_vec_t *range_vec, size_t i)
{
    NvU64 pde_coverage = uvm_mmu_pde_coverage(range_vec->tree, range_vec->page_size);
    NvU64 range_start = range_vec_calc_range_start(range_vec, i);
    NvU64 max_range_end = UVM_ALIGN_UP(range_start + 1, pde_coverage);
    return min(range_vec->start + range_vec->size, max_range_end);
}

static NvU64 range_vec_calc_range_size(uvm_page_table_range_vec_t *range_vec, size_t i)
{
    return range_vec_calc_range_end(range_vec, i) - range_vec_calc_range_start(range_vec, i);
}

static size_t range_vec_calc_range_index(uvm_page_table_range_vec_t *range_vec, NvU64 addr)
{
    NvU64 pde_coverage = uvm_mmu_pde_coverage(range_vec->tree, range_vec->page_size);
    NvU64 aligned_start = UVM_ALIGN_DOWN(range_vec->start, pde_coverage);
    NvU64 aligned_addr = UVM_ALIGN_DOWN(addr, pde_coverage);
    UVM_ASSERT(addr >= range_vec->start);
    UVM_ASSERT(addr < range_vec->start + range_vec->size);
    return (size_t)uvm_div_pow2_64(aligned_addr - aligned_start, pde_coverage);
}

NV_STATUS uvm_page_table_range_vec_init(uvm_page_tree_t *tree,
                                        NvU64 start,
                                        NvU64 size,
                                        NvU64 page_size,
                                        uvm_pmm_alloc_flags_t pmm_flags,
                                        uvm_page_table_range_vec_t *range_vec)
{
    NV_STATUS status;
    size_t i;

    UVM_ASSERT(size != 0);
    UVM_ASSERT_MSG(IS_ALIGNED(start, page_size), "start 0x%llx page_size 0x%llx\n", start, page_size);
    UVM_ASSERT_MSG(IS_ALIGNED(size, page_size), "size 0x%llx page_size 0x%llx\n", size, page_size);

    range_vec->tree = tree;
    range_vec->page_size = page_size;
    range_vec->start = start;
    range_vec->size = size;
    range_vec->range_count = range_vec_calc_range_count(range_vec);

    range_vec->ranges = uvm_kvmalloc_zero(sizeof(*range_vec->ranges) * range_vec->range_count);
    if (!range_vec->ranges) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    for (i = 0; i < range_vec->range_count; ++i) {
        uvm_page_table_range_t *range = &range_vec->ranges[i];

        NvU64 range_start = range_vec_calc_range_start(range_vec, i);
        NvU64 range_size = range_vec_calc_range_size(range_vec, i);

        status = uvm_page_tree_get_ptes_async(tree,
                                              page_size,
                                              range_start,
                                              range_size,
                                              pmm_flags,
                                              range);
        if (status != NV_OK) {
            UVM_ERR_PRINT("Failed to get PTEs for subrange %zd [0x%llx, 0x%llx) size 0x%llx, part of [0x%llx, 0x%llx)\n",
                          i,
                          range_start,
                          range_start + range_size,
                          range_size,
                          start,
                          size);
            goto out;
        }
    }

    status = uvm_page_tree_wait(tree);

out:
    if (status != NV_OK)
        uvm_page_table_range_vec_deinit(range_vec);
    return status;
}

NV_STATUS uvm_page_table_range_vec_create(uvm_page_tree_t *tree,
                                          NvU64 start,
                                          NvU64 size,
                                          NvU64 page_size,
                                          uvm_pmm_alloc_flags_t pmm_flags,
                                          uvm_page_table_range_vec_t **range_vec_out)
{
    NV_STATUS status;
    uvm_page_table_range_vec_t *range_vec;

    range_vec = uvm_kvmalloc(sizeof(*range_vec));
    if (!range_vec)
        return NV_ERR_NO_MEMORY;

    status = uvm_page_table_range_vec_init(tree, start, size, page_size, pmm_flags, range_vec);
    if (status != NV_OK)
        goto error;

    *range_vec_out = range_vec;

    return NV_OK;

error:
    uvm_kvfree(range_vec);
    return status;
}

NV_STATUS uvm_page_table_range_vec_split_upper(uvm_page_table_range_vec_t *range_vec,
                                               NvU64 new_end,
                                               uvm_page_table_range_vec_t *new_range_vec)
{
    size_t split_index;
    size_t num_remaining_pages = 0;
    uvm_page_table_range_t *range = NULL;
    NvU64 pde_coverage = uvm_mmu_pde_coverage(range_vec->tree, range_vec->page_size);
    NvU64 new_start = new_end + 1;
    NvU64 range_base_va = UVM_ALIGN_DOWN(new_start, pde_coverage);

    UVM_ASSERT(new_range_vec);
    UVM_ASSERT(new_end != 0);
    UVM_ASSERT(new_end > range_vec->start && new_end < range_vec->start + range_vec->size);
    UVM_ASSERT(IS_ALIGNED(new_start, range_vec->page_size));
    UVM_ASSERT(range_vec->range_count > 0);

    split_index = range_vec_calc_range_index(range_vec, new_start);
    range = &range_vec->ranges[split_index];
    if (range_vec->start > range_base_va)
        num_remaining_pages = (new_start - range_vec->start) / range_vec->page_size;
    else
        num_remaining_pages = (new_start - range_base_va) / range_vec->page_size;

    new_range_vec->tree = range_vec->tree;
    new_range_vec->page_size = range_vec->page_size;
    new_range_vec->start = new_start;
    new_range_vec->size = range_vec->size - (new_range_vec->start - range_vec->start);
    new_range_vec->range_count = range_vec->range_count - split_index;

    new_range_vec->ranges = uvm_kvmalloc_zero(sizeof(*new_range_vec->ranges) * new_range_vec->range_count);
    if (!new_range_vec->ranges)
        return NV_ERR_NO_MEMORY;

    // Handle splitting a range
    if (num_remaining_pages) {
        uvm_page_table_range_get_upper(range_vec->tree,
                                       range,
                                       &new_range_vec->ranges[0],
                                       range->entry_count - num_remaining_pages);
        uvm_page_table_range_shrink(range_vec->tree, range, num_remaining_pages);
    }

    // Copy the remainder of the ranges from the existing vector to the new one.
    memcpy(new_range_vec->ranges + !!num_remaining_pages,
           range_vec->ranges + split_index + !!num_remaining_pages,
           (new_range_vec->range_count - !!num_remaining_pages) * sizeof(*range));

    // Adjust the coverage of range_vec. If the re-allocation of the ranges array
    // fails, the old array is left untouched.
    range_vec->size -= new_range_vec->size;
    range_vec->range_count -= new_range_vec->range_count - !!num_remaining_pages;
    range = uvm_kvrealloc(range_vec->ranges, range_vec->range_count * sizeof(*range));
    if (range)
        range_vec->ranges = range;

    return NV_OK;
}

static NV_STATUS uvm_page_table_range_vec_clear_ptes_cpu(uvm_page_table_range_vec_t *range_vec, uvm_membar_t tlb_membar)
{
    uvm_page_tree_t *tree = range_vec->tree;
    NvU32 entry_size = uvm_mmu_pte_size(tree, range_vec->page_size);
    NvU64 invalid_ptes[2] = {0, 0};
    uvm_push_t push;
    NV_STATUS status;
    size_t i;

    UVM_ASSERT(uvm_mmu_use_cpu(tree));

    for (i = 0; i < range_vec->range_count; ++i) {
        uvm_page_table_range_t *range = &range_vec->ranges[i];
        uvm_mmu_page_table_alloc_t *dir = &range->table->phys_alloc;

        if (entry_size == 8)
            uvm_mmu_page_table_cpu_memset_8(tree->gpu, dir, range->start_index, invalid_ptes[0], range->entry_count);
        else
            uvm_mmu_page_table_cpu_memset_16(tree->gpu, dir, range->start_index, invalid_ptes, range->entry_count);
    }

    // A CPU membar is needed between the PTE writes and the subsequent TLB
    // invalidate. Work submission guarantees such a membar.
    status = page_tree_begin_acquire(tree,
                                     NULL,
                                     &push,
                                     "Invalidating [0x%llx, 0x%llx)",
                                     range_vec->start,
                                     range_vec->start + range_vec->size);
    if (status != NV_OK)
        return status;

    uvm_tlb_batch_single_invalidate(tree, &push, range_vec->start, range_vec->size, range_vec->page_size, tlb_membar);
    return page_tree_end_and_wait(tree, &push);
}

static NV_STATUS uvm_page_table_range_vec_clear_ptes_gpu(uvm_page_table_range_vec_t *range_vec, uvm_membar_t tlb_membar)
{
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;
    size_t i;
    uvm_page_tree_t *tree = range_vec->tree;
    uvm_gpu_t *gpu = tree->gpu;
    NvU64 page_size = range_vec->page_size;
    NvU32 entry_size = uvm_mmu_pte_size(tree, page_size);
    NvU64 invalid_pte = 0;
    uvm_push_t push;
    uvm_tracker_t tracker = UVM_TRACKER_INIT();

    uvm_pte_batch_t pte_batch;

    UVM_ASSERT(range_vec);
    UVM_ASSERT(tree);
    UVM_ASSERT(gpu);
    UVM_ASSERT(!uvm_mmu_use_cpu(tree));

    i = 0;
    while (i < range_vec->range_count) {
        // Acquiring the previous push is not necessary for correctness as all
        // the memsets can be done independently, but scheduling a lot of
        // independent work for a big range could end up hogging the GPU
        // for a long time while not providing much improvement.
        status = page_tree_begin_acquire(tree, &tracker, &push, "Clearing PTEs for [0x%llx, 0x%llx)",
                    range_vec->start, range_vec->start + range_vec->size);

        if (status != NV_OK)
            goto done;

        uvm_pte_batch_begin(&push, &pte_batch);

        for (; i < range_vec->range_count; ++i) {
            uvm_page_table_range_t *range = &range_vec->ranges[i];
            uvm_gpu_phys_address_t first_entry_pa = uvm_page_table_range_entry_address(tree, range, 0);
            uvm_pte_batch_clear_ptes(&pte_batch, first_entry_pa, invalid_pte, entry_size, range->entry_count);

            if (!uvm_push_has_space(&push, 512)) {
                // Stop pushing the clears once we get close to a full push
                break;
            }
        }

        uvm_pte_batch_end(&pte_batch);

        if (i == range_vec->range_count)
            uvm_tlb_batch_single_invalidate(tree, &push, range_vec->start, range_vec->size, page_size, tlb_membar);

        page_tree_end(tree, &push);

        // Skip the tracking if in unit test mode
        if (!tree->gpu->channel_manager)
            continue;

        // The push acquired the tracker so it's ok to just overwrite it with
        // the entry tracking the push.
        uvm_tracker_overwrite_with_push(&tracker, &push);
    }

done:
    tracker_status = uvm_tracker_wait_deinit(&tracker);
    if (status == NV_OK)
        status = tracker_status;

    return status;
}

NV_STATUS uvm_page_table_range_vec_clear_ptes(uvm_page_table_range_vec_t *range_vec, uvm_membar_t tlb_membar)
{
    if (uvm_mmu_use_cpu(range_vec->tree))
        return uvm_page_table_range_vec_clear_ptes_cpu(range_vec, tlb_membar);
    else
        return uvm_page_table_range_vec_clear_ptes_gpu(range_vec, tlb_membar);
}

void uvm_page_table_range_vec_deinit(uvm_page_table_range_vec_t *range_vec)
{
    size_t i;
    if (!range_vec)
        return;

    if (range_vec->ranges) {
        for (i = 0; i < range_vec->range_count; ++i) {
            uvm_page_table_range_t *range = &range_vec->ranges[i];
            if (!range->entry_count)
                break;
            uvm_page_tree_put_ptes_async(range_vec->tree, range);
        }
        (void)uvm_page_tree_wait(range_vec->tree);

        uvm_kvfree(range_vec->ranges);
    }

    memset(range_vec, 0, sizeof(*range_vec));
}

void uvm_page_table_range_vec_destroy(uvm_page_table_range_vec_t *range_vec)
{
    if (!range_vec)
        return;

    uvm_page_table_range_vec_deinit(range_vec);

    uvm_kvfree(range_vec);
}

static NV_STATUS uvm_page_table_range_vec_write_ptes_cpu(uvm_page_table_range_vec_t *range_vec,
                                                         uvm_membar_t tlb_membar,
                                                         uvm_page_table_range_pte_maker_t pte_maker,
                                                         void *caller_data)
{
    NV_STATUS status;
    size_t i;
    uvm_page_tree_t *tree = range_vec->tree;
    NvU32 entry_size = uvm_mmu_pte_size(tree, range_vec->page_size);
    uvm_push_t push;
    NvU64 offset = 0;

    UVM_ASSERT(uvm_mmu_use_cpu(tree));

    // Enforce ordering with prior accesses to the pages being mapped before the
    // mappings are activated.
    mb();

    for (i = 0; i < range_vec->range_count; ++i) {
        uvm_page_table_range_t *range = &range_vec->ranges[i];
        uvm_mmu_page_table_alloc_t *dir = &range->table->phys_alloc;
        NvU32 entry;

        for (entry = 0; entry < range->entry_count; ++entry) {
            NvU64 pte_bits[2] = {pte_maker(range_vec, offset, caller_data), 0};

            if (entry_size == 8)
                uvm_mmu_page_table_cpu_memset_8(tree->gpu, dir, range->start_index + entry, pte_bits[0], 1);
            else
                uvm_mmu_page_table_cpu_memset_16(tree->gpu, dir, range->start_index + entry, pte_bits, 1);

            offset += range_vec->page_size;
        }
    }

    status = page_tree_begin_acquire(tree,
                                     NULL,
                                     &push,
                                     "Invalidating [0x%llx, 0x%llx)",
                                     range_vec->start,
                                     range_vec->start + range_vec->size);
    if (status != NV_OK)
        return status;

    uvm_tlb_batch_single_invalidate(tree, &push, range_vec->start, range_vec->size, range_vec->page_size, tlb_membar);
    return page_tree_end_and_wait(tree, &push);
}

static NV_STATUS uvm_page_table_range_vec_write_ptes_gpu(uvm_page_table_range_vec_t *range_vec,
                                                         uvm_membar_t tlb_membar,
                                                         uvm_page_table_range_pte_maker_t pte_maker,
                                                         void *caller_data)
{
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;
    NvU32 entry;
    size_t i;
    uvm_page_tree_t *tree = range_vec->tree;
    uvm_gpu_t *gpu = tree->gpu;
    NvU32 entry_size = uvm_mmu_pte_size(tree, range_vec->page_size);

    uvm_push_t push;
    uvm_pte_batch_t pte_batch;
    NvU64 offset = 0;
    uvm_tracker_t tracker = UVM_TRACKER_INIT();
    bool last_push = false;

    // Use as much push space as possible leaving 1K of margin
    static const NvU32 max_total_entry_size_per_push = UVM_MAX_PUSH_SIZE - 1024;

    NvU32 max_entries_per_push = max_total_entry_size_per_push / entry_size;

    UVM_ASSERT(!uvm_mmu_use_cpu(tree));

    for (i = 0; i < range_vec->range_count; ++i) {
        uvm_page_table_range_t *range = &range_vec->ranges[i];
        NvU64 range_start = range_vec_calc_range_start(range_vec, i);
        NvU64 range_size = range_vec_calc_range_size(range_vec, i);
        uvm_gpu_phys_address_t entry_addr = uvm_page_table_range_entry_address(tree, range, 0);
        entry = 0;

        while (entry < range->entry_count) {
            NvU32 entry_limit_this_push = min(range->entry_count, entry + max_entries_per_push);

            // Acquiring the previous push is not necessary for correctness as all
            // the PTE writes can be done independently, but scheduling a lot of
            // independent work for a big range could end up hogging the GPU
            // for a long time while not providing much improvement.
            status = page_tree_begin_acquire(tree,
                                             &tracker,
                                             &push,
                                             "Writing PTEs for range at [0x%llx, 0x%llx), subrange of range vec at [0x%llx, 0x%llx)",
                                             range_start,
                                             range_start + range_size,
                                             range_vec->start,
                                             range_vec->start + range_vec->size);
            if (status != NV_OK) {
                UVM_ERR_PRINT("Failed to begin push for writing PTEs: %s GPU %s\n",
                              nvstatusToString(status),
                              uvm_gpu_name(gpu));
                goto done;
            }

            uvm_pte_batch_begin(&push, &pte_batch);

            for (; entry < entry_limit_this_push; ++entry) {
                NvU64 pte_bits = pte_maker(range_vec, offset, caller_data);
                uvm_pte_batch_write_pte(&pte_batch, entry_addr, pte_bits, entry_size);
                offset += range_vec->page_size;
                entry_addr.address += entry_size;
            }

            last_push = (i == range_vec->range_count - 1) && entry == range->entry_count;

            uvm_pte_batch_end(&pte_batch);

            if (last_push) {
                // Invalidate TLBs as part of the last push
                uvm_tlb_batch_single_invalidate(tree,
                                                &push,
                                                range_vec->start,
                                                range_vec->size,
                                                range_vec->page_size,
                                                tlb_membar);
            }
            else {
                // For pushes prior to the last one, uvm_pte_batch_end() has
                // already pushed a membar that's enough to order the PTE writes
                // with the TLB invalidate in the last push and that's all
                // that's needed.
                uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
            }

            page_tree_end(tree, &push);

            // Skip the tracking if in unit test mode
            if (!tree->gpu->channel_manager)
                continue;

            // The push acquired the tracker so it's ok to just overwrite it with
            // the entry tracking the push.
            uvm_tracker_overwrite_with_push(&tracker, &push);
        }
    }

done:
    tracker_status = uvm_tracker_wait_deinit(&tracker);
    if (status == NV_OK)
        status = tracker_status;
    return status;
}

NV_STATUS uvm_page_table_range_vec_write_ptes(uvm_page_table_range_vec_t *range_vec,
                                              uvm_membar_t tlb_membar,
                                              uvm_page_table_range_pte_maker_t pte_maker,
                                              void *caller_data)
{
    if (uvm_mmu_use_cpu(range_vec->tree))
        return uvm_page_table_range_vec_write_ptes_cpu(range_vec, tlb_membar, pte_maker, caller_data);
    else
        return uvm_page_table_range_vec_write_ptes_gpu(range_vec, tlb_membar, pte_maker, caller_data);
}

typedef struct identity_mapping_pte_maker_data_struct
{
    NvU64 phys_offset;
    uvm_aperture_t aperture;
} identity_mapping_pte_maker_data_t;

static NvU64 identity_mapping_pte_maker(uvm_page_table_range_vec_t *range_vec, NvU64 offset, void *data)
{
    identity_mapping_pte_maker_data_t *vpdata = (identity_mapping_pte_maker_data_t *)data;
    NvU64 pte_flags = vpdata->aperture == UVM_APERTURE_VID ? UVM_MMU_PTE_FLAGS_CACHED : UVM_MMU_PTE_FLAGS_NONE;
    return range_vec->tree->hal->make_pte(vpdata->aperture,
                                          offset + vpdata->phys_offset,
                                          UVM_PROT_READ_WRITE_ATOMIC,
                                          pte_flags);
}

static NV_STATUS create_identity_mapping(uvm_gpu_t *gpu,
                                         uvm_gpu_identity_mapping_t *mapping,
                                         NvU64 size,
                                         uvm_aperture_t aperture,
                                         NvU64 phys_offset,
                                         NvU64 page_size,
                                         uvm_pmm_alloc_flags_t pmm_flags)
{
    NV_STATUS status;
    identity_mapping_pte_maker_data_t data =
    {
        .phys_offset = phys_offset,
        .aperture = aperture
    };

    status = uvm_page_table_range_vec_create(&gpu->address_space_tree,
                                             mapping->base,
                                             size,
                                             page_size,
                                             pmm_flags,
                                             &mapping->range_vec);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init range vec for aperture %d identity mapping at [0x%llx, 0x%llx): %s, GPU %s\n",
                       aperture,
                       mapping->base,
                       mapping->base + size,
                       nvstatusToString(status),
                       uvm_gpu_name(gpu));
        return status;
    }

    status = uvm_page_table_range_vec_write_ptes(mapping->range_vec,
                                                 UVM_MEMBAR_NONE,
                                                 identity_mapping_pte_maker,
                                                 &data);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to write PTEs for aperture %d identity mapping at [0x%llx, 0x%llx): %s, GPU %s\n",
                      aperture,
                      mapping->base,
                      mapping->base + size,
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    mapping->ready = true;
    return NV_OK;
}

static void destroy_identity_mapping(uvm_gpu_identity_mapping_t *mapping)
{
    if (mapping->range_vec == NULL)
        return;

    // Tell the teardown routines they can't use this mapping as part of their
    // teardown.
    mapping->ready = false;

    (void)uvm_page_table_range_vec_clear_ptes(mapping->range_vec, UVM_MEMBAR_SYS);
    uvm_page_table_range_vec_destroy(mapping->range_vec);
    mapping->range_vec = NULL;
}

bool uvm_mmu_parent_gpu_needs_static_vidmem_mapping(uvm_parent_gpu_t *parent_gpu)
{
    return !parent_gpu->ce_phys_vidmem_write_supported;
}

bool uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(uvm_parent_gpu_t *parent_gpu)
{
    return uvm_parent_gpu_is_virt_mode_sriov_heavy(parent_gpu);
}

bool uvm_mmu_parent_gpu_needs_dynamic_sysmem_mapping(uvm_parent_gpu_t *parent_gpu)
{
    return uvm_parent_gpu_is_virt_mode_sriov_heavy(parent_gpu);
}

static NV_STATUS create_static_vidmem_mapping(uvm_gpu_t *gpu)
{
    NvU64 page_size;
    NvU64 size;
    uvm_aperture_t aperture = UVM_APERTURE_VID;
    NvU64 phys_offset = 0;
    uvm_gpu_identity_mapping_t *flat_mapping = &gpu->static_flat_mapping;

    if (!uvm_mmu_parent_gpu_needs_static_vidmem_mapping(gpu->parent))
        return NV_OK;

    UVM_ASSERT(!uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(gpu->parent));

    page_size = mmu_biggest_page_size(&gpu->address_space_tree, UVM_APERTURE_VID);
    size = UVM_ALIGN_UP(gpu->mem_info.max_allocatable_address + 1, page_size);

    UVM_ASSERT(page_size);
    UVM_ASSERT(size);
    UVM_ASSERT(size <= UVM_GPU_MAX_PHYS_MEM);

    flat_mapping->base = gpu->parent->flat_vidmem_va_base;

    return create_identity_mapping(gpu,
                                   flat_mapping,
                                   size,
                                   aperture,
                                   phys_offset,
                                   page_size,
                                   UVM_PMM_ALLOC_FLAGS_EVICT);
}

static void destroy_static_vidmem_mapping(uvm_gpu_t *gpu)
{
    if (!uvm_mmu_parent_gpu_needs_static_vidmem_mapping(gpu->parent))
        return;

    destroy_identity_mapping(&gpu->static_flat_mapping);
}

NV_STATUS uvm_mmu_create_peer_identity_mappings(uvm_gpu_t *gpu, uvm_gpu_t *peer)
{
    NvU64 page_size;
    NvU64 size;
    uvm_aperture_t aperture;
    NvU64 phys_offset;
    uvm_gpu_identity_mapping_t *peer_mapping;

    UVM_ASSERT(gpu->parent->peer_copy_mode < UVM_GPU_PEER_COPY_MODE_COUNT);

    if (gpu->parent->peer_copy_mode != UVM_GPU_PEER_COPY_MODE_VIRTUAL || peer->mem_info.size == 0)
        return NV_OK;

    aperture = uvm_gpu_peer_aperture(gpu, peer);
    page_size = mmu_biggest_page_size(&gpu->address_space_tree, aperture);
    size = UVM_ALIGN_UP(peer->mem_info.max_allocatable_address + 1, page_size);
    peer_mapping = uvm_gpu_get_peer_mapping(gpu, peer->id);
    phys_offset = 0ULL;

    if (uvm_gpus_are_nvswitch_connected(gpu, peer)) {
        // Add the 47-bit physical address routing bits for this peer to the
        // generated PTEs
        phys_offset = peer->parent->nvswitch_info.fabric_memory_window_start;
    }

    UVM_ASSERT(page_size);
    UVM_ASSERT(size);
    UVM_ASSERT(size <= UVM_PEER_IDENTITY_VA_SIZE);
    UVM_ASSERT(peer_mapping->base);

    return create_identity_mapping(gpu,
                                   peer_mapping,
                                   size,
                                   aperture,
                                   phys_offset,
                                   page_size,
                                   UVM_PMM_ALLOC_FLAGS_EVICT);
}

void uvm_mmu_destroy_peer_identity_mappings(uvm_gpu_t *gpu, uvm_gpu_t *peer)
{
    if (gpu->parent->peer_copy_mode == UVM_GPU_PEER_COPY_MODE_VIRTUAL)
        destroy_identity_mapping(uvm_gpu_get_peer_mapping(gpu, peer->id));
}

void uvm_mmu_init_gpu_chunk_sizes(uvm_parent_gpu_t *parent_gpu)
{
    uvm_chunk_sizes_mask_t sizes = page_sizes_for_big_page_size(parent_gpu, UVM_PAGE_SIZE_64K)  |
                                   page_sizes_for_big_page_size(parent_gpu, UVM_PAGE_SIZE_128K) |
                                   PAGE_SIZE;

    // Although we may have to map PTEs smaller than PAGE_SIZE, user (managed)
    // memory is never allocated with granularity smaller than PAGE_SIZE. Force
    // PAGE_SIZE to be supported and the smallest allowed size so we don't have
    // to handle allocating multiple chunks per page.
    parent_gpu->mmu_user_chunk_sizes = sizes & PAGE_MASK;

    // The maximum chunk size is 2MB (i.e., UVM_CHUNK_SIZE_MAX), therefore we
    // mask out any supported page size greater than UVM_CHUNK_SIZE_MAX from
    // the chunk size list.
    parent_gpu->mmu_user_chunk_sizes &= UVM_CHUNK_SIZES_MASK;

    parent_gpu->mmu_kernel_chunk_sizes = allocation_sizes_for_big_page_size(parent_gpu, UVM_PAGE_SIZE_64K) |
                                         allocation_sizes_for_big_page_size(parent_gpu, UVM_PAGE_SIZE_128K);
}

void uvm_mmu_init_gpu_peer_addresses(uvm_gpu_t *gpu)
{
    if (gpu->parent->peer_copy_mode == UVM_GPU_PEER_COPY_MODE_VIRTUAL) {
        uvm_gpu_id_t gpu_id;

        for_each_gpu_id(gpu_id) {
            uvm_gpu_get_peer_mapping(gpu, gpu_id)->base = gpu->parent->rm_va_base +
                                                          gpu->parent->rm_va_size +
                                                          UVM_PEER_IDENTITY_VA_SIZE * uvm_id_gpu_index(gpu_id);
        }

        UVM_ASSERT(gpu->parent->uvm_mem_va_base >=
                   uvm_gpu_get_peer_mapping(gpu, uvm_gpu_id_from_value(UVM_ID_MAX_GPUS - 1))->base +
                                            UVM_PEER_IDENTITY_VA_SIZE);
    }
}

static size_t root_chunk_mapping_index(uvm_gpu_t *gpu, uvm_gpu_root_chunk_mapping_t *root_chunk_mapping)
{
    return root_chunk_mapping - gpu->root_chunk_mappings.array;
}

static NvU64 root_chunk_mapping_physical_address(uvm_gpu_t *gpu, uvm_gpu_root_chunk_mapping_t *root_chunk_mapping)
{
    return UVM_CHUNK_SIZE_MAX * root_chunk_mapping_index(gpu, root_chunk_mapping);
}

static void root_chunk_mapping_lock(uvm_gpu_t *gpu, uvm_gpu_root_chunk_mapping_t *root_chunk_mapping)
{
    uvm_bit_lock(&gpu->root_chunk_mappings.bitlocks, root_chunk_mapping_index(gpu, root_chunk_mapping));
}

static void root_chunk_mapping_unlock(uvm_gpu_t *gpu, uvm_gpu_root_chunk_mapping_t *root_chunk_mapping)
{
    uvm_bit_unlock(&gpu->root_chunk_mappings.bitlocks, root_chunk_mapping_index(gpu, root_chunk_mapping));
}

static uvm_gpu_root_chunk_mapping_t *root_chunk_mapping_from_address(uvm_gpu_t *gpu, NvU64 addr)
{
    size_t index = addr / UVM_CHUNK_SIZE_MAX;

    UVM_ASSERT(addr <= gpu->mem_info.max_allocatable_address);

    return gpu->root_chunk_mappings.array + index;
}

static uvm_gpu_root_chunk_mapping_t *root_chunk_mapping_from_chunk(uvm_gpu_t *gpu, uvm_gpu_chunk_t *chunk)
{
    return root_chunk_mapping_from_address(gpu, chunk->address);
}

static void destroy_dynamic_vidmem_mapping(uvm_gpu_t *gpu)
{
    size_t i;

    if (!uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(gpu->parent))
        return;

    if (gpu->root_chunk_mappings.array == NULL)
        return;

    uvm_bit_locks_deinit(&gpu->root_chunk_mappings.bitlocks);

    for (i = 0; i < gpu->root_chunk_mappings.count; ++i) {
        uvm_gpu_root_chunk_mapping_t *root_chunk_mapping = gpu->root_chunk_mappings.array + i;

        UVM_ASSERT(root_chunk_mapping->range == NULL);
        UVM_ASSERT(root_chunk_mapping->num_mapped_pages == 0);
    }

    uvm_kvfree(gpu->root_chunk_mappings.array);
    gpu->root_chunk_mappings.array = NULL;
}

static NV_STATUS create_dynamic_vidmem_mapping(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    size_t count;

    if (!uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(gpu->parent))
        return NV_OK;

    UVM_ASSERT(!uvm_mmu_parent_gpu_needs_static_vidmem_mapping(gpu->parent));
    BUILD_BUG_ON(UVM_PAGE_SIZE_2M != UVM_CHUNK_SIZE_MAX);
    UVM_ASSERT(uvm_mmu_page_size_supported(&gpu->address_space_tree, UVM_PAGE_SIZE_2M));
    UVM_ASSERT(gpu->pmm.initialized);

    count = gpu->pmm.root_chunks.count;

    gpu->root_chunk_mappings.array = uvm_kvmalloc_zero(sizeof(*gpu->root_chunk_mappings.array) * count);
    if (gpu->root_chunk_mappings.array == NULL)
        return NV_ERR_NO_MEMORY;

    gpu->root_chunk_mappings.count = count;

    status = uvm_bit_locks_init(&gpu->root_chunk_mappings.bitlocks, count, UVM_LOCK_ORDER_CHUNK_MAPPING);
    if (status != NV_OK)
        goto error;

    return NV_OK;

error:
    destroy_dynamic_vidmem_mapping(gpu);
    return status;
}

// Remove a root chunk mapping from the flat mapping address segment.
//
// This function is a specialized version of destroy_identity_mapping
// because the mapping fits into a single 2MB PTE, so there is no need to use
// page table range vectors.
static void root_chunk_mapping_destroy(uvm_gpu_t *gpu, uvm_gpu_root_chunk_mapping_t *root_chunk_mapping)
{
    NvU64 pa, va;
    uvm_gpu_address_t gpu_virtual_address;
    uvm_page_tree_t *tree;
    uvm_gpu_phys_address_t entry_addr;
    uvm_push_t push;
    NvU32 entry_size;
    uvm_pte_batch_t pte_batch;
    NvU64 page_size;
    NvU64 size;
    NvU64 invalid_pte;
    uvm_page_table_range_t *range = root_chunk_mapping->range;

    UVM_ASSERT(range != NULL);

    tree = &gpu->address_space_tree;
    pa = root_chunk_mapping_physical_address(gpu, root_chunk_mapping);
    gpu_virtual_address = uvm_gpu_address_virtual_from_vidmem_phys(gpu, pa);
    va = gpu_virtual_address.address;
    page_size = range->page_size;
    size = page_size;

    (void) page_tree_begin_acquire(tree, NULL, &push, "Removing linear mapping at [0x%llx, 0x%llx)", va, va + size);

    uvm_pte_batch_begin(&push, &pte_batch);
    entry_addr = uvm_page_table_range_entry_address(tree, range, 0);
    invalid_pte = 0;
    entry_size = uvm_mmu_pte_size(tree, page_size);
    uvm_pte_batch_clear_ptes(&pte_batch, entry_addr, invalid_pte, entry_size, range->entry_count);
    uvm_pte_batch_end(&pte_batch);

    uvm_tlb_batch_single_invalidate(tree, &push, va, size, page_size, UVM_MEMBAR_SYS);

    (void) page_tree_end_and_wait(tree, &push);

    uvm_page_tree_put_ptes(tree, range);

    uvm_kvfree(range);
    root_chunk_mapping->range = NULL;
}

// Add a root chunk mapping to the flat mapping address segment located in the
// UVM-internal GPU kernel address space.
//
// This function is a specialized version of create_identity_mapping because the
// mapping fits into a single 2MB PTE, so there is no need to use page table
// range vectors.
static NV_STATUS root_chunk_mapping_create(uvm_gpu_t *gpu, uvm_gpu_root_chunk_mapping_t *root_chunk_mapping)
{
    NV_STATUS status;
    NvU64 pa, va;
    uvm_gpu_address_t gpu_virtual_address;
    uvm_page_table_range_t *range;
    uvm_page_tree_t *tree;
    uvm_gpu_phys_address_t entry_addr;
    uvm_push_t push;
    NvU64 pte_bits;
    NvU32 entry_size;
    NvU64 page_size = UVM_CHUNK_SIZE_MAX;
    NvU64 size = UVM_CHUNK_SIZE_MAX;

    range = uvm_kvmalloc_zero(sizeof(*range));
    if (range == NULL)
        return NV_ERR_NO_MEMORY;

    tree = &gpu->address_space_tree;
    pa = root_chunk_mapping_physical_address(gpu, root_chunk_mapping);
    gpu_virtual_address = uvm_gpu_address_virtual_from_vidmem_phys(gpu, pa);
    va = gpu_virtual_address.address;

    UVM_ASSERT(IS_ALIGNED(va, page_size));

    // A VA block lock is likely to be held, so eviction is not requested for
    // allocations of page tree backing storage.
    status = uvm_page_tree_get_ptes(tree, page_size, va, size, UVM_PMM_ALLOC_FLAGS_NONE, range);
    if (status != NV_OK) {
        uvm_kvfree(range);
        return status;
    }

    UVM_ASSERT(range->entry_count == 1);

    status = page_tree_begin_acquire(tree, NULL, &push, "Adding linear mapping at [0x%llx, 0x%llx)", va, va + size);
    if (status != NV_OK)
        goto error;

    entry_addr = uvm_page_table_range_entry_address(tree, range, 0);
    pte_bits = tree->hal->make_pte(UVM_APERTURE_VID, pa, UVM_PROT_READ_WRITE_ATOMIC, UVM_MMU_PTE_FLAGS_CACHED);
    entry_size = uvm_mmu_pte_size(tree, page_size);
    uvm_pte_batch_single_write_ptes(&push, entry_addr, &pte_bits, entry_size, range->entry_count);

    uvm_tlb_batch_single_invalidate(tree, &push, va, page_size, page_size, UVM_MEMBAR_NONE);

    status = page_tree_end_and_wait(tree, &push);
    if (status != NV_OK)
        goto error;

    root_chunk_mapping->range = range;

    return NV_OK;

error:
    uvm_page_tree_put_ptes(tree, range);
    uvm_kvfree(range);

    return status;
}

NV_STATUS uvm_mmu_chunk_map(uvm_gpu_chunk_t *chunk)
{
    uvm_gpu_root_chunk_mapping_t *root_chunk_mapping;
    uvm_chunk_size_t chunk_size;
    NvU16 num_mapped_pages;
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu = uvm_gpu_chunk_get_gpu(chunk);

    if (!uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(gpu->parent))
        return NV_OK;

    chunk_size = uvm_gpu_chunk_get_size(chunk);
    num_mapped_pages = chunk_size / PAGE_SIZE;

    // While there could be legitimate use cases for mapping a kernel chunk,
    // currently it is disallowed. There are a couple of reasons for this.
    // First, in SR-IOV heavy, disallowing kernel chunks prevents unintended
    // mappings of page tables/directories when they share the backing
    // allocation i.e. the memory backing the kernel chunk is in the same root
    // chunk as the memory backing a GPU page tree node. Second, root chunk
    // mappings are reference counted as multiples of PAGE_SIZE. User chunk
    // sizes are guaranteed to be a multiple of that page size, but kernel chunk
    // sizes can be smaller.
    UVM_ASSERT(uvm_pmm_gpu_memory_type_is_user(chunk->type));

    UVM_ASSERT(PAGE_ALIGNED(chunk_size));

    root_chunk_mapping = root_chunk_mapping_from_chunk(gpu, chunk);
    root_chunk_mapping_lock(gpu, root_chunk_mapping);

    if (root_chunk_mapping->num_mapped_pages == 0) {
        UVM_ASSERT(root_chunk_mapping->range == NULL);

        status = root_chunk_mapping_create(gpu, root_chunk_mapping);
        if (status != NV_OK)
            goto out;
    }

    // Check for reference counting overflow. The number of mapped pages can be
    // above UVM_CHUNK_SIZE_MAX / PAGE_SIZE because the same chunk can be mapped
    // multiple times.
    UVM_ASSERT((NV_U32_MAX - root_chunk_mapping->num_mapped_pages) >= num_mapped_pages);

    root_chunk_mapping->num_mapped_pages += num_mapped_pages;

 out:
    root_chunk_mapping_unlock(gpu, root_chunk_mapping);
    return status;
}

void uvm_mmu_chunk_unmap(uvm_gpu_chunk_t *chunk, uvm_tracker_t *tracker)
{
    uvm_gpu_root_chunk_mapping_t *root_chunk_mapping;
    uvm_chunk_size_t chunk_size;
    NvU16 num_unmapped_pages;
    uvm_gpu_t *gpu;

    if (chunk == NULL)
        return;

    gpu = uvm_gpu_chunk_get_gpu(chunk);
    if (!uvm_mmu_parent_gpu_needs_dynamic_vidmem_mapping(gpu->parent))
        return;

    if (tracker != NULL)
        uvm_tracker_wait(tracker);

    chunk_size = uvm_gpu_chunk_get_size(chunk);
    num_unmapped_pages = chunk_size / PAGE_SIZE;

    UVM_ASSERT(uvm_pmm_gpu_memory_type_is_user(chunk->type));
    UVM_ASSERT(PAGE_ALIGNED(chunk_size));

    root_chunk_mapping = root_chunk_mapping_from_chunk(gpu, chunk);
    root_chunk_mapping_lock(gpu, root_chunk_mapping);

    if (root_chunk_mapping->range != NULL) {
        UVM_ASSERT(root_chunk_mapping->num_mapped_pages >= num_unmapped_pages);

        root_chunk_mapping->num_mapped_pages -= num_unmapped_pages;

        if (root_chunk_mapping->num_mapped_pages == 0)
            root_chunk_mapping_destroy(gpu, root_chunk_mapping);
    }
    else {
        UVM_ASSERT(root_chunk_mapping->num_mapped_pages == 0);
    }

    root_chunk_mapping_unlock(gpu, root_chunk_mapping);
}


static size_t sysmem_mapping_index(uvm_gpu_t *gpu, uvm_gpu_identity_mapping_t *sysmem_mapping)
{
    return sysmem_mapping - gpu->sysmem_mappings.array;
}

static NvU64 sysmem_mapping_physical_address(uvm_gpu_t *gpu, uvm_gpu_identity_mapping_t *sysmem_mapping)
{
    return gpu->sysmem_mappings.mapping_size * sysmem_mapping_index(gpu, sysmem_mapping);
}

static uvm_gpu_identity_mapping_t *sysmem_mapping_from_address(uvm_gpu_t *gpu, NvU64 pa)
{
    size_t index = pa / gpu->sysmem_mappings.mapping_size;

    UVM_ASSERT(index < gpu->sysmem_mappings.count);

    return gpu->sysmem_mappings.array + index;
}

static void sysmem_mapping_lock(uvm_gpu_t *gpu, uvm_gpu_identity_mapping_t *sysmem_mapping)
{
    uvm_bit_lock(&gpu->sysmem_mappings.bitlocks, sysmem_mapping_index(gpu, sysmem_mapping));
}

static void sysmem_mapping_unlock(uvm_gpu_t *gpu, uvm_gpu_identity_mapping_t *sysmem_mapping)
{
    uvm_bit_unlock(&gpu->sysmem_mappings.bitlocks, sysmem_mapping_index(gpu, sysmem_mapping));
}

static void destroy_dynamic_sysmem_mapping(uvm_gpu_t *gpu)
{
    size_t i;

    if (!uvm_mmu_parent_gpu_needs_dynamic_sysmem_mapping(gpu->parent))
        return;

    if (gpu->sysmem_mappings.array == NULL)
        return;

    uvm_bit_locks_deinit(&gpu->sysmem_mappings.bitlocks);

    for (i = 0; i < gpu->sysmem_mappings.count; ++i)
        destroy_identity_mapping(gpu->sysmem_mappings.array + i);

    uvm_kvfree(gpu->sysmem_mappings.array);
    gpu->sysmem_mappings.array = NULL;
}

static NV_STATUS create_dynamic_sysmem_mapping(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    size_t count;
    NvU64 mapping_size;
    NvU64 flat_sysmem_va_size;

    if (!uvm_mmu_parent_gpu_needs_dynamic_sysmem_mapping(gpu->parent))
        return NV_OK;

    UVM_ASSERT(gpu->parent->flat_sysmem_va_base != 0);

    // The DMA addressable window is the maximum system physical memory
    // addressable by the GPU (this limit is 128TB in Pascal-Ada). The virtual
    // mapping to sysmem is linear, so its size matches that of the physical
    // address space.
    flat_sysmem_va_size = gpu->parent->dma_addressable_limit + 1 - gpu->parent->dma_addressable_start;

    // The optimal mapping granularity is dependent on multiple factors:
    // application access patterns, distribution of system memory pages across
    // the physical address space, mapping costs (themselves highly variable: in
    // SR-IOV each mapping addition adds a lot of overhead due to vGPU plugin
    // involvement), metadata memory footprint (inversely proportional to the
    // mapping size), etc.
    mapping_size = 4 * UVM_SIZE_1GB;

    // The mapping size should be at least 1GB, due to bitlock limitations. This
    // shouldn't be a problem because the expectation is to use 512MB PTEs, and
    // using a granularity of 1GB already results in allocating a large array of
    // sysmem mappings with 128K entries.
    UVM_ASSERT(is_power_of_2(mapping_size));
    UVM_ASSERT(mapping_size >= UVM_SIZE_1GB);
    UVM_ASSERT(mapping_size >= mmu_biggest_page_size(&gpu->address_space_tree, UVM_APERTURE_SYS));
    UVM_ASSERT(mapping_size <= flat_sysmem_va_size);

    flat_sysmem_va_size = UVM_ALIGN_UP(flat_sysmem_va_size, mapping_size);

    UVM_ASSERT((gpu->parent->flat_sysmem_va_base + flat_sysmem_va_size) <= gpu->parent->uvm_mem_va_base);

    count = flat_sysmem_va_size / mapping_size;

    gpu->sysmem_mappings.array = uvm_kvmalloc_zero(sizeof(*gpu->sysmem_mappings.array) * count);
    if (gpu->sysmem_mappings.array == NULL)
        return NV_ERR_NO_MEMORY;

    gpu->sysmem_mappings.mapping_size = mapping_size;
    gpu->sysmem_mappings.count = count;

    status = uvm_bit_locks_init(&gpu->sysmem_mappings.bitlocks, count, UVM_LOCK_ORDER_CHUNK_MAPPING);
    if (status != NV_OK)
        goto error;

    return NV_OK;

error:
    destroy_dynamic_sysmem_mapping(gpu);
    return status;
}

NV_STATUS uvm_mmu_sysmem_map(uvm_gpu_t *gpu, NvU64 pa, NvU64 size)
{
    NvU64 curr_pa;

    if (!uvm_mmu_parent_gpu_needs_dynamic_sysmem_mapping(gpu->parent))
        return NV_OK;

    curr_pa = UVM_ALIGN_DOWN(pa, gpu->sysmem_mappings.mapping_size);

    while (curr_pa < (pa + size)) {
        NV_STATUS status = NV_OK;
        uvm_gpu_identity_mapping_t *sysmem_mapping = sysmem_mapping_from_address(gpu, curr_pa);

        sysmem_mapping_lock(gpu, sysmem_mapping);

        if (sysmem_mapping->range_vec == NULL) {
            uvm_gpu_address_t virtual_address = uvm_parent_gpu_address_virtual_from_sysmem_phys(gpu->parent, curr_pa);
            NvU64 phys_offset = curr_pa;
            NvU64 page_size = mmu_biggest_page_size(&gpu->address_space_tree, UVM_APERTURE_SYS);
            uvm_pmm_alloc_flags_t pmm_flags;

            // No eviction is requested when allocating the page tree storage,
            // because in the common case the VA block lock is held.
            pmm_flags = UVM_PMM_ALLOC_FLAGS_NONE;

            sysmem_mapping->base = virtual_address.address;

            status = create_identity_mapping(gpu,
                                             sysmem_mapping,
                                             gpu->sysmem_mappings.mapping_size,
                                             UVM_APERTURE_SYS,
                                             phys_offset,
                                             page_size,
                                             pmm_flags);
        }

        sysmem_mapping_unlock(gpu, sysmem_mapping);

        // In case of error, don't undo previous mapping additions, since a
        // concurrent thread may already have pushed work to the GPU that is
        // dependent on the new mappings.
        if (status != NV_OK)
            return status;

        curr_pa += gpu->sysmem_mappings.mapping_size;
    }

    return NV_OK;
}

NV_STATUS uvm_mmu_create_flat_mappings(uvm_gpu_t *gpu)
{
    NV_STATUS status;

    status = create_static_vidmem_mapping(gpu);
    if (status != NV_OK)
        goto error;

    status = create_dynamic_sysmem_mapping(gpu);
    if (status != NV_OK)
        return status;

    status = create_dynamic_vidmem_mapping(gpu);
    if (status != NV_OK)
        goto error;

    return NV_OK;

error:
    uvm_mmu_destroy_flat_mappings(gpu);
    return status;
}

void uvm_mmu_destroy_flat_mappings(uvm_gpu_t *gpu)
{
    destroy_dynamic_vidmem_mapping(gpu);
    destroy_dynamic_sysmem_mapping(gpu);
    destroy_static_vidmem_mapping(gpu);
}

uvm_gpu_address_t uvm_mmu_gpu_address(uvm_gpu_t *gpu, uvm_gpu_phys_address_t phys_addr)
{
    if (phys_addr.aperture == UVM_APERTURE_VID && !gpu->parent->ce_phys_vidmem_write_supported)
        return uvm_gpu_address_virtual_from_vidmem_phys(gpu, phys_addr.address);

    return uvm_gpu_address_from_phys(phys_addr);
}

NV_STATUS uvm_test_invalidate_tlb(UVM_TEST_INVALIDATE_TLB_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_gpu_t *gpu = NULL;
    uvm_push_t push;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_gpu_va_space_t *gpu_va_space;

    // Check parameter values
    if (params->membar < UvmInvalidateTlbMemBarNone ||
        params->membar > UvmInvalidateTlbMemBarLocal) {
        return NV_ERR_INVALID_PARAMETER;
    }

    if (params->target_va_mode < UvmTargetVaModeAll ||
        params->target_va_mode > UvmTargetVaModeTargeted) {
        return NV_ERR_INVALID_PARAMETER;
    }

    if (params->page_table_level < UvmInvalidatePageTableLevelAll ||
        params->page_table_level > UvmInvalidatePageTableLevelPde4) {
        return NV_ERR_INVALID_PARAMETER;
    }

    uvm_va_space_down_read(va_space);

    gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->gpu_uuid);
    if (!gpu) {
        status = NV_ERR_INVALID_DEVICE;
        goto unlock_exit;
    }

    gpu_va_space = uvm_gpu_va_space_get(va_space, gpu);
    UVM_ASSERT(gpu_va_space);

    status = uvm_push_begin(gpu->channel_manager,
                            UVM_CHANNEL_TYPE_MEMOPS,
                            &push,
                            "Pushing test invalidate, GPU %s",
                            uvm_gpu_name(gpu));
    if (status == NV_OK)
        gpu->parent->host_hal->tlb_invalidate_test(&push, uvm_page_tree_pdb(&gpu_va_space->page_tables)->addr, params);

unlock_exit:
    // Wait for the invalidation to be performed
    if (status == NV_OK)
        status = uvm_push_end_and_wait(&push);

    uvm_va_space_up_read(va_space);

    return status;
}
