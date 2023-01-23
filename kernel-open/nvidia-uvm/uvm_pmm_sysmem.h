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

#ifndef __UVM_PMM_SYSMEM_H__
#define __UVM_PMM_SYSMEM_H__

#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_forward_decl.h"
#include "uvm_lock.h"

// Module to handle per-GPU user mappings to sysmem physical memory. Notably,
// this implements a reverse map of the DMA address to {va_block, virt_addr}.
// This is required by the GPU access counters feature since they may provide a
// physical address in the notification packet (GPA notifications). We use the
// table to obtain the VAs of the memory regions being accessed remotely. The
// reverse map is implemented by a radix tree, which is indexed using the
// DMA address. For now, only PAGE_SIZE translations are supported (i.e. no
// big/huge pages).
//
// TODO: Bug 1995015: add support for physically-contiguous mappings.
struct uvm_pmm_sysmem_mappings_struct
{
    uvm_gpu_t                                      *gpu;

    struct radix_tree_root             reverse_map_tree;

    uvm_mutex_t                        reverse_map_lock;
};

// See comments in uvm_linux.h
#ifdef NV_RADIX_TREE_REPLACE_SLOT_PRESENT
#define uvm_pmm_sysmem_mappings_indirect_supported() true
#else
#define uvm_pmm_sysmem_mappings_indirect_supported() false
#endif

// Global initialization/exit functions, that need to be called during driver
// initialization/tear-down. These are needed to allocate/free global internal
// data structures.
NV_STATUS uvm_pmm_sysmem_init(void);
void uvm_pmm_sysmem_exit(void);

// Initialize per-GPU sysmem mapping tracking
NV_STATUS uvm_pmm_sysmem_mappings_init(uvm_gpu_t *gpu, uvm_pmm_sysmem_mappings_t *sysmem_mappings);

// Destroy per-GPU sysmem mapping tracking. The caller must ensure that all the
// mappings have been removed before calling this function.
void uvm_pmm_sysmem_mappings_deinit(uvm_pmm_sysmem_mappings_t *sysmem_mappings);

// If the GPU used to initialize sysmem_mappings supports access counters, the
// dma_addr -> {va_block, virt_addr} mapping is inserted in the reverse map.
NV_STATUS uvm_pmm_sysmem_mappings_add_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                  NvU64 dma_addr,
                                                  NvU64 virt_addr,
                                                  NvU64 region_size,
                                                  uvm_va_block_t *va_block,
                                                  uvm_processor_id_t owner);

static NV_STATUS uvm_pmm_sysmem_mappings_add_gpu_chunk_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                               NvU64 dma_addr,
                                                               NvU64 virt_addr,
                                                               NvU64 region_size,
                                                               uvm_va_block_t *va_block,
                                                               uvm_gpu_id_t owner)
{
    if (!uvm_pmm_sysmem_mappings_indirect_supported())
        return NV_OK;

    return uvm_pmm_sysmem_mappings_add_gpu_mapping(sysmem_mappings,
                                                   dma_addr,
                                                   virt_addr,
                                                   region_size,
                                                   va_block,
                                                   owner);
}

// If the GPU used to initialize sysmem_mappings supports access counters, the
// entries for the physical region starting at dma_addr are removed from the
// reverse map.
void uvm_pmm_sysmem_mappings_remove_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings, NvU64 dma_addr);

static void uvm_pmm_sysmem_mappings_remove_gpu_chunk_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings, NvU64 dma_addr)
{
    if (uvm_pmm_sysmem_mappings_indirect_supported())
        uvm_pmm_sysmem_mappings_remove_gpu_mapping(sysmem_mappings, dma_addr);
}

// Like uvm_pmm_sysmem_mappings_remove_gpu_mapping but it doesn't assert if the
// mapping doesn't exist. See uvm_va_block_evict_chunks for more information.
void uvm_pmm_sysmem_mappings_remove_gpu_mapping_on_eviction(uvm_pmm_sysmem_mappings_t *sysmem_mappings, NvU64 dma_addr);

// If the GPU used to initialize sysmem_mappings supports access counters, the
// mapping for the region starting at dma_addr is updated with va_block.
// This is required on VA block split.
void uvm_pmm_sysmem_mappings_reparent_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                  NvU64 dma_addr,
                                                  uvm_va_block_t *va_block);

static void uvm_pmm_sysmem_mappings_reparent_gpu_chunk_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                               NvU64 dma_addr,
                                                               uvm_va_block_t *va_block)
{
    if (uvm_pmm_sysmem_mappings_indirect_supported())
        uvm_pmm_sysmem_mappings_reparent_gpu_mapping(sysmem_mappings, dma_addr, va_block);
}

// If the GPU used to initialize sysmem_mappings supports access counters, the
// mapping for the region starting at dma_addr is split into regions of
// new_region_size. new_region_size must be a power of two and smaller than the
// previously-registered size.
NV_STATUS uvm_pmm_sysmem_mappings_split_gpu_mappings(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                     NvU64 dma_addr,
                                                     NvU64 new_region_size);

static NV_STATUS uvm_pmm_sysmem_mappings_split_gpu_chunk_mappings(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                                  NvU64 dma_addr,
                                                                  NvU64 new_region_size)
{
    if (!uvm_pmm_sysmem_mappings_indirect_supported())
        return NV_OK;

    return uvm_pmm_sysmem_mappings_split_gpu_mappings(sysmem_mappings, dma_addr, new_region_size);
}

// If the GPU used to initialize sysmem_mappings supports access counters, all
// the mappings within the region [dma_addr, dma_addr + new_region_size) are
// merged into a single mapping. new_region_size must be a power of two. The
// whole region must be previously populated with mappings and all of them must
// have the same VA block and processor owner.
void uvm_pmm_sysmem_mappings_merge_gpu_mappings(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                NvU64 dma_addr,
                                                NvU64 new_region_size);

static void uvm_pmm_sysmem_mappings_merge_gpu_chunk_mappings(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                             NvU64 dma_addr,
                                                             NvU64 new_region_size)
{
    if (uvm_pmm_sysmem_mappings_indirect_supported())
        uvm_pmm_sysmem_mappings_merge_gpu_mappings(sysmem_mappings, dma_addr, new_region_size);
}

// Obtain the {va_block, virt_addr} information for the mappings in the given
// [dma_addr:dma_addr + region_size) range. dma_addr and region_size must be
// page-aligned.
//
// Valid translations are written to out_mappings sequentially (there are no
// gaps). max_out_mappings are written, at most. The caller is required to
// provide enough entries in out_mappings.
//
// The VA Block in each returned translation entry is retained, and it's up to
// the caller to release them
size_t uvm_pmm_sysmem_mappings_dma_to_virt(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                           NvU64 dma_addr,
                                           NvU64 region_size,
                                           uvm_reverse_map_t *out_mappings,
                                           size_t max_out_mappings);

#define UVM_CPU_CHUNK_SIZES PAGE_SIZE

#if UVM_CPU_CHUNK_SIZES == PAGE_SIZE
#define UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE() 1
typedef struct page uvm_cpu_chunk_t;

#define UVM_CPU_CHUNK_PAGE_INDEX(chunk, page_index) (page_index)

#else
#define UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE() 0
typedef struct uvm_cpu_chunk_struct uvm_cpu_chunk_t;

// CPU memory chunk descriptor.
// CPU memory chunks represent a physically contiguous CPU memory
// allocation.
// CPU memory chunks can be created due to CPU page allocation or
// CPU chunk splitting. Chunks created due to page allocations are
// referred to as "physical chunks", while chunks resulting from
// splitting are referred to as "logical chunks".
struct uvm_cpu_chunk_struct
{
    // Pointer to the CPU page backing this CPU chunk.
    // For physical chunks, this will point to the head page. Physical
    // chunk allocation will set the reference count for the struct
    // page (compound or not) to 1.
    //
    // For logical chunks, this will point to the struct page from
    // the compound page array corresponding to the correct page index.
    // Because freeing a logical chunk does not result in freeing of
    // any struct page(s) and both physical and logical chunks are
    // reference counted, there is no need to take separate references
    // to the struct page for logical chunks.
    struct page *page;

    // For logical chunks, this points to the parent chunk (which
    // could also be a logical chunk). For physical chunks, this
    // is NULL.
    uvm_cpu_chunk_t *parent;

    // Page offset of this chunk within the physical size of
    // the parent.
    uvm_page_index_t offset;

    // Region within the VA block covered by this CPU chunk.
    uvm_va_block_region_t region;

    // Chunk reference count used when a CPU chunk is split. Each
    // child sub-chunk will increment the reference count of its
    // parent.
    nv_kref_t refcount;

    // Size of the chunk.
    // For chunks resulting from page allocations (physical chunks),
    // this value is the size of the physical allocation.
    size_t log2_size : order_base_2(UVM_CHUNK_SIZE_MASK_SIZE);

    struct {
        // Per-GPU array of DMA mapping addresses for the chunk.
        // The DMA mapping addresses for logical chunks are adjusted
        // to the correct offset within the parent chunk.
        union {
            NvU64 static_entry;
            NvU64 *dynamic_entries;
        };
        uvm_processor_mask_t dma_addrs_mask;
    } gpu_mappings;

    // Lock protecting dirty_bitmap
    uvm_spinlock_t lock;

    // A dynamically allocated bitmap (one per PAGE_SIZE page) used
    // to track dirty state of each PAGE_SIZE page.
    // Dirty state is tracked only by physical chunks. Therefore,
    // for logical chunks this will be NULL;
    unsigned long *dirty_bitmap;
};

#define UVM_CPU_CHUNK_PAGE_INDEX(chunk, page_index) (chunk->region.first)
#endif // UVM_CPU_CHUNK_SIZES == PAGE_SIZE

// Return the set of allowed CPU chunk allocation sizes.
uvm_chunk_sizes_mask_t uvm_cpu_chunk_get_allocation_sizes(void);

// Allocate a physical CPU chunk for the specified page index and owned by
// va_block.
//
// The size of the allocated CPU chunk may be any of the allowed sizes and
// depends on several factors:
//     * Allocation will be attempted in reverse order - highest to lowest - in
//       order ensure that the highest possible size is used.
//     * An allocation size will be used if:
//         - the VA region within the block covered by the allocation size is
//           aligned to that allocation size,
//         - the VA block region corresponding to the allocation size is empty
//           (has no previously populated pages), and
//         - the system allows a page allocation of that size.
//
// If mm is not NULL, the chunks memory will be added to the mm's memory cgroup.
//
// If a CPU chunk allocation succeeds, NV_OK is returned. If new_chunk is not
// NULL it will be set to point to the newly allocated chunk. On failure,
// NV_ERR_NO_MEMORY is returned.
NV_STATUS uvm_cpu_chunk_alloc(uvm_va_block_t *va_block,
                              uvm_page_index_t page_index,
                              struct mm_struct *mm,
                              uvm_cpu_chunk_t **new_chunk);

// Insert a CPU chunk in the va_block's storage structures.
//
// On success, NV_OK is returned. On error,
//   - NV_ERR_NO_MEMORY is returned if memory allocation for any if the internal
//     structures did not succeed.
//   - NV_ERR_INVALID_ARGUMENT is returned if the size of the chunk to be inserted
//     in invalid.
//   - NV_ERR_INVALID_STATE is returned if a matching chunk already exists in the
//     block.
NV_STATUS uvm_cpu_chunk_insert_in_block(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Remove a CPU chunk from the va_block's storage structures.
// The chunk is not freed, only removed from the block's storage structures.
void uvm_cpu_chunk_remove_from_block(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Return the CPU chunk backing page_index within the VA block.
// If page_index is beyond the boundary of the VA block or a CPU chunk for
// the specified page has not been allocated and/or inserted into the block,
// NULL is returned.
uvm_cpu_chunk_t *uvm_cpu_chunk_get_chunk_for_page(uvm_va_block_t *block, uvm_page_index_t page_index);

uvm_chunk_size_t uvm_cpu_chunk_get_size(uvm_cpu_chunk_t *chunk);

// Return the number of base system pages covered by the CPU chunk.
static size_t uvm_cpu_chunk_num_pages(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);
    return uvm_cpu_chunk_get_size(chunk) / PAGE_SIZE;
}

static bool uvm_cpu_chunk_is_physical(uvm_cpu_chunk_t *chunk)
{
#if UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE()
    return true;
#else
    return chunk->parent == NULL;
#endif
}

// Return a pointer to the struct page backing page_index within the owning
// VA block.
struct page *uvm_cpu_chunk_get_cpu_page(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Take a reference to the CPU chunk.
void uvm_cpu_chunk_get(uvm_cpu_chunk_t *chunk);

// Release a reference to the CPU chunk. When the reference count
// drops to zero, the CPU chunk will be freed. Physical CPU chunks
// will also free the CPU pages backing the chunk.
void uvm_cpu_chunk_put(uvm_cpu_chunk_t *chunk);

NV_STATUS uvm_cpu_chunk_gpu_mapping_alloc(uvm_va_block_t *va_block, uvm_gpu_id_t id);
void uvm_cpu_chunk_gpu_mapping_split(uvm_va_block_t *existing, uvm_va_block_t *new, uvm_gpu_id_t id);
void uvm_cpu_chunk_gpu_mapping_free(uvm_va_block_t *va_block, uvm_gpu_id_t id);

// Set the CPU chunk's DMA mapping address for the specified GPU ID.
NV_STATUS uvm_cpu_chunk_set_gpu_mapping_addr(uvm_va_block_t *va_block,
                                             uvm_page_index_t page_index,
                                             uvm_cpu_chunk_t *chunk,
                                             uvm_gpu_id_t id,
                                             NvU64 dma_addr);

// Get the CPU chunk's DMA mapping address for the specified GPU ID.
NvU64 uvm_cpu_chunk_get_gpu_mapping_addr(uvm_va_block_t *block,
                                         uvm_page_index_t page_index,
                                         uvm_cpu_chunk_t *chunk,
                                         uvm_gpu_id_t id);

#if !UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE()
// Split a CPU chunk into a set of CPU chunks of size new_size.
// new_size has to be one of the supported CPU chunk allocation sizes and has to
// be smaller than the current size of chunk.
//
// On success, NV_OK is returned. On failure NV_ERR_NO_MEMORY will be returned.
NV_STATUS uvm_cpu_chunk_split(uvm_va_block_t *va_block,
                              uvm_cpu_chunk_t *chunk,
                              uvm_chunk_size_t new_size,
                              uvm_page_index_t page_index,
                              uvm_cpu_chunk_t **new_chunks);

// Merge chunks to merge_size.
//
// All input chunks must have the same parent and size. If not,
// NV_ERR_INVALID_ARGUMENT is returned.
//
// If a merge cannot be done, NV_WARN_NOTHING_TO_DO is returned.
//
// On success, NV_OK is returned and merged_chunk is set to point to the
// merged chunk.
NV_STATUS uvm_cpu_chunk_merge(uvm_va_block_t *va_block,
                              uvm_cpu_chunk_t **chunks,
                              size_t num_merge_chunks,
                              uvm_chunk_size_t merge_size,
                              uvm_cpu_chunk_t **merged_chunk);

// Mark the CPU sub-page page_index in the CPU chunk as dirty.
// page_index has to be a page withing the chunk's region.
void uvm_cpu_chunk_mark_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Mark the CPU sub-pages page_index in the CPU chunk as clean.
// page_index has to be a page withing the chunk's region.
void uvm_cpu_chunk_mark_clean(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Return true if the CPU sub-pages page_index in the CPU chunk are dirty.
// page_index has to be a page withing the chunk's region.
bool uvm_cpu_chunk_is_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

#else // UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE()

static NV_STATUS uvm_cpu_chunk_split(uvm_va_block_t *va_block,
                                     uvm_cpu_chunk_t *chunk,
                                     uvm_chunk_size_t new_size,
                                     uvm_page_index_t page_index,
                                     uvm_cpu_chunk_t **new_chunks)
{
    return NV_OK;
}

static NV_STATUS uvm_cpu_chunk_merge(uvm_va_block_t *va_block,
                                     uvm_cpu_chunk_t **chunk,
                                     size_t num_merge_chunks,
                                     uvm_chunk_size_t merge_size,
                                     uvm_cpu_chunk_t **merged_chunk)
{
    return NV_WARN_NOTHING_TO_DO;
}

static void uvm_cpu_chunk_mark_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    SetPageDirty(chunk);
}

static void uvm_cpu_chunk_mark_clean(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    ClearPageDirty(chunk);
}

static bool uvm_cpu_chunk_is_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    return PageDirty(chunk);
}
#endif // !UVM_CPU_CHUNK_SIZE_IS_PAGE_SIZE()

// Return the first CPU chunk in the block. If no CPU chunks have been
// allocated and/or inserted into the block, NULL is returned.
// If not NULL, page_index will be set to the first page of the block covered by
// the returned chunk.
uvm_cpu_chunk_t *uvm_cpu_chunk_first_in_block(uvm_va_block_t *va_block, uvm_page_index_t *out_page_index);

// Return the next CPU chunk in the block owning chunk.
// previous_page_index is the index after which to start searching. Its value
// will be updated with the starting page index of the next chunk in the block.
uvm_cpu_chunk_t *uvm_cpu_chunk_next(uvm_va_block_t *va_block, uvm_page_index_t *previous_page_index);

#define for_each_cpu_chunk_in_block(chunk, page_index, va_block)                                                     \
    for ((chunk) = uvm_cpu_chunk_first_in_block((va_block), &(page_index));                                          \
         (chunk) != NULL;                                                                                            \
         (page_index) += uvm_cpu_chunk_num_pages(chunk) - 1, (chunk) = uvm_cpu_chunk_next((va_block), &(page_index)))

#define for_each_cpu_chunk_in_block_safe(chunk, page_index, next_page_index, va_block)                  \
    for ((chunk) = uvm_cpu_chunk_first_in_block((va_block), &(page_index)),                             \
             (next_page_index) = (page_index) + ((chunk) ? uvm_cpu_chunk_num_pages(chunk) : 0);         \
         (chunk) != NULL;                                                                               \
         (page_index) = (next_page_index) - 1, (chunk) = uvm_cpu_chunk_next((va_block), &(page_index)), \
             (next_page_index) = (page_index) + ((chunk) ? uvm_cpu_chunk_num_pages(chunk) : 0))

// Use a special symbol for the region so it does not replace the chunk's region
// structure member.
#define for_each_cpu_chunk_in_block_region(chunk, page_index, va_block, __region)                                   \
    for ((page_index) = uvm_va_block_first_page_in_mask((__region), &(va_block)->cpu.allocated),                    \
             (chunk) = uvm_cpu_chunk_get_chunk_for_page((va_block), (page_index));                                  \
         (chunk) != NULL && page_index < (__region).outer;                                                          \
         (page_index) += uvm_cpu_chunk_num_pages(chunk) - 1, (chunk) = uvm_cpu_chunk_next((va_block), &(page_index))

#define for_each_cpu_chunk_in_block_region_safe(chunk, page_index, next_page_index, va_block, __region) \
    for ((page_index) = uvm_va_block_first_page_in_mask((__region), &(va_block)->cpu.allocated),        \
             (chunk) = uvm_cpu_chunk_get_chunk_for_page((va_block), (page_index)),                      \
             (next_page_index) = (page_index) + (chunk ? uvm_cpu_chunk_num_pages(chunk) : 0);           \
         (chunk) != NULL && page_index < (__region).outer;                                              \
         (page_index) = (next_page_index) - 1, (chunk) = uvm_cpu_chunk_next((va_block), &(page_index)), \
             (next_page_index) = (page_index) + (chunk ? uvm_cpu_chunk_num_pages(chunk) : 0))

static NV_STATUS uvm_test_get_cpu_chunk_allocation_sizes(UVM_TEST_GET_CPU_CHUNK_ALLOC_SIZES_PARAMS *params,
                                                                struct file *filp)
{
        params->alloc_size_mask = (NvU32)uvm_cpu_chunk_get_allocation_sizes();
        return NV_OK;
}
#endif
