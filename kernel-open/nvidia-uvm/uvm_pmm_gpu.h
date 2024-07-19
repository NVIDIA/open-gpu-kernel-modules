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

#ifndef __UVM_PMM_GPU_H__
#define __UVM_PMM_GPU_H__

//
// The Physical Memory Manager (PMM) manages the life cycle of GPU physical
// memory.
//
// The memory is managed in GPU chunks of different sizes (uvm_chunk_size_t) and
// users of PMM need to explicitly register the chunk sizes they need to be
// supported (see chunk_size_init_func in uvm_pmm_gpu_init()).
//
// Two memory types (uvm_pmm_gpu_memory_type_t) are supported, one for user and
// one for kernel allocations. The user memory type is used only for backing
// user data managed by VA blocks and kernel memory type is used for everything
// else. The distinction exists to support oversubscription, which requires the
// ability to evict already allocated memory from its users on-demand to satisfy
// new memory allocations when no more unused memory is available. Eviction is
// limited to the user memory type as it's a very complex operation requiring
// integration between PMM and other UVM driver modules. The assumption is that
// the vast majority of memory should be used for user data as everything else
// can be considered overhead and should be minimized. Two flavors of
// oversubscription exist: internal oversubscription allowing PMM allocations to
// evict other PMM allocations and external oversubscription allowing other PMA
// clients to evict memory used by PMM.
//
// Both allocation and freeing of memory support asynchronous operations where
// the allocated/freed GPU memory chunks can have pending GPU operations
// returned when allocating memory and passed in when freeing it via trackers.
//

#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_processors.h"
#include "uvm_tracker.h"
#include "uvm_va_block_types.h"
#include "uvm_linux.h"
#include "uvm_types.h"
#include "nv_uvm_types.h"
#if UVM_IS_CONFIG_HMM()
#include <linux/memremap.h>
#endif

typedef enum
{
    UVM_CHUNK_SIZE_1       =           1,
    UVM_CHUNK_SIZE_2       =           2,
    UVM_CHUNK_SIZE_4       =           4,
    UVM_CHUNK_SIZE_8       =           8,
    UVM_CHUNK_SIZE_16      =          16,
    UVM_CHUNK_SIZE_32      =          32,
    UVM_CHUNK_SIZE_64      =          64,
    UVM_CHUNK_SIZE_128     =         128,
    UVM_CHUNK_SIZE_256     =         256,
    UVM_CHUNK_SIZE_512     =         512,
    UVM_CHUNK_SIZE_1K      =        1024,
    UVM_CHUNK_SIZE_2K      =      2*1024,
    UVM_CHUNK_SIZE_4K      =      4*1024,
    UVM_CHUNK_SIZE_8K      =      8*1024,
    UVM_CHUNK_SIZE_16K     =     16*1024,
    UVM_CHUNK_SIZE_32K     =     32*1024,
    UVM_CHUNK_SIZE_64K     =     64*1024,
    UVM_CHUNK_SIZE_128K    =    128*1024,
    UVM_CHUNK_SIZE_256K    =    256*1024,
    UVM_CHUNK_SIZE_512K    =    512*1024,
    UVM_CHUNK_SIZE_1M      =   1024*1024,
    UVM_CHUNK_SIZE_2M      = 2*1024*1024,
    UVM_CHUNK_SIZE_MAX     = UVM_CHUNK_SIZE_2M,
    UVM_CHUNK_SIZE_INVALID = UVM_CHUNK_SIZE_MAX * 2
} uvm_chunk_size_t;

#define UVM_CHUNK_SIZES_MASK     (uvm_chunk_sizes_mask_t)(UVM_CHUNK_SIZE_MAX | (UVM_CHUNK_SIZE_MAX-1))

typedef enum
{
    // Memory type for backing user pages. On Pascal+ it can be evicted.
    UVM_PMM_GPU_MEMORY_TYPE_USER,
    // When the Confidential Computing feature is enabled, the protected flavor
    // allocates memory out of the VPR region. When it's disabled, all flavors
    // have no effects and are equivalent to the base type.
    UVM_PMM_GPU_MEMORY_TYPE_USER_PROTECTED = UVM_PMM_GPU_MEMORY_TYPE_USER,
    UVM_PMM_GPU_MEMORY_TYPE_USER_UNPROTECTED,

    // Memory type for internal UVM allocations. It cannot be evicted.
    UVM_PMM_GPU_MEMORY_TYPE_KERNEL,
    // See user types for the behavior description when the Confidential
    // Computing feature is ON or OFF.
    UVM_PMM_GPU_MEMORY_TYPE_KERNEL_PROTECTED = UVM_PMM_GPU_MEMORY_TYPE_KERNEL,
    UVM_PMM_GPU_MEMORY_TYPE_KERNEL_UNPROTECTED,

    // Number of types - MUST BE LAST.
    UVM_PMM_GPU_MEMORY_TYPE_COUNT
} uvm_pmm_gpu_memory_type_t;

const char *uvm_pmm_gpu_memory_type_string(uvm_pmm_gpu_memory_type_t type);

// Returns true if the given memory type is used to back user pages.
bool uvm_pmm_gpu_memory_type_is_user(uvm_pmm_gpu_memory_type_t type);

// Returns true if the given memory type is used to back internal UVM
// allocations.
static bool uvm_pmm_gpu_memory_type_is_kernel(uvm_pmm_gpu_memory_type_t type)
{
    return !uvm_pmm_gpu_memory_type_is_user(type);
}

typedef enum
{
    // Chunk belongs to PMA. Code outside PMM should not have access to
    // it and it is likely a bug in UVM code (either in PMM or outside)
    // if that happens.
    UVM_PMM_GPU_CHUNK_STATE_PMA_OWNED,

    // Chunk is on free list. That is it can be reused or returned to PMA
    // as soon as its tracker is done. Code outside PMM should not have
    // access to this chunk and it is likely a bug in UVM code (either in
    // PMM or outside) if that happens.
    UVM_PMM_GPU_CHUNK_STATE_FREE,

    // Chunk is split into subchunks.
    UVM_PMM_GPU_CHUNK_STATE_IS_SPLIT,

    // Chunk is temporarily pinned.
    //
    // This state is used for user memory chunks that have been allocated, but haven't
    // been unpinned yet and also internally when a chunk is about to be split.
    UVM_PMM_GPU_CHUNK_STATE_TEMP_PINNED,

    // Chunk is allocated. That is it is backing some VA block
    UVM_PMM_GPU_CHUNK_STATE_ALLOCATED,

    // Number of states - MUST BE LAST
    UVM_PMM_GPU_CHUNK_STATE_COUNT
} uvm_pmm_gpu_chunk_state_t;

const char *uvm_pmm_gpu_chunk_state_string(uvm_pmm_gpu_chunk_state_t state);

typedef enum
{
    // No flags passed
    UVM_PMM_ALLOC_FLAGS_NONE,

    // If there is no free memory, allocation may evict chunks instead of
    // returning error immediately. Therefore it must not be called under the
    // VA block lock.
    UVM_PMM_ALLOC_FLAGS_EVICT = (1 << 0),

    // Do not use batching in this call if PMA page allocaion is required
    UVM_PMM_ALLOC_FLAGS_DONT_BATCH = (1 << 1),

    UVM_PMM_ALLOC_FLAGS_MASK = (1 << 2) - 1
} uvm_pmm_alloc_flags_t;


typedef enum
{
    // Identifier for lists with zeroed chunks
    UVM_PMM_LIST_ZERO,

    // Identifier for lists with non-zeroed chunks
    UVM_PMM_LIST_NO_ZERO,

    // Number of states for zeroed/non-zeroed chunk lists - MUST BE LAST
    UVM_PMM_LIST_ZERO_COUNT
} uvm_pmm_list_zero_t;

static void uvm_pmm_list_zero_checks(void)
{
    BUILD_BUG_ON(UVM_PMM_LIST_ZERO_COUNT > 2);
}

// Maximum chunk sizes per type of allocation in single GPU.
// The worst case today is Maxwell with 4 allocations sizes for page tables and
// 2 page sizes used by uvm_mem_t. Notably one of the allocations for page
// tables is 2M which is our common root chunk size.
#define UVM_MAX_CHUNK_SIZES 6

// This specifies a maximum GAP between 2 allocation levels.
#define UVM_PMM_MAX_SUBCHUNKS UVM_CHUNK_SIZE_MAX

#define UVM_PMM_CHUNK_SPLIT_CACHE_SIZES (ilog2(UVM_PMM_MAX_SUBCHUNKS) + 1)
#define UVM_CHUNK_SIZE_MASK_SIZE (ilog2(UVM_CHUNK_SIZE_MAX) + 1)

typedef uvm_chunk_size_t uvm_chunk_sizes_mask_t;

typedef struct uvm_pmm_gpu_chunk_suballoc_struct uvm_pmm_gpu_chunk_suballoc_t;

#if UVM_IS_CONFIG_HMM()

typedef struct uvm_pmm_gpu_struct uvm_pmm_gpu_t;

typedef struct
{
    struct dev_pagemap pagemap;
} uvm_pmm_gpu_devmem_t;

// Return the GPU chunk for a given device private struct page.
uvm_gpu_chunk_t *uvm_pmm_devmem_page_to_chunk(struct page *page);

// Return the GPU id for a given device private struct page.
uvm_gpu_id_t uvm_pmm_devmem_page_to_gpu_id(struct page *page);

// Return the PFN of the device private struct page for the given GPU chunk.
unsigned long uvm_pmm_gpu_devmem_get_pfn(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk);

#endif

struct uvm_gpu_chunk_struct
{
    // Physical address of GPU chunk. This may be removed to save memory
    // if we will be able to get it from reverse map and changed
    // into smaller index for subchunks.
    NvU64 address;

    struct
    {
        // We use +1 in the order_base_2 calls appropriately to avoid compiler
        // warnings due to the bitfields being too narrow for the values of
        // their types.
        uvm_pmm_gpu_memory_type_t type : order_base_2(UVM_PMM_GPU_MEMORY_TYPE_COUNT + 1);

        // The eviction flag is internal and used only for root chunks. It's
        // set by the eviction path once a chunk is chosen for eviction in
        // chunk_start_eviction(). Also see the (root_)chunk_is_in_eviction()
        // helpers.
        bool in_eviction : 1;

        bool inject_split_error : 1;

        // This flag is initalized when allocating a new root chunk from PMA.
        // It is set to true, if PMA already scrubbed the chunk. The flag is
        // only valid at allocation time (after uvm_pmm_gpu_alloc call), and
        // the caller is not required to clear it before freeing the chunk. The
        // VA block chunk population code can query it to skip zeroing the
        // chunk.
        bool is_zero : 1;

        // This flag indicates an allocated chunk is referenced by a device
        // private struct page PTE and therefore expects a page_free() callback.
        bool is_referenced : 1;

        uvm_pmm_gpu_chunk_state_t state : order_base_2(UVM_PMM_GPU_CHUNK_STATE_COUNT + 1);

        size_t log2_size : order_base_2(UVM_CHUNK_SIZE_MASK_SIZE);

        // Start page index within va_block
        uvm_page_index_t va_block_page_index : order_base_2(PAGES_PER_UVM_VA_BLOCK + 1);

        // This allows determining what PMM owns the chunk. Users of this field
        // must only use it if the owning GPU is retained.
        // TODO: Bug 2008200: Enforce single PMM instance per GPU
        NvU32 gpu_index : order_base_2(UVM_ID_MAX_PROCESSORS);
    };

    // List entry.
    //
    // Guaranteed to be a valid list node at all times for simplicity.
    //
    // Protected by PMM's list_lock when managed by PMM. Notably the list node
    // can be used by the allocator of the chunk after alloc and before the
    // chunk is unpinned or freed.
    struct list_head list;

    // The VA block using the chunk, if any.
    // User chunks that are not backed by a VA block are considered to be
    // temporarily pinned and cannot be evicted.
    uvm_va_block_t *va_block;

    // If this is subchunk it points to the parent - in other words
    // chunk of bigger size which contains this chunk.
    uvm_gpu_chunk_t *parent;

    // Array describing suballocations
    uvm_pmm_gpu_chunk_suballoc_t *suballoc;
};

typedef struct uvm_gpu_root_chunk_struct
{
    uvm_gpu_chunk_t chunk;

    // Pending operations for all GPU chunks under the root chunk.
    //
    // Protected by the corresponding root chunk bit lock.
    uvm_tracker_t tracker;
} uvm_gpu_root_chunk_t;

typedef struct uvm_pmm_gpu_struct
{
    // Sizes of the MMU
    uvm_chunk_sizes_mask_t chunk_sizes[UVM_PMM_GPU_MEMORY_TYPE_COUNT];

    // PMA (Physical Memory Allocator) opaque handle
    void *pma;

    // PMA statistics used for eviction heuristics
    const UvmPmaStatistics *pma_stats;

    struct
    {
        // Array of all root chunks indexed by their physical address divided by
        // UVM_CHUNK_SIZE_MAX.
        //
        // This array is pre-allocated during uvm_pmm_gpu_init() for all
        // possible physical addresses (based on
        // gpu::vidmem_max_physical_address).
        size_t count;
        uvm_gpu_root_chunk_t *array;

        // Bit locks for the root chunks with 1 bit per each root chunk
        uvm_bit_locks_t bitlocks;

        // List of root chunks unused by VA blocks, i.e. allocated, but not
        // holding any resident pages. These take priority when evicting as no
        // data needs to be migrated for them to be evicted.
        //
        // For simplicity, the list is approximate, tracking unused chunks only
        // from root chunk sized (2M) VA blocks.
        //
        // Updated by the VA block code with
        // uvm_pmm_gpu_mark_root_chunk_(un)used().
        struct list_head va_block_unused;

        // List of root chunks used by VA blocks
        struct list_head va_block_used;

        // List of chunks needing to be lazily freed and a queue for processing
        // the list. TODO: Bug 3881835: revisit whether to use nv_kthread_q_t
        // or workqueue.
        struct list_head va_block_lazy_free;
        nv_kthread_q_item_t va_block_lazy_free_q_item;
    } root_chunks;

#if UVM_IS_CONFIG_HMM()
    uvm_pmm_gpu_devmem_t devmem;
#endif

    // Lock protecting PMA allocation, freeing and eviction
    uvm_rw_semaphore_t pma_lock;

    // Lock protecting splits, merges and walks of chunks.
    uvm_mutex_t lock;

    // Lock protecting lists and chunk's state transitions.
    uvm_spinlock_t list_lock;

    // Free chunk lists. There are separate lists for non-zero and zero chunks.
    struct list_head free_list[UVM_PMM_GPU_MEMORY_TYPE_COUNT][UVM_MAX_CHUNK_SIZES][UVM_PMM_LIST_ZERO_COUNT];

    // Inject an error after evicting a number of chunks. 0 means no error left
    // to be injected.
    NvU32 inject_pma_evict_error_after_num_chunks;

    // The mask of the initialized chunk sizes
    DECLARE_BITMAP(chunk_split_cache_initialized, UVM_PMM_CHUNK_SPLIT_CACHE_SIZES);

    bool initialized;

    bool pma_address_cache_initialized;
} uvm_pmm_gpu_t;

// Return containing GPU
uvm_gpu_t *uvm_pmm_to_gpu(uvm_pmm_gpu_t *pmm);

// Initialize PMM on GPU
NV_STATUS uvm_pmm_gpu_init(uvm_pmm_gpu_t *pmm);

// Deinitialize the PMM on GPU
void uvm_pmm_gpu_deinit(uvm_pmm_gpu_t *pmm);

static uvm_chunk_size_t uvm_gpu_chunk_get_size(uvm_gpu_chunk_t *chunk)
{
    return ((uvm_chunk_size_t)1) << chunk->log2_size;
}

static void uvm_gpu_chunk_set_size(uvm_gpu_chunk_t *chunk, uvm_chunk_size_t size)
{
    chunk->log2_size = ilog2(size);
}

// Retrieve the GPU associated with the chunk. Users of this helper must only
// use it if the owning GPU is retained.
uvm_gpu_t *uvm_gpu_chunk_get_gpu(const uvm_gpu_chunk_t *chunk);

// Return the first struct page corresponding to the physical address range
// of the given chunk.
//
// Notes:
// - The GPU must have NUMA support enabled.
// - For chunks smaller than a system page, this function returns the struct
// page containing the chunk's starting address.
struct page *uvm_gpu_chunk_to_page(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk);

// Allocates num_chunks chunks of size chunk_size in caller-supplied array (chunks).
//
// Returned chunks are in the TEMP_PINNED state, requiring a call to either
// uvm_pmm_gpu_unpin_allocated, uvm_pmm_gpu_unpin_referenced, or
// uvm_pmm_gpu_free. If a tracker is passed in, all
// the pending operations on the allocated chunks will be added to it
// guaranteeing that all the entries come from the same GPU as the PMM.
// Otherwise, when tracker is NULL, all the pending operations will be
// synchronized before returning to the caller.
//
// Each of the allocated chunks list nodes (uvm_gpu_chunk_t::list) can be used
// by the caller until the chunk is unpinned (uvm_pmm_gpu_unpin_allocated,
// uvm_pmm_gpu_unpin_referenced) or freed (uvm_pmm_gpu_free). If used, the list
// node has to be returned to a valid state before calling either of the APIs.
//
// In case of an error, the chunks array is guaranteed to be cleared.
//
// If the memory returned by the PMM allocator cannot be physically addressed,
// the MMU interface provides user chunk mapping and unmapping functions
// (uvm_mmu_chunk_map/unmap) that enable virtual addressing.
NV_STATUS uvm_pmm_gpu_alloc(uvm_pmm_gpu_t *pmm,
                            size_t num_chunks,
                            uvm_chunk_size_t chunk_size,
                            uvm_pmm_gpu_memory_type_t mem_type,
                            uvm_pmm_alloc_flags_t flags,
                            uvm_gpu_chunk_t **chunks,
                            uvm_tracker_t *out_tracker);

// Helper for allocating kernel memory
//
// Internally calls uvm_pmm_gpu_alloc() and sets the state of all chunks to
// allocated on success.
//
// If Confidential Computing is enabled, this helper allocates protected kernel
// memory.
static NV_STATUS uvm_pmm_gpu_alloc_kernel(uvm_pmm_gpu_t *pmm,
                                          size_t num_chunks,
                                          uvm_chunk_size_t chunk_size,
                                          uvm_pmm_alloc_flags_t flags,
                                          uvm_gpu_chunk_t **chunks,
                                          uvm_tracker_t *out_tracker)
{
    return uvm_pmm_gpu_alloc(pmm, num_chunks, chunk_size, UVM_PMM_GPU_MEMORY_TYPE_KERNEL, flags, chunks, out_tracker);
}

// Helper for allocating user memory
//
// Simple wrapper that just uses UVM_PMM_GPU_MEMORY_TYPE_USER for the memory
// type.
//
// If Confidential Computing is enabled, this helper allocates protected user
// memory.
static NV_STATUS uvm_pmm_gpu_alloc_user(uvm_pmm_gpu_t *pmm,
                                        size_t num_chunks,
                                        uvm_chunk_size_t chunk_size,
                                        uvm_pmm_alloc_flags_t flags,
                                        uvm_gpu_chunk_t **chunks,
                                        uvm_tracker_t *out_tracker)
{
    return uvm_pmm_gpu_alloc(pmm, num_chunks, chunk_size, UVM_PMM_GPU_MEMORY_TYPE_USER, flags, chunks, out_tracker);
}

// Unpin a temporarily pinned chunk, set its reverse map to a VA block, and
// mark it as allocated.
//
// Can only be used on user memory.
void uvm_pmm_gpu_unpin_allocated(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk, uvm_va_block_t *va_block);

// Unpin a temporarily pinned chunk, set its reverse map to a VA block, and
// mark it as referenced.
//
// Can only be used on user memory.
void uvm_pmm_gpu_unpin_referenced(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk, uvm_va_block_t *va_block);

// Frees the chunk. This also unpins the chunk if it is temporarily pinned.
//
// The tracker is optional and a NULL tracker indicates that no new operation
// has been pushed for the chunk, but the tracker returned as part of
// its allocation doesn't have to be completed as PMM will synchronize it
// internally if needed. A non-NULL tracker indicates any additional pending
// operations on the chunk pushed by the caller that need to be synchronized
// before freeing or re-using the chunk.
void uvm_pmm_gpu_free(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk, uvm_tracker_t *tracker);

// Splits the input chunk in-place into smaller chunks of subchunk_size. No data
// is moved, and the smaller chunks remain allocated.
//
// If the subchunks array is non-NULL, it will be filled with
// (uvm_gpu_chunk_get_size(chunk) / subchunk_size) chunks in address order. The
// new chunks must all be freed individually.
//
// If the subchunks array is NULL, the split chunks can be retrieved later by
// passing the original parent chunk to uvm_pmm_gpu_get_subchunks.
//
// On error, the original chunk remains unmodified.
//
// The chunk must be in the ALLOCATED state with the owning VA block lock held,
// or the TEMP_PINNED state.
//
// subchunk_size must be a valid chunk size for the given type.
//
// The chunk can be re-merged if desired using uvm_pmm_gpu_merge_chunk.
NV_STATUS uvm_pmm_gpu_split_chunk(uvm_pmm_gpu_t *pmm,
                                  uvm_gpu_chunk_t *chunk,
                                  uvm_chunk_size_t subchunk_size,
                                  uvm_gpu_chunk_t **subchunks);

// Retrieve leaf subchunks under parent. Up to num_subchunks chunks are copied
// into the subchunks array in address order, starting with the subchunk at
// start_index. start_index can be thought of as the number of leaf subchunks to
// skip before beginning the copy.
//
// parent can be in the ALLOCATED state, in which case parent is the only chunk
// which may be copied into the subchunks array.
//
// num_subchunks may be 0.
//
// Returns the number of subchunks written to the array. This may be less than
// num_subchunks depending on the value of start_index and how many subchunks
// are present under parent.
size_t uvm_pmm_gpu_get_subchunks(uvm_pmm_gpu_t *pmm,
                                 uvm_gpu_chunk_t *parent,
                                 size_t start_index,
                                 size_t num_subchunks,
                                 uvm_gpu_chunk_t **subchunks);

// Merges a chunk previously split with uvm_pmm_gpu_split_chunk. All of chunk's
// leaf children must be allocated.
void uvm_pmm_gpu_merge_chunk(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk);

// Waits for all free chunk trackers (removing their completed entries) to complete.
//
// This inherently races with any chunks being freed to this PMM. The assumption
// is that the caller doesn't care about preventing new chunks from being freed,
// just that any already-freed chunks will be synced.
void uvm_pmm_gpu_sync(uvm_pmm_gpu_t *pmm);

// Mark an allocated chunk as evicted
void uvm_pmm_gpu_mark_chunk_evicted(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk);

// Returns the physical address for use by accessing_gpu of a vidmem allocation
// on the peer pmm->gpu. This address can be used for making PTEs on
// accessing_gpu, but not for copying between the two GPUs. For that, use
// uvm_gpu_peer_copy_address.
uvm_gpu_phys_address_t uvm_pmm_gpu_peer_phys_address(uvm_pmm_gpu_t *pmm,
                                                     uvm_gpu_chunk_t *chunk,
                                                     uvm_gpu_t *accessing_gpu);

// Returns the physical or virtual address for use by accessing_gpu to copy to/
// from a vidmem allocation on the peer pmm->gpu. This may be different from
// uvm_gpu_peer_phys_address to handle CE limitations in addressing peer
// physical memory directly.
uvm_gpu_address_t uvm_pmm_gpu_peer_copy_address(uvm_pmm_gpu_t *pmm,
                                                uvm_gpu_chunk_t *chunk,
                                                uvm_gpu_t *accessing_gpu);

// Mark a user chunk as used
//
// If the chunk is pinned or selected for eviction, this won't do anything. The
// chunk can be pinned when it's being initially populated by the VA block.
// Allow that state to make this API easy to use for the caller.
void uvm_pmm_gpu_mark_root_chunk_used(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk);

// Mark an allocated user chunk as unused
void uvm_pmm_gpu_mark_root_chunk_unused(uvm_pmm_gpu_t *pmm, uvm_gpu_chunk_t *chunk);

static bool uvm_gpu_chunk_same_root(uvm_gpu_chunk_t *chunk1, uvm_gpu_chunk_t *chunk2)
{
    return UVM_ALIGN_DOWN(chunk1->address, UVM_CHUNK_SIZE_MAX) == UVM_ALIGN_DOWN(chunk2->address, UVM_CHUNK_SIZE_MAX);
}

// Finds the first (smallest) size in the chunk_sizes mask
static uvm_chunk_size_t uvm_chunk_find_first_size(uvm_chunk_sizes_mask_t chunk_sizes)
{
    UVM_ASSERT(chunk_sizes);
    return (uvm_chunk_size_t)1 << __ffs(chunk_sizes);
}

// Finds the last (biggest) size in the chunk_sizes mask
static uvm_chunk_size_t uvm_chunk_find_last_size(uvm_chunk_sizes_mask_t chunk_sizes)
{
    UVM_ASSERT(chunk_sizes);
    return (uvm_chunk_size_t)1 << __fls(chunk_sizes);
}

// Finds the smallest size in the chunk_sizes mask which is larger than
// chunk_size. If there is no such value returns UVM_CHUNK_SIZE_INVALID.
static uvm_chunk_size_t uvm_chunk_find_next_size(uvm_chunk_sizes_mask_t chunk_sizes, uvm_chunk_size_t chunk_size)
{
    UVM_ASSERT(is_power_of_2(chunk_size));
    UVM_ASSERT(chunk_sizes & chunk_size);
    BUILD_BUG_ON(sizeof(chunk_sizes) > sizeof(unsigned long));
    return (uvm_chunk_size_t)1 << __ffs((chunk_sizes & ~((chunk_size << 1) - 1)) | UVM_CHUNK_SIZE_INVALID);
}

// Finds the largest size in the chunk_sizes mask which is smaller than
// chunk_size. If there is no such value returns UVM_CHUNK_SIZE_INVALID.
static uvm_chunk_size_t uvm_chunk_find_prev_size(uvm_chunk_sizes_mask_t chunk_sizes, uvm_chunk_size_t chunk_size)
{
    UVM_ASSERT(is_power_of_2(chunk_size));
    UVM_ASSERT(chunk_sizes & chunk_size);
    chunk_sizes = chunk_sizes & (chunk_size - 1);
    if (!chunk_sizes)
        return UVM_CHUNK_SIZE_INVALID;
    return (uvm_chunk_size_t)1 << __fls(chunk_sizes);
}

// Obtain the {va_block, virt_addr} information for the chunks in the given
// [phys_addr:phys_addr + region_size) range. One entry per chunk is returned.
// phys_addr and region_size must be page-aligned.
//
// Valid translations are written to out_mappings sequentially (there are no
// gaps). The caller is required to provide enough entries in out_pages for the
// whole region. The function returns the number of entries written to
// out_mappings.
//
// The returned reverse map is a snapshot: it is stale as soon as it is
// returned, and the caller is responsible for locking the VA block(s) and
// checking that the chunks are still there. Also, the VA block(s) are
// retained, and it's up to the caller to release them.
NvU32 uvm_pmm_gpu_phys_to_virt(uvm_pmm_gpu_t *pmm, NvU64 phys_addr, NvU64 region_size, uvm_reverse_map_t *out_mappings);

// Iterates over every size in the input mask from smallest to largest
#define for_each_chunk_size(__size, __chunk_sizes)                                  \
    for ((__size) = (__chunk_sizes) ? uvm_chunk_find_first_size(__chunk_sizes) :    \
                                      UVM_CHUNK_SIZE_INVALID;                       \
         (__size) != UVM_CHUNK_SIZE_INVALID;                                        \
         (__size) = uvm_chunk_find_next_size((__chunk_sizes), (__size)))

// Iterates over every size in the input mask from largest to smallest
#define for_each_chunk_size_rev(__size, __chunk_sizes)                          \
    for ((__size) = (__chunk_sizes) ? uvm_chunk_find_last_size(__chunk_sizes) : \
                                      UVM_CHUNK_SIZE_INVALID;                   \
         (__size) != UVM_CHUNK_SIZE_INVALID;                                    \
         (__size) = uvm_chunk_find_prev_size((__chunk_sizes), (__size)))

// Iterates over every size in the input mask from smallest to largest, starting
// from and including __size. __size must be present in the mask.
#define for_each_chunk_size_from(__size, __chunk_sizes)                 \
    for (; (__size) != UVM_CHUNK_SIZE_INVALID;                          \
         (__size) = uvm_chunk_find_next_size((__chunk_sizes), (__size)))

// Iterates over every size in the input mask from largest to smallest, starting
// from and including __size. __size must be present in the mask.
#define for_each_chunk_size_rev_from(__size, __chunk_sizes)             \
    for (; (__size) != UVM_CHUNK_SIZE_INVALID;                          \
         (__size) = uvm_chunk_find_prev_size((__chunk_sizes), (__size)))

#endif
