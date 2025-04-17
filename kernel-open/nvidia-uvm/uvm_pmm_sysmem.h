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

#ifndef __UVM_PMM_SYSMEM_H__
#define __UVM_PMM_SYSMEM_H__

#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_pmm_gpu.h"

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

// If the GPU used to initialize sysmem_mappings supports access counters, the
// entries for the physical region starting at dma_addr are removed from the
// reverse map.
void uvm_pmm_sysmem_mappings_remove_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings, NvU64 dma_addr);

// Like uvm_pmm_sysmem_mappings_remove_gpu_mapping but it doesn't assert if the
// mapping doesn't exist. See uvm_va_block_evict_chunks for more information.
void uvm_pmm_sysmem_mappings_remove_gpu_mapping_on_eviction(uvm_pmm_sysmem_mappings_t *sysmem_mappings, NvU64 dma_addr);

// If the GPU used to initialize sysmem_mappings supports access counters, the
// mapping for the region starting at dma_addr is updated with va_block.
// This is required on VA block split.
void uvm_pmm_sysmem_mappings_reparent_gpu_mapping(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                  NvU64 dma_addr,
                                                  uvm_va_block_t *va_block);

// If the GPU used to initialize sysmem_mappings supports access counters, the
// mapping for the region starting at dma_addr is split into regions of
// new_region_size. new_region_size must be a power of two and smaller than the
// previously-registered size.
NV_STATUS uvm_pmm_sysmem_mappings_split_gpu_mappings(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                     NvU64 dma_addr,
                                                     NvU64 new_region_size);

// If the GPU used to initialize sysmem_mappings supports access counters, all
// the mappings within the region [dma_addr, dma_addr + new_region_size) are
// merged into a single mapping. new_region_size must be a power of two. The
// whole region must be previously populated with mappings and all of them must
// have the same VA block and processor owner.
void uvm_pmm_sysmem_mappings_merge_gpu_mappings(uvm_pmm_sysmem_mappings_t *sysmem_mappings,
                                                NvU64 dma_addr,
                                                NvU64 new_region_size);

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

#define UVM_CPU_CHUNK_SIZES (UVM_PAGE_SIZE_2M | UVM_PAGE_SIZE_64K | PAGE_SIZE)

typedef enum
{
    UVM_CPU_CHUNK_ALLOC_FLAGS_NONE = 0,

    // Zero the chunk.
    UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO = (1 << 0),

    // Account for the chunk in the cgroup context.
    UVM_CPU_CHUNK_ALLOC_FLAGS_ACCOUNT = (1 << 1),

    // Be strict about NUMA locality of the allocation.
    // Attempt the allocation only on the requested NUMA
    // node.
    UVM_CPU_CHUNK_ALLOC_FLAGS_STRICT = (1 << 2),

    // Allow chunk allocations from ZONE_MOVABLE.
    UVM_CPU_CHUNK_ALLOC_FLAGS_ALLOW_MOVABLE = (1 << 3),
} uvm_cpu_chunk_alloc_flags_t;

typedef enum
{
    UVM_CPU_CHUNK_TYPE_PHYSICAL,
    UVM_CPU_CHUNK_TYPE_LOGICAL,
    UVM_CPU_CHUNK_TYPE_HMM
} uvm_cpu_chunk_type_t;

// CPU memory chunk descriptor.
// CPU memory chunks represent a physically contiguous CPU memory
// allocation.
// CPU memory chunks can be created due to CPU page allocation or
// CPU chunk splitting. Chunks created due to page allocations are
// referred to as "physical chunks", while chunks resulting from
// splitting are referred to as "logical chunks".
struct uvm_cpu_chunk_struct
{
    uvm_cpu_chunk_type_t type:2;

    // Size of the chunk.
    // For chunks resulting from page allocations (physical chunks),
    // this value is the size of the physical allocation.
    size_t log2_size : order_base_2(UVM_CHUNK_SIZE_MASK_SIZE);

    // Chunk reference count used when a CPU chunk is split. Each
    // child sub-chunk will increment the reference count of its
    // parent.
    // The reference count is set to 1 when the chunk is created.
    // This initial reference is dropped if the chunk is split in
    // order to automatically destroy the chunk when all logical
    // chunks resulting from the split are destroyed.
    nv_kref_t refcount;

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
};

typedef struct
{
    // Physical GPU DMA address of the CPU chunk.
    NvU64 dma_addr;

    // Reference count of all sub_processors using this mapping across logical
    // and physical chunks.
    NvU32 map_count;

    // Mask of MIG instances or physical GPU.
    // This is only valid for physical CPU chunks that have not been split into
    // logical chunks. When the chunk is split, all the
    // uvm_cpu_logical_chunk_t::mapped_gpus masks have a bit set for each
    // count in map_count and sub_processors is set to zero.
    uvm_sub_processor_mask_t sub_processors;
} uvm_cpu_phys_mapping_t;

typedef struct
{
    uvm_cpu_chunk_t common;

    // Lock protecting dirty_bitmap and gpu_mappings.
    uvm_mutex_t lock;

    struct
    {
        // Per-GPU array of DMA mapping addresses for the chunk.
        // The DMA mapping addresses for logical chunks are adjusted
        // to the correct offset within the parent chunk.
        union
        {
            uvm_cpu_phys_mapping_t static_entry;
            uvm_cpu_phys_mapping_t *dynamic_entries;
        };

        // Maximum number of physical mapping entries available.
        // The initial value is 1 since the static_entry is always
        // available.
        // When using the dynamic_entries, it holds the size of the
        // dynamic_entries array. This may be more than the number
        // of GPUs with active mappings. The number of active entries
        // is the number of set bits in dma_addrs_mask.
        size_t max_entries;

        // The set of GPU parent ID's that have an active physical mapping.
        // Since physical mappings are shared by all GPUs under a
        // parent GPU, this mask only needs to track uvm_parent_gpu_t.
        uvm_parent_processor_mask_t dma_addrs_mask;
    } gpu_mappings;

    // A dynamically allocated bitmap (one per PAGE_SIZE page) used
    // to track dirty state of each PAGE_SIZE page.
    // Large CPU chunks are allocated as compound pages. For such
    // pages, the kernel keeps dirtiness state with a single bit
    // (in the compound page head) that covers the entire compound
    // page.
    //
    // In the case of UVM-Lite GPUs, using the dirty bit of the
    // the compound page will cause performance regression due to
    // the copying of extra data. We mitigate this by using this
    // bitmap to track which base pages are dirty.
    unsigned long *dirty_bitmap;

} uvm_cpu_physical_chunk_t;

typedef struct
{
    uvm_cpu_chunk_t common;

    // Pointer to the parent chunk (which could also be a logical chunk).
    uvm_cpu_chunk_t *parent;

    // This is a reference per bit but also recorded in mapping->map_count.
    uvm_processor_mask_t mapped_gpus;
} uvm_cpu_logical_chunk_t;

// Return the set of allowed CPU chunk allocation sizes.
uvm_chunk_sizes_mask_t uvm_cpu_chunk_get_allocation_sizes(void);

// Allocate a physical CPU chunk of the specified size.
//
// The nid argument is used to indicate a memory node preference. If the
// value is a memory node ID, the chunk allocation will be attempted on
// that memory node. If the chunk cannot be allocated on that memory node,
// it will be allocated on any memory node allowed by the process's policy.
//
// If the value of nid is a memory node ID that is not in the set of
// current process's allowed memory nodes, it will be allocated on one of the
// nodes in the allowed set.
//
// If the value of nid is NUMA_NO_NODE, the chunk will be allocated from any
// of the allowed memory nodes by the process policy.
//
// If a CPU chunk allocation succeeds, NV_OK is returned. new_chunk will be set
// to point to the newly allocated chunk. On failure, NV_ERR_NO_MEMORY is
// returned.
NV_STATUS uvm_cpu_chunk_alloc(uvm_chunk_size_t alloc_size,
                              uvm_cpu_chunk_alloc_flags_t flags,
                              int nid,
                              uvm_cpu_chunk_t **new_chunk);

// Allocate a HMM CPU chunk.
//
// HMM chunks differ from normal CPU chunks in that the kernel has already
// allocated the page for them. This means we don't allocate any CPU memory
// here. It also means the kernel holds the reference to the page, so we
// shouldn't call put_page() when freeing the chunk.
//
// If a CPU chunk allocation succeeds NV_OK is returned and new_chunk will be
// set to point to the newly allocated chunk. On failure, NV_ERR_NO_MEMORY is
// returned.
//
// Note that the kernel retains logical ownership of the page. This means page
// properties should not be directly modified by UVM. In particular page flags
// such as PageDirty should not be modified by UVM, nor can UVM directly free
// the page. The kernel is also responsible for mapping/unmapping the page on
// the CPU. We create a CPU chunk for the page primarily to allow GPU mappings
// for the page to be created.
NV_STATUS uvm_cpu_chunk_alloc_hmm(struct page *page,
                                  uvm_cpu_chunk_t **new_chunk);

// Convert a physical chunk to an HMM chunk.
static void uvm_cpu_chunk_make_hmm(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk->type == UVM_CPU_CHUNK_TYPE_PHYSICAL);

    chunk->type = UVM_CPU_CHUNK_TYPE_HMM;
}

uvm_chunk_size_t uvm_cpu_chunk_get_size(uvm_cpu_chunk_t *chunk);

// Return the number of base system pages covered by the CPU chunk.
static size_t uvm_cpu_chunk_num_pages(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(chunk);
    return uvm_cpu_chunk_get_size(chunk) / PAGE_SIZE;
}

static inline bool uvm_cpu_chunk_is_hmm(uvm_cpu_chunk_t *chunk)
{
    return chunk->type == UVM_CPU_CHUNK_TYPE_HMM;
}

static bool uvm_cpu_chunk_is_physical(uvm_cpu_chunk_t *chunk)
{
    return (chunk->type == UVM_CPU_CHUNK_TYPE_PHYSICAL || uvm_cpu_chunk_is_hmm(chunk));
}

static bool uvm_cpu_chunk_is_logical(uvm_cpu_chunk_t *chunk)
{
    return chunk->type == UVM_CPU_CHUNK_TYPE_LOGICAL;
}

static uvm_cpu_physical_chunk_t *uvm_cpu_chunk_to_physical(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(uvm_cpu_chunk_is_physical(chunk));
    return container_of((chunk), uvm_cpu_physical_chunk_t, common);
}

static uvm_cpu_logical_chunk_t *uvm_cpu_chunk_to_logical(uvm_cpu_chunk_t *chunk)
{
    UVM_ASSERT(uvm_cpu_chunk_is_logical(chunk));
    return container_of((chunk), uvm_cpu_logical_chunk_t, common);
}

// Return the NUMA node ID of the physical page backing the chunk.
int uvm_cpu_chunk_get_numa_node(uvm_cpu_chunk_t *chunk);

// Free a CPU chunk.
// This may not result in the immediate freeing of the physical pages of the
// chunk if this is a logical chunk and there are other logical chunks holding
// references to the physical chunk.
// If any DMA mappings to this chunk are still active, they are implicitly
// destroyed.
void uvm_cpu_chunk_free(uvm_cpu_chunk_t *chunk);

// In some configurations such as SR-IOV heavy, a CPU chunk cannot be
// referenced using its physical address. There needs to be a kernel virtual
// mapping created.
//
// This helper function creates a DMA mapping on the GPU and if necessary,
// a kernel virtual mapping for the chunk. The virtual mapping persists until
// GPU deinitialization, such that no unmap functionality is exposed.
// For more details see uvm_mmu_sysmem_map().
NV_STATUS uvm_cpu_chunk_map_gpu(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu);

// Destroy a CPU chunk's DMA mapping for the given GPU.
// If chunk is a logical chunk, this call may not necessarily destroy the DMA
// mapping of the parent physical chunk since all logical chunks and MIG
// partitions share the parent's DMA mapping.
void uvm_cpu_chunk_unmap_gpu(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu);

// Get the CPU chunk's DMA mapping address for the specified GPU ID.
// If there is no mapping for the GPU, 0 is returned.
NvU64 uvm_cpu_chunk_get_gpu_phys_addr(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu);

// Split a CPU chunk into a set of CPU chunks of the next size down from the set
// of enabled CPU chunk sizes.
//
// This function expects that the chunk to be split is larger than the minimum
// enabled chunk size and that new_chunks has enough space for all chunks
// resulting from the split.
//
// On success, NV_OK is returned and the caller-provided new_chunks array will
// be filled out with the newly-created logical chunks.
//
// After a successfull split, the input chunk can no longer be used.
//
// On failure NV_ERR_NO_MEMORY will be returned.
//
// Should never be called for HMM chunks as these don't need splitting (they can
// only be PAGE_SIZE) and even if larger chunks could exist UVM could not split
// them without kernel interaction which currently isn't exported. Will return
// NV_ERR_INVALID_ARGUMENT for a HMM chunk.
// TODO: Bug 3368756: add support for transparent huge page (THP)
NV_STATUS uvm_cpu_chunk_split(uvm_cpu_chunk_t *chunk, uvm_cpu_chunk_t **new_chunks);

// Merge an array of logical chunks into their parent chunk. All chunks have to
// have the same size, parent, and set of mapped GPUs.
uvm_cpu_chunk_t *uvm_cpu_chunk_merge(uvm_cpu_chunk_t **chunks);

// Mark the page_index sub-page of the chunk as dirty.
// page_index is an offset into the chunk.
//
// Note that dirty status for HMM chunks should not be modified directly from
// UVM. Instead the kernel will mark the backing struct pages dirty either on
// fault when written to from the CPU, or when the PTE is mirrored to the GPU
// using hmm_range_fault().
void uvm_cpu_chunk_mark_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Mark the page_index sub-page of the chunk as clean.
// page_index is an offset into the chunk.
void uvm_cpu_chunk_mark_clean(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Return true if the page_index base page of the CPU chunk is dirty.
bool uvm_cpu_chunk_is_dirty(uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

static NV_STATUS uvm_test_get_cpu_chunk_allocation_sizes(UVM_TEST_GET_CPU_CHUNK_ALLOC_SIZES_PARAMS *params,
                                                                struct file *filp)
{
        params->alloc_size_mask = (NvU32)uvm_cpu_chunk_get_allocation_sizes();
        return NV_OK;
}
#endif
