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

#include "uvm_linux.h"
#include "uvm_common.h"
#include "uvm_api.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_va_block.h"
#include "uvm_hal_types.h"
#include "uvm_kvmalloc.h"
#include "uvm_tools.h"
#include "uvm_processors.h"
#include "uvm_push.h"
#include "uvm_hal.h"
#include "uvm_perf_thrashing.h"
#include "uvm_perf_prefetch.h"
#include "uvm_mem.h"
#include "uvm_gpu_access_counters.h"
#include "uvm_va_space_mm.h"
#include "uvm_test_ioctl.h"
#include "uvm_conf_computing.h"

typedef enum
{
    BLOCK_PTE_OP_MAP,
    BLOCK_PTE_OP_REVOKE,
    BLOCK_PTE_OP_COUNT
} block_pte_op_t;

static NvU64 uvm_perf_authorized_cpu_fault_tracking_window_ns = 300000;

static struct kmem_cache *g_uvm_va_block_cache __read_mostly;
static struct kmem_cache *g_uvm_va_block_gpu_state_cache __read_mostly;
static struct kmem_cache *g_uvm_page_mask_cache __read_mostly;
static struct kmem_cache *g_uvm_va_block_context_cache __read_mostly;
static struct kmem_cache *g_uvm_va_block_cpu_node_state_cache __read_mostly;

static int uvm_fault_force_sysmem __read_mostly = 0;
module_param(uvm_fault_force_sysmem, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(uvm_fault_force_sysmem, "Force (1) using sysmem storage for pages that faulted. Default: 0.");

static int uvm_perf_map_remote_on_eviction __read_mostly = 1;
module_param(uvm_perf_map_remote_on_eviction, int, S_IRUGO);

static int uvm_block_cpu_to_cpu_copy_with_ce __read_mostly = 0;
module_param(uvm_block_cpu_to_cpu_copy_with_ce, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(uvm_block_cpu_to_cpu_copy_with_ce, "Use GPU CEs for CPU-to-CPU migrations.");

// Caching is always disabled for mappings to remote memory. The following two
// module parameters can be used to force caching for GPU peer/sysmem mappings.
//
// However, it is important to note that it may not be safe to enable caching
// in the general case so the enablement should only be used for experiments.
static unsigned uvm_exp_gpu_cache_peermem __read_mostly = 0;
module_param(uvm_exp_gpu_cache_peermem, uint, S_IRUGO);
MODULE_PARM_DESC(uvm_exp_gpu_cache_peermem,
                 "Force caching for mappings to peer memory. "
                 "This is an experimental parameter that may cause correctness issues if used.");

static unsigned uvm_exp_gpu_cache_sysmem __read_mostly = 0;
module_param(uvm_exp_gpu_cache_sysmem, uint, S_IRUGO);
MODULE_PARM_DESC(uvm_exp_gpu_cache_sysmem,
                 "Force caching for mappings to system memory. "
                 "This is an experimental parameter that may cause correctness issues if used.");

static void block_add_eviction_mappings_entry(void *args);

uvm_va_space_t *uvm_va_block_get_va_space_maybe_dead(uvm_va_block_t *va_block)
{
#if UVM_IS_CONFIG_HMM()
    if (va_block->hmm.va_space)
        return va_block->hmm.va_space;
#endif

    if (va_block->va_range)
        return va_block->va_range->va_space;

    return NULL;
}

uvm_va_space_t *uvm_va_block_get_va_space(uvm_va_block_t *va_block)
{
    uvm_va_space_t *va_space;

    UVM_ASSERT(!uvm_va_block_is_dead(va_block));

    va_space = uvm_va_block_get_va_space_maybe_dead(va_block);
    UVM_ASSERT(va_space);

    return va_space;
}

static NvU64 block_gpu_pte_flag_cacheable(uvm_va_block_t *block, uvm_gpu_t *gpu, uvm_processor_id_t resident_id)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

    UVM_ASSERT(UVM_ID_IS_VALID(resident_id));

    // Local vidmem is always cached
    if (uvm_id_equal(resident_id, gpu->id))
        return UVM_MMU_PTE_FLAGS_CACHED;

    if (UVM_ID_IS_CPU(resident_id))
        return uvm_exp_gpu_cache_sysmem == 0 ? UVM_MMU_PTE_FLAGS_NONE : UVM_MMU_PTE_FLAGS_CACHED;

    UVM_ASSERT(uvm_processor_mask_test(&va_space->can_access[uvm_id_value(gpu->id)], resident_id));

    return uvm_exp_gpu_cache_peermem == 0 ? UVM_MMU_PTE_FLAGS_NONE : UVM_MMU_PTE_FLAGS_CACHED;
}

static uvm_gpu_t *block_get_gpu(uvm_va_block_t *block, uvm_gpu_id_t gpu_id)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

    return uvm_va_space_get_gpu(va_space, gpu_id);
}

static const char *block_processor_name(uvm_va_block_t *block, uvm_processor_id_t id)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

    return uvm_va_space_processor_name(va_space, id);
}

static bool block_processor_has_memory(uvm_va_block_t *block, uvm_processor_id_t id)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

    return uvm_va_space_processor_has_memory(va_space, id);
}

static bool is_uvm_fault_force_sysmem_set(void)
{
    // Only enforce this during testing
    return uvm_enable_builtin_tests && uvm_fault_force_sysmem != 0;
}

bool uvm_va_space_map_remote_on_eviction(uvm_va_space_t *va_space)
{
    return uvm_perf_map_remote_on_eviction &&
           uvm_va_space_has_access_counter_migrations(va_space);
}

static const uvm_processor_mask_t *block_get_uvm_lite_gpus(uvm_va_block_t *va_block)
{
    // Note that for HMM we always return a pointer to a zero bitmap
    // (not allocated on the stack) since uvm_lite GPUs are not supported.
    if (uvm_va_block_is_hmm(va_block))
        return &g_uvm_processor_mask_empty;
    else
        return &va_block->va_range->uvm_lite_gpus;
}

void uvm_va_block_retry_init(uvm_va_block_retry_t *retry)
{
    if (!retry)
        return;

    uvm_tracker_init(&retry->tracker);
    INIT_LIST_HEAD(&retry->used_chunks);
    INIT_LIST_HEAD(&retry->free_chunks);
}

static size_t node_to_index(int nid)
{
    UVM_ASSERT(nid != NUMA_NO_NODE);
    UVM_ASSERT(nid < MAX_NUMNODES);
    return __nodes_weight(&node_possible_map, nid);
}

static uvm_va_block_cpu_node_state_t *block_node_state_get(uvm_va_block_t *block, int nid)
{
    size_t index = node_to_index(nid);
    UVM_ASSERT(block->cpu.node_state[index]);
    return block->cpu.node_state[index];
}

static uvm_page_mask_t *block_tracking_node_mask_get(uvm_va_block_context_t *va_block_context, int nid)
{
    size_t index = node_to_index(nid);
    UVM_ASSERT(va_block_context->make_resident.cpu_pages_used.node_masks[index]);
    return va_block_context->make_resident.cpu_pages_used.node_masks[index];
}

// The bottom bit of uvm_va_block_t::chunks is used to indicate how CPU chunks
// are stored.
//
// CPU chunk storage is handled in three different ways depending on the
// type of chunks the VA block owns. This is done to minimize the memory
// required to hold metadata.
typedef enum
{
    // The uvm_va_block_t::chunk pointer points to a single 2MB
    // CPU chunk.
    UVM_CPU_CHUNK_STORAGE_CHUNK = 0,

    // The uvm_va_block_t::chunks pointer points to a
    // structure of mixed (64K and 4K) chunks.
    UVM_CPU_CHUNK_STORAGE_MIXED,
    UVM_CPU_CHUNK_STORAGE_COUNT,
} uvm_cpu_chunk_storage_type_t;

#define UVM_CPU_CHUNK_STORAGE_MASK 0x1

// The maximum number of slots in the mixed chunk mode (64K + 4K chunks) is
// MAX_BIG_PAGES_PER_UVM_VA_BLOCK. Any leading/trailing misaligned pages will
// be stored in the first/last entry, respectively.
#define MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK MAX_BIG_PAGES_PER_UVM_VA_BLOCK

#define MAX_SMALL_CHUNKS_PER_BIG_SLOT (UVM_MIN_BIG_PAGE_SIZE / PAGE_SIZE)

// This structure is used when a VA block contains 64K or a mix of 64K and 4K
// CPU chunks.
// For every 64K CPU chunks, big_chunks will have its corresponding bit set
// and the corresponding index in slots will point directly to the
// uvm_cpu_chunk_t structure.
//
// For 4K CPU chunks, the corresponding bit in big_chunks will be clear and
// the element in slots will point to an array of 16 uvm_cpu_chunk_t pointers.
typedef struct {
    DECLARE_BITMAP(big_chunks, MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK);
    void *slots[MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK];
} uvm_cpu_chunk_storage_mixed_t;

static uvm_va_block_region_t uvm_cpu_chunk_block_region(uvm_va_block_t *va_block,
                                                        uvm_cpu_chunk_t *chunk,
                                                        uvm_page_index_t page_index)
{
    UVM_ASSERT(chunk);
    return uvm_va_block_chunk_region(va_block, uvm_cpu_chunk_get_size(chunk), page_index);
}

static void *uvm_cpu_storage_get_ptr(uvm_va_block_cpu_node_state_t *node_state)
{
    return (void *)(node_state->chunks & ~UVM_CPU_CHUNK_STORAGE_MASK);
}

static uvm_cpu_chunk_storage_type_t uvm_cpu_storage_get_type(uvm_va_block_cpu_node_state_t *node_state)
{
    return node_state->chunks & UVM_CPU_CHUNK_STORAGE_MASK;
}

static int block_get_page_node_residency(uvm_va_block_t *block, uvm_page_index_t page_index)
{
    int nid;

    for_each_possible_uvm_node(nid) {
        if (uvm_va_block_cpu_is_page_resident_on(block, nid, page_index))
            return nid;
    }

    return NUMA_NO_NODE;
}

static uvm_page_index_t compute_page_prefix(uvm_va_block_t *va_block, uvm_chunk_size_t size)
{
    return (UVM_ALIGN_UP(va_block->start, size) - va_block->start) / PAGE_SIZE;
}

static size_t compute_slot_index(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    uvm_va_block_region_t block_region = uvm_va_block_region_from_block(va_block);
    uvm_page_index_t prefix;
    size_t slot_index;

    UVM_ASSERT(page_index < block_region.outer);
    prefix = compute_page_prefix(va_block, UVM_PAGE_SIZE_64K);

    if (page_index < prefix)
        return 0;

    slot_index = ((page_index - prefix) / MAX_SMALL_CHUNKS_PER_BIG_SLOT) + !!prefix;
    UVM_ASSERT(slot_index < MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK);

    return slot_index;
}

static size_t compute_small_index(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    size_t prefix = compute_page_prefix(va_block, UVM_PAGE_SIZE_64K);

    if (page_index < prefix)
        return page_index;

    return (page_index - prefix) % MAX_SMALL_CHUNKS_PER_BIG_SLOT;
}

NV_STATUS uvm_cpu_chunk_insert_in_block(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(chunk);
    uvm_va_block_region_t chunk_region = uvm_va_block_region(page_index, page_index + uvm_cpu_chunk_num_pages(chunk));
    int nid = uvm_cpu_chunk_get_numa_node(chunk);
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(va_block, nid);
    size_t slot_index;
    uvm_cpu_chunk_storage_mixed_t *mixed;
    uvm_cpu_chunk_t **chunks = NULL;

    // We only want to use the bottom bit of a pointer.
    BUILD_BUG_ON(UVM_CPU_CHUNK_STORAGE_COUNT > 2);

    // We want to protect against two threads manipulating the VA block's CPU
    // chunks at the same time. However, when a block is split, the new block's
    // lock is locked without tracking. So, we can't use
    // uvm_assert_mutex_locked().
    UVM_ASSERT(mutex_is_locked(&va_block->lock.m));

    if (chunk_size == UVM_CHUNK_SIZE_2M) {
        UVM_ASSERT(uvm_va_block_size(va_block) == UVM_PAGE_SIZE_2M);
        UVM_ASSERT(!node_state->chunks);
        node_state->chunks = (unsigned long)chunk | UVM_CPU_CHUNK_STORAGE_CHUNK;
    }
    else {
        if (!node_state->chunks) {
            mixed = uvm_kvmalloc_zero(sizeof(*mixed));
            if (!mixed)
                return NV_ERR_NO_MEMORY;

            node_state->chunks = (unsigned long)mixed | UVM_CPU_CHUNK_STORAGE_MIXED;
        }

        UVM_ASSERT(uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_MIXED);
        mixed = uvm_cpu_storage_get_ptr(node_state);
        slot_index = compute_slot_index(va_block, page_index);
        UVM_ASSERT(compute_slot_index(va_block, page_index + uvm_cpu_chunk_num_pages(chunk) - 1) == slot_index);
        UVM_ASSERT(!test_bit(slot_index, mixed->big_chunks));

        if (chunk_size == UVM_CHUNK_SIZE_64K) {
            mixed->slots[slot_index] = chunk;
            set_bit(slot_index, mixed->big_chunks);
        }
        else {
            size_t small_index;

            UVM_ASSERT(chunk_size == UVM_CHUNK_SIZE_4K);
            chunks = mixed->slots[slot_index];

            if (!chunks) {
                chunks = uvm_kvmalloc_zero(sizeof(*chunks) * MAX_SMALL_CHUNKS_PER_BIG_SLOT);
                if (!chunks)
                    return NV_ERR_NO_MEMORY;
                mixed->slots[slot_index] = chunks;
            }

            small_index = compute_small_index(va_block, page_index);
            chunks[small_index] = chunk;
        }
    }

    uvm_page_mask_region_fill(&node_state->allocated, chunk_region);
    uvm_page_mask_region_fill(&va_block->cpu.allocated, chunk_region);
    return NV_OK;
}

uvm_cpu_chunk_t *uvm_cpu_chunk_get_chunk_for_page(uvm_va_block_t *va_block, int nid, uvm_page_index_t page_index)
{
    uvm_va_block_cpu_node_state_t *node_state;
    uvm_cpu_chunk_storage_mixed_t *mixed;
    uvm_cpu_chunk_t *chunk;
    uvm_cpu_chunk_t **chunks;
    size_t slot_index;

    UVM_ASSERT(page_index < uvm_va_block_num_cpu_pages(va_block));
    UVM_ASSERT(nid != NUMA_NO_NODE);
    node_state = block_node_state_get(va_block, nid);
    if (!uvm_page_mask_test(&node_state->allocated, page_index))
        return NULL;

    UVM_ASSERT(node_state->chunks);

    if (uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_CHUNK) {
        return uvm_cpu_storage_get_ptr(node_state);
    }
    else {
        mixed = uvm_cpu_storage_get_ptr(node_state);
        slot_index = compute_slot_index(va_block, page_index);
        UVM_ASSERT(mixed->slots[slot_index] != NULL);
        if (test_bit(slot_index, mixed->big_chunks))
            return mixed->slots[slot_index];

        chunks = mixed->slots[slot_index];
        chunk = chunks[compute_small_index(va_block, page_index)];
    }

    UVM_ASSERT(chunk);
    return chunk;
}

uvm_cpu_chunk_t *uvm_cpu_chunk_get_any_chunk_for_page(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    int nid;
    uvm_va_block_cpu_node_state_t *node_state;

    // Callers for managed blocks should already know the correct nid and
    // shouldn't need to call this function.
    UVM_ASSERT(uvm_va_block_is_hmm(va_block));

    for_each_possible_uvm_node(nid) {
        node_state = block_node_state_get(va_block, nid);
        if (uvm_page_mask_test(&node_state->allocated, page_index))
            return uvm_cpu_chunk_get_chunk_for_page(va_block, nid, page_index);
    }

    return NULL;
}

static uvm_cpu_chunk_t *uvm_cpu_chunk_get_chunk_for_page_resident(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    uvm_cpu_chunk_t *chunk = NULL;
    int nid = block_get_page_node_residency(va_block, page_index);

    if (nid != NUMA_NO_NODE)
        chunk = uvm_cpu_chunk_get_chunk_for_page(va_block, nid, page_index);

    return chunk;
}

void uvm_cpu_chunk_remove_from_block(uvm_va_block_t *va_block, int nid, uvm_page_index_t page_index)
{
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(va_block, nid);
    uvm_cpu_chunk_storage_mixed_t *mixed;
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(va_block, nid, page_index);
    uvm_va_block_region_t chunk_region = uvm_cpu_chunk_block_region(va_block, chunk, page_index);
    size_t slot_index;
    uvm_cpu_chunk_t **chunks;
    int nid_iter;

    // We want to protect against two threads manipulating the VA block's CPU
    // chunks at the same time. However, when a block is split, the new block's
    // lock is locked without tracking. So, we can't use
    // uvm_assert_mutex_locked().
    UVM_ASSERT(mutex_is_locked(&va_block->lock.m));
    UVM_ASSERT(node_state->chunks);
    UVM_ASSERT(uvm_va_block_region_num_pages(chunk_region) == uvm_cpu_chunk_num_pages(chunk));

    if (uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_CHUNK) {
        UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_2M);
        UVM_ASSERT(uvm_cpu_storage_get_ptr(node_state) == chunk);
        node_state->chunks = 0;
    }
    else {
        UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) != UVM_CHUNK_SIZE_2M);
        mixed = uvm_cpu_storage_get_ptr(node_state);
        slot_index = compute_slot_index(va_block, page_index);
        UVM_ASSERT(mixed->slots[slot_index] != NULL);

        if (test_bit(slot_index, mixed->big_chunks)) {
            UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_64K);
            UVM_ASSERT(mixed->slots[slot_index] == chunk);
            mixed->slots[slot_index] = NULL;
            clear_bit(slot_index, mixed->big_chunks);
        }
        else {
            size_t small_index;

            UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_4K);
            chunks = mixed->slots[slot_index];
            small_index = compute_small_index(va_block, page_index);
            UVM_ASSERT(chunks[small_index] == chunk);
            chunks[small_index] = NULL;

            for (small_index = 0; small_index < MAX_SMALL_CHUNKS_PER_BIG_SLOT; small_index++) {
                if (chunks[small_index])
                    break;
            }

            if (small_index == MAX_SMALL_CHUNKS_PER_BIG_SLOT) {
                uvm_kvfree(chunks);
                mixed->slots[slot_index] = NULL;
            }
        }
    }

    uvm_page_mask_region_clear(&node_state->allocated, chunk_region);
    uvm_page_mask_zero(&va_block->cpu.allocated);
    for_each_possible_uvm_node(nid_iter) {
        uvm_va_block_cpu_node_state_t *iter_node_state = block_node_state_get(va_block, nid_iter);
        uvm_page_mask_or(&va_block->cpu.allocated, &va_block->cpu.allocated, &iter_node_state->allocated);
    }

    if (uvm_page_mask_empty(&node_state->allocated) && node_state->chunks) {
        uvm_kvfree(uvm_cpu_storage_get_ptr(node_state));
        node_state->chunks = 0;
    }
}

struct page *uvm_cpu_chunk_get_cpu_page(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index)
{
    uvm_va_block_region_t chunk_region;

    UVM_ASSERT(chunk);
    UVM_ASSERT(chunk->page);
    chunk_region = uvm_va_block_chunk_region(va_block, uvm_cpu_chunk_get_size(chunk), page_index);
    return chunk->page + (page_index - chunk_region.first);
}

struct page *uvm_va_block_get_cpu_page(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page_resident(va_block, page_index);

    return uvm_cpu_chunk_get_cpu_page(va_block, chunk, page_index);
}

static uvm_cpu_chunk_t *uvm_cpu_chunk_first_in_region(uvm_va_block_t *va_block,
                                                      uvm_va_block_region_t region,
                                                      int nid,
                                                      uvm_page_index_t *first_chunk_page)
{
    uvm_cpu_chunk_t *chunk = NULL;
    uvm_page_index_t page_index;
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(va_block, nid);

    if (!node_state)
        return NULL;

    page_index = uvm_va_block_first_page_in_mask(region, &node_state->allocated);
    if (page_index < region.outer)
        chunk = uvm_cpu_chunk_get_chunk_for_page(va_block, nid, page_index);

    if (first_chunk_page && chunk) {
        uvm_va_block_region_t chunk_region = uvm_cpu_chunk_block_region(va_block, chunk, page_index);
        *first_chunk_page = chunk_region.first;
    }

    return chunk;
}

static uvm_cpu_chunk_t *uvm_cpu_chunk_next_in_region(uvm_va_block_t *va_block,
                                                     uvm_va_block_region_t region,
                                                     int nid,
                                                     uvm_page_index_t prev_page_index,
                                                     uvm_page_index_t *next_chunk_page)
{
    if (prev_page_index >= region.outer)
        return NULL;

    return uvm_cpu_chunk_first_in_region(va_block,
                                         uvm_va_block_region(prev_page_index, region.outer),
                                         nid, next_chunk_page);
}

#define for_each_cpu_chunk_in_block_region(chunk, chunk_start, va_block, nid, region)                                  \
    for ((chunk) = uvm_cpu_chunk_first_in_region((va_block), (region), (nid), &(chunk_start));                         \
         (chunk) != NULL;                                                                                              \
         (chunk) = uvm_cpu_chunk_next_in_region((va_block),                                                            \
                                                (region),                                                              \
                                                (nid),                                                                 \
                                                (chunk_start) + uvm_cpu_chunk_num_pages((chunk)),                      \
                                                &(chunk_start)))

#define for_each_cpu_chunk_in_block_region_safe(chunk, chunk_start, next_chunk_start, va_block, nid, region)           \
    for ((chunk) = uvm_cpu_chunk_first_in_region((va_block), (region), (nid), &(chunk_start)),                         \
             (next_chunk_start) = (chunk_start) + (chunk ? uvm_cpu_chunk_num_pages(chunk) : 0);                        \
         (chunk) != NULL;                                                                                              \
         (chunk) = uvm_cpu_chunk_next_in_region((va_block), (region), (nid), (next_chunk_start), &(chunk_start)),      \
             (next_chunk_start) = (chunk_start) + ((chunk) ? uvm_cpu_chunk_num_pages((chunk)) : 0))

#define for_each_cpu_chunk_in_block(chunk, chunk_start, va_block, nid)                                                 \
    for_each_cpu_chunk_in_block_region((chunk),                                                                        \
                                       (chunk_start),                                                                  \
                                       (va_block),                                                                     \
                                       (nid),                                                                          \
                                       uvm_va_block_region_from_block((va_block)))

#define for_each_cpu_chunk_in_block_safe(chunk, chunk_start, next_chunk_start, va_block, nid)                          \
    for_each_cpu_chunk_in_block_region_safe((chunk),                                                                   \
                                            (chunk_start),                                                             \
                                            (next_chunk_start),                                                        \
                                            (va_block),                                                                \
                                            (nid),                                                                     \
                                            uvm_va_block_region_from_block((va_block)))

static void block_update_cpu_resident_mask(uvm_va_block_t *va_block)
{
    int nid;

    uvm_page_mask_zero(&va_block->cpu.resident);
    for_each_possible_uvm_node(nid) {
        uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(va_block, nid);
        uvm_page_mask_or(&va_block->cpu.resident, &va_block->cpu.resident, &node_state->resident);
    }
}

void uvm_va_block_cpu_set_resident_page(uvm_va_block_t *va_block, int nid, uvm_page_index_t page_index)
{
    uvm_va_block_cpu_node_state_t *node_state;

    node_state = block_node_state_get(va_block, nid);
    UVM_ASSERT(node_state);
    UVM_ASSERT(uvm_page_mask_test(&node_state->allocated, page_index));
    uvm_page_mask_set(&node_state->resident, page_index);
    uvm_page_mask_set(&va_block->cpu.resident, page_index);
    uvm_processor_mask_set(&va_block->resident, UVM_ID_CPU);
}

// Set all CPU pages in the mask as resident on NUMA node nid.
// nid cannot be NUMA_NO_NODE.
static void uvm_va_block_cpu_set_resident_mask(uvm_va_block_t *va_block, int nid, const uvm_page_mask_t *mask)
{
    uvm_va_block_cpu_node_state_t *node_state;

    node_state = block_node_state_get(va_block, nid);
    UVM_ASSERT(node_state);
    UVM_ASSERT(uvm_page_mask_subset(mask, &node_state->allocated));
    uvm_page_mask_or(&node_state->resident, &node_state->resident, mask);
    uvm_page_mask_or(&va_block->cpu.resident, &va_block->cpu.resident, mask);
}

static void uvm_va_block_cpu_set_resident_all_chunks(uvm_va_block_t *va_block,
                                                     uvm_va_block_context_t *va_block_context,
                                                     const uvm_page_mask_t *page_mask)
{
    uvm_make_resident_page_tracking_t *tracking = &va_block_context->make_resident.cpu_pages_used;
    uvm_page_mask_t *node_pages_mask = &va_block_context->make_resident.node_pages_mask;
    uvm_page_mask_t *page_mask_copy = &va_block_context->scratch_page_mask;
    int nid;

    if (uvm_page_mask_empty(page_mask))
        return;

    uvm_page_mask_copy(page_mask_copy, page_mask);
    for_each_node_mask(nid, tracking->nodes) {
        uvm_page_mask_t *node_mask = block_tracking_node_mask_get(va_block_context, nid);

        if (uvm_page_mask_and(node_pages_mask, page_mask_copy, node_mask)) {
            uvm_va_block_cpu_set_resident_mask(va_block, nid, node_pages_mask);
            uvm_page_mask_andnot(page_mask_copy, page_mask_copy, node_pages_mask);
        }
    }

    UVM_ASSERT(uvm_page_mask_empty(page_mask_copy));
}

// Clear residency for all CPU pages in the mask.
// nid cannot be NUMA_NO_NODE.
static void uvm_va_block_cpu_clear_resident_mask(uvm_va_block_t *va_block, int nid, const uvm_page_mask_t *mask)
{
    uvm_va_block_cpu_node_state_t *node_state;

    node_state = block_node_state_get(va_block, nid);
    UVM_ASSERT(node_state);
    uvm_page_mask_andnot(&node_state->resident, &node_state->resident, mask);
    block_update_cpu_resident_mask(va_block);
}

static void uvm_va_block_cpu_clear_resident_region(uvm_va_block_t *va_block, int nid, uvm_va_block_region_t region)
{
    uvm_va_block_cpu_node_state_t *node_state;

    node_state = block_node_state_get(va_block, nid);
    UVM_ASSERT(node_state);
    uvm_page_mask_region_clear(&node_state->resident, region);
    block_update_cpu_resident_mask(va_block);
}

// Clear residency bits from any/all processors that might have had pages resident.
// Note that both the destination processor and any CPU NUMA nodes where pages are
// migrating to need to be skipped as the block logic sets the new page residency
// before clearing the old ones (see uvm_va_block_make_resident_finish()).
static void uvm_va_block_cpu_clear_resident_all_chunks(uvm_va_block_t *va_block,
                                                       uvm_va_block_context_t *va_block_context,
                                                       uvm_page_mask_t *page_mask)
{
    int nid;

    if (UVM_ID_IS_CPU(va_block_context->make_resident.dest_id) &&
        nodes_empty(va_block_context->make_resident.cpu_pages_used.nodes))
        return;

    for_each_possible_uvm_node(nid) {
        // If the destination is the CPU and pages were allocated on this node
        // for the migration, clear residency on the node only for pages that
        // are in the page_mask but not in the node's allocated mask.
        if (UVM_ID_IS_CPU(va_block_context->make_resident.dest_id) &&
            node_isset(nid, va_block_context->make_resident.cpu_pages_used.nodes)) {
            uvm_page_mask_t *node_pages_mask = &va_block_context->make_resident.node_pages_mask;
            uvm_page_mask_t *node_alloc_mask = block_tracking_node_mask_get(va_block_context, nid);
            uvm_page_mask_t *nid_resident = uvm_va_block_resident_mask_get(va_block, UVM_ID_CPU, nid);
            uvm_page_mask_t *migrated_pages = &va_block_context->make_resident.pages_migrated;

            uvm_page_mask_andnot(node_pages_mask, nid_resident, node_alloc_mask);
            if (uvm_page_mask_and(node_pages_mask, migrated_pages, node_pages_mask))
                uvm_va_block_cpu_clear_resident_mask(va_block, nid, node_pages_mask);
        }
        else {
            uvm_va_block_cpu_clear_resident_mask(va_block, nid, page_mask);
        }
    }
}

bool uvm_va_block_cpu_is_page_resident_on(uvm_va_block_t *va_block, int nid, uvm_page_index_t page_index)
{
    uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(va_block, UVM_ID_CPU, nid);

    return uvm_page_mask_test(resident_mask, page_index);
}

bool uvm_va_block_cpu_is_region_resident_on(uvm_va_block_t *va_block, int nid, uvm_va_block_region_t region)
{
    uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(va_block, UVM_ID_CPU, nid);

    return uvm_page_mask_region_full(resident_mask, region);
}

// Return the preferred NUMA node ID for the block's policy.
// If the preferred node ID is NUMA_NO_NODE, the nearest NUMA node ID
// with memory is returned. In most cases, this should be the current
// NUMA node.
static int uvm_va_block_context_get_node(uvm_va_block_context_t *va_block_context)
{
    if (va_block_context->make_resident.dest_nid != NUMA_NO_NODE)
        return va_block_context->make_resident.dest_nid;

    return numa_mem_id();
}

struct vm_area_struct *uvm_va_block_find_vma_region(uvm_va_block_t *va_block,
                                                    struct mm_struct *mm,
                                                    NvU64 start,
                                                    uvm_va_block_region_t *region)
{
    struct vm_area_struct *vma;
    NvU64 end;

    if (start > va_block->end)
        return NULL;

    vma = find_vma_intersection(mm, start, va_block->end + 1);
    if (!vma)
        return NULL;

    if (start < vma->vm_start)
        start = vma->vm_start;

    end = vma->vm_end - 1;
    if (end > va_block->end)
        end = va_block->end;

    *region = uvm_va_block_region_from_start_end(va_block, start, end);

    return vma;
}

static bool block_check_cpu_chunks(uvm_va_block_t *block)
{
    int nid;
    uvm_page_mask_t *temp_resident_mask;

    temp_resident_mask = kmem_cache_alloc(g_uvm_page_mask_cache, NV_UVM_GFP_FLAGS | __GFP_ZERO);

    for_each_possible_uvm_node(nid) {
        uvm_cpu_chunk_t *chunk;
        uvm_page_index_t page_index;
        uvm_va_block_region_t prev_region = {0};
        uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, nid);
        size_t alloced_pages = 0;

        for_each_cpu_chunk_in_block(chunk, page_index, block, nid) {
            uvm_va_block_region_t chunk_region = uvm_cpu_chunk_block_region(block, chunk, page_index);
            size_t num_chunk_pages = uvm_cpu_chunk_num_pages(chunk);
            uvm_page_index_t chunk_page;

            UVM_ASSERT(prev_region.outer <= chunk_region.first);
            UVM_ASSERT(IS_ALIGNED(uvm_va_block_region_start(block, chunk_region), uvm_cpu_chunk_get_size(chunk)));
            UVM_ASSERT(chunk_region.outer <= uvm_va_block_num_cpu_pages(block));

            alloced_pages += uvm_cpu_chunk_num_pages(chunk);
            UVM_ASSERT(uvm_page_mask_region_full(&node_state->allocated, chunk_region));
            prev_region = chunk_region;

            for (chunk_page = page_index; chunk_page < page_index + num_chunk_pages; chunk_page++)
                UVM_ASSERT(uvm_cpu_chunk_get_chunk_for_page(block, nid, chunk_page) == chunk);
        }

        UVM_ASSERT(alloced_pages == uvm_page_mask_weight(&node_state->allocated));
        UVM_ASSERT(uvm_page_mask_subset(&node_state->resident, &node_state->allocated));
        UVM_ASSERT(uvm_page_mask_subset(&node_state->resident, &block->cpu.resident));
        if (temp_resident_mask && !uvm_page_mask_empty(&node_state->resident)) {
            UVM_ASSERT(!uvm_page_mask_intersects(&node_state->resident, temp_resident_mask));
            uvm_page_mask_or(temp_resident_mask, temp_resident_mask, &node_state->resident);
        }
    }

    if (temp_resident_mask) {
        UVM_ASSERT(uvm_page_mask_equal(temp_resident_mask, &block->cpu.resident));
        kmem_cache_free(g_uvm_page_mask_cache, temp_resident_mask);
    }

    return true;
}

// Frees any left-over free chunks and unpins all the used chunks
void uvm_va_block_retry_deinit(uvm_va_block_retry_t *retry, uvm_va_block_t *va_block)
{
    uvm_gpu_t *gpu;
    uvm_gpu_chunk_t *gpu_chunk;
    uvm_gpu_chunk_t *next_chunk;

    if (!retry)
        return;

    uvm_tracker_deinit(&retry->tracker);

    // Free any unused chunks
    list_for_each_entry_safe(gpu_chunk, next_chunk, &retry->free_chunks, list) {
        list_del_init(&gpu_chunk->list);
        gpu = uvm_gpu_chunk_get_gpu(gpu_chunk);
        uvm_pmm_gpu_free(&gpu->pmm, gpu_chunk, NULL);
    }

    // Unpin all the used chunks now that we are done
    list_for_each_entry_safe(gpu_chunk, next_chunk, &retry->used_chunks, list) {
        list_del_init(&gpu_chunk->list);
        gpu = uvm_gpu_chunk_get_gpu(gpu_chunk);
        // HMM should have already moved allocated blocks to the referenced
        // state so any left over were not migrated and should be freed.
        if (uvm_va_block_is_hmm(va_block))
            uvm_pmm_gpu_free(&gpu->pmm, gpu_chunk, NULL);
        else
            uvm_pmm_gpu_unpin_allocated(&gpu->pmm, gpu_chunk, va_block);
    }
}

static void block_retry_add_free_chunk(uvm_va_block_retry_t *retry, uvm_gpu_chunk_t *gpu_chunk)
{
    list_add_tail(&gpu_chunk->list, &retry->free_chunks);
}

static void block_retry_add_used_chunk(uvm_va_block_retry_t *retry, uvm_gpu_chunk_t *gpu_chunk)
{
    list_add_tail(&gpu_chunk->list, &retry->used_chunks);
}

static uvm_gpu_chunk_t *block_retry_get_free_chunk(uvm_va_block_retry_t *retry, uvm_gpu_t *gpu, uvm_chunk_size_t size)
{
    uvm_gpu_chunk_t *gpu_chunk;

    list_for_each_entry(gpu_chunk, &retry->free_chunks, list) {
        if (uvm_gpu_chunk_get_gpu(gpu_chunk) == gpu && uvm_gpu_chunk_get_size(gpu_chunk) == size) {
            list_del_init(&gpu_chunk->list);
            return gpu_chunk;
        }
    }

    return NULL;
}

// Encapsulates a reference to a physical page belonging to a specific processor
// within a VA block.
typedef struct
{
    // Processor the page is on
    uvm_processor_id_t processor;

    // The page index
    uvm_page_index_t page_index;

    // If processor is the CPU, the NUMA node of the page.
    int nid;
} block_phys_page_t;

static block_phys_page_t block_phys_page(uvm_processor_id_t processor, int nid, uvm_page_index_t page_index)
{
    if (UVM_ID_IS_CPU(processor))
        UVM_ASSERT(nid != NUMA_NO_NODE);

    return (block_phys_page_t){ processor, page_index, nid };
}

NV_STATUS uvm_va_block_init(void)
{
    if (uvm_enable_builtin_tests)
        g_uvm_va_block_cache = NV_KMEM_CACHE_CREATE("uvm_va_block_wrapper_t", uvm_va_block_wrapper_t);
    else
        g_uvm_va_block_cache = NV_KMEM_CACHE_CREATE("uvm_va_block_t", uvm_va_block_t);

    if (!g_uvm_va_block_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_va_block_gpu_state_cache = NV_KMEM_CACHE_CREATE("uvm_va_block_gpu_state_t", uvm_va_block_gpu_state_t);
    if (!g_uvm_va_block_gpu_state_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_page_mask_cache = NV_KMEM_CACHE_CREATE("uvm_page_mask_t", uvm_page_mask_t);
    if (!g_uvm_page_mask_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_va_block_context_cache = NV_KMEM_CACHE_CREATE("uvm_va_block_context_t", uvm_va_block_context_t);
    if (!g_uvm_va_block_context_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_va_block_cpu_node_state_cache = NV_KMEM_CACHE_CREATE("uvm_va_block_cpu_node_state_t",
                                                               uvm_va_block_cpu_node_state_t);
    if (!g_uvm_va_block_cpu_node_state_cache)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_va_block_exit(void)
{
    kmem_cache_destroy_safe(&g_uvm_va_block_cpu_node_state_cache);
    kmem_cache_destroy_safe(&g_uvm_va_block_context_cache);
    kmem_cache_destroy_safe(&g_uvm_page_mask_cache);
    kmem_cache_destroy_safe(&g_uvm_va_block_gpu_state_cache);
    kmem_cache_destroy_safe(&g_uvm_va_block_cache);
}

static void block_context_free_tracking(uvm_make_resident_page_tracking_t *tracking)
{
    size_t index;

    for (index = 0; index < num_possible_nodes(); index++) {
        if (tracking->node_masks[index])
            kmem_cache_free(g_uvm_page_mask_cache, tracking->node_masks[index]);
    }

    uvm_kvfree(tracking->node_masks);
}

static NV_STATUS block_context_alloc_tracking(uvm_make_resident_page_tracking_t *tracking)
{
    size_t index;

    tracking->node_masks = uvm_kvmalloc_zero(num_possible_nodes() * sizeof(*tracking->node_masks));
    if (!tracking->node_masks)
        return NV_ERR_NO_MEMORY;

    for (index = 0; index < num_possible_nodes(); index++) {
        tracking->node_masks[index] = kmem_cache_alloc(g_uvm_page_mask_cache, NV_UVM_GFP_FLAGS);
        if (!tracking->node_masks[index])
            goto error;
    }

    return NV_OK;

error:
    block_context_free_tracking(tracking);
    return NV_ERR_NO_MEMORY;
}

uvm_va_block_context_t *uvm_va_block_context_alloc(struct mm_struct *mm)
{
    uvm_va_block_context_t *block_context = kmem_cache_alloc(g_uvm_va_block_context_cache, NV_UVM_GFP_FLAGS);
    NV_STATUS status;

    if (!block_context)
        return NULL;

    status = block_context_alloc_tracking(&block_context->make_resident.cpu_pages_used);
    if (status != NV_OK) {
        kmem_cache_free(g_uvm_va_block_context_cache, block_context);
        return NULL;
    }

    uvm_va_block_context_init(block_context, mm);
    return block_context;
}

void uvm_va_block_context_init(uvm_va_block_context_t *va_block_context, struct mm_struct *mm)
{
    UVM_ASSERT(va_block_context);

    // Write garbage into the VA Block context to ensure that the UVM code
    // clears masks appropriately
    if (UVM_IS_DEBUG()) {
        uvm_page_mask_t **mask_array = va_block_context->make_resident.cpu_pages_used.node_masks;
        int nid;

        memset(va_block_context, 0xff, sizeof(*va_block_context));

        for_each_possible_uvm_node(nid)
            uvm_page_mask_fill(mask_array[node_to_index(nid)]);

        va_block_context->make_resident.cpu_pages_used.node_masks = mask_array;
    }

    va_block_context->mm = mm;
    va_block_context->make_resident.dest_nid = NUMA_NO_NODE;
    nodes_clear(va_block_context->make_resident.cpu_pages_used.nodes);
}

void uvm_va_block_context_free(uvm_va_block_context_t *va_block_context)
{
    if (va_block_context) {
        block_context_free_tracking(&va_block_context->make_resident.cpu_pages_used);
        kmem_cache_free(g_uvm_va_block_context_cache, va_block_context);
    }
}

// Convert from page_index to chunk_index. The goal is for each system page in
// the region [start, start + size) to be covered by the largest naturally-
// aligned user chunk size.
size_t uvm_va_block_gpu_chunk_index_range(NvU64 start,
                                          NvU64 size,
                                          uvm_gpu_t *gpu,
                                          uvm_page_index_t page_index,
                                          uvm_chunk_size_t *out_chunk_size)
{
    uvm_chunk_sizes_mask_t chunk_sizes = gpu->parent->mmu_user_chunk_sizes;
    uvm_chunk_size_t chunk_size, final_chunk_size;
    size_t num_chunks, num_chunks_total;
    NvU64 addr, end, aligned_start, aligned_addr, aligned_end, temp_size;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(size));
    UVM_ASSERT(size > 0);
    UVM_ASSERT(size <= UVM_CHUNK_SIZE_2M);
    UVM_ASSERT(UVM_ALIGN_DOWN(start, UVM_CHUNK_SIZE_2M) == UVM_ALIGN_DOWN(start + size - 1, UVM_CHUNK_SIZE_2M));
    BUILD_BUG_ON(UVM_VA_BLOCK_SIZE != UVM_CHUNK_SIZE_2M);

    // PAGE_SIZE needs to be the lowest natively-supported chunk size in the
    // mask, since we never deal with chunk sizes smaller than that (although we
    // may have PTEs mapping pages smaller than that).
    UVM_ASSERT(uvm_chunk_find_first_size(chunk_sizes) == PAGE_SIZE);

    // Optimize the ideal Pascal+ case: the whole block is covered by a single
    // 2M page.
    if ((chunk_sizes & UVM_CHUNK_SIZE_2M) && size == UVM_CHUNK_SIZE_2M) {
        UVM_ASSERT(IS_ALIGNED(start, UVM_CHUNK_SIZE_2M));
        final_chunk_size = UVM_CHUNK_SIZE_2M;
        num_chunks_total = 0;
        goto out;
    }

    // Only one 2M chunk can fit within a VA block on any GPU architecture, so
    // remove that size from consideration.
    chunk_sizes &= ~UVM_CHUNK_SIZE_2M;

    // Next common case: the whole block is aligned and sized to perfectly fit
    // the largest page size.
    final_chunk_size = uvm_chunk_find_last_size(chunk_sizes);
    if (IS_ALIGNED(start, final_chunk_size) && IS_ALIGNED(size, final_chunk_size)) {
        num_chunks_total = (size_t)uvm_div_pow2_64(page_index * PAGE_SIZE, final_chunk_size);
        goto out;
    }

    // We didn't hit our special paths. Do it the hard way.

    num_chunks_total = 0;
    addr = start + page_index * PAGE_SIZE;
    end = start + size;
    final_chunk_size = 0;
    UVM_ASSERT(addr < end);

    // The below loop collapses almost completely when chunk_size == PAGE_SIZE
    // since in that lowest-common-denominator case everything is already
    // aligned. Skip it and handle that specially after the loop.
    //
    // Note that since we removed 2M already above, this loop will only iterate
    // once on x86 Pascal+ since only 64K is left.
    chunk_sizes &= ~PAGE_SIZE;

    // This loop calculates the number of chunks between start and addr by
    // calculating the number of whole chunks of each size between them,
    // starting with the largest allowed chunk size. This requires fewer
    // iterations than if we began from start and kept calculating the next
    // larger chunk size boundary.
    for_each_chunk_size_rev(chunk_size, chunk_sizes) {
        aligned_start = UVM_ALIGN_UP(start, chunk_size);
        aligned_addr  = UVM_ALIGN_DOWN(addr, chunk_size);
        aligned_end   = UVM_ALIGN_DOWN(end, chunk_size);

        // If addr and start are within the same chunk, try smaller
        if (aligned_start > aligned_addr)
            continue;

        // If addr and end are not in the same chunk, then addr is covered by a
        // single chunk of the current size. Ignore smaller boundaries between
        // addr and aligned_addr.
        if (aligned_addr < aligned_end && final_chunk_size == 0) {
            addr = aligned_addr;
            final_chunk_size = chunk_size;
        }

        // How many chunks of this size are between start and addr? Note that
        // this might be 0 since aligned_addr and aligned_start could be in the
        // same chunk.
        num_chunks = uvm_div_pow2_32(((NvU32)aligned_addr - aligned_start), chunk_size);
        num_chunks_total += num_chunks;

        // We've already accounted for these chunks, so "remove" them by
        // bringing start, addr, and end closer together to calculate the
        // remaining chunk sizes.
        temp_size = num_chunks * chunk_size;
        addr -= temp_size;
        end -= temp_size;

        // Once there's no separation between addr and start, and we've
        // successfully found the right chunk size when taking end into account,
        // we're done.
        if (addr == start && final_chunk_size)
            break;
    }

    // Handle PAGE_SIZE cleanup since we skipped it in the loop
    num_chunks_total += (addr - start) / PAGE_SIZE;
    if (final_chunk_size == 0)
        final_chunk_size = PAGE_SIZE;

out:
    if (out_chunk_size)
        *out_chunk_size = final_chunk_size;

    return num_chunks_total;
}

static size_t block_gpu_chunk_index_range(uvm_va_block_t *va_block,
                                          NvU64 start,
                                          NvU64 size,
                                          uvm_gpu_t *gpu,
                                          uvm_page_index_t page_index,
                                          uvm_chunk_size_t *out_chunk_size)
{
    if (uvm_va_block_is_hmm(va_block)) {
        if (out_chunk_size)
            *out_chunk_size = PAGE_SIZE;
        return page_index;
    }

    return uvm_va_block_gpu_chunk_index_range(start, size, gpu, page_index, out_chunk_size);
}

static size_t block_gpu_chunk_index(uvm_va_block_t *block,
                                    uvm_gpu_t *gpu,
                                    uvm_page_index_t page_index,
                                    uvm_chunk_size_t *out_chunk_size)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_chunk_size_t size;
    uvm_gpu_chunk_t *chunk;
    size_t index;

    index = block_gpu_chunk_index_range(block, block->start, uvm_va_block_size(block), gpu, page_index, &size);

    UVM_ASSERT(size >= PAGE_SIZE);

    if (gpu_state) {
        UVM_ASSERT(gpu_state->chunks);
        chunk = gpu_state->chunks[index];
        if (chunk) {
            UVM_ASSERT(uvm_gpu_chunk_get_size(chunk) == size);
            UVM_ASSERT(chunk->state != UVM_PMM_GPU_CHUNK_STATE_PMA_OWNED);
            UVM_ASSERT(chunk->state != UVM_PMM_GPU_CHUNK_STATE_FREE);
        }
    }

    if (out_chunk_size)
        *out_chunk_size = size;

    return index;
}

// Compute the size of the chunk known to start at start_page_index
static uvm_chunk_size_t block_gpu_chunk_size(uvm_va_block_t *block, uvm_gpu_t *gpu, uvm_page_index_t start_page_index)
{
    uvm_chunk_sizes_mask_t chunk_sizes = gpu->parent->mmu_user_chunk_sizes;
    uvm_chunk_sizes_mask_t start_alignments, pow2_leq_size, allowed_sizes;
    NvU64 start = uvm_va_block_cpu_page_address(block, start_page_index);
    NvU64 size = block->end - start + 1;

    if (uvm_va_block_is_hmm(block))
        return PAGE_SIZE;

    // Create a mask of all sizes for which start is aligned. x ^ (x-1) yields a
    // mask of the rightmost 1 bit in x, as well as all trailing 0 bits in x.
    // Example: 1011000 -> 0001111
    start_alignments = (uvm_chunk_sizes_mask_t)(start ^ (start - 1));

    // Next, compute all sizes (powers of two) which are <= size.
    pow2_leq_size = (uvm_chunk_sizes_mask_t)rounddown_pow_of_two(size);
    pow2_leq_size |= pow2_leq_size - 1;

    // Now and them all together to get our list of GPU-supported chunk sizes
    // which are aligned to start and will fit within size.
    allowed_sizes = chunk_sizes & start_alignments & pow2_leq_size;

    // start and size must always be aligned to at least the smallest supported
    // chunk size (PAGE_SIZE).
    UVM_ASSERT(allowed_sizes >= PAGE_SIZE);

    // Take the largest allowed size
    return uvm_chunk_find_last_size(allowed_sizes);
}

static size_t block_num_gpu_chunks(uvm_va_block_t *block, uvm_gpu_t *gpu)
{
    return block_gpu_chunk_index(block, gpu, uvm_va_block_cpu_page_index(block, block->end), NULL) + 1;
}

static size_t block_num_gpu_chunks_range(uvm_va_block_t *block, NvU64 start, NvU64 size, uvm_gpu_t *gpu)
{
    uvm_page_index_t last_page_index = (size_t)((size / PAGE_SIZE) - 1);
    return block_gpu_chunk_index_range(block, start, size, gpu, last_page_index, NULL) + 1;
}

uvm_gpu_chunk_t *uvm_va_block_lookup_gpu_chunk(uvm_va_block_t *va_block, uvm_gpu_t *gpu, NvU64 address)
{
    size_t chunk_index;
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    uvm_page_index_t page_index = uvm_va_block_cpu_page_index(va_block, address);

    uvm_assert_mutex_locked(&va_block->lock);

    if (!gpu_state)
        return NULL;

    chunk_index = block_gpu_chunk_index(va_block, gpu, page_index, NULL);

    return gpu_state->chunks[chunk_index];
}

static void uvm_va_block_free(uvm_va_block_t *block)
{
    if (uvm_enable_builtin_tests) {
        uvm_va_block_wrapper_t *block_wrapper = container_of(block, uvm_va_block_wrapper_t, block);

        kmem_cache_free(g_uvm_va_block_cache, block_wrapper);
    }
    else {
        kmem_cache_free(g_uvm_va_block_cache, block);
    }
}

NV_STATUS uvm_va_block_create(uvm_va_range_t *va_range,
                              NvU64 start,
                              NvU64 end,
                              uvm_va_block_t **out_block)
{
    uvm_va_block_t *block = NULL;
    NvU64 size = end - start + 1;
    int nid;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(end + 1));
    UVM_ASSERT(PAGE_ALIGNED(size));
    UVM_ASSERT(size > 0);
    UVM_ASSERT(size <= UVM_VA_BLOCK_SIZE);

    if (va_range) {
        // Create a managed va_block.
        UVM_ASSERT(start >= va_range->node.start);
        UVM_ASSERT(end <= va_range->node.end);
        UVM_ASSERT(va_range->type == UVM_VA_RANGE_TYPE_MANAGED);
    }

    // Blocks can't span a block alignment boundary
    UVM_ASSERT(UVM_VA_BLOCK_ALIGN_DOWN(start) == UVM_VA_BLOCK_ALIGN_DOWN(end));

    if (uvm_enable_builtin_tests) {
        uvm_va_block_wrapper_t *block_wrapper = nv_kmem_cache_zalloc(g_uvm_va_block_cache, NV_UVM_GFP_FLAGS);

        if (block_wrapper) {
            block = &block_wrapper->block;
            block_wrapper->test.cpu_chunk_allocation_target_id = NUMA_NO_NODE;
            block_wrapper->test.cpu_chunk_allocation_actual_id = NUMA_NO_NODE;
        }
    }
    else {
        block = nv_kmem_cache_zalloc(g_uvm_va_block_cache, NV_UVM_GFP_FLAGS);
    }

    if (!block)
        return NV_ERR_NO_MEMORY;

    block->cpu.node_state = uvm_kvmalloc_zero(sizeof(*block->cpu.node_state) * num_possible_nodes());
    if (!block->cpu.node_state)
        goto error_block_free;

    for_each_possible_uvm_node(nid) {
        size_t index = node_to_index(nid);

        block->cpu.node_state[index] = nv_kmem_cache_zalloc(g_uvm_va_block_cpu_node_state_cache, NV_UVM_GFP_FLAGS);
        if (!block->cpu.node_state[index])
            goto error;
    }

    nv_kref_init(&block->kref);
    uvm_mutex_init(&block->lock, UVM_LOCK_ORDER_VA_BLOCK);
    block->start = start;
    block->end = end;
    block->va_range = va_range;
    uvm_tracker_init(&block->tracker);
    block->prefetch_info.last_migration_proc_id = UVM_ID_INVALID;

    nv_kthread_q_item_init(&block->eviction_mappings_q_item, block_add_eviction_mappings_entry, block);

    *out_block = block;
    return NV_OK;

error:
    if (block->cpu.node_state) {
        for_each_possible_uvm_node(nid) {
            size_t index = node_to_index(nid);

            if (block->cpu.node_state[index])
                kmem_cache_free(g_uvm_va_block_cpu_node_state_cache, block->cpu.node_state[index]);
        }
    }

    uvm_kvfree(block->cpu.node_state);

error_block_free:
    uvm_va_block_free(block);
    return NV_ERR_NO_MEMORY;
}

static void cpu_chunk_remove_sysmem_gpu_mapping(uvm_cpu_chunk_t *chunk, uvm_gpu_t *gpu)
{
    NvU64 gpu_mapping_addr = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu);
    if (gpu_mapping_addr == 0)
        return;

    uvm_pmm_sysmem_mappings_remove_gpu_mapping(&gpu->pmm_reverse_sysmem_mappings, gpu_mapping_addr);
    uvm_cpu_chunk_unmap_gpu(chunk, gpu);
}

static NV_STATUS cpu_chunk_add_sysmem_gpu_mapping(uvm_cpu_chunk_t *chunk,
                                                  uvm_va_block_t *block,
                                                  uvm_page_index_t page_index,
                                                  uvm_gpu_t *gpu)
{
    NV_STATUS status;
    uvm_chunk_size_t chunk_size;

    // When the Confidential Computing feature is enabled the transfers don't
    // use the DMA mapping of CPU chunks (since it's protected memory), but
    // the DMA address of the unprotected dma buffer.
    if (g_uvm_global.conf_computing_enabled)
        return NV_OK;

    status = uvm_cpu_chunk_map_gpu(chunk, gpu);
    if (status != NV_OK)
        return status;

    chunk_size = uvm_cpu_chunk_get_size(chunk);

    status = uvm_pmm_sysmem_mappings_add_gpu_mapping(&gpu->pmm_reverse_sysmem_mappings,
                                                     uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu),
                                                     uvm_va_block_cpu_page_address(block, page_index),
                                                     chunk_size,
                                                     block,
                                                     UVM_ID_CPU);
    if (status != NV_OK)
        uvm_cpu_chunk_unmap_gpu(chunk, gpu);

    return status;
}

static void block_gpu_unmap_phys_all_cpu_pages(uvm_va_block_t *block, uvm_gpu_t *gpu)
{
    uvm_cpu_chunk_t *chunk;
    uvm_page_index_t page_index;
    int nid;

    for_each_possible_uvm_node(nid) {
        for_each_cpu_chunk_in_block(chunk, page_index, block, nid)
            cpu_chunk_remove_sysmem_gpu_mapping(chunk, gpu);
    }
}

static NV_STATUS block_gpu_map_phys_all_cpu_pages(uvm_va_block_t *block, uvm_gpu_t *gpu)
{
    NV_STATUS status;
    uvm_cpu_chunk_t *chunk;
    NvU64 block_mapping_size = uvm_va_block_size(block);
    uvm_page_index_t page_index;
    int nid;

    UVM_ASSERT(IS_ALIGNED(block_mapping_size, UVM_PAGE_SIZE_4K));

    for_each_possible_uvm_node(nid) {
        for_each_cpu_chunk_in_block(chunk, page_index, block, nid) {
            UVM_ASSERT_MSG(uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu) == 0,
                           "GPU%u DMA address 0x%llx\n",
                           uvm_id_value(gpu->id),
                           uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu));

            status = cpu_chunk_add_sysmem_gpu_mapping(chunk, block, page_index, gpu);
            if (status != NV_OK)
                goto error;
        }
    }

    return NV_OK;

error:
    block_gpu_unmap_phys_all_cpu_pages(block, gpu);
    return status;
}

// Retrieves the gpu_state for the given GPU. The returned pointer is
// internally managed and will be allocated (and freed) automatically,
// rather than by the caller.
static uvm_va_block_gpu_state_t *block_gpu_state_get_alloc(uvm_va_block_t *block, uvm_gpu_t *gpu)
{
    NV_STATUS status;
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);

    if (gpu_state)
        return gpu_state;

    gpu_state = nv_kmem_cache_zalloc(g_uvm_va_block_gpu_state_cache, NV_UVM_GFP_FLAGS);
    if (!gpu_state)
        return NULL;

    gpu_state->chunks = uvm_kvmalloc_zero(block_num_gpu_chunks(block, gpu) * sizeof(gpu_state->chunks[0]));
    if (!gpu_state->chunks)
        goto error;

    block->gpus[uvm_id_gpu_index(gpu->id)] = gpu_state;

    status = block_gpu_map_phys_all_cpu_pages(block, gpu);
    if (status != NV_OK)
        goto error;

    return gpu_state;

error:
    uvm_kvfree(gpu_state->chunks);
    kmem_cache_free(g_uvm_va_block_gpu_state_cache, gpu_state);
    block->gpus[uvm_id_gpu_index(gpu->id)] = NULL;

    return NULL;
}

NV_STATUS uvm_va_block_gpu_state_alloc(uvm_va_block_t *va_block)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_gpu_id_t gpu_id;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);

    for_each_gpu_id_in_mask(gpu_id, &va_space->registered_gpus) {
        if (!block_gpu_state_get_alloc(va_block, uvm_va_space_get_gpu(va_space, gpu_id)))
            return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

void uvm_va_block_unmap_cpu_chunk_on_gpus(uvm_va_block_t *block,
                                          uvm_cpu_chunk_t *chunk)
{
    uvm_gpu_id_t id;

    for_each_gpu_id(id) {
        if (uvm_va_block_gpu_state_get(block, id))
            cpu_chunk_remove_sysmem_gpu_mapping(chunk, block_get_gpu(block, id));
    }
}

NV_STATUS uvm_va_block_map_cpu_chunk_on_gpus(uvm_va_block_t *block,
                                             uvm_cpu_chunk_t *chunk,
                                             uvm_page_index_t page_index)
{
    NV_STATUS status;
    uvm_gpu_id_t id;
    uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(chunk);
    uvm_va_block_region_t chunk_region = uvm_va_block_chunk_region(block, chunk_size, page_index);

    // We can't iterate over va_space->registered_gpus because we might be
    // on the eviction path, which does not have the VA space lock held. We have
    // the VA block lock held however, so the gpu_states can't change.
    uvm_assert_mutex_locked(&block->lock);

    for_each_gpu_id(id) {
        uvm_gpu_t *gpu;

        if (!uvm_va_block_gpu_state_get(block, id))
            continue;

        gpu = block_get_gpu(block, id);
        status = cpu_chunk_add_sysmem_gpu_mapping(chunk, block, chunk_region.first, gpu);
        if (status != NV_OK)
            goto error;
    }

    return NV_OK;

error:
    uvm_va_block_unmap_cpu_chunk_on_gpus(block, chunk);
    return status;
}

void uvm_va_block_remove_cpu_chunks(uvm_va_block_t *va_block, uvm_va_block_region_t region)
{
    uvm_cpu_chunk_t *chunk;
    uvm_page_index_t page_index, next_page_index;
    uvm_va_block_region_t chunk_region;
    int nid;

    for_each_possible_uvm_node(nid) {
        for_each_cpu_chunk_in_block_region_safe(chunk, page_index, next_page_index, va_block, nid, region) {
            chunk_region = uvm_va_block_region(page_index, page_index + uvm_cpu_chunk_num_pages(chunk));

            uvm_page_mask_region_clear(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ], chunk_region);
            uvm_page_mask_region_clear(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE], chunk_region);
            uvm_va_block_cpu_clear_resident_region(va_block, nid, chunk_region);
            uvm_cpu_chunk_remove_from_block(va_block, nid, page_index);
            uvm_va_block_unmap_cpu_chunk_on_gpus(va_block, chunk);
            uvm_cpu_chunk_free(chunk);
        }
    }

    if (uvm_page_mask_empty(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ]))
        uvm_processor_mask_clear(&va_block->mapped, UVM_ID_CPU);

    if (uvm_page_mask_empty(uvm_va_block_resident_mask_get(va_block, UVM_ID_CPU, NUMA_NO_NODE)))
        uvm_processor_mask_clear(&va_block->resident, UVM_ID_CPU);
}

// Mark a CPU page as dirty.
static void block_mark_cpu_page_dirty(uvm_va_block_t *block, uvm_page_index_t page_index, int nid)
{
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);
    uvm_va_block_region_t chunk_region = uvm_va_block_chunk_region(block, uvm_cpu_chunk_get_size(chunk), page_index);
    uvm_cpu_chunk_mark_dirty(chunk, page_index - chunk_region.first);
}

// Mark a CPU page as clean.
static void block_mark_cpu_page_clean(uvm_va_block_t *block, uvm_page_index_t page_index, int nid)
{
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);
    uvm_va_block_region_t chunk_region = uvm_va_block_chunk_region(block, uvm_cpu_chunk_get_size(chunk), page_index);
    uvm_cpu_chunk_mark_clean(chunk, page_index - chunk_region.first);
}

// Check if a CPU page is dirty.
static bool block_cpu_page_is_dirty(uvm_va_block_t *block, uvm_page_index_t page_index, int nid)
{
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);
    uvm_va_block_region_t chunk_region = uvm_va_block_chunk_region(block, uvm_cpu_chunk_get_size(chunk), page_index);
    return uvm_cpu_chunk_is_dirty(chunk, page_index - chunk_region.first);
}

// Allocate a CPU chunk with the given properties. This may involve retrying if
// allocations fail. Allocating larger chunk sizes takes priority over
// allocating on the specified node in the following manner:

// 1. Attempt to allocate the largest chunk on nid.
// 2. If that fails attempt allocation of the largest chunk on any nid.
// 3. If that fails attempt progressively smaller allocations on any nid.
//
// Returns NV_OK on success. Returns NV_WARN_MORE_PROCESSING_REQUIRED if
// UVM_CPU_CHUNK_ALLOC_FLAGS_STRICT was ignored to successfully allocate.
static NV_STATUS block_alloc_cpu_chunk(uvm_va_block_t *block,
                                       uvm_chunk_sizes_mask_t cpu_allocation_sizes,
                                       uvm_cpu_chunk_alloc_flags_t flags,
                                       int nid,
                                       uvm_cpu_chunk_t **chunk)
{
    uvm_va_block_test_t *block_test = uvm_va_block_get_test(block);
    NV_STATUS status = NV_ERR_NO_MEMORY;
    uvm_chunk_size_t alloc_size;
    bool numa_fallback = false;

    if (block_test) {
        // Return out of memory error if the tests have requested it. As opposed
        // to other error injection settings, this one fails N times and then
        // succeeds.
        // TODO: Bug 3701182: This will print a warning in Linux kernels newer
        // than 5.16.0-rc1+.
        if (block_test->inject_cpu_chunk_allocation_error_count) {
            if (block_test->inject_cpu_chunk_allocation_error_count != ~(NvU32)0)
                block_test->inject_cpu_chunk_allocation_error_count--;
            return NV_ERR_NO_MEMORY;
        }

        if (block_test->cpu_chunk_allocation_actual_id != NUMA_NO_NODE)
            nid = block_test->cpu_chunk_allocation_actual_id;
    }

    for_each_chunk_size_rev(alloc_size, cpu_allocation_sizes) {
        status = uvm_cpu_chunk_alloc(alloc_size, flags, nid, chunk);
        if (status == NV_OK)
            break;

        if (flags & UVM_CPU_CHUNK_ALLOC_FLAGS_STRICT) {
            flags &= ~UVM_CPU_CHUNK_ALLOC_FLAGS_STRICT;
            numa_fallback = true;
            status = uvm_cpu_chunk_alloc(alloc_size, flags, NUMA_NO_NODE, chunk);
            if (status == NV_OK)
                break;
        }
    }

    UVM_ASSERT(status == NV_OK || status == NV_ERR_NO_MEMORY);

    if (numa_fallback && status == NV_OK)
        status = NV_WARN_MORE_PROCESSING_REQUIRED;

    return status;
}

// Same as block_alloc_cpu_chunk() but allocate a chunk suitable for use as
// a HMM destination page. The main difference is UVM does not own the reference
// on the struct page backing these chunks.
static NV_STATUS block_alloc_hmm_cpu_chunk(uvm_va_block_t *block,
                                           uvm_chunk_sizes_mask_t cpu_allocation_sizes,
                                           uvm_cpu_chunk_alloc_flags_t flags,
                                           int nid,
                                           uvm_cpu_chunk_t **chunk)
{
    NV_STATUS status;

    UVM_ASSERT(uvm_va_block_is_hmm(block));

    status = block_alloc_cpu_chunk(block, cpu_allocation_sizes, flags, nid, chunk);
    if (status == NV_OK)
        (*chunk)->type = UVM_CPU_CHUNK_TYPE_HMM;

    return status;
}

// Find the largest allocation size we can use for the given page_index in the
// given block. Returns the mask of possible sizes and region covered by the
// largest. Callers may also elect to use a smaller size.
static uvm_chunk_sizes_mask_t block_calculate_largest_alloc_size(uvm_va_block_t *va_block,
                                                                 uvm_page_index_t page_index,
                                                                 uvm_page_mask_t *allocated_mask,
                                                                 uvm_chunk_sizes_mask_t cpu_allocation_sizes,
                                                                 uvm_va_block_region_t *allocated_region)
{
    uvm_chunk_size_t alloc_size;
    uvm_chunk_sizes_mask_t allocation_sizes = cpu_allocation_sizes;

    for_each_chunk_size_rev(alloc_size, cpu_allocation_sizes) {
        NvU64 alloc_virt_addr;

        // Page must be aligned to the allocation size.
        alloc_virt_addr = UVM_ALIGN_DOWN(uvm_va_block_cpu_page_address(va_block, page_index), alloc_size);

        // Allocation region must fit within the VA block.
        if (!uvm_va_block_contains_address(va_block, alloc_virt_addr) ||
            !uvm_va_block_contains_address(va_block, alloc_virt_addr + alloc_size - 1)) {
            allocation_sizes &= ~alloc_size;
            continue;
        }

        *allocated_region = uvm_va_block_region_from_start_end(va_block,
                                                               alloc_virt_addr,
                                                               alloc_virt_addr + alloc_size - 1);

        // Allocation region can't overlap previously allocated regions.
        if (!uvm_page_mask_region_empty(allocated_mask, *allocated_region)) {
            allocation_sizes &= ~alloc_size;
            continue;
        }

        return allocation_sizes;
    }

    // No possible size was found.
    allocated_region->first = 0;
    allocated_region->outer = 0;

    return UVM_CHUNK_SIZE_INVALID;
}

// Handle insertion of overlapping CPU chunks.
// In cases where the kernel allocates CPU chunks on NUMA nodes that already
// have existing chunks, it's possible that the newly allocated chunk overlaps
// existing chunks.
// In such cases, the newly allocated chunk has to be appropriately split and
// only the non-overlapping subchunks inserted into the block.
// The subchunks, which are not inserted are freed.
// If there is an error during split, insertion, or mapping, any sub-chunks that
// have already been successfully inserted will remain in the block. The rest of
// the sub-chunks will be freed in order to maintain proper refcounts on the
// parent chunk.
static NV_STATUS block_populate_overlapping_cpu_chunks(uvm_va_block_t *block,
                                                       uvm_page_mask_t *node_pages_mask,
                                                       uvm_cpu_chunk_t *chunk,
                                                       uvm_page_index_t page_index)
{
    int nid = uvm_cpu_chunk_get_numa_node(chunk);
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, nid);
    uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(chunk);
    uvm_va_block_region_t chunk_region = uvm_va_block_chunk_region(block, chunk_size, page_index);
    uvm_page_index_t running_page_index;
    uvm_cpu_chunk_t **split_chunks;
    uvm_cpu_chunk_t **small_chunks = NULL;
    uvm_cpu_chunk_t *chunk_ptr;
    uvm_chunk_size_t split_size;
    size_t i;
    NV_STATUS status;

    UVM_ASSERT(IS_ALIGNED(uvm_va_block_cpu_page_address(block, page_index), chunk_size));

    // Get a mask of all the chunk pages that are not overlapping existing
    // chunks.
    uvm_page_mask_init_from_region(node_pages_mask, chunk_region, NULL);
    uvm_page_mask_andnot(node_pages_mask, node_pages_mask, &node_state->allocated);

    split_size = uvm_chunk_find_prev_size(uvm_cpu_chunk_get_allocation_sizes(), chunk_size);
    split_chunks = uvm_kvmalloc_zero((chunk_size / split_size) * sizeof(*split_chunks));
    if (!split_chunks) {
        uvm_cpu_chunk_free(chunk);
        return NV_ERR_NO_MEMORY;
    }

    if (split_size > UVM_PAGE_SIZE_4K) {
        small_chunks = uvm_kvmalloc_zero(MAX_SMALL_CHUNKS_PER_BIG_SLOT * sizeof(*small_chunks));
        if (!small_chunks) {
            uvm_kvfree(split_chunks);
            uvm_cpu_chunk_free(chunk);
            return NV_ERR_NO_MEMORY;
        }
    }

    // If we are here, we have to do at least one split.
    // We can't call any of the block_split_cpu_chunk_to_* functions since they
    // insert all of the split chunks into the block.
    // We only want to insert the sub-chunks that don't overlap. So, we have to
    // handle that by calling uvm_cpu_chunk_split() directly.
    status = uvm_cpu_chunk_split(chunk, split_chunks);
    if (status != NV_OK)
        goto done;

    // Insert all split chunks that don't overlap existing allocations.
    // Note that this handles both splitting to 64K and 4K.
    running_page_index = page_index;
    for (i = 0; i < chunk_size / split_size; i++) {
        uvm_va_block_region_t subchunk_region = uvm_va_block_chunk_region(block, split_size, running_page_index);

        // - If all the pages covered by the split chunk are missing, insert the
        //   chunk into the block.
        // - If none of the pages are missing, free the chunk.
        // - Otherwise, some of the pages covered by the chunk are missing and a
        //   second split will be needed.
        if (uvm_page_mask_region_full(node_pages_mask, subchunk_region)) {
            status = uvm_cpu_chunk_insert_in_block(block, split_chunks[i], running_page_index);
            if (status != NV_OK)
                goto done;

            // To prevent double chunk freeing on error, clear the array pointer
            // before mapping.
            chunk_ptr = split_chunks[i];
            split_chunks[i] = NULL;
            status = uvm_va_block_map_cpu_chunk_on_gpus(block, chunk_ptr, running_page_index);
            if (status != NV_OK)
                goto done;
        }
        else if (uvm_page_mask_region_empty(node_pages_mask, subchunk_region)) {
            uvm_cpu_chunk_free(split_chunks[i]);
            split_chunks[i] = NULL;
        }

        running_page_index = subchunk_region.outer;
    }

    if (split_size > UVM_PAGE_SIZE_4K) {
        // Split any 64K chunks that overlap 4K chunks.
        for (i = 0; i < chunk_size / split_size; i++) {
            size_t j;

            if (!split_chunks[i])
                continue;

            running_page_index = page_index + ((split_size * i) / PAGE_SIZE);
            status = uvm_cpu_chunk_split(split_chunks[i], small_chunks);
            if (status != NV_OK)
                goto done;

            for (j = 0; j < MAX_SMALL_CHUNKS_PER_BIG_SLOT; j++) {
                size_t chunk_num_pages = uvm_cpu_chunk_num_pages(small_chunks[j]);

                if (uvm_page_mask_test(node_pages_mask, running_page_index)) {
                    status = uvm_cpu_chunk_insert_in_block(block, small_chunks[j], running_page_index);
                    if (status != NV_OK)
                        goto done;

                    // To prevent double chunk freeing on error, clear the array pointer
                    // before mapping.
                    chunk_ptr = small_chunks[j];
                    small_chunks[j] = NULL;
                    status = uvm_va_block_map_cpu_chunk_on_gpus(block, chunk_ptr, running_page_index);
                    if (status != NV_OK)
                        goto done;
                }
                else {
                    uvm_cpu_chunk_free(small_chunks[j]);
                }

                running_page_index += chunk_num_pages;
            }
        }
    }

done:
    if (status != NV_OK) {
        // First, free any small chunks that have not been inserted.
        if (small_chunks) {
            for (i = 0; i < MAX_SMALL_CHUNKS_PER_BIG_SLOT; i++)
                uvm_cpu_chunk_free(small_chunks[i]);
        }

        // Next, free any large chunks that have not been inserted.
        for (i = 0; i < chunk_size / split_size; i++)
            uvm_cpu_chunk_free(split_chunks[i]);
    }

    uvm_kvfree(small_chunks);
    uvm_kvfree(split_chunks);
    return status;
}

// Add the already allocated chunk to the block. Note that this
// handles chunk management on failure, so the caller must not free
// the chunk on failure.
static NV_STATUS block_add_cpu_chunk(uvm_va_block_t *block,
                                     uvm_page_mask_t *node_pages_mask,
                                     uvm_cpu_chunk_t *chunk,
                                     uvm_va_block_region_t region)
{
    NV_STATUS status = NV_OK;
    int alloced_nid;
    uvm_va_block_cpu_node_state_t *node_state;
    uvm_page_index_t page_index = region.first;

    alloced_nid = uvm_cpu_chunk_get_numa_node(chunk);
    node_state = block_node_state_get(block, alloced_nid);
    if (!uvm_page_mask_region_empty(&node_state->allocated, region)) {
        // We may have ended up falling back to allocating the chunk on a
        // non-preferred node which may already had a chunk allocated on it in
        // which case we can discard the new chunk.
        if (uvm_page_mask_region_full(&node_state->allocated, region)) {
            uvm_cpu_chunk_free(chunk);
        }
        else {
            // There is no need to free the chunk on failure since
            // block_populate_overlapping_cpu_chunks() would already have
            // done it.
            status = block_populate_overlapping_cpu_chunks(block, node_pages_mask, chunk, page_index);
        }

        return status;
    }
    else {
        status = uvm_cpu_chunk_insert_in_block(block, chunk, page_index);
        if (status != NV_OK)
            goto out;

        status = uvm_va_block_map_cpu_chunk_on_gpus(block, chunk, page_index);
        if (status != NV_OK) {
            uvm_cpu_chunk_remove_from_block(block, uvm_cpu_chunk_get_numa_node(chunk), page_index);
            goto out;
        }
    }

out:
    if (status != NV_OK) {
        // We free the chunk even though it was allocated by the caller because
        // block_populate_overlapping_cpu_chunks() can fail after freeing the
        // orignal chunk so we need to do the same here.
        uvm_cpu_chunk_free(chunk);
    }

    return status;
}

// Allocates the input page in the block, if it doesn't already exist
//
// Also maps the page for physical access by all GPUs used by the block, which
// is required for IOMMU support. Skipped on GPUs without access to CPU memory.
// e.g., this happens when the Confidential Computing Feature is enabled.
static NV_STATUS block_populate_pages_cpu(uvm_va_block_t *block,
                                          uvm_page_mask_t *populate_page_mask,
                                          uvm_va_block_region_t populate_region,
                                          uvm_va_block_context_t *block_context,
                                          bool staged)
{
    NV_STATUS status = NV_OK;
    uvm_cpu_chunk_t *chunk;
    uvm_va_block_test_t *block_test = uvm_va_block_get_test(block);
    uvm_chunk_sizes_mask_t cpu_allocation_sizes = uvm_cpu_chunk_get_allocation_sizes();
    uvm_page_mask_t *resident_mask = &block_context->scratch_page_mask;
    uvm_page_mask_t *allocated_mask;
    uvm_cpu_chunk_alloc_flags_t alloc_flags = UVM_CPU_CHUNK_ALLOC_FLAGS_NONE;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    const uvm_va_policy_t *policy = uvm_va_policy_get_region(block, populate_region);
    uvm_page_index_t page_index;
    uvm_gpu_id_t id;
    int preferred_nid = block_context->make_resident.dest_nid;

    if (block_test && block_test->cpu_chunk_allocation_target_id != NUMA_NO_NODE)
        preferred_nid = block_test->cpu_chunk_allocation_target_id;

    // If the VA range has a preferred NUMA node, use it.
    if (preferred_nid == NUMA_NO_NODE)
        preferred_nid = policy->preferred_nid;

    // TODO: Bug 4158598: Using NUMA_NO_NODE for staging allocations is sub-optimal.
    if (preferred_nid != NUMA_NO_NODE) {
        uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, preferred_nid);
        allocated_mask = &node_state->allocated;
        alloc_flags |= UVM_CPU_CHUNK_ALLOC_FLAGS_STRICT;
    }
    else {
        allocated_mask = &block->cpu.allocated;
    }

    if (va_space->test.allow_allocation_from_movable)
        alloc_flags |= UVM_CPU_CHUNK_ALLOC_FLAGS_ALLOW_MOVABLE;

    // Check whether all requested pages have already been allocated.
    uvm_page_mask_init_from_region(&block_context->scratch_page_mask, populate_region, populate_page_mask);
    if (!uvm_page_mask_andnot(&block_context->scratch_page_mask,
                              &block_context->scratch_page_mask,
                              allocated_mask))
        return NV_OK;

    if (block_test) {
        if (block_test->cpu_chunk_allocation_size_mask)
            cpu_allocation_sizes &= block_test->cpu_chunk_allocation_size_mask;
    }

    uvm_page_mask_zero(resident_mask);
    for_each_id_in_mask(id, &block->resident)
        uvm_page_mask_or(resident_mask, resident_mask, uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE));

    // If the VA space has a UVM-Lite GPU registered, only PAGE_SIZE allocations
    // should be used in order to avoid extra copies due to dirty compound
    // pages. HMM va_blocks also require PAGE_SIZE allocations.
    // TODO: Bug 3368756: add support for HMM transparent huge page (THP)
    // migrations.

    if (!uvm_processor_mask_empty(&va_space->non_faultable_processors) || uvm_va_block_is_hmm(block))
        cpu_allocation_sizes = PAGE_SIZE;

    if (block_context->mm && !uvm_va_block_is_hmm(block))
        alloc_flags |= UVM_CPU_CHUNK_ALLOC_FLAGS_ACCOUNT;

    UVM_ASSERT(cpu_allocation_sizes >= PAGE_SIZE);
    UVM_ASSERT(cpu_allocation_sizes & PAGE_SIZE);

    for_each_va_block_page_in_region_mask(page_index, populate_page_mask, populate_region) {
        uvm_cpu_chunk_alloc_flags_t chunk_alloc_flags = alloc_flags;
        uvm_va_block_region_t region = populate_region;
        uvm_page_mask_t *node_pages_mask = &block_context->make_resident.node_pages_mask;
        uvm_chunk_sizes_mask_t allocation_sizes;

        if (uvm_page_mask_test(allocated_mask, page_index) ||
            uvm_va_block_cpu_is_page_resident_on(block, preferred_nid, page_index)) {
            page_index = uvm_va_block_next_unset_page_in_mask(populate_region, allocated_mask, page_index) - 1;
            continue;
        }

        allocation_sizes = block_calculate_largest_alloc_size(block,
                                                              page_index,
                                                              allocated_mask,
                                                              cpu_allocation_sizes,
                                                              &region);
        if (allocation_sizes == UVM_CHUNK_SIZE_INVALID)
            return NV_ERR_NO_MEMORY;

        // If not all pages in the allocation region are resident somewhere,
        // zero out the allocated page.
        // This could be wasteful if only a few pages in high-order
        // allocation need to be zero'ed out but the alternative is to map
        // single sub-pages one-by-one.
        if (!uvm_page_mask_region_full(resident_mask, region))
            chunk_alloc_flags |= UVM_CPU_CHUNK_ALLOC_FLAGS_ZERO;

        // Management of a page used for a staged migration is never handed off
        // to the kernel and is really just a driver managed page. Therefore
        // don't allocate a HMM chunk in this case.
        if (uvm_va_block_is_hmm(block) && !staged)
            status = block_alloc_hmm_cpu_chunk(block, allocation_sizes, chunk_alloc_flags, preferred_nid, &chunk);
        else
            status = block_alloc_cpu_chunk(block, allocation_sizes, chunk_alloc_flags, preferred_nid, &chunk);

        if (status == NV_WARN_MORE_PROCESSING_REQUIRED) {
            alloc_flags &= ~UVM_CPU_CHUNK_ALLOC_FLAGS_STRICT;
            preferred_nid = NUMA_NO_NODE;
            block_context->make_resident.dest_nid = NUMA_NO_NODE;
        }
        else if (status != NV_OK) {
            return status;
        }

        // A smaller chunk than the maximum size may have been allocated, update the region accordingly.
        region = uvm_va_block_chunk_region(block, uvm_cpu_chunk_get_size(chunk), page_index);
        status = block_add_cpu_chunk(block, node_pages_mask, chunk, region);
        if (status != NV_OK)
            return status;

        // Skip iterating over all pages covered by the allocated chunk.
        page_index = region.outer - 1;

#if UVM_IS_CONFIG_HMM()
        if (uvm_va_block_is_hmm(block) && block_context)
            block_context->hmm.dst_pfns[page_index] = migrate_pfn(page_to_pfn(chunk->page));
#endif
    }

    return NV_OK;
}

// Note this clears the block_context caller_page_mask.
NV_STATUS uvm_va_block_populate_page_cpu(uvm_va_block_t *va_block, uvm_page_index_t page_index, uvm_va_block_context_t *block_context)
{
    uvm_page_mask_t *page_mask = &block_context->caller_page_mask;

    uvm_page_mask_zero(page_mask);
    uvm_page_mask_set(page_mask, page_index);
    return block_populate_pages_cpu(va_block, page_mask, uvm_va_block_region_from_block(va_block), block_context, false);
}

// Try allocating a chunk. If eviction was required,
// NV_ERR_MORE_PROCESSING_REQUIRED will be returned since the block's lock was
// unlocked and relocked. The caller is responsible for adding the chunk to the
// retry used_chunks list.
static NV_STATUS block_alloc_gpu_chunk(uvm_va_block_t *block,
                                       uvm_va_block_retry_t *retry,
                                       uvm_gpu_t *gpu,
                                       uvm_chunk_size_t size,
                                       uvm_gpu_chunk_t **out_gpu_chunk)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_chunk_t *gpu_chunk;

    // First try getting a free chunk from previously-made allocations.
    gpu_chunk = block_retry_get_free_chunk(retry, gpu, size);
    if (!gpu_chunk) {
        uvm_va_block_test_t *block_test = uvm_va_block_get_test(block);
        if (block_test && block_test->user_pages_allocation_retry_force_count > 0) {
            // Force eviction by pretending the allocation failed with no memory
            --block_test->user_pages_allocation_retry_force_count;
            status = NV_ERR_NO_MEMORY;
        }
        else {
            // Try allocating a new one without eviction
            status = uvm_pmm_gpu_alloc_user(&gpu->pmm, 1, size, UVM_PMM_ALLOC_FLAGS_NONE, &gpu_chunk, &retry->tracker);
        }

        if (status == NV_ERR_NO_MEMORY) {
            // If that fails with no memory, try allocating with eviction and
            // return back to the caller immediately so that the operation can
            // be restarted.
            uvm_mutex_unlock(&block->lock);

            status = uvm_pmm_gpu_alloc_user(&gpu->pmm, 1, size, UVM_PMM_ALLOC_FLAGS_EVICT, &gpu_chunk, &retry->tracker);
            if (status == NV_OK) {
                block_retry_add_free_chunk(retry, gpu_chunk);
                status = NV_ERR_MORE_PROCESSING_REQUIRED;
            }

            uvm_mutex_lock(&block->lock);
            return status;
        }
        else if (status != NV_OK) {
            return status;
        }
    }

    *out_gpu_chunk = gpu_chunk;
    return NV_OK;
}

static bool block_gpu_has_page_tables(uvm_va_block_t *block, uvm_gpu_t *gpu)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);

    if (!gpu_state)
        return false;

    return gpu_state->page_table_range_4k.table  ||
           gpu_state->page_table_range_big.table ||
           gpu_state->page_table_range_2m.table;
}

// A helper to get a known-to-be-present GPU VA space given a VA block that's
// locked. In order to use this function, the caller must know that at least one
// of these conditions is true:
//
// 1) The VA space lock is held
// 2) The VA block has active page tables for the GPU
//
// If the VA space lock is held (#1), then the gpu_va_space obviously can't go
// away.
//
// On the eviction path, we don't have a lock on the VA space state. However,
// since remove_gpu_va_space walks each block to unmap the GPU and free GPU page
// tables before destroying the gpu_va_space, we're guaranteed that if this GPU
// has page tables (#2), the gpu_va_space can't go away while we're holding the
// block lock.
static uvm_gpu_va_space_t *uvm_va_block_get_gpu_va_space(uvm_va_block_t *va_block, uvm_gpu_t *gpu)
{
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    UVM_ASSERT(gpu);

    if (!block_gpu_has_page_tables(va_block, gpu))
        uvm_assert_rwsem_locked(&va_space->lock);

    UVM_ASSERT(uvm_processor_mask_test(&va_space->registered_gpu_va_spaces, gpu->id));

    gpu_va_space = va_space->gpu_va_spaces[uvm_id_gpu_index(gpu->id)];

    UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_ACTIVE);
    UVM_ASSERT(gpu_va_space->va_space == va_space);
    UVM_ASSERT(gpu_va_space->gpu == gpu);

    return gpu_va_space;
}

static bool block_gpu_supports_2m(uvm_va_block_t *block, uvm_gpu_t *gpu)
{
    uvm_gpu_va_space_t *gpu_va_space;

    // TODO: Bug 3368756: add HMM support for transparent huge page migrations.
    if (uvm_va_block_size(block) < UVM_PAGE_SIZE_2M || uvm_va_block_is_hmm(block))
        return false;

    UVM_ASSERT(uvm_va_block_size(block) == UVM_PAGE_SIZE_2M);

    gpu_va_space = uvm_va_block_get_gpu_va_space(block, gpu);
    return uvm_mmu_page_size_supported(&gpu_va_space->page_tables, UVM_PAGE_SIZE_2M);
}

NvU64 uvm_va_block_gpu_big_page_size(uvm_va_block_t *va_block, uvm_gpu_t *gpu)
{
    uvm_gpu_va_space_t *gpu_va_space;

    gpu_va_space = uvm_va_block_get_gpu_va_space(va_block, gpu);
    return gpu_va_space->page_tables.big_page_size;
}

static uvm_va_block_region_t range_big_page_region_all(NvU64 start, NvU64 end, NvU64 big_page_size)
{
    NvU64 first_addr = UVM_ALIGN_UP(start, big_page_size);
    NvU64 outer_addr = UVM_ALIGN_DOWN(end + 1, big_page_size);

    // The range must fit within a VA block
    UVM_ASSERT(UVM_VA_BLOCK_ALIGN_DOWN(start) == UVM_VA_BLOCK_ALIGN_DOWN(end));

    if (outer_addr <= first_addr)
        return uvm_va_block_region(0, 0);

    return uvm_va_block_region((first_addr - start) / PAGE_SIZE, (outer_addr - start) / PAGE_SIZE);
}

static size_t range_num_big_pages(NvU64 start, NvU64 end, NvU64 big_page_size)
{
    uvm_va_block_region_t region = range_big_page_region_all(start, end, big_page_size);
    return (size_t)uvm_div_pow2_64(uvm_va_block_region_size(region), big_page_size);
}

uvm_va_block_region_t uvm_va_block_big_page_region_all(uvm_va_block_t *va_block, NvU64 big_page_size)
{
    return range_big_page_region_all(va_block->start, va_block->end, big_page_size);
}

uvm_va_block_region_t uvm_va_block_big_page_region_subset(uvm_va_block_t *va_block,
                                                          uvm_va_block_region_t region,
                                                          NvU64 big_page_size)
{
    NvU64 start = uvm_va_block_region_start(va_block, region);
    NvU64 end = uvm_va_block_region_end(va_block, region);
    uvm_va_block_region_t big_region;

    UVM_ASSERT(start < va_block->end);
    UVM_ASSERT(end <= va_block->end);

    big_region = range_big_page_region_all(start, end, big_page_size);
    if (big_region.outer) {
        big_region.first += region.first;
        big_region.outer += region.first;
    }

    return big_region;
}

size_t uvm_va_block_num_big_pages(uvm_va_block_t *va_block, NvU64 big_page_size)
{
    return range_num_big_pages(va_block->start, va_block->end, big_page_size);
}

NvU64 uvm_va_block_big_page_addr(uvm_va_block_t *va_block, size_t big_page_index, NvU64 big_page_size)
{
    NvU64 addr = UVM_ALIGN_UP(va_block->start, big_page_size) + (big_page_index * big_page_size);
    UVM_ASSERT(addr >= va_block->start);
    UVM_ASSERT(addr < va_block->end);
    return addr;
}

uvm_va_block_region_t uvm_va_block_big_page_region(uvm_va_block_t *va_block, size_t big_page_index, NvU64 big_page_size)
{
    NvU64 page_addr = uvm_va_block_big_page_addr(va_block, big_page_index, big_page_size);

    // Assume that we don't have to handle multiple big PTEs per system page.
    // It's not terribly difficult to implement, but we don't currently have a
    // use case.
    UVM_ASSERT(big_page_size >= PAGE_SIZE);

    return uvm_va_block_region_from_start_size(va_block, page_addr, big_page_size);
}

// Returns the big page index (the bit index within
// uvm_va_block_gpu_state_t::big_ptes) corresponding to page_index. If
// page_index cannot be covered by a big PTE due to alignment or block size,
// MAX_BIG_PAGES_PER_UVM_VA_BLOCK is returned.
size_t uvm_va_block_big_page_index(uvm_va_block_t *va_block, uvm_page_index_t page_index, NvU64 big_page_size)
{
    uvm_va_block_region_t big_region_all = uvm_va_block_big_page_region_all(va_block, big_page_size);
    size_t big_index;

    // Note that this condition also handles the case of having no big pages in
    // the block, in which case .first >= .outer.
    if (page_index < big_region_all.first || page_index >= big_region_all.outer)
        return MAX_BIG_PAGES_PER_UVM_VA_BLOCK;

    big_index = (size_t)uvm_div_pow2_64((page_index - big_region_all.first) * PAGE_SIZE, big_page_size);

    UVM_ASSERT(uvm_va_block_big_page_addr(va_block, big_index, big_page_size) >= va_block->start);
    UVM_ASSERT(uvm_va_block_big_page_addr(va_block, big_index, big_page_size) + big_page_size <= va_block->end + 1);

    return big_index;
}

static void uvm_page_mask_init_from_big_ptes(uvm_va_block_t *block,
                                             uvm_gpu_t *gpu,
                                             uvm_page_mask_t *mask_out,
                                             const unsigned long *big_ptes_in)
{
    uvm_va_block_region_t big_region;
    size_t big_page_index;
    NvU64 big_page_size = uvm_va_block_gpu_big_page_size(block, gpu);

    uvm_page_mask_zero(mask_out);

    for_each_set_bit(big_page_index, big_ptes_in, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) {
        big_region = uvm_va_block_big_page_region(block, big_page_index, big_page_size);
        uvm_page_mask_region_fill(mask_out, big_region);
    }
}

NvU64 uvm_va_block_page_size_cpu(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    if (!uvm_page_mask_test(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ], page_index))
        return 0;

    UVM_ASSERT(uvm_processor_mask_test(&va_block->mapped, UVM_ID_CPU));

    // Despite the fact that physical CPU memory can be allocated at sizes
    // greater than PAGE_SIZE, vm_insert_page(s)() always maps CPU memory
    // with 4K PTEs. Until the core kernel adds support for PMD mappings,
    // the return value of this function will remain at PAGE_SIZE.
    return PAGE_SIZE;
}

NvU64 uvm_va_block_page_size_gpu(uvm_va_block_t *va_block, uvm_gpu_id_t gpu_id, uvm_page_index_t page_index)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu_id);
    size_t big_page_size, big_page_index;

    if (!gpu_state)
        return 0;

    if (!uvm_page_mask_test(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ], page_index))
        return 0;

    UVM_ASSERT(uvm_processor_mask_test(&va_block->mapped, gpu_id));

    if (gpu_state->pte_is_2m)
        return UVM_PAGE_SIZE_2M;

    big_page_size = uvm_va_block_gpu_big_page_size(va_block, block_get_gpu(va_block, gpu_id));
    big_page_index = uvm_va_block_big_page_index(va_block, page_index, big_page_size);
    if (big_page_index != MAX_BIG_PAGES_PER_UVM_VA_BLOCK && test_bit(big_page_index, gpu_state->big_ptes))
        return big_page_size;

    return UVM_PAGE_SIZE_4K;
}

// Get the size of the physical allocation backing the page, or 0 if not
// resident. Note that this is different from uvm_va_block_page_size_* because
// those return the size of the PTE which maps the page index, which may be
// smaller than the physical allocation.
static NvU64 block_phys_page_size(uvm_va_block_t *block, block_phys_page_t page)
{
    uvm_va_block_gpu_state_t *gpu_state;
    uvm_chunk_size_t chunk_size;

    if (UVM_ID_IS_CPU(page.processor)) {
        uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, page.nid, page.page_index);
        uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(block, page.processor, NUMA_NO_NODE);

        if (!uvm_page_mask_test(resident_mask, page.page_index))
            return 0;

        UVM_ASSERT(uvm_processor_mask_test(&block->resident, UVM_ID_CPU));
        return uvm_cpu_chunk_get_size(chunk);
    }

    gpu_state = uvm_va_block_gpu_state_get(block, page.processor);
    if (!gpu_state || !uvm_page_mask_test(&gpu_state->resident, page.page_index))
        return 0;

    UVM_ASSERT(uvm_processor_mask_test(&block->resident, page.processor));
    block_gpu_chunk_index(block, block_get_gpu(block, page.processor), page.page_index, &chunk_size);
    return chunk_size;
}

NvU64 uvm_va_block_get_physical_size(uvm_va_block_t *block,
                                     uvm_processor_id_t processor,
                                     uvm_page_index_t page_index)
{
    int nid = NUMA_NO_NODE;
    block_phys_page_t page;

    UVM_ASSERT(block);

    uvm_assert_mutex_locked(&block->lock);

    if (UVM_ID_IS_CPU(processor)) {
        nid = block_get_page_node_residency(block, page_index);
        if (nid == NUMA_NO_NODE)
            return 0;
    }

    page = block_phys_page(processor, nid, page_index);

    return block_phys_page_size(block, page);
}

static uvm_pte_bits_cpu_t get_cpu_pte_bit_index(uvm_prot_t prot)
{
    uvm_pte_bits_cpu_t pte_bit_index = UVM_PTE_BITS_CPU_MAX;

    // ATOMIC and WRITE are synonyms for the CPU
    if (prot == UVM_PROT_READ_WRITE_ATOMIC || prot == UVM_PROT_READ_WRITE)
        pte_bit_index = UVM_PTE_BITS_CPU_WRITE;
    else if (prot == UVM_PROT_READ_ONLY)
        pte_bit_index = UVM_PTE_BITS_CPU_READ;
    else
        UVM_ASSERT_MSG(false, "Invalid access permissions %s\n", uvm_prot_string(prot));

    return pte_bit_index;
}

static uvm_pte_bits_gpu_t get_gpu_pte_bit_index(uvm_prot_t prot)
{
    uvm_pte_bits_gpu_t pte_bit_index = UVM_PTE_BITS_GPU_MAX;

    if (prot == UVM_PROT_READ_WRITE_ATOMIC)
        pte_bit_index = UVM_PTE_BITS_GPU_ATOMIC;
    else if (prot == UVM_PROT_READ_WRITE)
        pte_bit_index = UVM_PTE_BITS_GPU_WRITE;
    else if (prot == UVM_PROT_READ_ONLY)
        pte_bit_index = UVM_PTE_BITS_GPU_READ;
    else
        UVM_ASSERT_MSG(false, "Invalid access permissions %s\n", uvm_prot_string(prot));

    return pte_bit_index;
}

uvm_page_mask_t *uvm_va_block_resident_mask_get(uvm_va_block_t *block, uvm_processor_id_t processor, int nid)
{
    uvm_va_block_gpu_state_t *gpu_state;
    uvm_page_mask_t *resident_mask;

    if (UVM_ID_IS_CPU(processor)) {
        uvm_va_block_cpu_node_state_t *node_state;

        if (nid == NUMA_NO_NODE) {
            resident_mask = &block->cpu.resident;
        }
        else {
            node_state = block_node_state_get(block, nid);
            resident_mask = &node_state->resident;
        }
    }
    else {
        gpu_state = uvm_va_block_gpu_state_get(block, processor);
        UVM_ASSERT(gpu_state);
        resident_mask = &gpu_state->resident;
    }

    return resident_mask;
}

// Get the page residency mask for a processor
//
// Notably this will allocate GPU state if not yet present and if that fails
// NULL is returned.
static uvm_page_mask_t *block_resident_mask_get_alloc(uvm_va_block_t *block, uvm_processor_id_t processor, int nid)
{
    uvm_va_block_gpu_state_t *gpu_state;

    if (UVM_ID_IS_CPU(processor))
        return uvm_va_block_resident_mask_get(block, processor, nid);

    gpu_state = block_gpu_state_get_alloc(block, block_get_gpu(block, processor));
    if (!gpu_state)
        return NULL;

    return &gpu_state->resident;
}

static const uvm_page_mask_t *block_map_with_prot_mask_get(uvm_va_block_t *block,
                                                           uvm_processor_id_t processor,
                                                           uvm_prot_t prot)
{
    uvm_va_block_gpu_state_t *gpu_state;

    if (UVM_ID_IS_CPU(processor))
        return &block->cpu.pte_bits[get_cpu_pte_bit_index(prot)];

    gpu_state = uvm_va_block_gpu_state_get(block, processor);

    UVM_ASSERT(gpu_state);
    return &gpu_state->pte_bits[get_gpu_pte_bit_index(prot)];
}

const uvm_page_mask_t *uvm_va_block_map_mask_get(uvm_va_block_t *block, uvm_processor_id_t processor)
{
    return block_map_with_prot_mask_get(block, processor, UVM_PROT_READ_ONLY);
}

void uvm_va_block_unmapped_pages_get(uvm_va_block_t *va_block,
                                     uvm_va_block_region_t region,
                                     uvm_page_mask_t *out_mask)
{
    uvm_processor_id_t id;

    uvm_assert_mutex_locked(&va_block->lock);

    if (!uvm_va_block_is_hmm(va_block)) {
        uvm_page_mask_complement(out_mask, &va_block->maybe_mapped_pages);
        return;
    }

    uvm_page_mask_region_fill(out_mask, region);

    for_each_id_in_mask(id, &va_block->mapped) {
        uvm_page_mask_andnot(out_mask, out_mask, uvm_va_block_map_mask_get(va_block, id));
    }
}

static const uvm_page_mask_t *block_evicted_mask_get(uvm_va_block_t *block, uvm_gpu_id_t gpu_id)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu_id);
    UVM_ASSERT(gpu_state);

    return &gpu_state->evicted;
}

static bool block_is_page_resident_anywhere(uvm_va_block_t *block, uvm_page_index_t page_index)
{
    uvm_processor_id_t id;
    for_each_id_in_mask(id, &block->resident) {
        if (uvm_page_mask_test(uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE), page_index))
            return true;
    }

    return false;
}

static bool block_processor_page_is_populated(uvm_va_block_t *block, uvm_processor_id_t proc, uvm_page_index_t page_index)
{
    uvm_va_block_gpu_state_t *gpu_state;
    size_t chunk_index;

    if (UVM_ID_IS_CPU(proc))
        return uvm_page_mask_test(&block->cpu.allocated, page_index);

    gpu_state = uvm_va_block_gpu_state_get(block, proc);
    if (!gpu_state)
        return false;

    chunk_index = block_gpu_chunk_index(block, block_get_gpu(block, proc), page_index, NULL);
    return gpu_state->chunks[chunk_index] != NULL;
}

// Compute the gpus that have at least the given access permissions for the
// range described by region and page_mask. The function sets the bit if any
// page in the region has the permissions.
static void block_region_authorized_gpus(uvm_va_block_t *va_block,
                                         uvm_va_block_region_t region,
                                         uvm_prot_t access_permission,
                                         uvm_processor_mask_t *authorized_gpus)
{
    uvm_gpu_id_t gpu_id;
    uvm_pte_bits_gpu_t search_gpu_bit = get_gpu_pte_bit_index(access_permission);

    uvm_processor_mask_zero(authorized_gpus);

    // Test all GPUs with mappings on the block
    for_each_gpu_id_in_mask(gpu_id, &va_block->mapped) {
        uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu_id);
        if (gpu_state && !uvm_page_mask_region_empty(&gpu_state->pte_bits[search_gpu_bit], region))
            uvm_processor_mask_set(authorized_gpus, gpu_id);
    }
}

// Compute the processors that have at least the given access permissions for
// the range described by region and page_mask. The function sets the bit if any
// page in the region has the permissions.
static void block_region_authorized_processors(uvm_va_block_t *va_block,
                                               uvm_va_block_region_t region,
                                               uvm_prot_t access_permission,
                                               uvm_processor_mask_t *authorized_processors)
{
    uvm_pte_bits_cpu_t search_cpu_bit = get_cpu_pte_bit_index(access_permission);

    // Compute GPUs
    block_region_authorized_gpus(va_block, region, access_permission, authorized_processors);

    // Test CPU
    if (uvm_processor_mask_test(&va_block->mapped, UVM_ID_CPU) &&
        !uvm_page_mask_region_empty(&va_block->cpu.pte_bits[search_cpu_bit], region)) {
        uvm_processor_mask_set(authorized_processors, UVM_ID_CPU);
    }
}

static void block_page_authorized_processors(uvm_va_block_t *va_block,
                                             uvm_page_index_t page_index,
                                             uvm_prot_t access_permission,
                                             uvm_processor_mask_t *authorized_processors)
{
    block_region_authorized_processors(va_block,
                                       uvm_va_block_region_for_page(page_index),
                                       access_permission,
                                       authorized_processors);
}

static bool block_is_gpu_authorized_on_whole_region(uvm_va_block_t *va_block,
                                                    uvm_va_block_region_t region,
                                                    uvm_gpu_id_t gpu_id,
                                                    uvm_prot_t required_prot)
{
    uvm_pte_bits_gpu_t search_gpu_bit = get_gpu_pte_bit_index(required_prot);
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu_id);

    if (!gpu_state)
        return false;

    return uvm_page_mask_region_full(&gpu_state->pte_bits[search_gpu_bit], region);
}

static bool block_is_processor_authorized_on_whole_region(uvm_va_block_t *va_block,
                                                          uvm_va_block_region_t region,
                                                          uvm_processor_id_t processor_id,
                                                          uvm_prot_t required_prot)
{
    if (UVM_ID_IS_CPU(processor_id)) {
        uvm_pte_bits_cpu_t search_cpu_bit = get_cpu_pte_bit_index(required_prot);

        return uvm_page_mask_region_full(&va_block->cpu.pte_bits[search_cpu_bit], region);
    }
    else {
        return block_is_gpu_authorized_on_whole_region(va_block, region, processor_id, required_prot);
    }
}

bool uvm_va_block_page_is_gpu_authorized(uvm_va_block_t *va_block,
                                         uvm_page_index_t page_index,
                                         uvm_gpu_id_t gpu_id,
                                         uvm_prot_t required_prot)
{
    return block_is_gpu_authorized_on_whole_region(va_block,
                                                   uvm_va_block_region_for_page(page_index),
                                                   gpu_id,
                                                   required_prot);
}

static bool block_page_is_processor_authorized(uvm_va_block_t *va_block,
                                               uvm_page_index_t page_index,
                                               uvm_processor_id_t processor_id,
                                               uvm_prot_t required_prot)
{
    return block_is_processor_authorized_on_whole_region(va_block,
                                                         uvm_va_block_region_for_page(page_index),
                                                         processor_id,
                                                         required_prot);
}

// Compute the gpus that have a copy of the given page resident in their memory
static void block_page_resident_gpus(uvm_va_block_t *va_block,
                                     uvm_page_index_t page_index,
                                     uvm_processor_mask_t *resident_gpus)
{
    uvm_gpu_id_t id;
    uvm_processor_mask_zero(resident_gpus);

    for_each_gpu_id_in_mask(id, &va_block->resident) {
        if (uvm_page_mask_test(uvm_va_block_resident_mask_get(va_block, id, NUMA_NO_NODE), page_index)) {
            UVM_ASSERT(block_processor_page_is_populated(va_block, id, page_index));
            uvm_processor_mask_set(resident_gpus, id);
        }
    }
}

// Compute the processors that have a copy of the given page resident in their
// memory.
void uvm_va_block_page_resident_processors(uvm_va_block_t *va_block,
                                           uvm_page_index_t page_index,
                                           uvm_processor_mask_t *resident_processors)
{
    block_page_resident_gpus(va_block, page_index, resident_processors);

    if (uvm_page_mask_test(uvm_va_block_resident_mask_get(va_block, UVM_ID_CPU, NUMA_NO_NODE), page_index)) {
        UVM_ASSERT(block_processor_page_is_populated(va_block, UVM_ID_CPU, page_index));
        uvm_processor_mask_set(resident_processors, UVM_ID_CPU);
    }
}

NvU32 uvm_va_block_page_resident_processors_count(uvm_va_block_t *va_block,
                                                  uvm_va_block_context_t *va_block_context,
                                                  uvm_page_index_t page_index)
{
    uvm_processor_mask_t *resident_processors = &va_block_context->scratch_processor_mask;
    uvm_va_block_page_resident_processors(va_block, page_index, resident_processors);

    return uvm_processor_mask_get_count(resident_processors);
}

uvm_processor_id_t uvm_va_block_page_get_closest_resident(uvm_va_block_t *va_block,
                                                          uvm_va_block_context_t *va_block_context,
                                                          uvm_page_index_t page_index,
                                                          uvm_processor_id_t processor)
{
    uvm_processor_id_t id;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    uvm_processor_mask_copy(&va_block_context->scratch_processor_mask, &va_block->resident);

    for_each_closest_id(id, &va_block_context->scratch_processor_mask, processor, va_space) {
        if (uvm_page_mask_test(uvm_va_block_resident_mask_get(va_block, id, NUMA_NO_NODE), page_index))
            return id;
    }

    return UVM_ID_INVALID;
}

// We don't track the specific aperture of each mapped page. Instead, we assume
// that each virtual mapping from a given processor always targets the closest
// processor on which that page is resident (with special rules for UVM-Lite).
//
// This function verifies that assumption: before a page becomes resident on a
// new location, assert that no processor has a valid mapping to a farther
// processor on that page.
static bool block_check_resident_proximity(uvm_va_block_t *block,
                                           uvm_va_block_context_t *block_context,
                                           uvm_page_index_t page_index,
                                           uvm_processor_id_t new_residency)
{
    uvm_processor_id_t mapped_id, closest_id;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_processor_mask_t *resident_procs = &block_context->scratch_processor_mask;
    const uvm_processor_mask_t *uvm_lite_gpus = block_get_uvm_lite_gpus(block);

    for_each_id_in_mask(mapped_id, &block->mapped) {
        if (uvm_processor_mask_test(uvm_lite_gpus, mapped_id))
            continue;

        if (!uvm_page_mask_test(uvm_va_block_map_mask_get(block, mapped_id), page_index))
            continue;

        uvm_va_block_page_resident_processors(block, page_index, resident_procs);
        UVM_ASSERT(!uvm_processor_mask_empty(resident_procs));
        UVM_ASSERT(!uvm_processor_mask_test(resident_procs, new_residency));
        uvm_processor_mask_set(resident_procs, new_residency);
        closest_id = uvm_processor_mask_find_closest_id(va_space, resident_procs, mapped_id);
        UVM_ASSERT(!uvm_id_equal(closest_id, new_residency));
    }

    return true;
}

// Returns the processor to which page_index should be mapped on gpu
static uvm_processor_id_t block_gpu_get_processor_to_map(uvm_va_block_t *block,
                                                         uvm_va_block_context_t *block_context,
                                                         uvm_gpu_t *gpu,
                                                         uvm_page_index_t page_index)
{
    uvm_processor_id_t dest_id;

    // UVM-Lite GPUs can only map pages on the preferred location
    if (uvm_processor_mask_test(block_get_uvm_lite_gpus(block), gpu->id))
        return uvm_va_range_get_policy(block->va_range)->preferred_location;

    // Otherwise we always map the closest resident processor
    dest_id = uvm_va_block_page_get_closest_resident(block, block_context, page_index, gpu->id);
    UVM_ASSERT(UVM_ID_IS_VALID(dest_id));
    return dest_id;
}

// Returns the processor to which page_index should be mapped on mapping_id
static uvm_processor_id_t block_get_processor_to_map(uvm_va_block_t *block,
                                                     uvm_va_block_context_t *block_context,
                                                     uvm_processor_id_t mapping_id,
                                                     uvm_page_index_t page_index)
{

    if (UVM_ID_IS_CPU(mapping_id))
        return uvm_va_block_page_get_closest_resident(block, block_context, page_index, mapping_id);

    return block_gpu_get_processor_to_map(block, block_context, block_get_gpu(block, mapping_id), page_index);
}

static void block_get_mapped_processors(uvm_va_block_t *block,
                                        uvm_va_block_context_t *block_context,
                                        uvm_processor_id_t resident_id,
                                        uvm_page_index_t page_index,
                                        uvm_processor_mask_t *mapped_procs)
{
    uvm_processor_id_t mapped_id;

    uvm_processor_mask_zero(mapped_procs);

    for_each_id_in_mask(mapped_id, &block->mapped) {
        if (uvm_page_mask_test(uvm_va_block_map_mask_get(block, mapped_id), page_index)) {
            uvm_processor_id_t to_map_id = block_get_processor_to_map(block, block_context, mapped_id, page_index);

            if (uvm_id_equal(to_map_id, resident_id))
                uvm_processor_mask_set(mapped_procs, mapped_id);
        }
    }
}

// We use block_gpu_get_processor_to_map to find the destination processor of a
// given GPU mapping. This function is called when the mapping is established to
// sanity check that the destination of the mapping matches the query.
static bool block_check_mapping_residency_region(uvm_va_block_t *block,
                                                 uvm_va_block_context_t *block_context,
                                                 uvm_gpu_t *gpu,
                                                 uvm_processor_id_t mapping_dest,
                                                 uvm_va_block_region_t region,
                                                 const uvm_page_mask_t *page_mask)
{
    uvm_page_index_t page_index;
    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        NvU64 va = uvm_va_block_cpu_page_address(block, page_index);
        uvm_processor_id_t proc_to_map = block_gpu_get_processor_to_map(block, block_context, gpu, page_index);
        UVM_ASSERT_MSG(uvm_id_equal(mapping_dest, proc_to_map),
                       "VA 0x%llx on %s: mapping %s, supposed to map %s",
                       va,
                       uvm_gpu_name(gpu),
                       block_processor_name(block, mapping_dest),
                       block_processor_name(block, proc_to_map));
    }
    return true;
}

static bool block_check_mapping_residency(uvm_va_block_t *block,
                                          uvm_va_block_context_t *block_context,
                                          uvm_gpu_t *gpu,
                                          uvm_processor_id_t mapping_dest,
                                          const uvm_page_mask_t *page_mask)
{
    return block_check_mapping_residency_region(block,
                                                block_context,
                                                gpu,
                                                mapping_dest,
                                                uvm_va_block_region_from_block(block),
                                                page_mask);
}

// Check that there are no mappings targeting resident_id from any processor in
// the block.
static bool block_check_processor_not_mapped(uvm_va_block_t *block,
                                             uvm_va_block_context_t *block_context,
                                             uvm_processor_id_t resident_id)
{
    uvm_processor_id_t mapped_id;
    uvm_page_index_t page_index;

    for_each_id_in_mask(mapped_id, &block->mapped) {
        const uvm_page_mask_t *map_mask = uvm_va_block_map_mask_get(block, mapped_id);

        for_each_va_block_page_in_mask(page_index, map_mask, block) {
            uvm_processor_id_t to_map_id = block_get_processor_to_map(block, block_context, mapped_id, page_index);
            UVM_ASSERT(!uvm_id_equal(to_map_id, resident_id));
        }
    }

    return true;
}

// Zero all pages of the newly-populated chunk which are not resident anywhere
// else in the system, adding that work to the block's tracker. In all cases,
// this function adds a dependency on passed in tracker to the block's tracker.
static NV_STATUS block_zero_new_gpu_chunk(uvm_va_block_t *block,
                                          uvm_gpu_t *gpu,
                                          uvm_gpu_chunk_t *chunk,
                                          uvm_va_block_region_t chunk_region,
                                          uvm_tracker_t *tracker)
{
    uvm_va_block_gpu_state_t *gpu_state;
    NV_STATUS status;
    uvm_gpu_address_t memset_addr_base, memset_addr;
    uvm_push_t push;
    uvm_gpu_id_t id;
    uvm_va_block_region_t subregion;
    uvm_page_mask_t *zero_mask;

    UVM_ASSERT(uvm_va_block_region_size(chunk_region) == uvm_gpu_chunk_get_size(chunk));

    if (chunk->is_zero)
        return NV_OK;

    gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    zero_mask = kmem_cache_alloc(g_uvm_page_mask_cache, NV_UVM_GFP_FLAGS);

    if (!zero_mask)
        return NV_ERR_NO_MEMORY;

    // Tradeoff: zeroing entire chunk vs zeroing only the pages needed for the
    // operation.
    //
    // We may over-zero the page with this approach. For example, we might be
    // populating a 2MB chunk because only a single page within that chunk needs
    // to be made resident. If we also zero non-resident pages outside of the
    // strict region, we could waste the effort if those pages are populated on
    // another processor later and migrated here.
    //
    // We zero all non-resident pages in the chunk anyway for two reasons:
    //
    // 1) Efficiency. It's better to do all zeros as pipelined transfers once
    //    rather than scatter them around for each populate operation.
    //
    // 2) Optimizing the common case of block_populate_gpu_chunk being called
    //    for already-populated chunks. If we zero once at initial populate, we
    //    can simply check whether the chunk is present in the array. Otherwise
    //    we'd have to recompute the "is any page resident" mask every time.

    // Roll up all pages in chunk_region which are resident somewhere
    uvm_page_mask_zero(zero_mask);
    for_each_id_in_mask(id, &block->resident)
        uvm_page_mask_or(zero_mask, zero_mask, uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE));

    // If all pages in the chunk are resident somewhere, we don't need to clear
    // anything. Just make sure the chunk is tracked properly.
    if (uvm_page_mask_region_full(zero_mask, chunk_region)) {
        status = uvm_tracker_add_tracker_safe(&block->tracker, tracker);
        goto out;
    }

    // Complement to get the pages which are not resident anywhere. These
    // are the pages which must be zeroed.
    uvm_page_mask_complement(zero_mask, zero_mask);

    memset_addr_base = uvm_gpu_address_copy(gpu, uvm_gpu_phys_address(UVM_APERTURE_VID, chunk->address));
    memset_addr = memset_addr_base;

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_GPU_INTERNAL,
                                    tracker,
                                    &push,
                                    "Zero out chunk [0x%llx, 0x%llx) for region [0x%llx, 0x%llx) in va block [0x%llx, 0x%llx)",
                                    chunk->address,
                                    chunk->address + uvm_gpu_chunk_get_size(chunk),
                                    uvm_va_block_region_start(block, chunk_region),
                                    uvm_va_block_region_end(block, chunk_region) + 1,
                                    block->start,
                                    block->end + 1);
    if (status != NV_OK)
        goto out;

    for_each_va_block_subregion_in_mask(subregion, zero_mask, chunk_region) {
        // Pipeline the memsets since they never overlap with each other
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);

        // We'll push one membar later for all memsets in this loop
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

        memset_addr.address = memset_addr_base.address + (subregion.first - chunk_region.first) * PAGE_SIZE;
        gpu->parent->ce_hal->memset_8(&push, memset_addr, 0, uvm_va_block_region_size(subregion));
    }

    // A membar from this GPU is required between this memset and any PTE write
    // pointing this or another GPU to this chunk. Otherwise an engine could
    // read the PTE then access the page before the memset write is visible to
    // that engine.
    //
    // This memset writes GPU memory, so local mappings need only a GPU-local
    // membar. We can't easily determine here whether a peer GPU will ever map
    // this page in the future, so always use a sysmembar. uvm_push_end provides
    // one by default.
    //
    // TODO: Bug 1766424: Use GPU-local membars if no peer can currently map
    //       this page. When peer access gets enabled, do a MEMBAR_SYS at that
    //       point.
    uvm_push_end(&push);
    status = uvm_tracker_add_push_safe(&block->tracker, &push);

out:
    if (zero_mask)
        kmem_cache_free(g_uvm_page_mask_cache, zero_mask);

    return status;
}

static NV_STATUS block_populate_gpu_chunk(uvm_va_block_t *block,
                                          uvm_va_block_retry_t *retry,
                                          uvm_gpu_t *gpu,
                                          size_t chunk_index,
                                          uvm_va_block_region_t chunk_region)
{
    uvm_va_block_gpu_state_t *gpu_state = block_gpu_state_get_alloc(block, gpu);
    uvm_gpu_chunk_t *chunk = NULL;
    uvm_chunk_size_t chunk_size = uvm_va_block_region_size(chunk_region);
    uvm_va_block_test_t *block_test = uvm_va_block_get_test(block);
    NV_STATUS status;

    if (!gpu_state)
        return NV_ERR_NO_MEMORY;

    uvm_assert_mutex_locked(&block->lock);
    UVM_ASSERT(chunk_index < block_num_gpu_chunks(block, gpu));
    UVM_ASSERT(chunk_size & gpu->parent->mmu_user_chunk_sizes);

    // We zero chunks as necessary at initial population, so if the chunk is
    // already populated we're done. See the comment in
    // block_zero_new_gpu_chunk.
    if (gpu_state->chunks[chunk_index])
        return NV_OK;

    UVM_ASSERT(uvm_page_mask_region_empty(&gpu_state->resident, chunk_region));

    status = block_alloc_gpu_chunk(block, retry, gpu, chunk_size, &chunk);
    if (status != NV_OK)
        return status;

    // In some configurations such as SR-IOV heavy, the chunk cannot be
    // referenced using its physical address. Create a virtual mapping.
    status = uvm_mmu_chunk_map(chunk);
    if (status != NV_OK)
        goto chunk_free;

    status = block_zero_new_gpu_chunk(block, gpu, chunk, chunk_region, &retry->tracker);
    if (status != NV_OK)
        goto chunk_unmap;

    // It is safe to modify the page index field without holding any PMM locks
    // because the chunk is pinned, which means that none of the other fields in
    // the bitmap can change.
    chunk->va_block_page_index = chunk_region.first;

    // va_block_page_index is a bitfield of size PAGE_SHIFT. Make sure at
    // compile-time that it can store VA Block page indexes.
    BUILD_BUG_ON(PAGES_PER_UVM_VA_BLOCK >= PAGE_SIZE);

    if (block_test && block_test->inject_populate_error) {
        block_test->inject_populate_error = false;

        // Use NV_ERR_MORE_PROCESSING_REQUIRED to force a retry rather than
        // causing a fatal OOM failure.
        status = NV_ERR_MORE_PROCESSING_REQUIRED;
        goto chunk_unmap;
    }

    // Record the used chunk so that it can be unpinned at the end of the whole
    // operation.
    block_retry_add_used_chunk(retry, chunk);
    gpu_state->chunks[chunk_index] = chunk;

    return NV_OK;

chunk_unmap:
    uvm_mmu_chunk_unmap(chunk, &block->tracker);

chunk_free:
    // block_zero_new_gpu_chunk may have pushed memsets on this chunk which it
    // placed in the block tracker.
    uvm_pmm_gpu_free(&gpu->pmm, chunk, &block->tracker);

    return status;
}

// Populate all chunks which cover the given region and page mask.
static NV_STATUS block_populate_pages_gpu(uvm_va_block_t *block,
                                          uvm_va_block_retry_t *retry,
                                          uvm_gpu_t *gpu,
                                          uvm_va_block_region_t region,
                                          const uvm_page_mask_t *populate_mask)
{
    uvm_va_block_region_t chunk_region, check_region;
    size_t chunk_index;
    uvm_page_index_t page_index;
    uvm_chunk_size_t chunk_size;
    NV_STATUS status;

    page_index = uvm_va_block_first_page_in_mask(region, populate_mask);
    if (page_index == region.outer)
        return NV_OK;

    chunk_index = block_gpu_chunk_index(block, gpu, page_index, &chunk_size);
    chunk_region = uvm_va_block_chunk_region(block, chunk_size, page_index);

    while (1) {
        check_region = uvm_va_block_region(max(chunk_region.first, region.first),
                                           min(chunk_region.outer, region.outer));
        page_index = uvm_va_block_first_page_in_mask(check_region, populate_mask);
        if (page_index != check_region.outer) {
            status = block_populate_gpu_chunk(block, retry, gpu, chunk_index, chunk_region);
            if (status != NV_OK)
                return status;
        }

        if (check_region.outer == region.outer)
            break;

        ++chunk_index;
        chunk_size = block_gpu_chunk_size(block, gpu, chunk_region.outer);
        chunk_region = uvm_va_block_region(chunk_region.outer, chunk_region.outer + (chunk_size / PAGE_SIZE));
    }

    return NV_OK;
}

static const uvm_processor_mask_t *block_get_can_copy_from_mask(uvm_va_block_t *block, uvm_processor_id_t from)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

    return &va_space->can_copy_from[uvm_id_value(from)];
}

static NV_STATUS block_populate_pages(uvm_va_block_t *block,
                                      uvm_va_block_retry_t *retry,
                                      uvm_va_block_context_t *block_context,
                                      uvm_processor_id_t dest_id,
                                      uvm_va_block_region_t region,
                                      const uvm_page_mask_t *page_mask)
{
    NV_STATUS status;
    const uvm_page_mask_t *resident_mask = block_resident_mask_get_alloc(block,
                                                                         dest_id,
                                                                         block_context->make_resident.dest_nid);
    uvm_page_mask_t *populate_page_mask = &block_context->make_resident.page_mask;
    uvm_page_mask_t *pages_staged = &block_context->make_resident.pages_staged;
    uvm_page_mask_t *cpu_populate_mask;
    uvm_memcg_context_t memcg_context;

    if (!resident_mask)
        return NV_ERR_NO_MEMORY;

    if (page_mask)
        uvm_page_mask_andnot(populate_page_mask, page_mask, resident_mask);
    else
        uvm_page_mask_complement(populate_page_mask, resident_mask);

    if (UVM_ID_IS_GPU(dest_id)) {
        const uvm_processor_mask_t *can_copy_from_processors;
        uvm_processor_mask_t *tmp_processor_mask;
        uvm_page_mask_t *scratch_page_mask = &block_context->scratch_page_mask;
        uvm_page_mask_t *id_resident_mask;
        uvm_processor_id_t id;

        tmp_processor_mask = uvm_processor_mask_cache_alloc();
        if (!tmp_processor_mask)
            return NV_ERR_NO_MEMORY;

        status = block_populate_pages_gpu(block, retry, block_get_gpu(block, dest_id), region, populate_page_mask);
        if (status != NV_OK) {
            uvm_processor_mask_cache_free(tmp_processor_mask);
            return status;
        }

        uvm_page_mask_zero(pages_staged);

        // Get the mask of all processors that have resident pages from which
        // the destination cannot copy directly.
        can_copy_from_processors = block_get_can_copy_from_mask(block, dest_id);
        if (!uvm_processor_mask_andnot(tmp_processor_mask, &block->resident, can_copy_from_processors)) {
            uvm_processor_mask_cache_free(tmp_processor_mask);
            return status;
        }

        // Compute the pages that will be staged through the CPU by:
        //   1. Computing all of the pages resident on the processors from which
        //      dest_id cannot directly copy.
        for_each_id_in_mask(id, tmp_processor_mask) {
            id_resident_mask = uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE);
            uvm_page_mask_and(scratch_page_mask, populate_page_mask, id_resident_mask);
            uvm_page_mask_or(pages_staged, pages_staged, scratch_page_mask);
        }

        //   2. Remove any pages in pages_staged that are on any resident processor
        //      dest_id can copy from.
        if (uvm_processor_mask_and(tmp_processor_mask, can_copy_from_processors, &block->resident)) {
            for_each_id_in_mask(id, tmp_processor_mask) {
                id_resident_mask = uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE);
                uvm_page_mask_andnot(pages_staged, pages_staged, id_resident_mask);
            }
        }

        //   3. Removing any pages not in the populate mask.
        uvm_page_mask_region_clear_outside(pages_staged, region);
        cpu_populate_mask = pages_staged;

        uvm_processor_mask_cache_free(tmp_processor_mask);
    }
    else {
        cpu_populate_mask = populate_page_mask;
    }

    uvm_memcg_context_start(&memcg_context, block_context->mm);
    status = block_populate_pages_cpu(block, cpu_populate_mask, region, block_context, UVM_ID_IS_GPU(dest_id));
    uvm_memcg_context_end(&memcg_context);
    return status;
}

static bool block_can_copy_from(uvm_va_block_t *va_block, uvm_processor_id_t from, uvm_processor_id_t to)
{
    return uvm_processor_mask_test(block_get_can_copy_from_mask(va_block, to), from);
}

// Get the chunk containing the given page, along with the offset of that page
// within the chunk.
static uvm_gpu_chunk_t *block_phys_page_chunk(uvm_va_block_t *block, block_phys_page_t block_page, size_t *chunk_offset)
{
    uvm_gpu_t *gpu = block_get_gpu(block, block_page.processor);
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, block_page.processor);
    size_t chunk_index;
    uvm_gpu_chunk_t *chunk;
    uvm_chunk_size_t chunk_size;

    UVM_ASSERT(gpu_state);

    chunk_index = block_gpu_chunk_index(block, gpu, block_page.page_index, &chunk_size);
    chunk = gpu_state->chunks[chunk_index];
    UVM_ASSERT(chunk);

    if (chunk_offset) {
        size_t page_offset = block_page.page_index -
                             uvm_va_block_chunk_region(block,chunk_size, block_page.page_index).first;
        *chunk_offset = page_offset * PAGE_SIZE;
    }

    return chunk;
}

// Get the physical GPU address of a block's page from the POV of the specified
// GPU. This is the address that should be used for making PTEs for the
// specified GPU.
static uvm_gpu_phys_address_t block_phys_page_address(uvm_va_block_t *block,
                                                      block_phys_page_t block_page,
                                                      uvm_gpu_t *gpu)
{
    uvm_va_block_gpu_state_t *accessing_gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    size_t chunk_offset;
    uvm_gpu_chunk_t *chunk;

    UVM_ASSERT(accessing_gpu_state);

    if (UVM_ID_IS_CPU(block_page.processor)) {
        uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, block_page.nid, block_page.page_index);
        NvU64 dma_addr = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu);
        uvm_va_block_region_t chunk_region = uvm_va_block_chunk_region(block,
                                                                       uvm_cpu_chunk_get_size(chunk),
                                                                       block_page.page_index);

        // The page should be mapped for physical access already as we do that
        // eagerly on CPU page population and GPU state alloc.
        UVM_ASSERT(dma_addr != 0);
        dma_addr += (block_page.page_index - chunk_region.first) * PAGE_SIZE;

        return uvm_gpu_phys_address(UVM_APERTURE_SYS, dma_addr);
    }

    chunk = block_phys_page_chunk(block, block_page, &chunk_offset);

    if (uvm_id_equal(block_page.processor, gpu->id)) {
        return uvm_gpu_phys_address(UVM_APERTURE_VID, chunk->address + chunk_offset);
    }
    else {
        uvm_gpu_phys_address_t phys_addr;
        uvm_gpu_t *owning_gpu = block_get_gpu(block, block_page.processor);
        uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

        UVM_ASSERT(uvm_va_space_peer_enabled(va_space, gpu, owning_gpu));
        phys_addr = uvm_pmm_gpu_peer_phys_address(&owning_gpu->pmm, chunk, gpu);
        phys_addr.address += chunk_offset;
        return phys_addr;
    }
}

// Get the physical GPU address of a block's page from the POV of the specified
// GPU, suitable for accessing the memory from UVM-internal CE channels.
//
// Notably this is may be different from block_phys_page_address() to handle CE
// limitations in addressing physical memory directly.
static uvm_gpu_address_t block_phys_page_copy_address(uvm_va_block_t *block,
                                                      block_phys_page_t block_page,
                                                      uvm_gpu_t *gpu)
{
    uvm_gpu_t *owning_gpu;
    size_t chunk_offset;
    uvm_gpu_chunk_t *chunk;
    uvm_gpu_address_t copy_addr;
    uvm_va_space_t *va_space;

    UVM_ASSERT_MSG(block_can_copy_from(block, gpu->id, block_page.processor),
                   "from %s to %s\n",
                   block_processor_name(block, gpu->id),
                   block_processor_name(block, block_page.processor));

    // CPU and local GPU accesses can rely on block_phys_page_address, but the
    // resulting physical address may need to be converted into virtual.
    if (UVM_ID_IS_CPU(block_page.processor) || uvm_id_equal(block_page.processor, gpu->id))
        return uvm_gpu_address_copy(gpu, block_phys_page_address(block, block_page, gpu));

    va_space = uvm_va_block_get_va_space(block);

    // See the comments on the peer_identity_mappings_supported assignments in
    // the HAL for why we disable direct copies between peers.
    owning_gpu = block_get_gpu(block, block_page.processor);

    UVM_ASSERT(uvm_va_space_peer_enabled(va_space, gpu, owning_gpu));

    chunk = block_phys_page_chunk(block, block_page, &chunk_offset);
    copy_addr = uvm_pmm_gpu_peer_copy_address(&owning_gpu->pmm, chunk, gpu);
    copy_addr.address += chunk_offset;
    return copy_addr;
}

uvm_gpu_phys_address_t uvm_va_block_res_phys_page_address(uvm_va_block_t *va_block,
                                                          uvm_page_index_t page_index,
                                                          uvm_processor_id_t residency,
                                                          uvm_gpu_t *gpu)
{
    int nid = NUMA_NO_NODE;

    uvm_assert_mutex_locked(&va_block->lock);
    if (UVM_ID_IS_CPU(residency)) {
        nid = block_get_page_node_residency(va_block, page_index);
        UVM_ASSERT(nid != NUMA_NO_NODE);
    }

    return block_phys_page_address(va_block, block_phys_page(residency, nid, page_index), gpu);
}

uvm_gpu_phys_address_t uvm_va_block_gpu_phys_page_address(uvm_va_block_t *va_block,
                                                          uvm_page_index_t page_index,
                                                          uvm_gpu_t *gpu)
{
    return uvm_va_block_res_phys_page_address(va_block, page_index, gpu->id, gpu);
}

typedef struct
{
    // Location of the memory
    uvm_processor_id_t id;

    // NUMA node ID if the processor is the CPU. Ignored otherwise.
    int nid;

    // Whether the whole block has a single physically-contiguous chunk of
    // storage on the processor.
    bool is_block_contig;

    // Starting address of the physically-contiguous allocation, from the view
    // of the copying GPU. Valid only if is_block_contig.
    uvm_gpu_address_t gpu_address;
} block_copy_addr_t;

typedef struct
{
    block_copy_addr_t src;
    block_copy_addr_t dst;
    uvm_conf_computing_dma_buffer_t *dma_buffer;
    // True if at least one CE transfer (such as a memcopy) has already been
    // pushed to the GPU during the VA block copy thus far.
    bool copy_pushed;
} block_copy_state_t;

// Begin a push appropriate for copying data from src_id processor to dst_id processor.
// One of src_id and dst_id needs to be a GPU.
static NV_STATUS block_copy_begin_push(uvm_va_block_t *va_block,
                                       block_copy_state_t *copy_state,
                                       uvm_tracker_t *tracker,
                                       uvm_push_t *push)
{
    uvm_gpu_t *gpu;
    NV_STATUS status;
    uvm_channel_type_t channel_type;
    uvm_tracker_t *tracker_ptr = tracker;
    uvm_processor_id_t dst_id = copy_state->dst.id;
    uvm_processor_id_t src_id = copy_state->src.id;
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    if (!(uvm_block_cpu_to_cpu_copy_with_ce || va_space->test.force_cpu_to_cpu_copy_with_ce) ||
        UVM_ID_IS_GPU(src_id) ||
        UVM_ID_IS_GPU(dst_id)) {
        UVM_ASSERT_MSG(!uvm_id_equal(src_id, dst_id),
                       "Unexpected copy to self, processor %s\n",
                       block_processor_name(va_block, src_id));
    }

    if (UVM_ID_IS_CPU(src_id) && UVM_ID_IS_CPU(dst_id)) {
        uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

        gpu = uvm_va_space_find_first_gpu_attached_to_cpu_node(va_space, copy_state->src.nid);
        if (!gpu)
            gpu = uvm_va_space_find_first_gpu(va_space);

        channel_type = UVM_CHANNEL_TYPE_CPU_TO_GPU;
    }
    else if (UVM_ID_IS_CPU(src_id)) {
        gpu = block_get_gpu(va_block, dst_id);
        channel_type = UVM_CHANNEL_TYPE_CPU_TO_GPU;
    }
    else if (UVM_ID_IS_CPU(dst_id)) {
        gpu = block_get_gpu(va_block, src_id);
        channel_type = UVM_CHANNEL_TYPE_GPU_TO_CPU;
    }
    else {
        // For GPU to GPU copies, prefer to "push" the data from the source as
        // that works better at least for P2P over PCI-E.
        gpu = block_get_gpu(va_block, src_id);

        channel_type = UVM_CHANNEL_TYPE_GPU_TO_GPU;
    }

    UVM_ASSERT_MSG(block_can_copy_from(va_block, gpu->id, dst_id),
                   "GPU %s dst %s src %s\n",
                   block_processor_name(va_block, gpu->id),
                   block_processor_name(va_block, dst_id),
                   block_processor_name(va_block, src_id));
    UVM_ASSERT_MSG(block_can_copy_from(va_block, gpu->id, src_id),
                   "GPU %s dst %s src %s\n",
                   block_processor_name(va_block, gpu->id),
                   block_processor_name(va_block, dst_id),
                   block_processor_name(va_block, src_id));

    if (channel_type == UVM_CHANNEL_TYPE_GPU_TO_GPU) {
        uvm_gpu_t *dst_gpu = block_get_gpu(va_block, dst_id);
        return uvm_push_begin_acquire_gpu_to_gpu(gpu->channel_manager,
                                                 dst_gpu,
                                                 tracker,
                                                 push,
                                                 "Copy from %s to %s for block [0x%llx, 0x%llx]",
                                                 block_processor_name(va_block, src_id),
                                                 block_processor_name(va_block, dst_id),
                                                 va_block->start,
                                                 va_block->end);
    }

    if (g_uvm_global.conf_computing_enabled) {
        // When Confidential Computing is enabled, additional dependencies
        // apply to the input tracker as well as the dma_buffer tracker.
        // * In the CPU to GPU case, because UVM performs CPU side
        //   crypto-operations first before the GPU copy, we both need to
        //   ensure that the dma_buffer and the input tracker are completed.
        // * In the GPU to CPU case, the GPU copy happens first, but the same
        //   principles apply. Hence, UVM acquires the input tracker and the
        //   dma buffer.
        status = uvm_tracker_overwrite_safe(&local_tracker, tracker);
        if (status != NV_OK)
            goto error;

        UVM_ASSERT(copy_state->dma_buffer == NULL);
        status = uvm_conf_computing_dma_buffer_alloc(&gpu->conf_computing.dma_buffer_pool,
                                                     &copy_state->dma_buffer,
                                                     &local_tracker);

        if (status != NV_OK)
            goto error;

        if (channel_type == UVM_CHANNEL_TYPE_CPU_TO_GPU) {
            status = uvm_tracker_wait(&local_tracker);
            if (status != NV_OK)
                goto error;
        }

        tracker_ptr = &local_tracker;
    }

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    channel_type,
                                    tracker_ptr,
                                    push,
                                    "Copy from %s to %s for block [0x%llx, 0x%llx]",
                                    block_processor_name(va_block, src_id),
                                    block_processor_name(va_block, dst_id),
                                    va_block->start,
                                    va_block->end);

error:
    // Caller is responsible for freeing the DMA buffer on error
    uvm_tracker_deinit(&local_tracker);
    return status;
}

// A page is clean iff...
// the destination is equal to the preferred location
// the source is the CPU and
// the destination is not the CPU
// the destination does not support faults/eviction and
// the CPU page is not dirty
static bool block_page_is_clean(uvm_va_block_t *block,
                                uvm_processor_id_t dst_id,
                                int dst_nid,
                                uvm_processor_id_t src_id,
                                int src_nid,
                                uvm_page_index_t page_index)
{
    return !uvm_va_block_is_hmm(block) &&
           uvm_va_policy_preferred_location_equal(uvm_va_range_get_policy(block->va_range), dst_id, dst_nid) &&
           UVM_ID_IS_CPU(src_id) &&
           !UVM_ID_IS_CPU(dst_id) &&
           !block_get_gpu(block, dst_id)->parent->isr.replayable_faults.handling &&
           !block_cpu_page_is_dirty(block, page_index, src_nid);
}

// When the destination is the CPU...
// if the source is the preferred location and NUMA node id, mark as clean
// otherwise, mark as dirty
static void block_update_page_dirty_state(uvm_va_block_t *block,
                                          uvm_processor_id_t dst_id,
                                          int dst_nid,
                                          uvm_processor_id_t src_id,
                                          int src_nid,
                                          uvm_page_index_t page_index)
{
    uvm_va_policy_t *policy;

    if (UVM_ID_IS_GPU(dst_id))
        return;

    policy = uvm_va_range_get_policy(block->va_range);
    if (uvm_va_policy_preferred_location_equal(policy, src_id, src_nid))
        block_mark_cpu_page_clean(block, page_index, dst_nid);
    else
        block_mark_cpu_page_dirty(block, page_index, dst_nid);
}

static void block_mark_memory_used(uvm_va_block_t *block, uvm_processor_id_t id)
{
    uvm_gpu_t *gpu;

    if (UVM_ID_IS_CPU(id))
        return;

    gpu = block_get_gpu(block, id);

    // If the block is of the max size and the GPU supports eviction, mark the
    // root chunk as used in PMM.
    // HMM always allocates PAGE_SIZE GPU chunks so skip HMM va_blocks.
    if (!uvm_va_block_is_hmm(block) &&
        uvm_va_block_size(block) == UVM_CHUNK_SIZE_MAX &&
        uvm_parent_gpu_supports_eviction(gpu->parent)) {
        // The chunk has to be there if this GPU is resident
        UVM_ASSERT(uvm_processor_mask_test(&block->resident, id));
        uvm_pmm_gpu_mark_root_chunk_used(&gpu->pmm, uvm_va_block_gpu_state_get(block, gpu->id)->chunks[0]);
    }
}

static void block_set_resident_processor(uvm_va_block_t *block, uvm_processor_id_t id)
{
    UVM_ASSERT(!uvm_page_mask_empty(uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE)));

    if (uvm_processor_mask_test_and_set(&block->resident, id))
        return;

    block_mark_memory_used(block, id);
}

static void block_clear_resident_processor(uvm_va_block_t *block, uvm_processor_id_t id)
{
    uvm_gpu_t *gpu;

    UVM_ASSERT(uvm_page_mask_empty(uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE)));

    if (!uvm_processor_mask_test_and_clear(&block->resident, id))
        return;

    if (UVM_ID_IS_CPU(id))
        return;

    gpu = block_get_gpu(block, id);

    // If the block is of the max size and the GPU supports eviction, mark the
    // root chunk as unused in PMM.
    if (!uvm_va_block_is_hmm(block) &&
        uvm_va_block_size(block) == UVM_CHUNK_SIZE_MAX &&
        uvm_parent_gpu_supports_eviction(gpu->parent)) {
        // The chunk may not be there any more when residency is cleared.
        uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
        if (gpu_state && gpu_state->chunks[0])
            uvm_pmm_gpu_mark_root_chunk_unused(&gpu->pmm, gpu_state->chunks[0]);
    }
}

static bool block_phys_copy_contig_check(uvm_va_block_t *block,
                                         uvm_page_index_t page_index,
                                         const uvm_gpu_address_t *base_address,
                                         uvm_processor_id_t proc_id,
                                         int nid,
                                         uvm_gpu_t *copying_gpu)
{
    uvm_gpu_address_t page_address;
    uvm_gpu_address_t contig_address = *base_address;

    contig_address.address += page_index * PAGE_SIZE;
    page_address = block_phys_page_copy_address(block, block_phys_page(proc_id, nid, page_index), copying_gpu);

    return uvm_gpu_addr_cmp(page_address, contig_address) == 0;
}

// Check if the VA block has a single physically-contiguous chunk of storage
// on the processor.
static bool is_block_phys_contig(uvm_va_block_t *block, uvm_processor_id_t id, int nid)
{
    uvm_cpu_chunk_t *chunk;

    if (UVM_ID_IS_GPU(id))
        return uvm_va_block_size(block) == block_gpu_chunk_size(block, block_get_gpu(block, id), 0);

    UVM_ASSERT(nid != NUMA_NO_NODE);
    chunk = uvm_cpu_chunk_first_in_region(block, uvm_va_block_region_from_block(block), nid, NULL);
    return chunk && (uvm_va_block_size(block) == uvm_cpu_chunk_get_size(chunk));
}

static uvm_va_block_region_t block_phys_contig_region(uvm_va_block_t *block,
                                                      uvm_page_index_t page_index,
                                                      uvm_processor_id_t resident_id,
                                                      int nid)
{
    if (UVM_ID_IS_CPU(resident_id)) {
        uvm_cpu_chunk_t *chunk;
        UVM_ASSERT(nid != NUMA_NO_NODE);
        chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);
        return uvm_cpu_chunk_block_region(block, chunk, page_index);
    }
    else {
        uvm_chunk_size_t chunk_size;
        (void)block_gpu_chunk_index(block, block_get_gpu(block, resident_id), page_index, &chunk_size);
        return uvm_va_block_chunk_region(block, chunk_size, page_index);
    }
}

// Like block_phys_page_copy_address, but uses the address cached in bca when
// possible.
static uvm_gpu_address_t block_copy_get_address(uvm_va_block_t *block,
                                                block_copy_addr_t *bca,
                                                uvm_page_index_t page_index,
                                                uvm_gpu_t *copying_gpu)
{
    if (bca->is_block_contig) {
        uvm_gpu_address_t addr = bca->gpu_address;
        addr.address += page_index * PAGE_SIZE;
        UVM_ASSERT(block_phys_copy_contig_check(block, page_index, &bca->gpu_address, bca->id, bca->nid, copying_gpu));
        return addr;
    }

    return block_phys_page_copy_address(block, block_phys_page(bca->id, bca->nid, page_index), copying_gpu);
}

// When the Confidential Computing feature is enabled, the function performs
// CPU side page encryption and GPU side decryption to the CPR.
// GPU operations respect the caller's membar previously set in the push.
static void conf_computing_block_copy_push_cpu_to_gpu(uvm_va_block_t *block,
                                                      block_copy_state_t *copy_state,
                                                      uvm_va_block_region_t region,
                                                      uvm_push_t *push)
{
    uvm_push_flag_t push_membar_flag = UVM_PUSH_FLAG_COUNT;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    uvm_page_index_t page_index = region.first;
    uvm_conf_computing_dma_buffer_t *dma_buffer = copy_state->dma_buffer;
    struct page *src_page;
    uvm_gpu_address_t staging_buffer = uvm_mem_gpu_address_virtual_kernel(dma_buffer->alloc, gpu);
    uvm_gpu_address_t auth_tag_buffer = uvm_mem_gpu_address_virtual_kernel(dma_buffer->auth_tag, gpu);
    char *cpu_auth_tag_buffer = (char *)uvm_mem_get_cpu_addr_kernel(dma_buffer->auth_tag) +
                                        (page_index * UVM_CONF_COMPUTING_AUTH_TAG_SIZE);
    uvm_gpu_address_t dst_address = block_copy_get_address(block, &copy_state->dst, page_index, gpu);
    char *cpu_va_staging_buffer = (char *)uvm_mem_get_cpu_addr_kernel(dma_buffer->alloc) + (page_index * PAGE_SIZE);
    uvm_cpu_chunk_t *chunk;
    uvm_va_block_region_t chunk_region;

    UVM_ASSERT(UVM_ID_IS_CPU(copy_state->src.id));
    UVM_ASSERT(UVM_ID_IS_GPU(copy_state->dst.id));
    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    // See comment in block_copy_begin_push.
    UVM_ASSERT(uvm_tracker_is_completed(&block->tracker));

    chunk = uvm_cpu_chunk_get_chunk_for_page(block, copy_state->src.nid, page_index);
    UVM_ASSERT(chunk);

    // The caller guarantees that all pages in region are contiguous,
    // meaning they're guaranteed to be part of the same compound page.
    chunk_region = uvm_va_block_chunk_region(block, uvm_cpu_chunk_get_size(chunk), page_index);
    UVM_ASSERT(uvm_va_block_region_contains_region(chunk_region, region));

    src_page = uvm_cpu_chunk_get_cpu_page(block, chunk, page_index);
    staging_buffer.address += page_index * PAGE_SIZE;
    auth_tag_buffer.address += page_index * UVM_CONF_COMPUTING_AUTH_TAG_SIZE;

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE))
        push_membar_flag = UVM_PUSH_FLAG_NEXT_MEMBAR_NONE;
    else if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU))
        push_membar_flag = UVM_PUSH_FLAG_NEXT_MEMBAR_GPU;

    // kmap() only guarantees PAGE_SIZE contiguity, all encryption and
    // decryption must happen on a PAGE_SIZE basis.
    for_each_va_block_page_in_region(page_index, region) {
        void *src_cpu_virt_addr;

        src_cpu_virt_addr = kmap(src_page);
        uvm_conf_computing_cpu_encrypt(push->channel,
                                       cpu_va_staging_buffer,
                                       src_cpu_virt_addr,
                                       NULL,
                                       PAGE_SIZE,
                                       cpu_auth_tag_buffer);
        kunmap(src_page);

        // All but the first decryption can be pipelined. The first decryption
        // uses the caller's pipelining settings.
        if (page_index > region.first)
            uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);

        if (page_index < (region.outer - 1))
            uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        else if (push_membar_flag != UVM_PUSH_FLAG_COUNT)
            uvm_push_set_flag(push, push_membar_flag);

        gpu->parent->ce_hal->decrypt(push, dst_address, staging_buffer, PAGE_SIZE, auth_tag_buffer);

        src_page++;
        dst_address.address += PAGE_SIZE;
        cpu_va_staging_buffer += PAGE_SIZE;
        staging_buffer.address += PAGE_SIZE;
        cpu_auth_tag_buffer += UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
        auth_tag_buffer.address += UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
    }
}

// When the Confidential Computing feature is enabled, the function performs
// GPU side page encryption. GPU operations respect the caller's membar
// previously set in the push.
static void conf_computing_block_copy_push_gpu_to_cpu(uvm_va_block_t *block,
                                                      block_copy_state_t *copy_state,
                                                      uvm_va_block_region_t region,
                                                      uvm_push_t *push)
{
    uvm_push_flag_t push_membar_flag = UVM_PUSH_FLAG_COUNT;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    uvm_page_index_t page_index = region.first;
    uvm_conf_computing_dma_buffer_t *dma_buffer = copy_state->dma_buffer;
    uvm_gpu_address_t staging_buffer = uvm_mem_gpu_address_virtual_kernel(dma_buffer->alloc, gpu);
    uvm_gpu_address_t auth_tag_buffer = uvm_mem_gpu_address_virtual_kernel(dma_buffer->auth_tag, gpu);
    uvm_gpu_address_t src_address = block_copy_get_address(block, &copy_state->src, page_index, gpu);

    UVM_ASSERT(UVM_ID_IS_GPU(copy_state->src.id));
    UVM_ASSERT(UVM_ID_IS_CPU(copy_state->dst.id));
    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    staging_buffer.address += page_index * PAGE_SIZE;
    auth_tag_buffer.address += page_index * UVM_CONF_COMPUTING_AUTH_TAG_SIZE;

    if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE))
        push_membar_flag = UVM_PUSH_FLAG_NEXT_MEMBAR_NONE;
    else if (uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU))
        push_membar_flag = UVM_PUSH_FLAG_NEXT_MEMBAR_GPU;

    // Because we use kmap() for mapping pages for CPU side
    // crypto-operations and it only guarantees PAGE_SIZE contiguity, all
    // encryptions and decryptions must happen on a PAGE_SIZE basis.
    for_each_va_block_page_in_region(page_index, region) {
        uvm_conf_computing_log_gpu_encryption(push->channel, &dma_buffer->decrypt_iv[page_index]);

        // All but the first encryption can be pipelined. The first encryption
        // uses the caller's pipelining settings.
        if (page_index > region.first)
            uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);

        if (page_index < (region.outer - 1))
            uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        else if (push_membar_flag != UVM_PUSH_FLAG_COUNT)
            uvm_push_set_flag(push, push_membar_flag);

        gpu->parent->ce_hal->encrypt(push, staging_buffer, src_address, PAGE_SIZE, auth_tag_buffer);

        src_address.address += PAGE_SIZE;
        staging_buffer.address += PAGE_SIZE;
        auth_tag_buffer.address += UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
    }

    uvm_page_mask_region_fill(&dma_buffer->encrypted_page_mask, region);
}

static NV_STATUS conf_computing_copy_pages_finish(uvm_va_block_t *block,
                                                  block_copy_state_t *copy_state,
                                                  uvm_push_t *push)
{
    NV_STATUS status;
    uvm_page_index_t page_index;
    uvm_conf_computing_dma_buffer_t *dma_buffer = copy_state->dma_buffer;
    uvm_page_mask_t *encrypted_page_mask = &dma_buffer->encrypted_page_mask;
    void *auth_tag_buffer_base = uvm_mem_get_cpu_addr_kernel(dma_buffer->auth_tag);
    void *staging_buffer_base = uvm_mem_get_cpu_addr_kernel(dma_buffer->alloc);

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    if (UVM_ID_IS_GPU(copy_state->dst.id))
        return NV_OK;

    UVM_ASSERT(UVM_ID_IS_GPU(copy_state->src.id));

    status = uvm_push_wait(push);
    if (status != NV_OK)
        return status;

    // kmap() only guarantees PAGE_SIZE contiguity, all encryption and
    // decryption must happen on a PAGE_SIZE basis.
    for_each_va_block_page_in_mask(page_index, encrypted_page_mask, block) {
        // All CPU chunks for the copy have already been allocated in
        // block_populate_pages() and copy_state has been filled in based on
        // those allocations.
        uvm_cpu_chunk_t *cpu_chunk = uvm_cpu_chunk_get_chunk_for_page(block, copy_state->dst.nid, page_index);
        struct page *dst_page = uvm_cpu_chunk_get_cpu_page(block, cpu_chunk, page_index);
        void *staging_buffer = (char *)staging_buffer_base + (page_index * PAGE_SIZE);
        void *auth_tag_buffer = (char *)auth_tag_buffer_base + (page_index * UVM_CONF_COMPUTING_AUTH_TAG_SIZE);
        void *cpu_page_address = kmap(dst_page);

        status = uvm_conf_computing_cpu_decrypt(push->channel,
                                                cpu_page_address,
                                                staging_buffer,
                                                &dma_buffer->decrypt_iv[page_index],
                                                PAGE_SIZE,
                                                auth_tag_buffer);
        kunmap(dst_page);
        if (status != NV_OK) {
            // TODO: Bug 3814087: [UVM][HCC] Handle CSL auth_tag verification
            //                    failures & other failures gracefully.
            // uvm_conf_computing_cpu_decrypt() can fail if the authentication
            // tag verification fails. May this happen, it is considered a
            // critical failure and cannot be recovered.
            uvm_global_set_fatal_error(status);
            return status;
        }
    }

    return NV_OK;
}

static void block_copy_push(uvm_va_block_t *block,
                            block_copy_state_t *copy_state,
                            uvm_va_block_region_t region,
                            uvm_push_t *push)
{
    uvm_gpu_address_t gpu_dst_address, gpu_src_address;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    // Only the first transfer is not pipelined. Since the callees observe the
    // caller's pipeline settings, pipelining must be disabled in that first
    // transfer.
    if (copy_state->copy_pushed)
        uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
    else
        UVM_ASSERT(!uvm_push_test_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED));

    uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

    if (g_uvm_global.conf_computing_enabled) {
        if (UVM_ID_IS_CPU(copy_state->src.id))
            conf_computing_block_copy_push_cpu_to_gpu(block, copy_state, region, push);
        else
            conf_computing_block_copy_push_gpu_to_cpu(block, copy_state, region, push);
    }
    else {
        gpu_dst_address = block_copy_get_address(block, &copy_state->dst, region.first, gpu);
        gpu_src_address = block_copy_get_address(block, &copy_state->src, region.first, gpu);

        gpu->parent->ce_hal->memcopy(push, gpu_dst_address, gpu_src_address, uvm_va_block_region_size(region));
    }

    copy_state->copy_pushed = true;
}

static NV_STATUS block_copy_end_push(uvm_va_block_t *block,
                                     block_copy_state_t *copy_state,
                                     uvm_tracker_t *copy_tracker,
                                     NV_STATUS push_status,
                                     uvm_push_t *push)
{
    NV_STATUS tracker_status;

    // TODO: Bug 1766424: If the destination is a GPU and the copy was done
    //       by that GPU, use a GPU-local membar if no peer can currently
    //       map this page. When peer access gets enabled, do a MEMBAR_SYS
    //       at that point.
    uvm_push_end(push);

    if ((push_status == NV_OK) && g_uvm_global.conf_computing_enabled)
        push_status = conf_computing_copy_pages_finish(block, copy_state, push);

    tracker_status = uvm_tracker_add_push_safe(copy_tracker, push);
    if (push_status == NV_OK)
        push_status = tracker_status;

    if (g_uvm_global.conf_computing_enabled) {
        uvm_tracker_t local_tracker = UVM_TRACKER_INIT();

        uvm_tracker_overwrite_with_push(&local_tracker, push);
        uvm_conf_computing_dma_buffer_free(&push->gpu->conf_computing.dma_buffer_pool,
                                           copy_state->dma_buffer,
                                           &local_tracker);
        copy_state->dma_buffer = NULL;
        uvm_tracker_deinit(&local_tracker);
    }

    return push_status;
}

// Copies use CEs if:
//   - uvm_block_cpu_to_cpu_copy_with_ce or
//     uvm_test_force_block_cpu_to_cpu_copy_with_ce are set AND there are
//     registered GPUs in the VA space.
//   - the source and destination are not the CPU.
static bool block_copy_should_use_push(uvm_va_block_t *block, block_copy_state_t *copy_state)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

    return ((uvm_block_cpu_to_cpu_copy_with_ce || va_space->test.force_cpu_to_cpu_copy_with_ce) &&
            uvm_processor_mask_get_gpu_count(&va_space->registered_gpus)) ||
        !(UVM_ID_IS_CPU(copy_state->src.id) && uvm_id_equal(copy_state->src.id, copy_state->dst.id));
}

static NV_STATUS block_copy_pages(uvm_va_block_t *va_block,
                                  block_copy_state_t *copy_state,
                                  uvm_va_block_region_t region,
                                  uvm_push_t *push)
{
    if (!block_copy_should_use_push(va_block, copy_state)) {
        uvm_cpu_chunk_t *src_chunk = uvm_cpu_chunk_get_chunk_for_page(va_block, copy_state->src.nid, region.first);
        uvm_cpu_chunk_t *dst_chunk = uvm_cpu_chunk_get_chunk_for_page(va_block, copy_state->dst.nid, region.first);
        uvm_va_block_region_t src_chunk_region = uvm_cpu_chunk_block_region(va_block, src_chunk, region.first);
        uvm_va_block_region_t dst_chunk_region = uvm_cpu_chunk_block_region(va_block, dst_chunk, region.first);
        struct page *src_chunk_page = uvm_cpu_chunk_get_cpu_page(va_block, src_chunk, src_chunk_region.first);
        struct page *dst_chunk_page = uvm_cpu_chunk_get_cpu_page(va_block, dst_chunk, dst_chunk_region.first);
        uvm_page_index_t page_index;
        NV_STATUS status;

        UVM_ASSERT(dst_chunk);
        UVM_ASSERT(uvm_cpu_chunk_get_size(src_chunk) >= uvm_va_block_region_size(region));
        UVM_ASSERT(uvm_va_block_region_size(region) <= uvm_cpu_chunk_get_size(dst_chunk));

        // CPU-to-CPU copies using memcpy() don't have any inherent ordering with
        // copies using GPU CEs. So, we have to make sure that all previously
        // submitted work is complete.
        status = uvm_tracker_wait(&va_block->tracker);
        if (status != NV_OK)
            return status;

        for_each_va_block_page_in_region(page_index, region) {
            struct page *src_page = src_chunk_page + (page_index - src_chunk_region.first);
            struct page *dst_page = dst_chunk_page + (page_index - dst_chunk_region.first);
            void *src_addr = kmap(src_page);
            void *dst_addr = kmap(dst_page);

            memcpy(dst_addr, src_addr, PAGE_SIZE);
            kunmap(src_addr);
            kunmap(dst_addr);

            if (block_cpu_page_is_dirty(va_block, page_index, copy_state->src.nid))
                block_mark_cpu_page_dirty(va_block, page_index, copy_state->dst.nid);
        }
    }
    else {
        block_copy_push(va_block, copy_state, region, push);
    }

    return NV_OK;
}

// Copies pages resident on the src_id processor to the dst_id processor
//
// The function adds the pages that were successfully copied to the output
// migrated_pages mask and returns the number of pages in copied_pages. These
// fields are reliable even if an error is returned.
//
// Acquires the block's tracker and adds all of its pushes to the copy_tracker.
static NV_STATUS block_copy_resident_pages_between(uvm_va_block_t *block,
                                                   uvm_va_block_context_t *block_context,
                                                   uvm_processor_id_t dst_id,
                                                   int dst_nid,
                                                   uvm_processor_id_t src_id,
                                                   int src_nid,
                                                   uvm_va_block_region_t region,
                                                   uvm_page_mask_t *copy_mask,
                                                   const uvm_page_mask_t *prefetch_page_mask,
                                                   uvm_va_block_transfer_mode_t transfer_mode,
                                                   uvm_page_mask_t *migrated_pages,
                                                   NvU32 *copied_pages,
                                                   uvm_tracker_t *copy_tracker)
{
    NV_STATUS status = NV_OK;
    uvm_page_mask_t *dst_resident_mask = uvm_va_block_resident_mask_get(block, dst_id, dst_nid);
    uvm_gpu_t *copying_gpu = NULL;
    uvm_push_t push;
    uvm_page_index_t page_index;
    uvm_page_index_t contig_start_index = region.outer;
    uvm_page_index_t last_index = region.outer;
    uvm_range_group_range_t *rgr = NULL;
    bool rgr_has_changed = false;
    uvm_make_resident_cause_t cause = block_context->make_resident.cause;
    uvm_make_resident_cause_t contig_cause = cause;
    const bool may_prefetch = (cause == UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT ||
                               cause == UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT ||
                               cause == UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER) && !!prefetch_page_mask;
    block_copy_state_t copy_state = {0};
    uvm_va_range_t *va_range = block->va_range;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_va_block_region_t contig_region = {0};
    NvU64 cpu_migration_begin_timestamp = 0;

    *copied_pages = 0;

    if (UVM_ID_IS_CPU(src_id))
        UVM_ASSERT(src_nid != NUMA_NO_NODE);

    if (UVM_ID_IS_CPU(dst_id))
        UVM_ASSERT(dst_nid != NUMA_NO_NODE);

    // If there are no pages to be copied, exit early
    if (!uvm_page_mask_andnot(copy_mask, copy_mask, dst_resident_mask))
        return NV_OK;

    if (migrated_pages && !uvm_page_mask_andnot(copy_mask, copy_mask, migrated_pages))
        return NV_OK;

    copy_state.src.id = src_id;
    copy_state.dst.id = dst_id;
    copy_state.src.nid = src_nid;
    copy_state.dst.nid = dst_nid;

    copy_state.src.is_block_contig = is_block_phys_contig(block, src_id, copy_state.src.nid);
    copy_state.dst.is_block_contig = is_block_phys_contig(block, dst_id, copy_state.dst.nid);

    // uvm_range_group_range_iter_first should only be called when the va_space
    // lock is held, which is always the case unless an eviction is taking
    // place.
    if (cause != UVM_MAKE_RESIDENT_CAUSE_EVICTION) {
        rgr = uvm_range_group_range_iter_first(va_space,
                                               uvm_va_block_region_start(block, region),
                                               uvm_va_block_region_end(block, region));
        rgr_has_changed = true;
    }

    // TODO: Bug 3745051: This function is complicated and needs refactoring
    for_each_va_block_page_in_region_mask(page_index, copy_mask, region) {
        NvU64 page_start = uvm_va_block_cpu_page_address(block, page_index);
        uvm_make_resident_cause_t page_cause = (may_prefetch && uvm_page_mask_test(prefetch_page_mask, page_index)) ?
                                                UVM_MAKE_RESIDENT_CAUSE_PREFETCH:
                                                cause;

        UVM_ASSERT(block_check_resident_proximity(block, block_context, page_index, dst_id));
        UVM_ASSERT(block_processor_page_is_populated(block, dst_id, page_index));

        // If we're not evicting and we're migrating away from the preferred
        // location, then we should add the range group range to the list of
        // migrated ranges in the range group. It's safe to skip this because
        // the use of range_group's migrated_ranges list is a UVM-Lite
        // optimization - eviction is not supported on UVM-Lite GPUs.
        if (cause != UVM_MAKE_RESIDENT_CAUSE_EVICTION && !uvm_va_block_is_hmm(block) &&
            uvm_va_policy_preferred_location_equal(uvm_va_range_get_policy(va_range), src_id, src_nid)) {
            // rgr_has_changed is used to minimize the number of times the
            // migrated_ranges_lock is taken. It is set to false when the range
            // group range pointed by rgr is added to the migrated_ranges list,
            // and it is just set back to true when we move to a different
            // range group range.

            // The current page could be after the end of rgr. Iterate over the
            // range group ranges until rgr's end location is greater than or
            // equal to the current page.
            while (rgr && rgr->node.end < page_start) {
                rgr = uvm_range_group_range_iter_next(va_space, rgr, uvm_va_block_region_end(block, region));
                rgr_has_changed = true;
            }

            // Check whether the current page lies within rgr. A single page
            // must entirely reside within a range group range. Since we've
            // incremented rgr until its end is higher than page_start, we now
            // check if page_start lies within rgr.
            if (rgr && rgr_has_changed && page_start >= rgr->node.start && page_start <= rgr->node.end) {
                uvm_spin_lock(&rgr->range_group->migrated_ranges_lock);
                if (list_empty(&rgr->range_group_migrated_list_node))
                    list_move_tail(&rgr->range_group_migrated_list_node, &rgr->range_group->migrated_ranges);
                uvm_spin_unlock(&rgr->range_group->migrated_ranges_lock);

                rgr_has_changed = false;
            }
        }

        // No need to copy pages that haven't changed.  Just clear residency
        // information
        if (block_page_is_clean(block, dst_id, copy_state.dst.nid, src_id, copy_state.src.nid, page_index))
            continue;

        if (last_index == region.outer) {
            // Record all processors involved in the copy.
            uvm_processor_mask_set(&block_context->make_resident.all_involved_processors, dst_id);
            uvm_processor_mask_set(&block_context->make_resident.all_involved_processors, src_id);
        }

        if (block_copy_should_use_push(block, &copy_state)) {
            if (!copying_gpu) {
                status = block_copy_begin_push(block, &copy_state, &block->tracker, &push);

                if (status != NV_OK)
                    break;

                copying_gpu = uvm_push_get_gpu(&push);

                // Ensure that there is GPU state that can be used for CPU-to-CPU copies
                if (UVM_ID_IS_CPU(dst_id) && uvm_id_equal(src_id, dst_id)) {
                    uvm_va_block_gpu_state_t *gpu_state = block_gpu_state_get_alloc(block, copying_gpu);
                    if (!gpu_state) {
                        status = NV_ERR_NO_MEMORY;
                        break;
                    }
                }

                // Record the GPU involved in the copy
                uvm_processor_mask_set(&block_context->make_resident.all_involved_processors, copying_gpu->id);

                // This function is called just once per VA block and needs to
                // receive the "main" cause for the migration (it mainly checks if
                // we are in the eviction path). Therefore, we pass cause instead
                // of contig_cause
                uvm_tools_record_block_migration_begin(block, &push, dst_id, src_id, page_start, cause);
            }
        }
        else {
            // For CPU-to-CPU copies using memcpy(), record the start of the
            // migration here. This will be reported in the migration event.
            cpu_migration_begin_timestamp = NV_GETTIME();
        }

        if (!uvm_va_block_is_hmm(block))
            block_update_page_dirty_state(block, dst_id, copy_state.dst.nid, src_id, copy_state.src.nid, page_index);

        if (last_index == region.outer) {
            bool can_cache_src_phys_addr = copy_state.src.is_block_contig;
            bool can_cache_dst_phys_addr = copy_state.dst.is_block_contig;
            contig_start_index = page_index;
            contig_cause = page_cause;

            if (block_copy_should_use_push(block, &copy_state)) {
                // When CC is enabled, transfers between GPU and CPU don't rely on
                // any GPU mapping of CPU chunks, physical or virtual.
            if (UVM_ID_IS_CPU(src_id) && g_uvm_global.conf_computing_enabled)
                    can_cache_src_phys_addr = false;

            if (UVM_ID_IS_CPU(dst_id) && g_uvm_global.conf_computing_enabled)
                    can_cache_dst_phys_addr = false;
                // Computing the physical address is a non-trivial operation and
                // seems to be a performance limiter on systems with 2 or more
                // NVLINK links. Therefore, for physically-contiguous block
                // storage, we cache the start address and compute the page address
                // using the page index.
                if (can_cache_src_phys_addr) {
                    copy_state.src.gpu_address = block_phys_page_copy_address(block,
                                                                              block_phys_page(src_id,
                                                                                              copy_state.src.nid,
                                                                                              0),
                                                                              copying_gpu);
                }
                if (can_cache_dst_phys_addr) {
                    copy_state.dst.gpu_address = block_phys_page_copy_address(block,
                                                                              block_phys_page(dst_id,
                                                                                              copy_state.dst.nid,
                                                                                              0),
                                                                              copying_gpu);
                }
            }
        }
        else if ((page_index != last_index + 1) || contig_cause != page_cause) {
            contig_region = uvm_va_block_region(contig_start_index, last_index + 1);
            UVM_ASSERT(uvm_va_block_region_contains_region(region, contig_region));

            // If both src and dst are physically-contiguous, consolidate copies
            // of contiguous pages into a single method.
            if (copy_state.src.is_block_contig && copy_state.dst.is_block_contig) {
                status = block_copy_pages(block, &copy_state, contig_region, &push);
                if (status != NV_OK)
                    break;
            }

            if (block_copy_should_use_push(block, &copy_state)) {
                uvm_perf_event_notify_migration(&va_space->perf_events,
                                                &push,
                                                block,
                                                dst_id,
                                                src_id,
                                                uvm_va_block_region_start(block, contig_region),
                                                uvm_va_block_region_size(contig_region),
                                                transfer_mode,
                                                contig_cause,
                                                &block_context->make_resident);
            }
            else {
                uvm_perf_event_notify_migration_cpu(&va_space->perf_events,
                                                    block,
                                                    copy_state.dst.nid,
                                                    copy_state.src.nid,
                                                    uvm_va_block_region_start(block, contig_region),
                                                    uvm_va_block_region_size(contig_region),
                                                    cpu_migration_begin_timestamp,
                                                    transfer_mode,
                                                    contig_cause,
                                                    &block_context->make_resident);
            }

            contig_start_index = page_index;
            contig_cause = page_cause;
        }

        if (!copy_state.src.is_block_contig || !copy_state.dst.is_block_contig) {
            status = block_copy_pages(block, &copy_state, uvm_va_block_region_for_page(page_index), &push);
            if (status != NV_OK)
                return status;
        }

        last_index = page_index;
    }

    // Copy the remaining pages
    contig_region = uvm_va_block_region(contig_start_index, last_index + 1);
    if (uvm_va_block_region_size(contig_region) && uvm_va_block_region_contains_region(region, contig_region)) {
        if (copy_state.src.is_block_contig && copy_state.dst.is_block_contig) {
            status = block_copy_pages(block, &copy_state, contig_region, &push);
            if (status != NV_OK)
                return status;
        }

        if (block_copy_should_use_push(block, &copy_state)) {
            uvm_perf_event_notify_migration(&va_space->perf_events,
                                            &push,
                                            block,
                                            dst_id,
                                            src_id,
                                            uvm_va_block_region_start(block, contig_region),
                                            uvm_va_block_region_size(contig_region),
                                            transfer_mode,
                                            contig_cause,
                                            &block_context->make_resident);
        }
        else {
            uvm_perf_event_notify_migration_cpu(&va_space->perf_events,
                                                block,
                                                copy_state.dst.nid,
                                                copy_state.src.nid,
                                                uvm_va_block_region_start(block, contig_region),
                                                uvm_va_block_region_size(contig_region),
                                                cpu_migration_begin_timestamp,
                                                transfer_mode,
                                                contig_cause,
                                                &block_context->make_resident);
        }

        if (block_copy_should_use_push(block, &copy_state) && copying_gpu)
            status = block_copy_end_push(block, &copy_state, copy_tracker, status, &push);
    }

    // Update VA block status bits
    //
    // Only update the bits for the pages that succeeded
    if (status != NV_OK)
        uvm_page_mask_region_clear(copy_mask, uvm_va_block_region(page_index, PAGES_PER_UVM_VA_BLOCK));

    *copied_pages = uvm_page_mask_weight(copy_mask);
    if (*copied_pages && migrated_pages)
        uvm_page_mask_or(migrated_pages, migrated_pages, copy_mask);

    return status;
}

static NV_STATUS block_copy_resident_pages_from(uvm_va_block_t *block,
                                                uvm_va_block_context_t *block_context,
                                                uvm_processor_id_t dst_id,
                                                uvm_processor_id_t src_id,
                                                int src_nid,
                                                uvm_va_block_region_t region,
                                                const uvm_page_mask_t *page_mask,
                                                const uvm_page_mask_t *prefetch_page_mask,
                                                uvm_va_block_transfer_mode_t transfer_mode,
                                                uvm_page_mask_t *migrated_pages,
                                                NvU32 *copied_pages_out,
                                                uvm_tracker_t *copy_tracker)
{
    uvm_page_mask_t *copy_mask = &block_context->make_resident.copy_resident_pages_mask;
    uvm_page_mask_t *src_resident_mask;
    uvm_page_mask_t *node_pages_mask = &block_context->make_resident.node_pages_mask;
    uvm_make_resident_page_tracking_t *page_tracking = &block_context->make_resident.cpu_pages_used;
    NvU32 copied_pages_from_src;
    NV_STATUS status = NV_OK;
    int dst_nid;

    src_resident_mask = uvm_va_block_resident_mask_get(block, src_id, src_nid);
    uvm_page_mask_init_from_region(copy_mask, region, src_resident_mask);

    if (page_mask)
        uvm_page_mask_and(copy_mask, copy_mask, page_mask);

    if (UVM_ID_IS_CPU(dst_id)) {
        for_each_node_mask(dst_nid, page_tracking->nodes) {
            if (!uvm_page_mask_and(node_pages_mask, copy_mask, block_tracking_node_mask_get(block_context, dst_nid)))
                continue;

            status = block_copy_resident_pages_between(block,
                                                       block_context,
                                                       dst_id,
                                                       dst_nid,
                                                       src_id,
                                                       src_nid,
                                                       region,
                                                       node_pages_mask,
                                                       prefetch_page_mask,
                                                       transfer_mode,
                                                       migrated_pages,
                                                       &copied_pages_from_src,
                                                       copy_tracker);

            *copied_pages_out += copied_pages_from_src;

            if (status != NV_OK)
                break;

            if (!uvm_page_mask_andnot(copy_mask, copy_mask, node_pages_mask))
                break;
        }
    }
    else {
        status = block_copy_resident_pages_between(block,
                                                   block_context,
                                                   dst_id,
                                                   NUMA_NO_NODE,
                                                   src_id,
                                                   src_nid,
                                                   region,
                                                   copy_mask,
                                                   prefetch_page_mask,
                                                   transfer_mode,
                                                   migrated_pages,
                                                   &copied_pages_from_src,
                                                   copy_tracker);
        *copied_pages_out += copied_pages_from_src;
    }

    return status;
}

// Copy resident pages to the destination from all source processors in the
// src_processor_mask
//
// The function adds the pages that were successfully copied to the output
// migrated_pages mask and returns the number of pages in copied_pages. These
// fields are reliable even if an error is returned.
static NV_STATUS block_copy_resident_pages_mask(uvm_va_block_t *block,
                                                uvm_va_block_context_t *block_context,
                                                uvm_processor_id_t dst_id,
                                                const uvm_processor_mask_t *src_processor_mask,
                                                uvm_va_block_region_t region,
                                                const uvm_page_mask_t *page_mask,
                                                const uvm_page_mask_t *prefetch_page_mask,
                                                uvm_va_block_transfer_mode_t transfer_mode,
                                                NvU32 max_pages_to_copy,
                                                uvm_page_mask_t *migrated_pages,
                                                NvU32 *copied_pages_out,
                                                uvm_tracker_t *tracker_out)
{
    NV_STATUS status = NV_OK;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_processor_id_t src_id;
    uvm_processor_mask_t *search_mask;

    *copied_pages_out = 0;

    search_mask = uvm_processor_mask_cache_alloc();
    if (!search_mask)
        return NV_ERR_NO_MEMORY;

    uvm_processor_mask_copy(search_mask, src_processor_mask);

    for_each_closest_id(src_id, search_mask, dst_id, va_space) {
        NV_STATUS status;

        if (UVM_ID_IS_CPU(src_id)) {
            int nid;

            for_each_possible_uvm_node(nid) {
                status = block_copy_resident_pages_from(block,
                                                        block_context,
                                                        dst_id,
                                                        src_id,
                                                        nid,
                                                        region,
                                                        page_mask,
                                                        prefetch_page_mask,
                                                        transfer_mode,
                                                        migrated_pages,
                                                        copied_pages_out,
                                                        tracker_out);

                if (status != NV_OK)
                    break;
            }
        }
        else {
            status = block_copy_resident_pages_from(block,
                                                    block_context,
                                                    dst_id,
                                                    src_id,
                                                    NUMA_NO_NODE,
                                                    region,
                                                    page_mask,
                                                    prefetch_page_mask,
                                                    transfer_mode,
                                                    migrated_pages,
                                                    copied_pages_out,
                                                    tracker_out);

        }

        UVM_ASSERT(*copied_pages_out <= max_pages_to_copy);

        if (status != NV_OK)
            break;

        // Break out once we copied max pages already
        if (*copied_pages_out == max_pages_to_copy)
            break;
    }

    uvm_processor_mask_cache_free(search_mask);
    return status;
}

static void break_read_duplication_in_region(uvm_va_block_t *block,
                                             uvm_va_block_context_t *block_context,
                                             uvm_processor_id_t dst_id,
                                             uvm_va_block_region_t region,
                                             const uvm_page_mask_t *page_mask)
{
    uvm_processor_id_t id;
    uvm_page_mask_t *break_pages_in_region = &block_context->scratch_page_mask;

    uvm_page_mask_init_from_region(break_pages_in_region, region, page_mask);

    UVM_ASSERT(
        uvm_page_mask_subset(break_pages_in_region, uvm_va_block_resident_mask_get(block, dst_id, NUMA_NO_NODE)));

    // Clear read_duplicated bit for all pages in region
    uvm_page_mask_andnot(&block->read_duplicated_pages, &block->read_duplicated_pages, break_pages_in_region);

    // Clear residency bits for all processors other than dst_id
    for_each_id_in_mask(id, &block->resident) {
        uvm_page_mask_t *other_resident_mask;

        // Skip the destination processor, unless it's the CPU and a specific
        // NUMA node is the target destination. This is because CPU-to-CPU
        // migrations will switch the residency from one NUMA node to another
        // but the resident processor will remain the CPU.
        if (uvm_id_equal(id, dst_id) &&
            (!UVM_ID_IS_CPU(dst_id) || block_context->make_resident.dest_nid == NUMA_NO_NODE))
            continue;

        if (UVM_ID_IS_CPU(id)) {
            uvm_va_block_cpu_clear_resident_all_chunks(block, block_context, break_pages_in_region);
            other_resident_mask = uvm_va_block_resident_mask_get(block, UVM_ID_CPU, NUMA_NO_NODE);
        }
        else {
            other_resident_mask = uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE);
            uvm_page_mask_andnot(other_resident_mask, other_resident_mask, break_pages_in_region);
        }

        if (uvm_page_mask_empty(other_resident_mask))
            block_clear_resident_processor(block, id);
    }
}

static void block_copy_set_first_touch_residency(uvm_va_block_t *block,
                                                 uvm_va_block_context_t *block_context,
                                                 uvm_processor_id_t dst_id,
                                                 uvm_va_block_region_t region,
                                                 const uvm_page_mask_t *page_mask)
{
    uvm_page_index_t page_index;
    uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(block, dst_id, NUMA_NO_NODE);
    uvm_page_mask_t *first_touch_mask = &block_context->make_resident.page_mask;

    if (page_mask)
        uvm_page_mask_andnot(first_touch_mask, page_mask, resident_mask);
    else
        uvm_page_mask_complement(first_touch_mask, resident_mask);

    uvm_page_mask_region_clear_outside(first_touch_mask, region);

    for_each_va_block_page_in_mask(page_index, first_touch_mask, block) {
        UVM_ASSERT(!block_is_page_resident_anywhere(block, page_index));
        UVM_ASSERT(block_processor_page_is_populated(block, dst_id, page_index));
        UVM_ASSERT(block_check_resident_proximity(block, block_context, page_index, dst_id));
    }

    if (UVM_ID_IS_CPU(dst_id)) {
        uvm_va_block_cpu_set_resident_all_chunks(block, block_context, first_touch_mask);
        resident_mask = uvm_va_block_resident_mask_get(block, UVM_ID_CPU, NUMA_NO_NODE);
    }
    else {
        uvm_page_mask_or(resident_mask, resident_mask, first_touch_mask);
    }

    if (!uvm_page_mask_empty(resident_mask))
        block_set_resident_processor(block, dst_id);

    // Add them to the output mask, too
    uvm_page_mask_or(&block_context->make_resident.pages_changed_residency,
                     &block_context->make_resident.pages_changed_residency,
                     first_touch_mask);
}

// Select the set of CPU pages to be used for the migration. The pages selected
// could be used for either CPU destination pages (when the destination of the
// migration is the CPU) or staging pages (when the migration to the destination
// processor requires staging through the CPU).
static void block_select_cpu_node_pages(uvm_va_block_t *block,
                                        uvm_va_block_context_t *block_context,
                                        const uvm_page_mask_t *page_mask,
                                        uvm_va_block_region_t region)
{
    uvm_va_block_cpu_node_state_t *node_state;
    uvm_make_resident_page_tracking_t *tracking = &block_context->make_resident.cpu_pages_used;
    uvm_page_mask_t *scratch_page_mask = &block_context->scratch_page_mask;
    uvm_page_mask_t *node_mask;
    int nid;

    if (uvm_page_mask_empty(page_mask))
        return;

    block_context->scratch_node_mask = node_possible_map;
    uvm_page_mask_init_from_region(scratch_page_mask, region, page_mask);

    for_each_closest_uvm_node(nid, uvm_va_block_context_get_node(block_context), block_context->scratch_node_mask) {
        node_state = block_node_state_get(block, nid);
        node_mask = block_tracking_node_mask_get(block_context, nid);
        if (uvm_page_mask_and(node_mask, scratch_page_mask, &node_state->allocated)) {
            node_set(nid, tracking->nodes);
            if (!uvm_page_mask_andnot(scratch_page_mask, scratch_page_mask, node_mask))
                return;
        }
    }
}

// Copy resident pages from other processors to the destination.
// All the pages on the destination need to be populated by the caller first.
// Pages not resident anywhere else need to be zeroed out as well.
// The transfer_mode is only used to tell uvm_perf_event_notify_migration()
// whether the copy is for a migration or read duplication.
static NV_STATUS block_copy_resident_pages(uvm_va_block_t *block,
                                           uvm_va_block_context_t *block_context,
                                           uvm_processor_id_t dst_id,
                                           uvm_va_block_region_t region,
                                           const uvm_page_mask_t *page_mask,
                                           const uvm_page_mask_t *prefetch_page_mask,
                                           uvm_va_block_transfer_mode_t transfer_mode)
{
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(block,
                                                                    dst_id,
                                                                    block_context->make_resident.dest_nid);
    NvU32 missing_pages_count;
    NvU32 pages_copied;
    NvU32 pages_copied_to_cpu = 0;
    uvm_processor_mask_t *src_processor_mask = NULL;
    uvm_page_mask_t *copy_page_mask = &block_context->make_resident.page_mask;
    uvm_page_mask_t *migrated_pages = &block_context->make_resident.pages_migrated;
    uvm_page_mask_t *pages_staged = &block_context->make_resident.pages_staged;
    uvm_page_mask_t *cpu_page_mask;
    uvm_page_mask_t *numa_resident_pages;
    int nid;

    uvm_page_mask_zero(migrated_pages);

    if (page_mask)
        uvm_page_mask_andnot(copy_page_mask, page_mask, resident_mask);
    else
        uvm_page_mask_complement(copy_page_mask, resident_mask);

    missing_pages_count = uvm_page_mask_region_weight(copy_page_mask, region);

    if (missing_pages_count == 0)
        goto out;

    src_processor_mask = uvm_processor_mask_cache_alloc();
    if (!src_processor_mask) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    // TODO: Bug 1753731: Add P2P2P copies staged through a GPU
    // TODO: Bug 1753731: When a page is resident in multiple locations due to
    //       read-duplication, spread out the source of the copy so we don't
    //       bottleneck on a single location.

    uvm_processor_mask_zero(src_processor_mask);

    if (UVM_ID_IS_GPU(dst_id)) {
        // If the destination is a GPU, first copy everything from processors
        // with copy access supported. Notably this will copy pages from the CPU
        // as well even if later some extra copies from CPU are required for
        // staged copies.
        uvm_processor_mask_and(src_processor_mask, block_get_can_copy_from_mask(block, dst_id), &block->resident);
        uvm_processor_mask_clear(src_processor_mask, dst_id);

        cpu_page_mask = pages_staged;
    }
    else {
        cpu_page_mask = copy_page_mask;
    }

    block_select_cpu_node_pages(block, block_context, cpu_page_mask, region);

    if (UVM_ID_IS_GPU(dst_id)) {
        status = block_copy_resident_pages_mask(block,
                                                block_context,
                                                dst_id,
                                                src_processor_mask,
                                                region,
                                                copy_page_mask,
                                                prefetch_page_mask,
                                                transfer_mode,
                                                missing_pages_count,
                                                migrated_pages,
                                                &pages_copied,
                                                &local_tracker);

        UVM_ASSERT(missing_pages_count >= pages_copied);
        missing_pages_count -= pages_copied;

        if (status != NV_OK)
            goto out;

        if (missing_pages_count == 0) {
            UVM_ASSERT(uvm_page_mask_empty(pages_staged));
            goto out;
        }

        if (pages_copied)
            uvm_page_mask_andnot(copy_page_mask, copy_page_mask, migrated_pages);
    }

    // Now copy from everywhere else to the CPU. This is both for when the
    // destination is the CPU (src_processor_mask empty) and for a staged copy
    // (src_processor_mask containing processors with copy access to dst_id).
    uvm_processor_mask_andnot(src_processor_mask, &block->resident, src_processor_mask);

    // If the destination is the CPU but not all pages are resident on the
    // destination NUMA node, the CPU is still a source.
    numa_resident_pages = uvm_va_block_resident_mask_get(block, UVM_ID_CPU, block_context->make_resident.dest_nid);
    if (!UVM_ID_IS_CPU(dst_id) || uvm_page_mask_subset(copy_page_mask, numa_resident_pages)) {
        uvm_processor_mask_clear(src_processor_mask, dst_id);
        uvm_processor_mask_clear(src_processor_mask, UVM_ID_CPU);
    }


    if (!uvm_page_mask_empty(cpu_page_mask)) {
        status = block_copy_resident_pages_mask(block,
                                                block_context,
                                                UVM_ID_CPU,
                                                src_processor_mask,
                                                region,
                                                cpu_page_mask,
                                                prefetch_page_mask,
                                                transfer_mode,
                                                missing_pages_count,
                                                UVM_ID_IS_CPU(dst_id) ? migrated_pages : NULL,
                                                &pages_copied_to_cpu,
                                                &local_tracker);

        if (status != NV_OK)
            goto out;
    }

    // If destination is the CPU then we copied everything there above
    if (!UVM_ID_IS_GPU(dst_id))
        goto out;

    // Add everything to the block's tracker so that the
    // block_copy_resident_pages_between() call below will acquire it.
    status = uvm_tracker_add_tracker_safe(&block->tracker, &local_tracker);
    if (status != NV_OK)
        goto out;
    uvm_tracker_clear(&local_tracker);

    // Now copy staged pages from the CPU to the destination.
    // The staging copy above could have allocated pages on any NUMA node.
    // Loop over all nodes where pages were allocated and copy from those
    // nodes.
    pages_copied = 0;
    for_each_node_mask(nid, block_context->make_resident.cpu_pages_used.nodes) {
        NvU32 pages_copied_from_node;
        uvm_page_mask_t *node_pages_mask = &block_context->make_resident.node_pages_mask;
        uvm_page_mask_t *node_alloc_mask = block_tracking_node_mask_get(block_context, nid);

        if (uvm_page_mask_and(node_pages_mask, pages_staged, node_alloc_mask)) {
            status = block_copy_resident_pages_between(block,
                                                       block_context,
                                                       dst_id,
                                                       NUMA_NO_NODE,
                                                       UVM_ID_CPU,
                                                       nid,
                                                       region,
                                                       node_pages_mask,
                                                       prefetch_page_mask,
                                                       transfer_mode,
                                                       migrated_pages,
                                                       &pages_copied_from_node,
                                                       &local_tracker);
            UVM_ASSERT(missing_pages_count >= pages_copied_from_node);
            missing_pages_count -= pages_copied_from_node;
            pages_copied += pages_copied_from_node;
        }

        if (status != NV_OK)
            break;
    }

    if (status != NV_OK)
        goto out;

    // If we get here, that means we were staging the copy through the CPU and
    // we should copy as many pages from the CPU as we copied to the CPU.
    UVM_ASSERT(pages_copied == pages_copied_to_cpu);

out:
    // Add everything from the local tracker to the block's tracker.
    // Notably this is also needed for handling
    // block_copy_resident_pages_between() failures in the first loop.
    tracker_status = uvm_tracker_add_tracker_safe(&block->tracker, &local_tracker);
    uvm_tracker_deinit(&local_tracker);
    uvm_processor_mask_cache_free(src_processor_mask);

    return status == NV_OK ? tracker_status : status;
}

NV_STATUS uvm_va_block_make_resident_copy(uvm_va_block_t *va_block,
                                          uvm_va_block_retry_t *va_block_retry,
                                          uvm_va_block_context_t *va_block_context,
                                          uvm_processor_id_t dest_id,
                                          uvm_va_block_region_t region,
                                          const uvm_page_mask_t *page_mask,
                                          const uvm_page_mask_t *prefetch_page_mask,
                                          uvm_make_resident_cause_t cause)
{
    NV_STATUS status = NV_OK;
    uvm_processor_mask_t *unmap_processor_mask;
    uvm_page_mask_t *unmap_page_mask = &va_block_context->make_resident.page_mask;
    uvm_page_mask_t *resident_mask;

    va_block_context->make_resident.dest_id = dest_id;
    va_block_context->make_resident.cause = cause;
    nodes_clear(va_block_context->make_resident.cpu_pages_used.nodes);

    if (prefetch_page_mask) {
        UVM_ASSERT(cause == UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT ||
                   cause == UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT ||
                   cause == UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER);
    }

    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(uvm_va_block_is_hmm(va_block) || va_block->va_range->type == UVM_VA_RANGE_TYPE_MANAGED);

    unmap_processor_mask = uvm_processor_mask_cache_alloc();
    if (!unmap_processor_mask) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    resident_mask = block_resident_mask_get_alloc(va_block, dest_id, va_block_context->make_resident.dest_nid);
    if (!resident_mask) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    // Unmap all mapped processors except for UVM-Lite GPUs as their mappings
    // are largely persistent.
    uvm_processor_mask_andnot(unmap_processor_mask, &va_block->mapped, block_get_uvm_lite_gpus(va_block));

    if (page_mask)
        uvm_page_mask_andnot(unmap_page_mask, page_mask, resident_mask);
    else
        uvm_page_mask_complement(unmap_page_mask, resident_mask);
    uvm_page_mask_region_clear_outside(unmap_page_mask, region);

    // Unmap all pages not resident on the destination
    status = uvm_va_block_unmap_mask(va_block, va_block_context, unmap_processor_mask, region, unmap_page_mask);
    if (status != NV_OK)
        goto out;

    if (page_mask)
        uvm_page_mask_and(unmap_page_mask, page_mask, &va_block->read_duplicated_pages);
    else
        uvm_page_mask_init_from_region(unmap_page_mask, region, &va_block->read_duplicated_pages);
    uvm_page_mask_region_clear_outside(unmap_page_mask, region);

    // Also unmap read-duplicated pages excluding dest_id
    uvm_processor_mask_clear(unmap_processor_mask, dest_id);
    status = uvm_va_block_unmap_mask(va_block, va_block_context, unmap_processor_mask, region, unmap_page_mask);
    if (status != NV_OK)
        goto out;

    uvm_tools_record_read_duplicate_invalidate(va_block,
                                               dest_id,
                                               region,
                                               unmap_page_mask);

    // Note that block_populate_pages and block_copy_resident_pages also use
    // va_block_context->make_resident.page_mask.
    unmap_page_mask = NULL;

    status = block_populate_pages(va_block, va_block_retry, va_block_context, dest_id, region, page_mask);
    if (status != NV_OK)
        goto out;

    status = block_copy_resident_pages(va_block,
                                       va_block_context,
                                       dest_id,
                                       region,
                                       page_mask,
                                       prefetch_page_mask,
                                       UVM_VA_BLOCK_TRANSFER_MODE_MOVE);

out:
    uvm_processor_mask_cache_free(unmap_processor_mask);
    return status;
}

static void block_make_resident_clear_evicted(uvm_va_block_t *va_block,
                                              uvm_processor_id_t dst_id,
                                              uvm_page_mask_t *page_mask)
{
    uvm_va_block_gpu_state_t *dst_gpu_state = uvm_va_block_gpu_state_get(va_block, dst_id);

    UVM_ASSERT(dst_gpu_state);

    if (!uvm_page_mask_andnot(&dst_gpu_state->evicted, &dst_gpu_state->evicted, page_mask))
        uvm_processor_mask_clear(&va_block->evicted_gpus, dst_id);
}

static void block_make_resident_update_state(uvm_va_block_t *va_block,
                                             uvm_va_block_context_t *va_block_context,
                                             uvm_processor_id_t dst_id,
                                             uvm_va_block_region_t region,
                                             uvm_page_mask_t *copy_mask,
                                             uvm_make_resident_cause_t cause)
{
    if (UVM_ID_IS_CPU(dst_id)) {
        // CPU chunks may not have been allocated on the preferred NUMA node. So,
        // the residency has to be updated based on the chunk's NUMA ID.
        uvm_va_block_cpu_set_resident_all_chunks(va_block, va_block_context, copy_mask);
    }
    else {
        uvm_page_mask_t *dst_resident_mask = uvm_va_block_resident_mask_get(va_block, dst_id, NUMA_NO_NODE);

        uvm_page_mask_or(dst_resident_mask, dst_resident_mask, copy_mask);
    }

    block_set_resident_processor(va_block, dst_id);

    // Accumulate the pages that migrated into the output mask.
    uvm_page_mask_or(&va_block_context->make_resident.pages_changed_residency,
                     &va_block_context->make_resident.pages_changed_residency,
                     copy_mask);

    // Any move operation implies that mappings have been removed from all
    // non-UVM-Lite GPUs.
    if (!uvm_va_block_is_hmm(va_block))
        uvm_page_mask_andnot(&va_block->maybe_mapped_pages, &va_block->maybe_mapped_pages, copy_mask);

    // If we are migrating due to an eviction, set the GPU as evicted and
    // mark the evicted pages. If we are migrating away from the CPU this
    // means that those pages are not evicted.
    if (cause == UVM_MAKE_RESIDENT_CAUSE_EVICTION) {
        uvm_processor_id_t src_id;

        UVM_ASSERT(UVM_ID_IS_CPU(dst_id));

        // Note that the destination is the CPU so this loop excludes it.
        for_each_gpu_id_in_mask(src_id, &va_block_context->make_resident.all_involved_processors) {
            uvm_va_block_gpu_state_t *src_gpu_state = uvm_va_block_gpu_state_get(va_block, src_id);

            UVM_ASSERT(src_gpu_state);

            uvm_page_mask_or(&src_gpu_state->evicted, &src_gpu_state->evicted, copy_mask);
            uvm_processor_mask_set(&va_block->evicted_gpus, src_id);
        }
    }
    else if (UVM_ID_IS_GPU(dst_id) && uvm_processor_mask_test(&va_block->evicted_gpus, dst_id))
        block_make_resident_clear_evicted(va_block, dst_id, copy_mask);
}

void uvm_va_block_make_resident_finish(uvm_va_block_t *va_block,
                                       uvm_va_block_context_t *va_block_context,
                                       uvm_va_block_region_t region,
                                       const uvm_page_mask_t *page_mask)
{
    uvm_page_mask_t *migrated_pages = &va_block_context->make_resident.pages_migrated;
    uvm_processor_id_t dst_id = va_block_context->make_resident.dest_id;

    uvm_assert_mutex_locked(&va_block->lock);

    if (page_mask)
        uvm_page_mask_and(migrated_pages, migrated_pages, page_mask);

    if (!uvm_page_mask_empty(migrated_pages)) {
        // The migrated pages are now resident on the destination.
        block_make_resident_update_state(va_block,
                                         va_block_context,
                                         dst_id,
                                         region,
                                         migrated_pages,
                                         va_block_context->make_resident.cause);
    }

    // Pages that weren't resident anywhere else were populated at the
    // destination directly. Mark them as resident now.
    block_copy_set_first_touch_residency(va_block, va_block_context, dst_id, region, page_mask);

    // Break read duplication and clear residency from other processors.
    break_read_duplication_in_region(va_block, va_block_context, dst_id, region, page_mask);

    // Update eviction heuristics, if needed. Notably this could repeat the call
    // done in block_set_resident_processor(), but that doesn't do anything bad
    // and it's simpler to keep it in both places.
    //
    // Skip this if we didn't do anything (the input region and/or page mask was
    // empty).
    if (uvm_processor_mask_test(&va_block->resident, dst_id))
        block_mark_memory_used(va_block, dst_id);

    // Check state of all chunks after residency change.
    // TODO: Bug 4207783: Check both CPU and GPU chunks.
    UVM_ASSERT(block_check_cpu_chunks(va_block));
}

NV_STATUS uvm_va_block_make_resident(uvm_va_block_t *va_block,
                                     uvm_va_block_retry_t *va_block_retry,
                                     uvm_va_block_context_t *va_block_context,
                                     uvm_processor_id_t dest_id,
                                     uvm_va_block_region_t region,
                                     const uvm_page_mask_t *page_mask,
                                     const uvm_page_mask_t *prefetch_page_mask,
                                     uvm_make_resident_cause_t cause)
{
    NV_STATUS status;

    status = uvm_va_block_make_resident_copy(va_block,
                                             va_block_retry,
                                             va_block_context,
                                             dest_id,
                                             region,
                                             page_mask,
                                             prefetch_page_mask,
                                             cause);
    if (status != NV_OK)
        return status;

    uvm_va_block_make_resident_finish(va_block,
                                      va_block_context,
                                      region,
                                      page_mask);

    return NV_OK;
}

// Combination function which prepares the input {region, page_mask} for
// entering read-duplication. It:
// - Unmaps all processors but revoke_id
// - Revokes write access from revoke_id
static NV_STATUS block_prep_read_duplicate_mapping(uvm_va_block_t *va_block,
                                                   uvm_va_block_context_t *va_block_context,
                                                   uvm_processor_id_t revoke_id,
                                                   uvm_va_block_region_t region,
                                                   const uvm_page_mask_t *page_mask)
{
    uvm_processor_mask_t *unmap_processor_mask;
    uvm_processor_id_t unmap_id;
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    NV_STATUS status, tracker_status;

    unmap_processor_mask = uvm_processor_mask_cache_alloc();
    if (!unmap_processor_mask)
        return NV_ERR_NO_MEMORY;

    // Unmap everybody except revoke_id
    uvm_processor_mask_andnot(unmap_processor_mask, &va_block->mapped, block_get_uvm_lite_gpus(va_block));
    uvm_processor_mask_clear(unmap_processor_mask, revoke_id);

    for_each_id_in_mask(unmap_id, unmap_processor_mask) {
        status = uvm_va_block_unmap(va_block, va_block_context, unmap_id, region, page_mask, &local_tracker);
        if (status != NV_OK)
            goto out;
    }

    // Revoke WRITE/ATOMIC access permissions from the remaining mapped
    // processor.
    status = uvm_va_block_revoke_prot(va_block,
                                      va_block_context,
                                      revoke_id,
                                      region,
                                      page_mask,
                                      UVM_PROT_READ_WRITE,
                                      &local_tracker);
    if (status != NV_OK)
        goto out;

out:
    tracker_status = uvm_tracker_add_tracker_safe(&va_block->tracker, &local_tracker);
    uvm_tracker_deinit(&local_tracker);
    uvm_processor_mask_cache_free(unmap_processor_mask);
    return status == NV_OK ? tracker_status : status;
}

NV_STATUS uvm_va_block_make_resident_read_duplicate(uvm_va_block_t *va_block,
                                                    uvm_va_block_retry_t *va_block_retry,
                                                    uvm_va_block_context_t *va_block_context,
                                                    uvm_processor_id_t dest_id,
                                                    uvm_va_block_region_t region,
                                                    const uvm_page_mask_t *page_mask,
                                                    const uvm_page_mask_t *prefetch_page_mask,
                                                    uvm_make_resident_cause_t cause)
{
    NV_STATUS status = NV_OK;
    uvm_processor_id_t src_id;
    uvm_page_mask_t *dst_resident_mask;
    uvm_page_mask_t *migrated_pages;
    uvm_page_mask_t *staged_pages;
    uvm_page_mask_t *scratch_residency_mask;

    // TODO: Bug 3660922: need to implement HMM read duplication support.
    UVM_ASSERT(!uvm_va_block_is_hmm(va_block));

    va_block_context->make_resident.dest_id = dest_id;
    va_block_context->make_resident.cause = cause;
    nodes_clear(va_block_context->make_resident.cpu_pages_used.nodes);

    if (prefetch_page_mask) {
        // TODO: Bug 1877578: investigate automatic read-duplicate policies
        UVM_ASSERT(cause == UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT ||
                   cause == UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT ||
                   cause == UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER);
    }

    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(!uvm_va_block_is_dead(va_block));

    scratch_residency_mask = kmem_cache_alloc(g_uvm_page_mask_cache, NV_UVM_GFP_FLAGS);
    if (!scratch_residency_mask)
        return NV_ERR_NO_MEMORY;

    // For pages that are entering read-duplication we need to unmap remote
    // mappings and revoke RW and higher access permissions.
    //
    // The current implementation:
    // - Unmaps pages from all processors but the one with the resident copy
    // - Revokes write access from the processor with the resident copy
    for_each_id_in_mask(src_id, &va_block->resident) {
        // Note that the below calls to block_populate_pages and
        // block_copy_resident_pages also use
        // va_block_context->make_resident.page_mask.
        uvm_page_mask_t *preprocess_page_mask = &va_block_context->make_resident.page_mask;
        const uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(va_block, src_id, NUMA_NO_NODE);
        UVM_ASSERT(!uvm_page_mask_empty(resident_mask));

        if (page_mask)
            uvm_page_mask_andnot(preprocess_page_mask, page_mask, &va_block->read_duplicated_pages);
        else
            uvm_page_mask_complement(preprocess_page_mask, &va_block->read_duplicated_pages);

        // If there are no pages that need to be unmapped/revoked, skip to the
        // next processor
        if (!uvm_page_mask_and(preprocess_page_mask, preprocess_page_mask, resident_mask))
            continue;

        status = block_prep_read_duplicate_mapping(va_block, va_block_context, src_id, region, preprocess_page_mask);
        if (status != NV_OK)
            goto out;
    }

    status = block_populate_pages(va_block, va_block_retry, va_block_context, dest_id, region, page_mask);
    if (status != NV_OK)
        goto out;

    status = block_copy_resident_pages(va_block,
                                       va_block_context,
                                       dest_id,
                                       region,
                                       page_mask,
                                       prefetch_page_mask,
                                       UVM_VA_BLOCK_TRANSFER_MODE_COPY);
    if (status != NV_OK)
        goto out;

    // Pages that weren't resident anywhere else were populated at the
    // destination directly. Mark them as resident now, since there were no
    // errors from block_copy_resident_pages() above.
    migrated_pages = &va_block_context->make_resident.pages_migrated;
    uvm_page_mask_init_from_region(scratch_residency_mask, region, page_mask);
    uvm_page_mask_andnot(scratch_residency_mask, scratch_residency_mask, migrated_pages);

    if (!uvm_page_mask_empty(scratch_residency_mask))
        block_copy_set_first_touch_residency(va_block, va_block_context, dest_id, region, scratch_residency_mask);

    staged_pages = &va_block_context->make_resident.pages_staged;
    if (!UVM_ID_IS_CPU(dest_id) && !uvm_page_mask_empty(staged_pages)) {
        uvm_va_block_cpu_set_resident_all_chunks(va_block, va_block_context, staged_pages);
        block_set_resident_processor(va_block, UVM_ID_CPU);
        uvm_page_mask_or(&va_block->read_duplicated_pages, &va_block->read_duplicated_pages, staged_pages);
        uvm_tools_record_read_duplicate(va_block, UVM_ID_CPU, region, staged_pages);
    }

    if (!uvm_page_mask_empty(migrated_pages)) {
        if (UVM_ID_IS_CPU(dest_id)) {
            // Check if the CPU is already in the resident set of processors.
            // We need to do this since we can't have multiple NUMA nodes with
            // resident pages.
            // If any of the migrate pages were already resident on the CPU, the
            // residency has to be switched to the destination NUMA node.
            if (uvm_processor_mask_test(&va_block->resident, UVM_ID_CPU) &&
                uvm_page_mask_and(scratch_residency_mask,
                                  uvm_va_block_resident_mask_get(va_block, UVM_ID_CPU, NUMA_NO_NODE),
                                  migrated_pages)) {
                uvm_va_block_cpu_clear_resident_all_chunks(va_block, va_block_context, scratch_residency_mask);
            }

            uvm_va_block_cpu_set_resident_all_chunks(va_block, va_block_context, migrated_pages);
        }
        else {
            dst_resident_mask = uvm_va_block_resident_mask_get(va_block, dest_id, NUMA_NO_NODE);
            uvm_page_mask_or(dst_resident_mask, dst_resident_mask, migrated_pages);
        }

        block_set_resident_processor(va_block, dest_id);
        uvm_page_mask_or(&va_block->read_duplicated_pages, &va_block->read_duplicated_pages, migrated_pages);
        uvm_tools_record_read_duplicate(va_block, dest_id, region, migrated_pages);
    }

    UVM_ASSERT(cause != UVM_MAKE_RESIDENT_CAUSE_EVICTION);
    if (UVM_ID_IS_GPU(dest_id) && uvm_processor_mask_test(&va_block->evicted_gpus, dest_id))
        block_make_resident_clear_evicted(va_block, dest_id, migrated_pages);

    // Update eviction heuristics, if needed. Notably this could repeat the call
    // done in block_set_resident_processor(), but that doesn't do anything bad
    // and it's simpler to keep it in both places.
    //
    // Skip this if we didn't do anything (the input region and/or page mask was
    // empty).
    if (uvm_processor_mask_test(&va_block->resident, dest_id))
        block_mark_memory_used(va_block, dest_id);

    // Check state of all chunks after residency change.
    // TODO: Bug 4207783: Check both CPU and GPU chunks.
    UVM_ASSERT(block_check_cpu_chunks(va_block));
out:
    kmem_cache_free(g_uvm_page_mask_cache, scratch_residency_mask);
    return status;
}

// Looks up the current CPU mapping state of page from the
// block->cpu.pte_bits bitmaps. If write access is enabled,
// UVM_PROT_READ_WRITE_ATOMIC is returned instead of UVM_PROT_READ_WRITE, since
// write access implies atomic access for CPUs.
static uvm_prot_t block_page_prot_cpu(uvm_va_block_t *block, uvm_page_index_t page_index)
{
    uvm_prot_t prot;

    UVM_ASSERT(!uvm_va_block_is_dead(block));

    if (uvm_page_mask_test(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE], page_index))
        prot = UVM_PROT_READ_WRITE_ATOMIC;
    else if (uvm_page_mask_test(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ], page_index))
        prot = UVM_PROT_READ_ONLY;
    else
        prot = UVM_PROT_NONE;

    return prot;
}

// Looks up the current GPU mapping state of page from the
// block->gpus[i]->pte_bits bitmaps.
static uvm_prot_t block_page_prot_gpu(uvm_va_block_t *block, uvm_gpu_t *gpu, uvm_page_index_t page_index)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_prot_t prot;

    UVM_ASSERT(!uvm_va_block_is_dead(block));

    if (!gpu_state)
        return UVM_PROT_NONE;

    if (uvm_page_mask_test(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_ATOMIC], page_index))
        prot = UVM_PROT_READ_WRITE_ATOMIC;
    else if (uvm_page_mask_test(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_WRITE], page_index))
        prot = UVM_PROT_READ_WRITE;
    else if (uvm_page_mask_test(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ], page_index))
        prot = UVM_PROT_READ_ONLY;
    else
        prot = UVM_PROT_NONE;

    return prot;
}

static uvm_prot_t block_page_prot(uvm_va_block_t *block, uvm_processor_id_t id, uvm_page_index_t page_index)
{
    if (UVM_ID_IS_CPU(id))
        return block_page_prot_cpu(block, page_index);
    else
        return block_page_prot_gpu(block, block_get_gpu(block, id), page_index);
}

// Returns true if the block has any valid CPU PTE mapping in the block region.
static bool block_has_valid_mapping_cpu(uvm_va_block_t *block, uvm_va_block_region_t region)
{
    size_t valid_page;

    UVM_ASSERT(region.outer <= uvm_va_block_num_cpu_pages(block));

    // Early-out: check whether any address in this block has a CPU mapping
    if (!uvm_processor_mask_test(&block->mapped, UVM_ID_CPU)) {
        UVM_ASSERT(uvm_page_mask_empty(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ]));
        UVM_ASSERT(uvm_page_mask_empty(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE]));
        return false;
    }

    // All valid mappings have at least read permissions so we only need to
    // inspect the read bits.
    valid_page = uvm_va_block_first_page_in_mask(region, &block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ]);
    if (valid_page == region.outer)
        return false;

    UVM_ASSERT(block_page_prot_cpu(block, valid_page) != UVM_PROT_NONE);
    return true;
}

// Sanity check the given GPU's chunks array
static bool block_check_gpu_chunks(uvm_va_block_t *block, uvm_gpu_id_t id)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, id);
    uvm_gpu_t *gpu;
    size_t i, num_chunks;
    uvm_page_index_t page_index;
    uvm_chunk_size_t chunk_size;

    if (!gpu_state)
        return true;

    gpu = block_get_gpu(block, id);

    num_chunks = block_num_gpu_chunks(block, gpu);
    for (page_index = 0, i = 0; i < num_chunks; i++) {
        uvm_gpu_chunk_t *chunk = gpu_state->chunks[i];
        size_t chunk_index = block_gpu_chunk_index(block, gpu, page_index, &chunk_size);

        if (chunk_index != i) {
            UVM_ERR_PRINT("chunk index mismatch: calculated %zu, is in %zu. VA block [0x%llx, 0x%llx) GPU %u page_index: %u\n",
                           chunk_index,
                           i,
                           block->start,
                           block->end + 1,
                           uvm_id_value(id),
                           page_index);
            return false;
        }

        if (chunk) {
            if (chunk_size != uvm_gpu_chunk_get_size(chunk)) {
                UVM_ERR_PRINT("chunk size mismatch: calc %u, actual %u. VA block [0x%llx, 0x%llx) GPU: %u page_index: %u chunk index: %lu\n",
                              chunk_size,
                              uvm_gpu_chunk_get_size(chunk),
                              block->start,
                              block->end + 1,
                              uvm_id_value(id),
                              page_index,
                              i);
                return false;
            }

            if (chunk->state != UVM_PMM_GPU_CHUNK_STATE_ALLOCATED) {
                UVM_ERR_PRINT("Invalid chunk state %s. VA block [0x%llx, 0x%llx) GPU: %u page_index: %u chunk index: %lu chunk_size: llu\n",
                              uvm_pmm_gpu_chunk_state_string(chunk->state),
                              block->start,
                              block->end + 1,
                              uvm_id_value(id),
                              page_index,
                              i,
                              chunk_size);
                return false;
            }

            UVM_ASSERT(chunk->va_block == block);
            UVM_ASSERT(chunk->va_block_page_index == page_index);
        }

        page_index += chunk_size / PAGE_SIZE;
    }

    return true;
}

static bool block_check_chunks(uvm_va_block_t *va_block)
{
    uvm_gpu_id_t id;

    for_each_gpu_id(id) {
        if (!block_check_gpu_chunks(va_block, id))
            return false;
    }

    return block_check_cpu_chunks(va_block);
}

typedef struct
{
    uvm_processor_mask_t atomic_mappings;
    uvm_processor_mask_t write_mappings;
    uvm_processor_mask_t read_mappings;
    uvm_processor_mask_t lite_read_mappings;
    uvm_processor_mask_t lite_atomic_mappings;
    uvm_processor_mask_t remaining_mappings;
    uvm_processor_mask_t temp_mappings;
    uvm_processor_mask_t resident_processors;
    uvm_processor_mask_t native_atomics;
    uvm_processor_mask_t non_native_atomics;
    uvm_processor_mask_t residency_accessible_from;
    uvm_processor_mask_t residency_has_native_atomics;
} mapping_masks_t;

// Sanity checks for page mappings
static bool block_check_mappings_page(uvm_va_block_t *block,
                                      uvm_va_block_context_t *block_context,
                                      uvm_page_index_t page_index)
{
    uvm_processor_mask_t *atomic_mappings, *write_mappings, *read_mappings;
    uvm_processor_mask_t *lite_read_mappings, *lite_atomic_mappings;
    uvm_processor_mask_t *remaining_mappings, *temp_mappings;
    uvm_processor_mask_t *resident_processors;
    uvm_processor_mask_t *native_atomics, *non_native_atomics;
    uvm_processor_mask_t *residency_accessible_from;
    uvm_processor_mask_t *residency_has_native_atomics;
    uvm_processor_id_t residency, id;
    uvm_va_range_t *va_range = block->va_range;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_processor_id_t preferred_location = va_range ?
                                            uvm_va_range_get_policy(va_range)->preferred_location :
                                            UVM_ID_INVALID;
    const uvm_processor_mask_t *uvm_lite_gpus = block_get_uvm_lite_gpus(block);
    mapping_masks_t *mapping_masks = uvm_kvmalloc(sizeof(*mapping_masks));

    // Since all subsequent checks are skipped if mapping_masks allocation
    // fails, assert so that assertion messages can be seen on non-release
    // builds.
    UVM_ASSERT(mapping_masks);

    if (!mapping_masks)
        return true;

    atomic_mappings = &mapping_masks->atomic_mappings;
    write_mappings = &mapping_masks->write_mappings;
    read_mappings = &mapping_masks->read_mappings;

    block_page_authorized_processors(block, page_index, UVM_PROT_READ_WRITE_ATOMIC, atomic_mappings);
    block_page_authorized_processors(block, page_index, UVM_PROT_READ_WRITE, write_mappings);
    block_page_authorized_processors(block, page_index, UVM_PROT_READ_ONLY, read_mappings);

    // Each access bit implies all accesses below it
    UVM_ASSERT(uvm_processor_mask_subset(atomic_mappings, write_mappings));
    UVM_ASSERT(uvm_processor_mask_subset(write_mappings, read_mappings));
    UVM_ASSERT(uvm_processor_mask_subset(read_mappings, &block->mapped));

    resident_processors = &mapping_masks->resident_processors;

    uvm_va_block_page_resident_processors(block, page_index, resident_processors);
    UVM_ASSERT(uvm_processor_mask_subset(resident_processors, &block->resident));

    remaining_mappings = &mapping_masks->remaining_mappings;
    temp_mappings = &mapping_masks->temp_mappings;

    // Sanity check block_get_mapped_processors
    uvm_processor_mask_copy(remaining_mappings, read_mappings);
    for_each_id_in_mask(residency, resident_processors) {
        block_get_mapped_processors(block, block_context, residency, page_index, temp_mappings);
        UVM_ASSERT(uvm_processor_mask_subset(temp_mappings, remaining_mappings));
        uvm_processor_mask_andnot(remaining_mappings, remaining_mappings, temp_mappings);
    }

    // Any remaining mappings point to non-resident locations, so they must be
    // UVM-Lite mappings.
    UVM_ASSERT(uvm_processor_mask_subset(remaining_mappings, uvm_lite_gpus));

    residency = uvm_processor_mask_find_first_id(resident_processors);

    residency_accessible_from = &mapping_masks->residency_accessible_from;
    residency_has_native_atomics = &mapping_masks->residency_has_native_atomics;

    if (uvm_processor_mask_get_count(resident_processors) > 0) {
        residency_accessible_from    = &va_space->accessible_from[uvm_id_value(residency)];
        residency_has_native_atomics = &va_space->has_native_atomics[uvm_id_value(residency)];
    }

    // If the page is not resident, there should be no valid mappings
    UVM_ASSERT_MSG(uvm_processor_mask_get_count(resident_processors) > 0 ||
                   uvm_processor_mask_get_count(read_mappings) == 0,
                   "Resident: 0x%lx - Mappings R: 0x%lx W: 0x%lx A: 0x%lx - SWA: 0x%lx - RD: 0x%lx\n",
                   *resident_processors->bitmap,
                   *read_mappings->bitmap, *write_mappings->bitmap, *atomic_mappings->bitmap,
                   *va_space->system_wide_atomics_enabled_processors.bitmap,
                   *block->read_duplicated_pages.bitmap);

    // Test read_duplicated_pages mask
    UVM_ASSERT_MSG((!uvm_page_mask_test(&block->read_duplicated_pages, page_index) &&
                    uvm_processor_mask_get_count(resident_processors) <= 1) ||
                   (uvm_page_mask_test(&block->read_duplicated_pages, page_index) &&
                    uvm_processor_mask_get_count(resident_processors) >= 1),
                   "Resident: 0x%lx - Mappings R: 0x%lx W: 0x%lx A: 0x%lx - SWA: 0x%lx - RD: 0x%lx\n",
                   *resident_processors->bitmap,
                   *read_mappings->bitmap,
                   *write_mappings->bitmap,
                   *atomic_mappings->bitmap,
                   *va_space->system_wide_atomics_enabled_processors.bitmap,
                   *block->read_duplicated_pages.bitmap);

    if (!uvm_processor_mask_empty(uvm_lite_gpus))
        UVM_ASSERT(UVM_ID_IS_VALID(preferred_location));

    lite_read_mappings = &mapping_masks->lite_read_mappings;
    lite_atomic_mappings = &mapping_masks->lite_atomic_mappings;

    // UVM-Lite checks. Since the range group is made non-migratable before the
    // actual migrations for that range group happen, we can only make those
    // checks which are valid on both migratable and non-migratable range
    // groups.
    uvm_processor_mask_and(lite_read_mappings, read_mappings, uvm_lite_gpus);
    uvm_processor_mask_and(lite_atomic_mappings, atomic_mappings, uvm_lite_gpus);

    // Any mapping from a UVM-Lite GPU must be atomic...
    UVM_ASSERT(uvm_processor_mask_equal(lite_read_mappings, lite_atomic_mappings));

    // ... and must have access to preferred_location
    if (UVM_ID_IS_VALID(preferred_location)) {
        const uvm_processor_mask_t *preferred_location_accessible_from;

        preferred_location_accessible_from = &va_space->accessible_from[uvm_id_value(preferred_location)];
        UVM_ASSERT(uvm_processor_mask_subset(lite_atomic_mappings, preferred_location_accessible_from));
    }

    for_each_id_in_mask(id, lite_atomic_mappings)
        UVM_ASSERT(uvm_processor_mask_test(&va_space->can_access[uvm_id_value(id)], preferred_location));

    // Exclude uvm_lite_gpus from mappings' masks after UVM-Lite tests
    uvm_processor_mask_andnot(read_mappings, read_mappings, uvm_lite_gpus);
    uvm_processor_mask_andnot(write_mappings, write_mappings, uvm_lite_gpus);
    uvm_processor_mask_andnot(atomic_mappings, atomic_mappings, uvm_lite_gpus);

    // Pages set to zero in maybe_mapped_pages must not be mapped on any
    // non-UVM-Lite GPU
    if (!uvm_va_block_is_hmm(block) && !uvm_page_mask_test(&block->maybe_mapped_pages, page_index)) {
        UVM_ASSERT_MSG(uvm_processor_mask_get_count(read_mappings) == 0,
                       "Resident: 0x%lx - Mappings Block: 0x%lx / Page R: 0x%lx W: 0x%lx A: 0x%lx\n",
                       *resident_processors->bitmap,
                       *block->mapped.bitmap,
                       *read_mappings->bitmap, *write_mappings->bitmap, *atomic_mappings->bitmap);
    }

    // atomic mappings from GPUs with disabled system-wide atomics are treated
    // as write mappings. Therefore, we remove them from the atomic mappings mask
    uvm_processor_mask_and(atomic_mappings, atomic_mappings, &va_space->system_wide_atomics_enabled_processors);

    if (!uvm_processor_mask_empty(read_mappings)) {
        // Read-duplicate: if a page is resident in multiple locations, it
        // must be resident locally on each mapped processor.
        if (uvm_processor_mask_get_count(resident_processors) > 1) {
            UVM_ASSERT_MSG(uvm_processor_mask_subset(read_mappings, resident_processors),
                           "Read-duplicate copies from remote processors\n"
                           "Resident: 0x%lx - Mappings R: 0x%lx W: 0x%lx A: 0x%lx - SWA: 0x%lx - RD: 0x%lx\n",
                           *resident_processors->bitmap,
                           *read_mappings->bitmap, *write_mappings->bitmap, *atomic_mappings->bitmap,
                           *va_space->system_wide_atomics_enabled_processors.bitmap,
                           *block->read_duplicated_pages.bitmap);
        }
        else {
            // Processors with mappings must have access to the processor that
            // has the valid copy
            UVM_ASSERT_MSG(uvm_processor_mask_subset(read_mappings, residency_accessible_from),
                           "Not all processors have access to %s\n"
                           "Resident: 0x%lx - Mappings R: 0x%lx W: 0x%lx A: 0x%lx -"
                           "Access: 0x%lx - Native Atomics: 0x%lx - SWA: 0x%lx\n",
                           uvm_va_space_processor_name(va_space, residency),
                           *resident_processors->bitmap,
                           *read_mappings->bitmap,
                           *write_mappings->bitmap,
                           *atomic_mappings->bitmap,
                           *residency_accessible_from->bitmap,
                           *residency_has_native_atomics->bitmap,
                           *va_space->system_wide_atomics_enabled_processors.bitmap);
            for_each_id_in_mask(id, read_mappings) {
                UVM_ASSERT(uvm_processor_mask_test(&va_space->can_access[uvm_id_value(id)], residency));
            }
        }
    }

    // If any processor has a writable mapping, there must only be one copy of
    // the page in the system
    if (!uvm_processor_mask_empty(write_mappings)) {
        UVM_ASSERT_MSG(uvm_processor_mask_get_count(resident_processors) == 1,
                       "Too many resident copies for pages with write_mappings\n"
                       "Resident: 0x%lx - Mappings R: 0x%lx W: 0x%lx A: 0x%lx - SWA: 0x%lx - RD: 0x%lx\n",
                       *resident_processors->bitmap,
                       *read_mappings->bitmap,
                       *write_mappings->bitmap,
                       *atomic_mappings->bitmap,
                       *va_space->system_wide_atomics_enabled_processors.bitmap,
                       *block->read_duplicated_pages.bitmap);
    }

    if (!uvm_processor_mask_empty(atomic_mappings)) {

        native_atomics = &mapping_masks->native_atomics;

        uvm_processor_mask_and(native_atomics, atomic_mappings, residency_has_native_atomics);

        if (uvm_processor_mask_empty(native_atomics)) {
            // No other faultable processor should be able to write
            uvm_processor_mask_and(write_mappings, write_mappings, &va_space->faultable_processors);

            UVM_ASSERT_MSG(uvm_processor_mask_get_count(write_mappings) == 1,
                           "Too many write mappings to %s from processors with non-native atomics\n"
                           "Resident: 0x%lx - Mappings R: 0x%lx W: 0x%lx A: 0x%lx -"
                           "Access: 0x%lx - Native Atomics: 0x%lx - SWA: 0x%lx\n",
                           uvm_va_space_processor_name(va_space, residency),
                           *resident_processors->bitmap,
                           *read_mappings->bitmap,
                           *write_mappings->bitmap,
                           *atomic_mappings->bitmap,
                           *residency_accessible_from->bitmap,
                           *residency_has_native_atomics->bitmap,
                           *va_space->system_wide_atomics_enabled_processors.bitmap);

            // Only one processor outside of the native group can have atomics enabled
            UVM_ASSERT_MSG(uvm_processor_mask_get_count(atomic_mappings) == 1,
                           "Too many atomics mappings to %s from processors with non-native atomics\n"
                           "Resident: 0x%lx - Mappings R: 0x%lx W: 0x%lx A: 0x%lx -"
                           "Access: 0x%lx - Native Atomics: 0x%lx - SWA: 0x%lx\n",
                           uvm_va_space_processor_name(va_space, residency),
                           *resident_processors->bitmap,
                           *read_mappings->bitmap,
                           *write_mappings->bitmap,
                           *atomic_mappings->bitmap,
                           *residency_accessible_from->bitmap,
                           *residency_has_native_atomics->bitmap,
                           *va_space->system_wide_atomics_enabled_processors.bitmap);
        }
        else {

            non_native_atomics = &mapping_masks->non_native_atomics;

            // One or more processors within the native group have atomics enabled.
            // All processors outside of that group may have write but not atomic
            // permissions.
            uvm_processor_mask_andnot(non_native_atomics, atomic_mappings, residency_has_native_atomics);

            UVM_ASSERT_MSG(uvm_processor_mask_empty(non_native_atomics),
                           "atomic mappings to %s from processors native and non-native\n"
                           "Resident: 0x%lx - Mappings R: 0x%lx W: 0x%lx A: 0x%lx -"
                           "Access: 0x%lx - Native Atomics: 0x%lx - SWA: 0x%lx\n",
                           uvm_va_space_processor_name(va_space, residency),
                           *resident_processors->bitmap,
                           *read_mappings->bitmap,
                           *write_mappings->bitmap,
                           *atomic_mappings->bitmap,
                           *residency_accessible_from->bitmap,
                           *residency_has_native_atomics->bitmap,
                           *va_space->system_wide_atomics_enabled_processors.bitmap);
        }
    }

    uvm_kvfree(mapping_masks);
    return true;
}

static bool block_check_mappings_ptes(uvm_va_block_t *block, uvm_va_block_context_t *block_context, uvm_gpu_t *gpu)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_va_block_gpu_state_t *resident_gpu_state;
    uvm_pte_bits_gpu_t pte_bit;
    uvm_processor_id_t resident_id;
    uvm_prot_t prot;
    NvU64 big_page_size;
    size_t num_big_pages, big_page_index;
    uvm_va_block_region_t big_region, chunk_region;
    uvm_gpu_chunk_t *chunk;

    if (!gpu_state->page_table_range_4k.table)
        UVM_ASSERT(!gpu_state->activated_4k);

    if (!gpu_state->page_table_range_big.table) {
        UVM_ASSERT(!gpu_state->initialized_big);
        UVM_ASSERT(!gpu_state->activated_big);
    }

    // It's only safe to check the PTE mappings if we have page tables. See
    // uvm_va_block_get_gpu_va_space.
    if (!block_gpu_has_page_tables(block, gpu)) {
        UVM_ASSERT(!uvm_processor_mask_test(&block->mapped, gpu->id));
        return true;
    }

    big_page_size = uvm_va_block_gpu_big_page_size(block, gpu);
    num_big_pages = uvm_va_block_num_big_pages(block, big_page_size);

    if (block_gpu_supports_2m(block, gpu)) {
        if (gpu_state->page_table_range_big.table || gpu_state->page_table_range_4k.table) {
            // 2M blocks require the 2M entry to be allocated for the lower
            // ranges to also be allocated.
            UVM_ASSERT(gpu_state->page_table_range_2m.table);
        }
        else if (gpu_state->page_table_range_2m.table) {
            // If the 2M entry is present but the lower ones aren't, the PTE
            // must be 2M.
            UVM_ASSERT(gpu_state->pte_is_2m);
        }
    }
    else {
        UVM_ASSERT(!gpu_state->page_table_range_2m.table);
        if (num_big_pages == 0)
            UVM_ASSERT(!gpu_state->page_table_range_big.table);
    }

    // If we have the big table and it's in use then it must have been
    // initialized, even if it doesn't currently contain active PTEs.
    if ((!block_gpu_supports_2m(block, gpu) && gpu_state->page_table_range_big.table) ||
        (block_gpu_supports_2m(block, gpu) && !gpu_state->pte_is_2m && gpu_state->activated_big))
        UVM_ASSERT(gpu_state->initialized_big);

    if (gpu_state->pte_is_2m) {
        UVM_ASSERT(block_gpu_supports_2m(block, gpu));
        UVM_ASSERT(gpu_state->page_table_range_2m.table);
        UVM_ASSERT(bitmap_empty(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
        UVM_ASSERT(!gpu_state->force_4k_ptes);

        // GPU architectures which support 2M pages only support 64K as the big
        // page size. All of the 2M code assumes that
        // MAX_BIG_PAGES_PER_UVM_VA_BLOCK covers a 2M PTE exactly (bitmap_full,
        // bitmap_complement, etc).
        BUILD_BUG_ON((UVM_PAGE_SIZE_2M / UVM_PAGE_SIZE_64K) != MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

        prot = block_page_prot_gpu(block, gpu, 0);

        // All page permissions match
        for (pte_bit = 0; pte_bit < UVM_PTE_BITS_GPU_MAX; pte_bit++) {
            if (prot == UVM_PROT_NONE || pte_bit > get_gpu_pte_bit_index(prot))
                UVM_ASSERT(uvm_page_mask_empty(&gpu_state->pte_bits[pte_bit]));
            else
                UVM_ASSERT(uvm_page_mask_full(&gpu_state->pte_bits[pte_bit]));
        }

        if (prot != UVM_PROT_NONE) {
            resident_id = block_gpu_get_processor_to_map(block, block_context, gpu, 0);

            // block_check_resident_proximity verifies that no closer processor
            // has a resident page, so we don't need to check that all pages
            // have the same resident_id.

            // block_check_mappings_page verifies that all pages marked resident
            // are backed by populated memory.

            // The mapped processor should be fully resident and physically-
            // contiguous.
            UVM_ASSERT(uvm_page_mask_full(uvm_va_block_resident_mask_get(block, resident_id, NUMA_NO_NODE)));

            if (UVM_ID_IS_GPU(resident_id)) {
                resident_gpu_state = uvm_va_block_gpu_state_get(block, resident_id);
                UVM_ASSERT(resident_gpu_state);
                UVM_ASSERT(uvm_gpu_chunk_get_size(resident_gpu_state->chunks[0]) == UVM_CHUNK_SIZE_2M);
            }
            else {
                uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page_resident(block, 0);
                int chunk_nid = uvm_cpu_chunk_get_numa_node(chunk);

                UVM_ASSERT(uvm_page_mask_full(&block->cpu.allocated));
                UVM_ASSERT(chunk);
                UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_2M);
                UVM_ASSERT(uvm_va_block_cpu_is_region_resident_on(block,
                                                                  chunk_nid,
                                                                  uvm_va_block_region_from_block(block)));
            }
        }
    }
    else if (!bitmap_empty(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK)) {
        UVM_ASSERT(gpu_state->page_table_range_big.table);
        UVM_ASSERT(!gpu_state->force_4k_ptes);
        UVM_ASSERT(num_big_pages > 0);
        UVM_ASSERT(gpu_state->initialized_big);

        for (big_page_index = 0; big_page_index < num_big_pages; big_page_index++) {
            big_region = uvm_va_block_big_page_region(block, big_page_index, big_page_size);

            if (!test_bit(big_page_index, gpu_state->big_ptes)) {
                // If there are valid mappings but this isn't a big PTE, the
                // mapping must be using the 4k PTEs.
                if (!uvm_page_mask_region_empty(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ], big_region))
                    UVM_ASSERT(gpu_state->page_table_range_4k.table);
                continue;
            }

            prot = block_page_prot_gpu(block, gpu, big_region.first);

            // All page permissions match
            for (pte_bit = 0; pte_bit < UVM_PTE_BITS_GPU_MAX; pte_bit++) {
                if (prot == UVM_PROT_NONE || pte_bit > get_gpu_pte_bit_index(prot))
                    UVM_ASSERT(uvm_page_mask_region_empty(&gpu_state->pte_bits[pte_bit], big_region));
                else
                    UVM_ASSERT(uvm_page_mask_region_full(&gpu_state->pte_bits[pte_bit], big_region));
            }

            if (prot != UVM_PROT_NONE) {
                resident_id = block_gpu_get_processor_to_map(block, block_context, gpu, big_region.first);

                // The mapped processor should be fully resident and physically-
                // contiguous. Exception: UVM-Lite GPUs always map the preferred
                // location even if the memory is resident elsewhere. Skip the
                // residency check but still verify contiguity.
                if (!uvm_processor_mask_test(block_get_uvm_lite_gpus(block), gpu->id)) {
                    UVM_ASSERT(
                        uvm_page_mask_region_full(uvm_va_block_resident_mask_get(block, resident_id, NUMA_NO_NODE),
                                                  big_region));
                }

                if (UVM_ID_IS_CPU(resident_id)) {
                    int resident_nid = block_get_page_node_residency(block, big_region.first);
                    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, resident_nid);
                    uvm_cpu_chunk_t *chunk;

                    UVM_ASSERT(resident_nid != NUMA_NO_NODE);
                    UVM_ASSERT(uvm_page_mask_region_full(&node_state->allocated, big_region));
                    chunk = uvm_cpu_chunk_get_chunk_for_page(block, resident_nid, big_region.first);
                    UVM_ASSERT(gpu->parent->can_map_sysmem_with_large_pages);
                    UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) >= uvm_va_block_region_size(big_region));
                    UVM_ASSERT(uvm_page_mask_region_full(&node_state->resident, big_region));
                }
                else {
                    // Check GPU chunks
                    chunk = block_phys_page_chunk(block,
                                                  block_phys_page(resident_id, NUMA_NO_NODE, big_region.first),
                                                  NULL);
                    chunk_region = uvm_va_block_chunk_region(block, uvm_gpu_chunk_get_size(chunk), big_region.first);
                    UVM_ASSERT(uvm_va_block_region_contains_region(chunk_region, big_region));
                }
            }
        }
    }

    return true;
}

static bool block_check_mappings(uvm_va_block_t *block, uvm_va_block_context_t *block_context)
{
    uvm_page_index_t page_index;
    uvm_processor_id_t id;

    // Verify the master masks, since block_check_mappings_page relies on them
    for_each_id(id) {
        const uvm_page_mask_t *resident_mask, *map_mask;

        if (UVM_ID_IS_GPU(id) && !uvm_va_block_gpu_state_get(block, id)) {
            UVM_ASSERT(!uvm_processor_mask_test(&block->resident, id));
            UVM_ASSERT(!uvm_processor_mask_test(&block->mapped, id));
            UVM_ASSERT(!uvm_processor_mask_test(&block->evicted_gpus, id));
            continue;
        }

        resident_mask = uvm_va_block_resident_mask_get(block, id, NUMA_NO_NODE);
        UVM_ASSERT(uvm_processor_mask_test(&block->resident, id) == !uvm_page_mask_empty(resident_mask));

        map_mask = uvm_va_block_map_mask_get(block, id);
        UVM_ASSERT(uvm_processor_mask_test(&block->mapped, id) == !uvm_page_mask_empty(map_mask));

        if (UVM_ID_IS_GPU(id)) {
            const uvm_page_mask_t *evicted_mask = block_evicted_mask_get(block, id);
            UVM_ASSERT(uvm_processor_mask_test(&block->evicted_gpus, id) == !uvm_page_mask_empty(evicted_mask));

            // Pages cannot be resident if they are marked as evicted
            UVM_ASSERT(!uvm_page_mask_intersects(evicted_mask, resident_mask));

            // Pages cannot be resident on a GPU with no memory
            if (!block_processor_has_memory(block, id))
                UVM_ASSERT(!uvm_processor_mask_test(&block->resident, id));
        }
    }

    // Check that every page has coherent mappings
    for_each_va_block_page(page_index, block)
        block_check_mappings_page(block, block_context, page_index);

    for_each_gpu_id(id) {
        if (uvm_va_block_gpu_state_get(block, id)) {
            uvm_gpu_t *gpu = block_get_gpu(block, id);

            // Check big and/or 2M PTE state
            block_check_mappings_ptes(block, block_context, gpu);
        }
    }

    return true;
}

// See the comments on uvm_va_block_unmap
static void block_unmap_cpu(uvm_va_block_t *block,
                            uvm_va_block_context_t *block_context,
                            uvm_va_block_region_t region,
                            const uvm_page_mask_t *unmap_pages)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_pte_bits_cpu_t pte_bit;
    bool unmapped_something = false;
    uvm_va_block_region_t subregion;
    NvU32 num_mapped_processors;

    // Early-out if nothing in the region is mapped or being unmapped.
    if (!block_has_valid_mapping_cpu(block, region) ||
        (unmap_pages && !uvm_page_mask_intersects(unmap_pages, &block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ])))
        return;

    // We can't actually unmap HMM ranges from the CPU here.
    // Unmapping happens as part of migrate_vma_setup().
    if (uvm_va_block_is_hmm(block)) {
        UVM_ASSERT(!uvm_va_block_is_hmm(block));
        return;
    }

    num_mapped_processors = uvm_processor_mask_get_count(&block->mapped);

    // If we are unmapping a page which we are tracking due to CPU faults with
    // correct permissions, clear the info. This will cover both the unmap and
    // revoke cases (since we implement CPU revocation by unmap + map)
    if (block->cpu.fault_authorized.first_fault_stamp &&
        uvm_page_mask_region_test(unmap_pages, region, block->cpu.fault_authorized.page_index))
        block->cpu.fault_authorized.first_fault_stamp = 0;

    for_each_va_block_subregion_in_mask(subregion, unmap_pages, region) {
        if (!block_has_valid_mapping_cpu(block, subregion))
            continue;

        unmap_mapping_range(va_space->mapping,
                            uvm_va_block_region_start(block, subregion),
                            uvm_va_block_region_size(subregion), 1);

        for (pte_bit = 0; pte_bit < UVM_PTE_BITS_CPU_MAX; pte_bit++)
            uvm_page_mask_region_clear(&block->cpu.pte_bits[pte_bit], subregion);

        // If the CPU is the only processor with mappings we can safely mark
        // the pages as fully unmapped
        if (num_mapped_processors == 1 && !uvm_va_block_is_hmm(block))
            uvm_page_mask_region_clear(&block->maybe_mapped_pages, subregion);

        unmapped_something = true;
    }

    if (!unmapped_something)
        return;

    // Check whether the block has any more mappings
    if (uvm_page_mask_empty(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ])) {
        UVM_ASSERT(uvm_page_mask_empty(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE]));
        uvm_processor_mask_clear(&block->mapped, UVM_ID_CPU);
    }

    UVM_ASSERT(block_check_mappings(block, block_context));
}

// Given a mask of mapped pages, returns true if any of the pages in the mask
// are mapped remotely by the given GPU.
static bool block_has_remote_mapping_gpu(uvm_va_block_t *block,
                                         uvm_page_mask_t *scratch_page_mask,
                                         uvm_gpu_id_t gpu_id,
                                         const uvm_page_mask_t *mapped_pages)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu_id);

    if (!gpu_state)
        return false;

    // The caller must ensure that all pages of the input mask are really mapped
    UVM_ASSERT(uvm_page_mask_subset(mapped_pages, &gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ]));

    // UVM-Lite GPUs map the preferred location if it's accessible, regardless
    // of the resident location.
    if (uvm_processor_mask_test(block_get_uvm_lite_gpus(block), gpu_id)) {
        if (uvm_page_mask_empty(mapped_pages))
            return false;

        return !uvm_va_policy_preferred_location_equal(uvm_va_range_get_policy(block->va_range), gpu_id, NUMA_NO_NODE);
    }

    // Remote pages are pages which are mapped but not resident locally
    return uvm_page_mask_andnot(scratch_page_mask, mapped_pages, &gpu_state->resident);
}

// Writes pte_clear_val to the 4k PTEs covered by clear_page_mask. If
// clear_page_mask is NULL, all 4k PTEs in the {block, gpu} are written.
//
// If tlb_batch is provided, the 4k PTEs written are added to the batch. The
// caller is responsible for ending the TLB batch with the appropriate membar.
static void block_gpu_pte_clear_4k(uvm_va_block_t *block,
                                   uvm_gpu_t *gpu,
                                   const uvm_page_mask_t *clear_page_mask,
                                   NvU64 pte_clear_val,
                                   uvm_pte_batch_t *pte_batch,
                                   uvm_tlb_batch_t *tlb_batch)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_gpu_phys_address_t pte_addr;
    NvU32 pte_size = uvm_mmu_pte_size(tree, UVM_PAGE_SIZE_4K);
    uvm_va_block_region_t region = uvm_va_block_region_from_block(block);
    uvm_va_block_region_t subregion;
    size_t num_ptes, ptes_per_page = PAGE_SIZE / UVM_PAGE_SIZE_4K;

    for_each_va_block_subregion_in_mask(subregion, clear_page_mask, region) {
        num_ptes = uvm_va_block_region_num_pages(subregion) * ptes_per_page;

        pte_addr = uvm_page_table_range_entry_address(tree,
                                                      &gpu_state->page_table_range_4k,
                                                      subregion.first * ptes_per_page);

        uvm_pte_batch_clear_ptes(pte_batch, pte_addr, pte_clear_val, pte_size, num_ptes);

        if (tlb_batch) {
            uvm_tlb_batch_invalidate(tlb_batch,
                                     uvm_va_block_region_start(block, subregion),
                                     uvm_va_block_region_size(subregion),
                                     UVM_PAGE_SIZE_4K,
                                     UVM_MEMBAR_NONE);
        }
    }
}

// Writes the 4k PTEs covered by write_page_mask using memory from resident_id
// with new_prot permissions. new_prot must not be UVM_PROT_NONE: use
// block_gpu_pte_clear_4k instead.
//
// If write_page_mask is NULL, all 4k PTEs in the {block, gpu} are written.
//
// If tlb_batch is provided, the 4k PTEs written are added to the batch. The
// caller is responsible for ending the TLB batch with the appropriate membar.
static void block_gpu_pte_write_4k(uvm_va_block_t *block,
                                   uvm_gpu_t *gpu,
                                   uvm_processor_id_t resident_id,
                                   uvm_prot_t new_prot,
                                   const uvm_page_mask_t *write_page_mask,
                                   uvm_pte_batch_t *pte_batch,
                                   uvm_tlb_batch_t *tlb_batch)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    NvU32 pte_size = uvm_mmu_pte_size(tree, UVM_PAGE_SIZE_4K);
    const size_t ptes_per_page = PAGE_SIZE / UVM_PAGE_SIZE_4K;
    uvm_va_block_region_t contig_region = {0};
    uvm_gpu_phys_address_t contig_addr = {0};
    uvm_gpu_phys_address_t page_addr = {0};
    uvm_page_index_t page_index;
    NvU64 pte_flags = block_gpu_pte_flag_cacheable(block, gpu, resident_id);
    int contig_nid = NUMA_NO_NODE;

    UVM_ASSERT(new_prot != UVM_PROT_NONE);
    UVM_ASSERT(UVM_ID_IS_VALID(resident_id));

    for_each_va_block_page_in_mask(page_index, write_page_mask, block) {
        uvm_gpu_phys_address_t pte_addr;
        size_t i;
        int nid = NUMA_NO_NODE;

        if (UVM_ID_IS_CPU(resident_id)) {
            nid = block_get_page_node_residency(block, page_index);
            UVM_ASSERT(nid != NUMA_NO_NODE);

            // Assume that this mapping will be used to write to the page
            if (new_prot > UVM_PROT_READ_ONLY && !uvm_va_block_is_hmm(block))
                block_mark_cpu_page_dirty(block, page_index, nid);
        }

        if (page_index >= contig_region.outer || nid != contig_nid) {
            contig_region = block_phys_contig_region(block, page_index, resident_id, nid);
            contig_addr = block_phys_page_address(block, block_phys_page(resident_id, nid, contig_region.first), gpu);
            page_addr = contig_addr;
            contig_nid = nid;
        }

        page_addr.address = contig_addr.address + (page_index - contig_region.first) * PAGE_SIZE;

        pte_addr = uvm_page_table_range_entry_address(tree,
                                                      &gpu_state->page_table_range_4k,
                                                      page_index * ptes_per_page);

        // Handle PAGE_SIZE > GPU PTE size
        for (i = 0; i < ptes_per_page; i++) {
            NvU64 pte_val = tree->hal->make_pte(page_addr.aperture, page_addr.address, new_prot, pte_flags);
            uvm_pte_batch_write_pte(pte_batch, pte_addr, pte_val, pte_size);
            page_addr.address += UVM_PAGE_SIZE_4K;
            pte_addr.address += pte_size;
        }

        if (tlb_batch) {
            NvU64 page_virt_addr = uvm_va_block_cpu_page_address(block, page_index);
            uvm_tlb_batch_invalidate(tlb_batch, page_virt_addr, PAGE_SIZE, UVM_PAGE_SIZE_4K, UVM_MEMBAR_NONE);
        }
    }
}

// Writes all 4k PTEs under the big PTE regions described by big_ptes_covered.
// This is used to initialize the 4k PTEs when splitting 2M and big PTEs. It
// only writes 4k PTEs, not big PTEs.
//
// For those 4k PTEs, new_pages_mask indicates which ones should inherit the
// mapping from the corresponding big page (0) and which ones should be written
// using memory from resident_id and new_prot (1). Unlike the other pte_write
// functions, new_prot may be UVM_PROT_NONE.
//
// If resident_id is UVM_ID_INVALID, this function looks up the resident ID
// which should inherit the current permissions. new_prot must be UVM_PROT_NONE
// in this case.
//
// new_pages_mask must not be NULL.
//
// No TLB invalidates are required since we've set up the lower PTEs to never be
// cached by the GPU's MMU when covered by larger PTEs.
static void block_gpu_pte_big_split_write_4k(uvm_va_block_t *block,
                                             uvm_va_block_context_t *block_context,
                                             uvm_gpu_t *gpu,
                                             uvm_processor_id_t resident_id,
                                             uvm_prot_t new_prot,
                                             const unsigned long *big_ptes_covered,
                                             const uvm_page_mask_t *new_pages_mask,
                                             uvm_pte_batch_t *pte_batch)
{
    uvm_va_block_region_t big_region;
    size_t big_page_index;
    uvm_processor_id_t curr_resident_id;
    uvm_prot_t curr_prot;
    NvU64 big_page_size = uvm_va_block_gpu_big_page_size(block, gpu);

    if (UVM_ID_IS_INVALID(resident_id))
        UVM_ASSERT(new_prot == UVM_PROT_NONE);

    for_each_set_bit(big_page_index, big_ptes_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) {
        big_region = uvm_va_block_big_page_region(block, big_page_index, big_page_size);

        curr_prot = block_page_prot_gpu(block, gpu, big_region.first);

        // The unmap path doesn't know the current residency ahead of time, so
        // we have to look it up.
        if (UVM_ID_IS_INVALID(resident_id)) {
            curr_resident_id = block_gpu_get_processor_to_map(block, block_context, gpu, big_region.first);
        }
        else {
            // Check that we aren't changing the aperture of the existing
            // mappings. It could be legal in some cases (switching from {RO, A}
            // to {RO, B} for example) but we'd need to issue TLB membars.
            if (curr_prot != UVM_PROT_NONE) {
                UVM_ASSERT(uvm_id_equal(block_gpu_get_processor_to_map(block,
                                                                       block_context,
                                                                       gpu,
                                                                       big_region.first),
                                        resident_id));
            }

            curr_resident_id = resident_id;
        }

        // pages in new_pages_mask under this big page get new_prot
        uvm_page_mask_zero(&block_context->scratch_page_mask);
        uvm_page_mask_region_fill(&block_context->scratch_page_mask, big_region);
        if (uvm_page_mask_and(&block_context->scratch_page_mask, &block_context->scratch_page_mask, new_pages_mask)) {
            if (new_prot == UVM_PROT_NONE) {
                block_gpu_pte_clear_4k(block, gpu, &block_context->scratch_page_mask, 0, pte_batch, NULL);
            }
            else {
                block_gpu_pte_write_4k(block,
                                       gpu,
                                       curr_resident_id,
                                       new_prot,
                                       &block_context->scratch_page_mask,
                                       pte_batch,
                                       NULL);
            }
        }

        // All other pages under this big page inherit curr_prot
        uvm_page_mask_zero(&block_context->scratch_page_mask);
        uvm_page_mask_region_fill(&block_context->scratch_page_mask, big_region);
        if (uvm_page_mask_andnot(&block_context->scratch_page_mask, &block_context->scratch_page_mask, new_pages_mask)) {
            if (curr_prot == UVM_PROT_NONE) {
                block_gpu_pte_clear_4k(block, gpu, &block_context->scratch_page_mask, 0, pte_batch, NULL);
            }
            else {
                block_gpu_pte_write_4k(block,
                                       gpu,
                                       curr_resident_id,
                                       curr_prot,
                                       &block_context->scratch_page_mask,
                                       pte_batch,
                                       NULL);
            }
        }
    }
}

// Writes pte_clear_val to the big PTEs in big_ptes_mask. If big_ptes_mask is
// NULL, all big PTEs in the {block, gpu} are cleared.
//
// If tlb_batch is provided, the big PTEs written are added to the batch. The
// caller is responsible for ending the TLB batch with the appropriate membar.
static void block_gpu_pte_clear_big(uvm_va_block_t *block,
                                    uvm_gpu_t *gpu,
                                    const unsigned long *big_ptes_mask,
                                    NvU64 pte_clear_val,
                                    uvm_pte_batch_t *pte_batch,
                                    uvm_tlb_batch_t *tlb_batch)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_gpu_va_space_t *gpu_va_space = uvm_va_block_get_gpu_va_space(block, gpu);
    NvU64 big_page_size = gpu_va_space->page_tables.big_page_size;
    uvm_gpu_phys_address_t pte_addr;
    NvU32 pte_size = uvm_mmu_pte_size(&gpu_va_space->page_tables, big_page_size);
    size_t big_page_index;
    DECLARE_BITMAP(big_ptes_to_clear, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    if (big_ptes_mask)
        bitmap_copy(big_ptes_to_clear, big_ptes_mask, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    else
        bitmap_set(big_ptes_to_clear, 0, uvm_va_block_num_big_pages(block, big_page_size));

    for_each_set_bit(big_page_index, big_ptes_to_clear, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) {
        pte_addr = uvm_page_table_range_entry_address(&gpu_va_space->page_tables,
                                                      &gpu_state->page_table_range_big,
                                                      big_page_index);
        uvm_pte_batch_clear_ptes(pte_batch, pte_addr, pte_clear_val, pte_size, 1);

        if (tlb_batch) {
            uvm_tlb_batch_invalidate(tlb_batch,
                                     uvm_va_block_big_page_addr(block, big_page_index, big_page_size),
                                     big_page_size,
                                     big_page_size,
                                     UVM_MEMBAR_NONE);
        }
    }
}

// Writes the big PTEs in big_ptes_mask using memory from resident_id with
// new_prot permissions. new_prot must not be UVM_PROT_NONE: use
// block_gpu_pte_clear_big instead.
//
// Unlike block_gpu_pte_clear_big, big_ptes_mask must not be NULL.
//
// If tlb_batch is provided, the big PTEs written are added to the batch. The
// caller is responsible for ending the TLB batch with the appropriate membar.
static void block_gpu_pte_write_big(uvm_va_block_t *block,
                                    uvm_gpu_t *gpu,
                                    uvm_processor_id_t resident_id,
                                    uvm_prot_t new_prot,
                                    const unsigned long *big_ptes_mask,
                                    uvm_pte_batch_t *pte_batch,
                                    uvm_tlb_batch_t *tlb_batch)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_gpu_va_space_t *gpu_va_space = uvm_va_block_get_gpu_va_space(block, gpu);
    uvm_page_tree_t *tree = &gpu_va_space->page_tables;
    NvU64 big_page_size = tree->big_page_size;
    NvU32 pte_size = uvm_mmu_pte_size(tree, big_page_size);
    size_t big_page_index;
    uvm_va_block_region_t contig_region = {0};
    uvm_gpu_phys_address_t contig_addr = {0};
    uvm_gpu_phys_address_t page_addr = {0};
    NvU64 pte_flags = block_gpu_pte_flag_cacheable(block, gpu, resident_id);
    int contig_nid = NUMA_NO_NODE;

    UVM_ASSERT(new_prot != UVM_PROT_NONE);
    UVM_ASSERT(UVM_ID_IS_VALID(resident_id));
    UVM_ASSERT(big_ptes_mask);

    if (!bitmap_empty(big_ptes_mask, MAX_BIG_PAGES_PER_UVM_VA_BLOCK)) {
        UVM_ASSERT(uvm_va_block_num_big_pages(block, big_page_size) > 0);

        if (!gpu->parent->can_map_sysmem_with_large_pages)
            UVM_ASSERT(UVM_ID_IS_GPU(resident_id));
    }

    for_each_set_bit(big_page_index, big_ptes_mask, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) {
        NvU64 pte_val;
        uvm_gpu_phys_address_t pte_addr;
        uvm_va_block_region_t big_region = uvm_va_block_big_page_region(block, big_page_index, big_page_size);
        int nid = NUMA_NO_NODE;

        if (UVM_ID_IS_CPU(resident_id)) {
            nid = block_get_page_node_residency(block, big_region.first);
            UVM_ASSERT(nid != NUMA_NO_NODE);

            // Assume that this mapping will be used to write to the page
            if (new_prot > UVM_PROT_READ_ONLY && !uvm_va_block_is_hmm(block)) {
                uvm_page_index_t page_index;

                for_each_va_block_page_in_region(page_index, big_region)
                    block_mark_cpu_page_dirty(block, page_index, nid);
            }
        }

        if (big_region.first >= contig_region.outer || nid != contig_nid) {
            contig_region = block_phys_contig_region(block, big_region.first, resident_id, nid);
            contig_addr = block_phys_page_address(block, block_phys_page(resident_id, nid, contig_region.first), gpu);
            page_addr = contig_addr;
            contig_nid = nid;
        }

        page_addr.address = contig_addr.address + (big_region.first - contig_region.first) * PAGE_SIZE;

        pte_addr = uvm_page_table_range_entry_address(tree, &gpu_state->page_table_range_big, big_page_index);
        pte_val = tree->hal->make_pte(page_addr.aperture, page_addr.address, new_prot, pte_flags);
        uvm_pte_batch_write_pte(pte_batch, pte_addr, pte_val, pte_size);

        if (tlb_batch) {
            uvm_tlb_batch_invalidate(tlb_batch,
                                     uvm_va_block_region_start(block, big_region),
                                     big_page_size,
                                     big_page_size,
                                     UVM_MEMBAR_NONE);
        }
    }
}

// Switches any mix of valid or invalid 4k PTEs under the big PTEs in
// big_ptes_to_merge to an unmapped big PTE. This also ends both pte_batch and
// tlb_batch in order to poison the now-unused 4k PTEs.
//
// The 4k PTEs are invalidated with the specified membar.
static void block_gpu_pte_merge_big_and_end(uvm_va_block_t *block,
                                            uvm_va_block_context_t *block_context,
                                            uvm_gpu_t *gpu,
                                            const unsigned long *big_ptes_to_merge,
                                            uvm_push_t *push,
                                            uvm_pte_batch_t *pte_batch,
                                            uvm_tlb_batch_t *tlb_batch,
                                            uvm_membar_t tlb_membar)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    NvU64 big_page_size = tree->big_page_size;
    NvU64 unmapped_pte_val = tree->hal->unmapped_pte(big_page_size);
    size_t big_page_index;
    DECLARE_BITMAP(dummy_big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    UVM_ASSERT(!bitmap_empty(big_ptes_to_merge, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
    UVM_ASSERT(!bitmap_and(dummy_big_ptes, gpu_state->big_ptes, big_ptes_to_merge, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));

    // We can be called with the 4k PTEs in two cases:
    // 1) 4k PTEs allocated. In this case the 4k PTEs are currently active.
    //
    // 2) 4k PTEs unallocated. In this case the GPU may not have invalid 4k PTEs
    //    active under the big PTE, depending on whether neighboring blocks
    //    caused the page tables to be allocated.
    //
    // In both cases we need to invalidate the 4k PTEs in case the GPU MMU has
    // them cached.

    // Each big PTE is currently invalid so the 4ks are active (or unallocated).
    // First make the big PTEs unmapped to disable future lookups of the 4ks
    // under it. We can't directly transition the entry from valid 4k PTEs to
    // valid big PTEs, because that could cause the GPU TLBs to cache the same
    // VA in different cache lines. That could cause memory ordering to not be
    // maintained.
    block_gpu_pte_clear_big(block, gpu, big_ptes_to_merge, unmapped_pte_val, pte_batch, tlb_batch);

    // Now invalidate the big PTEs we just wrote as well as all 4ks under them.
    // Subsequent MMU fills will stop at the now-unmapped big PTEs, so we only
    // need to invalidate the 4k PTEs without actually writing them.
    for_each_set_bit(big_page_index, big_ptes_to_merge, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) {
        uvm_tlb_batch_invalidate(tlb_batch,
                                 uvm_va_block_big_page_addr(block, big_page_index, big_page_size),
                                 big_page_size,
                                 big_page_size | UVM_PAGE_SIZE_4K,
                                 UVM_MEMBAR_NONE);
    }

    // End the batches for the caller. We need to do this here in order to
    // poison the 4ks below.
    uvm_pte_batch_end(pte_batch);
    uvm_tlb_batch_end(tlb_batch, push, tlb_membar);

    // As a guard against bad PTE writes/TLB invalidates, fill the now-unused
    // PTEs with a pattern which will trigger fatal faults on access. We have to
    // do this after the TLB invalidate of the big PTEs, or the GPU might use
    // the new values.
    if (UVM_IS_DEBUG() && gpu_state->page_table_range_4k.table) {
        uvm_page_mask_init_from_big_ptes(block, gpu, &block_context->scratch_page_mask, big_ptes_to_merge);
        uvm_pte_batch_begin(push, pte_batch);
        block_gpu_pte_clear_4k(block,
                               gpu,
                               &block_context->scratch_page_mask,
                               tree->hal->poisoned_pte(),
                               pte_batch,
                               NULL);
        uvm_pte_batch_end(pte_batch);
    }
}

// Writes 0 (invalid) to the 2M PTE for this {block, gpu}.
//
// If tlb_batch is provided, the 2M PTE is added to the batch. The caller is
// responsible for ending the TLB batch with the appropriate membar.
static void block_gpu_pte_clear_2m(uvm_va_block_t *block,
                                   uvm_gpu_t *gpu,
                                   uvm_pte_batch_t *pte_batch,
                                   uvm_tlb_batch_t *tlb_batch)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_gpu_phys_address_t pte_addr = uvm_page_table_range_entry_address(tree, &gpu_state->page_table_range_2m, 0);
    NvU32 pte_size = uvm_mmu_pte_size(tree, UVM_PAGE_SIZE_2M);

    // uvm_pte_batch_write_pte only writes the lower 8 bytes of the 16-byte PTE,
    // which would cause a problem when trying to make the entry invalid since
    // both halves must be 0. Using uvm_pte_batch_clear_ptes writes the entire
    // 16 bytes.
    uvm_pte_batch_clear_ptes(pte_batch, pte_addr, 0, pte_size, 1);

    if (tlb_batch)
        uvm_tlb_batch_invalidate(tlb_batch, block->start, UVM_PAGE_SIZE_2M, UVM_PAGE_SIZE_2M, UVM_MEMBAR_NONE);
}

// Writes the 2M PTE for {block, gpu} using memory from resident_id with
// new_prot permissions. new_prot must not be UVM_PROT_NONE: use
// block_gpu_pte_clear_2m instead.
//
// If tlb_batch is provided, the 2M PTE is added to the batch. The caller is
// responsible for ending the TLB batch with the appropriate membar.
static void block_gpu_pte_write_2m(uvm_va_block_t *block,
                                   uvm_gpu_t *gpu,
                                   uvm_processor_id_t resident_id,
                                   uvm_prot_t new_prot,
                                   uvm_pte_batch_t *pte_batch,
                                   uvm_tlb_batch_t *tlb_batch)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_gpu_phys_address_t pte_addr = uvm_page_table_range_entry_address(tree, &gpu_state->page_table_range_2m, 0);
    uvm_gpu_phys_address_t page_addr;
    NvU32 pte_size = uvm_mmu_pte_size(tree, UVM_PAGE_SIZE_2M);
    NvU64 pte_val;
    NvU64 pte_flags = block_gpu_pte_flag_cacheable(block, gpu, resident_id);
    int nid = NUMA_NO_NODE;

    UVM_ASSERT(new_prot != UVM_PROT_NONE);
    UVM_ASSERT(UVM_ID_IS_VALID(resident_id));

    if (UVM_ID_IS_CPU(resident_id)) {
        nid = block_get_page_node_residency(block, 0);
        UVM_ASSERT(nid != NUMA_NO_NODE);
        if (!uvm_va_block_is_hmm(block))
            block_mark_cpu_page_dirty(block, 0, nid);
    }

    page_addr = block_phys_page_address(block, block_phys_page(resident_id, nid, 0), gpu);
    pte_val = tree->hal->make_pte(page_addr.aperture, page_addr.address, new_prot, pte_flags);
    uvm_pte_batch_write_pte(pte_batch, pte_addr, pte_val, pte_size);

    if (tlb_batch)
        uvm_tlb_batch_invalidate(tlb_batch, block->start, UVM_PAGE_SIZE_2M, UVM_PAGE_SIZE_2M, UVM_MEMBAR_NONE);
}

static bool block_gpu_needs_to_activate_table(uvm_va_block_t *block, uvm_gpu_t *gpu)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);

    if (!block_gpu_supports_2m(block, gpu))
        return false;

    if ((gpu_state->page_table_range_big.table && !gpu_state->activated_big) ||
        (gpu_state->page_table_range_4k.table  && !gpu_state->activated_4k))
        return true;

    return false;
}

// Only used if 2M PTEs are supported. Either transitions a 2M PTE to a PDE, or
// activates a newly-allocated page table (big or 4k) while the other is already
// active. The caller must have already written the new PTEs under the table
// with the appropriate membar.
static void block_gpu_write_pde(uvm_va_block_t *block, uvm_gpu_t *gpu, uvm_push_t *push, uvm_tlb_batch_t *tlb_batch)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;

    if (!gpu_state->pte_is_2m)
        UVM_ASSERT(block_gpu_needs_to_activate_table(block, gpu));

    UVM_ASSERT(gpu_state->page_table_range_big.table || gpu_state->page_table_range_4k.table);

    // We always need a membar to order PDE/PTE writes with the TLB invalidate.
    // write_pde will do a MEMBAR_SYS by default.
    if (uvm_page_table_range_aperture(&gpu_state->page_table_range_2m) == UVM_APERTURE_VID)
        uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
    uvm_page_tree_write_pde(tree, &gpu_state->page_table_range_2m, push);

    gpu->parent->host_hal->wait_for_idle(push);

    // Invalidate just the PDE
    uvm_tlb_batch_invalidate(tlb_batch, block->start, UVM_PAGE_SIZE_2M, UVM_PAGE_SIZE_2M, UVM_MEMBAR_NONE);

    if (gpu_state->page_table_range_big.table)
        gpu_state->activated_big = true;

    if (gpu_state->page_table_range_4k.table)
        gpu_state->activated_4k = true;
}

// Called to switch the 2M PTE (valid or invalid) to a PDE. The caller should
// have written all lower PTEs as appropriate into the given pte_batch already.
// This function ends the PTE batch, activates the 2M PDE, and does a TLB
// invalidate.
//
// The caller does not need to do any TLB invalidates since none of the lower
// PTEs could be cached.
static void block_gpu_pte_finish_split_2m(uvm_va_block_t *block,
                                          uvm_gpu_t *gpu,
                                          uvm_push_t *push,
                                          uvm_pte_batch_t *pte_batch,
                                          uvm_tlb_batch_t *tlb_batch,
                                          uvm_membar_t tlb_membar)
{
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_prot_t curr_prot = block_page_prot_gpu(block, gpu, 0);

    // Step 1: Make the 2M entry invalid. We can't directly transition from a
    //         valid 2M PTE to valid lower PTEs, because that could cause the
    //         GPU TLBs to cache the same VA in different cache lines. That
    //         could cause memory ordering to not be maintained.
    //
    //         If the 2M PTE is already invalid, no TLB invalidate is needed.

    if (curr_prot == UVM_PROT_NONE) {
        // If we aren't downgrading, then we don't need a membar.
        UVM_ASSERT(tlb_membar == UVM_MEMBAR_NONE);

        // End the batch, which pushes a membar to ensure that the caller's PTE
        // writes below 2M are observed before the PDE write we're about to do.
        uvm_pte_batch_end(pte_batch);
    }
    else {
        // The 64k and 4k PTEs can't possibly be cached since the 2M entry is
        // not yet a PDE, so we just need to invalidate this single 2M entry.
        uvm_tlb_batch_begin(tree, tlb_batch);
        block_gpu_pte_clear_2m(block, gpu, pte_batch, tlb_batch);

        // Make sure the PTE writes are observed before the TLB invalidate
        uvm_pte_batch_end(pte_batch);
        uvm_tlb_batch_end(tlb_batch, push, tlb_membar);
    }

    // Step 2: Switch the 2M entry from invalid to a PDE. This activates the
    //         smaller PTEs.
    uvm_tlb_batch_begin(tree, tlb_batch);
    block_gpu_write_pde(block, gpu, push, tlb_batch);
    uvm_tlb_batch_end(tlb_batch, push, UVM_MEMBAR_NONE);
}

// Switches any mix of valid or invalid 4k or 64k PTEs to an invalid 2M PTE.
// Any lower PTEs are invalidated with the specified membar.
static void block_gpu_pte_merge_2m(uvm_va_block_t *block,
                                   uvm_va_block_context_t *block_context,
                                   uvm_gpu_t *gpu,
                                   uvm_push_t *push,
                                   uvm_membar_t tlb_membar)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    NvU32 tlb_inval_sizes;

    UVM_ASSERT(!gpu_state->pte_is_2m);
    UVM_ASSERT(gpu_state->page_table_range_big.table || gpu_state->page_table_range_4k.table);

    // The 2M entry is currently a PDE, so first make it invalid. We can't
    // directly transition the entry from a valid PDE to a valid 2M PTE, because
    // that could cause the GPU TLBs to cache the same VA in different cache
    // lines. That could cause memory ordering to not be maintained.
    uvm_pte_batch_begin(push, pte_batch);
    block_gpu_pte_clear_2m(block, gpu, pte_batch, NULL);
    uvm_pte_batch_end(pte_batch);

    // Now invalidate both the 2M entry we just wrote as well as all lower-level
    // entries which could be cached. Subsequent MMU fills will stop at the now-
    // invalid 2M entry, so we only need to invalidate the lower PTEs without
    // actually writing them.
    tlb_inval_sizes = UVM_PAGE_SIZE_2M;
    if (gpu_state->page_table_range_big.table)
        tlb_inval_sizes |= UVM_PAGE_SIZE_64K;

    // Strictly-speaking we only need to invalidate those 4k ranges which are
    // not covered by a big pte. However, any such invalidate will require
    // enough 4k invalidates to force the TLB batching to invalidate everything
    // anyway, so just do the simpler thing.
    if (!bitmap_full(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK))
        tlb_inval_sizes |= UVM_PAGE_SIZE_4K;

    uvm_tlb_batch_begin(tree, tlb_batch);
    uvm_tlb_batch_invalidate(tlb_batch, block->start, UVM_PAGE_SIZE_2M, tlb_inval_sizes, UVM_MEMBAR_NONE);
    uvm_tlb_batch_end(tlb_batch, push, tlb_membar);

    // As a guard against bad PTE writes/TLB invalidates, fill the now-unused
    // PTEs with a pattern which will trigger fatal faults on access. We have to
    // do this after the TLB invalidate of the 2M entry, or the GPU might use
    // the new values.
    if (UVM_IS_DEBUG()) {
        uvm_pte_batch_begin(push, pte_batch);

        if (gpu_state->page_table_range_big.table) {
            block_gpu_pte_clear_big(block,
                                    gpu,
                                    NULL,
                                    tree->hal->poisoned_pte(),
                                    pte_batch,
                                    NULL);
        }

        if (gpu_state->page_table_range_4k.table) {
            block_gpu_pte_clear_4k(block,
                                   gpu,
                                   NULL,
                                   tree->hal->poisoned_pte(),
                                   pte_batch,
                                   NULL);
        }

        uvm_pte_batch_end(pte_batch);
    }
}

static uvm_membar_t block_pte_op_membar(block_pte_op_t pte_op, uvm_gpu_t *gpu, uvm_processor_id_t resident_id)
{
    // Permissions upgrades (MAP) don't need membars
    if (pte_op == BLOCK_PTE_OP_MAP)
        return UVM_MEMBAR_NONE;

    UVM_ASSERT(UVM_ID_IS_VALID(resident_id));
    UVM_ASSERT(pte_op == BLOCK_PTE_OP_REVOKE);

    return uvm_hal_downgrade_membar_type(gpu, uvm_id_equal(gpu->id, resident_id));
}

// Write the 2M PTE for {block, gpu} to the memory on resident_id with new_prot
// permissions. If the 2M entry is currently a PDE, it is first merged into a
// PTE.
//
// new_prot must not be UVM_PROT_NONE: use block_gpu_unmap_to_2m instead.
//
// pte_op specifies whether this is a MAP or REVOKE operation, which determines
// the TLB membar required.
static void block_gpu_map_to_2m(uvm_va_block_t *block,
                                uvm_va_block_context_t *block_context,
                                uvm_gpu_t *gpu,
                                uvm_processor_id_t resident_id,
                                uvm_prot_t new_prot,
                                uvm_push_t *push,
                                block_pte_op_t pte_op)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_gpu_va_space_t *gpu_va_space = uvm_va_block_get_gpu_va_space(block, gpu);
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    uvm_membar_t tlb_membar;

    UVM_ASSERT(new_prot != UVM_PROT_NONE);

    // If we have a mix of big and 4k PTEs, we have to first merge them to an
    // invalid 2M PTE.
    if (!gpu_state->pte_is_2m) {
        block_gpu_pte_merge_2m(block, block_context, gpu, push, UVM_MEMBAR_NONE);

        gpu_state->pte_is_2m = true;
        bitmap_zero(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    }

    // Write the new permissions
    uvm_pte_batch_begin(push, pte_batch);
    uvm_tlb_batch_begin(&gpu_va_space->page_tables, tlb_batch);

    block_gpu_pte_write_2m(block, gpu, resident_id, new_prot, pte_batch, tlb_batch);

    uvm_pte_batch_end(pte_batch);

    tlb_membar = block_pte_op_membar(pte_op, gpu, resident_id);
    uvm_tlb_batch_end(tlb_batch, push, tlb_membar);
}

// Combination split + map operation, called when only part of a 2M PTE mapping
// is being changed. This splits an existing valid or invalid 2M PTE into the
// mix of big and 4k PTEs described by block_context->mapping.new_pte_state.
//
// The PTEs covering the pages in pages_to_write are written to the memory on
// resident_id with new_prot permissions. new_prot must not be UVM_PROT_NONE.
//
// The PTEs covering the pages not set in pages_to_write inherit the mapping of
// the current 2M PTE. If the current mapping is valid, it must target
// resident_id.
//
// pte_op specifies whether this is a MAP or REVOKE operation, which determines
// the TLB membar required.
static void block_gpu_map_split_2m(uvm_va_block_t *block,
                                   uvm_va_block_context_t *block_context,
                                   uvm_gpu_t *gpu,
                                   uvm_processor_id_t resident_id,
                                   const uvm_page_mask_t *pages_to_write,
                                   uvm_prot_t new_prot,
                                   uvm_push_t *push,
                                   block_pte_op_t pte_op)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_va_block_new_pte_state_t *new_pte_state = &block_context->mapping.new_pte_state;
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    uvm_prot_t curr_prot = block_page_prot_gpu(block, gpu, 0);
    uvm_membar_t tlb_membar;
    DECLARE_BITMAP(big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_inherit, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_new_prot, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    UVM_ASSERT(gpu_state->pte_is_2m);

    if (!gpu_state->page_table_range_4k.table)
        UVM_ASSERT(bitmap_full(new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));

    uvm_pte_batch_begin(push, pte_batch);

    // Since the 2M entry is active as a PTE, the GPU MMU can't fetch entries
    // from the lower levels. This means we don't need to issue a TLB invalidate
    // when writing those levels.

    // Cases to handle:
    // 1) Big PTEs which inherit curr_prot
    // 2) Big PTEs which get new_prot
    // 3) Big PTEs which are split to 4k
    //    a) 4k PTEs which inherit curr_prot under the split big PTEs
    //    b) 4k PTEs which get new_prot under the split big PTEs

    // Compute the big PTEs which will need to be split to 4k, if any.
    bitmap_complement(big_ptes_split, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    if (gpu_state->page_table_range_big.table) {
        // Case 1: Write the big PTEs which will inherit the 2M permissions, if
        // any. These are the big PTEs which are unchanged (uncovered) by the
        // operation.
        bitmap_andnot(big_ptes_inherit,
                      new_pte_state->big_ptes,
                      new_pte_state->big_ptes_covered,
                      MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

        if (curr_prot == UVM_PROT_NONE) {
            block_gpu_pte_clear_big(block,
                                    gpu,
                                    big_ptes_inherit,
                                    tree->hal->unmapped_pte(UVM_PAGE_SIZE_64K),
                                    pte_batch,
                                    NULL);
        }
        else {
            block_gpu_pte_write_big(block, gpu, resident_id, curr_prot, big_ptes_inherit, pte_batch, NULL);
        }

        // Case 2: Write the new big PTEs
        bitmap_and(big_ptes_new_prot,
                   new_pte_state->big_ptes,
                   new_pte_state->big_ptes_covered,
                   MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
        block_gpu_pte_write_big(block, gpu, resident_id, new_prot, big_ptes_new_prot, pte_batch, NULL);

        // Case 3: Write the big PTEs which cover 4k PTEs
        block_gpu_pte_clear_big(block, gpu, big_ptes_split, 0, pte_batch, NULL);

        // We just wrote all possible big PTEs, so mark them as initialized
        gpu_state->initialized_big = true;
    }
    else {
        UVM_ASSERT(bitmap_empty(new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
    }

    // Cases 3a and 3b: Write all 4k PTEs under all now-split big PTEs
    block_gpu_pte_big_split_write_4k(block,
                                     block_context,
                                     gpu,
                                     resident_id,
                                     new_prot,
                                     big_ptes_split,
                                     pages_to_write,
                                     pte_batch);

    // Activate the 2M PDE. This ends the pte_batch and issues a single TLB
    // invalidate for the 2M entry.
    tlb_membar = block_pte_op_membar(pte_op, gpu, resident_id);
    block_gpu_pte_finish_split_2m(block, gpu, push, pte_batch, tlb_batch, tlb_membar);

    gpu_state->pte_is_2m = false;
    bitmap_copy(gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
}

// Split the existing 2M PTE into big and 4k PTEs. No permissions are changed.
//
// new_big_ptes specifies which PTEs should be big. NULL means all PTEs should
// be 4k.
static void block_gpu_split_2m(uvm_va_block_t *block,
                               uvm_va_block_context_t *block_context,
                               uvm_gpu_t *gpu,
                               const unsigned long *new_big_ptes,
                               uvm_push_t *push)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    uvm_prot_t curr_prot = block_page_prot_gpu(block, gpu, 0);
    DECLARE_BITMAP(new_big_ptes_local, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    NvU64 unmapped_pte_val;
    uvm_processor_id_t curr_residency;

    UVM_ASSERT(gpu_state->pte_is_2m);

    if (new_big_ptes)
        bitmap_copy(new_big_ptes_local, new_big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    else
        bitmap_zero(new_big_ptes_local, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    if (!bitmap_empty(new_big_ptes_local, MAX_BIG_PAGES_PER_UVM_VA_BLOCK))
        UVM_ASSERT(gpu_state->page_table_range_big.table);

    // We're splitting from 2M to big only, so we'll be writing all big PTEs
    if (gpu_state->page_table_range_big.table)
        gpu_state->initialized_big = true;

    // Cases to handle:
    // 1) Big PTEs which inherit curr_prot
    // 2) Big PTEs which are split to 4k
    //    a) 4k PTEs inherit curr_prot under the split big PTEs

    // big_ptes_split will cover the 4k regions
    bitmap_complement(big_ptes_split, new_big_ptes_local, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    uvm_page_mask_init_from_big_ptes(block, gpu, &block_context->mapping.big_split_page_mask, big_ptes_split);

    uvm_pte_batch_begin(push, pte_batch);

    // Since the 2M entry is active as a PTE, the GPU MMU can't fetch entries
    // from the lower levels. This means we don't need to issue a TLB invalidate
    // when writing those levels.

    if (curr_prot == UVM_PROT_NONE) {
        unmapped_pte_val = tree->hal->unmapped_pte(tree->big_page_size);

        // Case 2a: Clear the 4k PTEs under big_ptes_split
        block_gpu_pte_clear_4k(block, gpu, &block_context->mapping.big_split_page_mask, 0, pte_batch, NULL);

        // Case 1: Make the remaining big PTEs unmapped
        block_gpu_pte_clear_big(block, gpu, new_big_ptes_local, unmapped_pte_val, pte_batch, NULL);
    }
    else {
        curr_residency = block_gpu_get_processor_to_map(block, block_context, gpu, 0);

        // Case 2a: Write the new 4k PTEs under big_ptes_split
        block_gpu_pte_write_4k(block,
                               gpu,
                               curr_residency,
                               curr_prot,
                               &block_context->mapping.big_split_page_mask,
                               pte_batch,
                               NULL);

        // Case 1: Write the new big PTEs
        block_gpu_pte_write_big(block, gpu, curr_residency, curr_prot, new_big_ptes_local, pte_batch, NULL);
    }

    // Case 2: Make big_ptes_split invalid to activate the 4k PTEs
    if (gpu_state->page_table_range_big.table)
        block_gpu_pte_clear_big(block, gpu, big_ptes_split, 0, pte_batch, NULL);

    // Activate the 2M PDE. This ends the pte_batch and issues a single TLB
    // invalidate for the 2M entry. No membar is necessary since we aren't
    // changing permissions.
    block_gpu_pte_finish_split_2m(block, gpu, push, pte_batch, tlb_batch, UVM_MEMBAR_NONE);

    gpu_state->pte_is_2m = false;
    bitmap_copy(gpu_state->big_ptes, new_big_ptes_local, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
}

// Split the big PTEs in big_ptes_to_split into 4k PTEs. No permissions are
// changed.
//
// big_ptes_to_split must not be NULL.
static void block_gpu_split_big(uvm_va_block_t *block,
                                uvm_va_block_context_t *block_context,
                                uvm_gpu_t *gpu,
                                const unsigned long *big_ptes_to_split,
                                uvm_push_t *push)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    NvU64 big_page_size = tree->big_page_size;
    uvm_va_block_region_t big_region;
    uvm_processor_id_t resident_id;
    size_t big_page_index;
    uvm_prot_t curr_prot;
    DECLARE_BITMAP(big_ptes_valid, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    UVM_ASSERT(!gpu_state->pte_is_2m);
    UVM_ASSERT(bitmap_subset(big_ptes_to_split, gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
    UVM_ASSERT(!bitmap_empty(big_ptes_to_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));

    uvm_pte_batch_begin(push, pte_batch);
    uvm_tlb_batch_begin(tree, tlb_batch);

    // Write all 4k PTEs under all big PTEs which are being split. We'll make
    // the big PTEs inactive below after flushing these writes. No TLB
    // invalidate is needed since the big PTE is active.
    bitmap_zero(big_ptes_valid, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    for_each_set_bit(big_page_index, big_ptes_to_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) {
        big_region = uvm_va_block_big_page_region(block, big_page_index, big_page_size);
        curr_prot = block_page_prot_gpu(block, gpu, big_region.first);

        uvm_page_mask_zero(&block_context->mapping.big_split_page_mask);
        uvm_page_mask_region_fill(&block_context->mapping.big_split_page_mask, big_region);
        if (curr_prot == UVM_PROT_NONE) {
            block_gpu_pte_clear_4k(block, gpu, &block_context->mapping.big_split_page_mask, 0, pte_batch, NULL);
        }
        else {
            __set_bit(big_page_index, big_ptes_valid);

            resident_id = block_gpu_get_processor_to_map(block, block_context, gpu, big_region.first);

            block_gpu_pte_write_4k(block,
                                   gpu,
                                   resident_id,
                                   curr_prot,
                                   &block_context->mapping.big_split_page_mask,
                                   pte_batch,
                                   NULL);
        }
    }

    // Unmap the big PTEs which are valid and are being split to 4k. We can't
    // directly transition from a valid big PTE to valid lower PTEs, because
    // that could cause the GPU TLBs to cache the same VA in different cache
    // lines. That could cause memory ordering to not be maintained.
    block_gpu_pte_clear_big(block, gpu, big_ptes_valid, tree->hal->unmapped_pte(big_page_size), pte_batch, tlb_batch);

    // End the batches. We have to commit the membars and TLB invalidates
    // before we finish splitting formerly-big PTEs. No membar is necessary
    // since we aren't changing permissions.
    uvm_pte_batch_end(pte_batch);
    uvm_tlb_batch_end(tlb_batch, push, UVM_MEMBAR_NONE);

    // Finish the split by switching the big PTEs from unmapped to invalid. This
    // causes the GPU MMU to start reading the 4k PTEs instead of stopping at
    // the unmapped big PTEs.
    uvm_pte_batch_begin(push, pte_batch);
    uvm_tlb_batch_begin(tree, tlb_batch);

    block_gpu_pte_clear_big(block, gpu, big_ptes_to_split, 0, pte_batch, tlb_batch);

    uvm_pte_batch_end(pte_batch);

    // Finally, activate the page tables if they're inactive
    if (block_gpu_needs_to_activate_table(block, gpu))
        block_gpu_write_pde(block, gpu, push, tlb_batch);

    uvm_tlb_batch_end(tlb_batch, push, UVM_MEMBAR_NONE);

    bitmap_andnot(gpu_state->big_ptes, gpu_state->big_ptes, big_ptes_to_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
}

// Changes permissions on some pre-existing mix of big and 4k PTEs into some
// other mix of big and 4k PTEs, as described by
// block_context->mapping.new_pte_state.
//
// The PTEs covering the pages in pages_to_write are written to the memory on
// resident_id with new_prot permissions. new_prot must not be UVM_PROT_NONE.
//
// pte_op specifies whether this is a MAP or REVOKE operation, which determines
// the TLB membar required.
static void block_gpu_map_big_and_4k(uvm_va_block_t *block,
                                     uvm_va_block_context_t *block_context,
                                     uvm_gpu_t *gpu,
                                     uvm_processor_id_t resident_id,
                                     const uvm_page_mask_t *pages_to_write,
                                     uvm_prot_t new_prot,
                                     uvm_push_t *push,
                                     block_pte_op_t pte_op)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_va_block_new_pte_state_t *new_pte_state = &block_context->mapping.new_pte_state;
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    DECLARE_BITMAP(big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_before_or_after, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_merge, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_mask, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    uvm_va_block_region_t big_region;
    size_t big_page_index;
    NvU64 big_page_size = tree->big_page_size;
    uvm_membar_t tlb_membar = block_pte_op_membar(pte_op, gpu, resident_id);

    UVM_ASSERT(!gpu_state->pte_is_2m);

    uvm_pte_batch_begin(push, pte_batch);
    uvm_tlb_batch_begin(tree, tlb_batch);

    // All of these cases might be perfomed in the same call:
    // 1) Split currently-big PTEs to 4k
    //    a) Write new 4k PTEs which inherit curr_prot under the split big PTEs
    //    b) Write new 4k PTEs which get new_prot under the split big PTEs
    // 2) Merge currently-4k PTEs to big with new_prot
    // 3) Write currently-big PTEs which wholly get new_prot
    // 4) Write currently-4k PTEs which get new_prot
    // 5) Initialize big PTEs which are not covered by this operation

    // Cases 1a and 1b: Write all 4k PTEs under all currently-big PTEs which are
    // being split. We'll make the big PTEs inactive below after flushing these
    // writes. No TLB invalidate is needed since the big PTE is active.
    //
    // Mask computation: big_before && !big_after
    bitmap_andnot(big_ptes_split, gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    block_gpu_pte_big_split_write_4k(block,
                                     block_context,
                                     gpu,
                                     resident_id,
                                     new_prot,
                                     big_ptes_split,
                                     pages_to_write,
                                     pte_batch);

    // Case 4: Write the 4k PTEs which weren't covered by a big PTE before, and
    // remain uncovered after the operation.
    //
    // Mask computation: !big_before && !big_after
    bitmap_or(big_ptes_before_or_after, gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    uvm_page_mask_init_from_big_ptes(block, gpu, &block_context->scratch_page_mask, big_ptes_before_or_after);
    if (uvm_page_mask_andnot(&block_context->scratch_page_mask, pages_to_write, &block_context->scratch_page_mask)) {
        block_gpu_pte_write_4k(block,
                               gpu,
                               resident_id,
                               new_prot,
                               &block_context->scratch_page_mask,
                               pte_batch,
                               tlb_batch);
    }

    // Case 5: If the big page table is newly-allocated, make sure that all big
    // PTEs we aren't otherwise writing (that is, those which cover 4k PTEs) are
    // all initialized to invalid.
    //
    // The similar case of making newly-allocated big PTEs unmapped when no
    // lower 4k table is present is handled by having
    // block_gpu_compute_new_pte_state set new_pte_state->big_ptes
    // appropriately.
    if (gpu_state->page_table_range_big.table && !gpu_state->initialized_big) {
        // TODO: Bug 1766424: If we have the 4k page table already, we could
        //       attempt to merge all uncovered big PTE regions when first
        //       allocating the big table. That's probably not worth doing.
        UVM_ASSERT(gpu_state->page_table_range_4k.table);
        UVM_ASSERT(bitmap_empty(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
        bitmap_complement(big_ptes_mask, new_pte_state->big_ptes, uvm_va_block_num_big_pages(block, big_page_size));
        block_gpu_pte_clear_big(block, gpu, big_ptes_mask, 0, pte_batch, tlb_batch);
        gpu_state->initialized_big = true;
    }

    // Case 1 (step 1): Unmap the currently-big PTEs which are valid and are
    // being split to 4k. We can't directly transition from a valid big PTE to
    // valid lower PTEs, because that could cause the GPU TLBs to cache the same
    // VA in different cache lines. That could cause memory ordering to not be
    // maintained.
    bitmap_zero(big_ptes_mask, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    for_each_set_bit(big_page_index, big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) {
        big_region = uvm_va_block_big_page_region(block, big_page_index, big_page_size);
        if (uvm_page_mask_test(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ], big_region.first))
            __set_bit(big_page_index, big_ptes_mask);
    }

    block_gpu_pte_clear_big(block, gpu, big_ptes_mask, tree->hal->unmapped_pte(big_page_size), pte_batch, tlb_batch);

    // Case 3: Write the currently-big PTEs which remain big PTEs, and are
    // wholly changing permissions.
    //
    // Mask computation: big_before && big_after && covered
    bitmap_and(big_ptes_mask, gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    if (bitmap_and(big_ptes_mask, big_ptes_mask, new_pte_state->big_ptes_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK))
        block_gpu_pte_write_big(block, gpu, resident_id, new_prot, big_ptes_mask, pte_batch, tlb_batch);

    // Case 2 (step 1): Merge the new big PTEs and end the batches, now that
    // we've done all of the independent PTE writes we can. This also merges
    // newly-allocated uncovered big PTEs to unmapped (see
    // block_gpu_compute_new_pte_state).
    //
    // Mask computation: !big_before && big_after
    if (bitmap_andnot(big_ptes_merge, new_pte_state->big_ptes, gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK)) {
        // This writes the newly-big PTEs to unmapped and ends the PTE and TLB
        // batches.
        block_gpu_pte_merge_big_and_end(block,
                                        block_context,
                                        gpu,
                                        big_ptes_merge,
                                        push,
                                        pte_batch,
                                        tlb_batch,
                                        tlb_membar);

        // Remove uncovered big PTEs. We needed to merge them to unmapped above,
        // but they shouldn't get new_prot below.
        bitmap_and(big_ptes_merge, big_ptes_merge, new_pte_state->big_ptes_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    }
    else {
        // End the batches. We have to commit the membars and TLB invalidates
        // before we finish splitting formerly-big PTEs.
        uvm_pte_batch_end(pte_batch);
        uvm_tlb_batch_end(tlb_batch, push, tlb_membar);
    }

    if (!bitmap_empty(big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) ||
        !bitmap_empty(big_ptes_merge, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) ||
        block_gpu_needs_to_activate_table(block, gpu)) {

        uvm_pte_batch_begin(push, pte_batch);
        uvm_tlb_batch_begin(tree, tlb_batch);

        // Case 1 (step 2): Finish splitting our big PTEs, if we have any, by
        // switching them from unmapped to invalid. This causes the GPU MMU to
        // start reading the 4k PTEs instead of stopping at the unmapped big
        // PTEs.
        block_gpu_pte_clear_big(block, gpu, big_ptes_split, 0, pte_batch, tlb_batch);

        // Case 2 (step 2): Finish merging our big PTEs, if we have any, by
        // switching them from unmapped to new_prot.
        block_gpu_pte_write_big(block, gpu, resident_id, new_prot, big_ptes_merge, pte_batch, tlb_batch);

        uvm_pte_batch_end(pte_batch);

        // Finally, activate the page tables if they're inactive
        if (block_gpu_needs_to_activate_table(block, gpu))
            block_gpu_write_pde(block, gpu, push, tlb_batch);

        uvm_tlb_batch_end(tlb_batch, push, UVM_MEMBAR_NONE);
    }

    // Update gpu_state
    bitmap_copy(gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
}

// Unmap all PTEs for {block, gpu}. If the 2M entry is currently a PDE, it is
// merged into a PTE.
static void block_gpu_unmap_to_2m(uvm_va_block_t *block,
                                  uvm_va_block_context_t *block_context,
                                  uvm_gpu_t *gpu,
                                  uvm_push_t *push,
                                  uvm_membar_t tlb_membar)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_gpu_va_space_t *gpu_va_space = uvm_va_block_get_gpu_va_space(block, gpu);
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;

    if (gpu_state->pte_is_2m) {
        // If we're already mapped as a valid 2M PTE, just write it to invalid
        uvm_pte_batch_begin(push, pte_batch);
        uvm_tlb_batch_begin(&gpu_va_space->page_tables, tlb_batch);

        block_gpu_pte_clear_2m(block, gpu, pte_batch, tlb_batch);

        uvm_pte_batch_end(pte_batch);
        uvm_tlb_batch_end(tlb_batch, push, tlb_membar);
    }
    else {
        // Otherwise we have a mix of big and 4K PTEs which need to be merged
        // into an invalid 2M PTE.
        block_gpu_pte_merge_2m(block, block_context, gpu, push, tlb_membar);

        gpu_state->pte_is_2m = true;
        bitmap_zero(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    }
}

// Combination split + unmap operation, called when only part of a valid 2M PTE
// mapping is being unmapped. The 2M PTE is split into a mix of valid and
// invalid big and/or 4k PTEs, as described by
// block_context->mapping.new_pte_state.
//
// The PTEs covering the pages in pages_to_unmap are cleared (unmapped).
//
// The PTEs covering the pages not set in pages_to_unmap inherit the mapping of
// the current 2M PTE.
static void block_gpu_unmap_split_2m(uvm_va_block_t *block,
                                     uvm_va_block_context_t *block_context,
                                     uvm_gpu_t *gpu,
                                     const uvm_page_mask_t *pages_to_unmap,
                                     uvm_push_t *push,
                                     uvm_membar_t tlb_membar)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_va_block_new_pte_state_t *new_pte_state = &block_context->mapping.new_pte_state;
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    uvm_prot_t curr_prot = block_page_prot_gpu(block, gpu, 0);
    uvm_processor_id_t resident_id;
    DECLARE_BITMAP(big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_inherit, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_new_prot, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    UVM_ASSERT(gpu_state->pte_is_2m);

    resident_id = block_gpu_get_processor_to_map(block, block_context, gpu, 0);

    uvm_pte_batch_begin(push, pte_batch);

    // Since the 2M entry is active as a PTE, the GPU MMU can't fetch entries
    // from the lower levels. This means we don't need to issue a TLB invalidate
    // when writing those levels.

    // Cases to handle:
    // 1) Big PTEs which inherit curr_prot
    // 2) Big PTEs which get unmapped
    // 3) Big PTEs which are split to 4k
    //    a) 4k PTEs which inherit curr_prot under the split big PTEs
    //    b) 4k PTEs which get unmapped under the split big PTEs

    // Compute the big PTEs which will need to be split to 4k, if any.
    bitmap_complement(big_ptes_split, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    if (gpu_state->page_table_range_big.table) {
        // Case 1: Write the big PTEs which will inherit the 2M permissions, if
        // any. These are the big PTEs which are unchanged (uncovered) by the
        // operation.
        bitmap_andnot(big_ptes_inherit,
                      new_pte_state->big_ptes,
                      new_pte_state->big_ptes_covered,
                      MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

        block_gpu_pte_write_big(block, gpu, resident_id, curr_prot, big_ptes_inherit, pte_batch, NULL);

        // Case 2: Clear the new big PTEs which get unmapped (those not covering
        // 4ks)
        bitmap_and(big_ptes_new_prot,
                   new_pte_state->big_ptes,
                   new_pte_state->big_ptes_covered,
                   MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

        block_gpu_pte_clear_big(block,
                                gpu,
                                big_ptes_new_prot,
                                tree->hal->unmapped_pte(UVM_PAGE_SIZE_64K),
                                pte_batch,
                                NULL);

        // Case 3: Write the big PTEs which cover 4k PTEs
        block_gpu_pte_clear_big(block, gpu, big_ptes_split, 0, pte_batch, NULL);

        // We just wrote all possible big PTEs, so mark them as initialized
        gpu_state->initialized_big = true;
    }
    else {
        UVM_ASSERT(bitmap_empty(new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
        UVM_ASSERT(bitmap_full(new_pte_state->big_ptes_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
    }

    // Cases 3a and 3b: Write all 4k PTEs under all now-split big PTEs
    block_gpu_pte_big_split_write_4k(block,
                                     block_context,
                                     gpu,
                                     resident_id,
                                     UVM_PROT_NONE,
                                     big_ptes_split,
                                     pages_to_unmap,
                                     pte_batch);

    // And activate the 2M PDE. This ends the pte_batch and issues a single TLB
    // invalidate for the 2M entry.
    block_gpu_pte_finish_split_2m(block, gpu, push, pte_batch, tlb_batch, tlb_membar);

    gpu_state->pte_is_2m = false;
    bitmap_copy(gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
}

// Unmap some pre-existing mix of big and 4k PTEs into some other mix of big
// and 4k PTEs.
//
// The PTEs covering the pages in pages_to_unmap are cleared (unmapped).
static void block_gpu_unmap_big_and_4k(uvm_va_block_t *block,
                                       uvm_va_block_context_t *block_context,
                                       uvm_gpu_t *gpu,
                                       const uvm_page_mask_t *pages_to_unmap,
                                       uvm_push_t *push,
                                       uvm_membar_t tlb_membar)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_page_tree_t *tree = &uvm_va_block_get_gpu_va_space(block, gpu)->page_tables;
    uvm_va_block_new_pte_state_t *new_pte_state = &block_context->mapping.new_pte_state;
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    DECLARE_BITMAP(big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_before_or_after, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    DECLARE_BITMAP(big_ptes_mask, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    NvU64 big_page_size = tree->big_page_size;
    NvU64 unmapped_pte_val = tree->hal->unmapped_pte(big_page_size);

    UVM_ASSERT(!gpu_state->pte_is_2m);

    uvm_pte_batch_begin(push, pte_batch);
    uvm_tlb_batch_begin(tree, tlb_batch);

    // All of these cases might be perfomed in the same call:
    // 1) Split currently-big PTEs to 4k
    //    a) Write new 4k PTEs which inherit curr_prot under the split big PTEs
    //    b) Clear new 4k PTEs which get unmapped under the split big PTEs
    // 2) Merge currently-4k PTEs to unmapped big
    // 3) Clear currently-big PTEs which wholly get unmapped
    // 4) Clear currently-4k PTEs which get unmapped
    // 5) Initialize big PTEs which are not covered by this operation

    // Cases 1a and 1b: Write all 4k PTEs under all currently-big PTEs which are
    // being split. We'll make the big PTEs inactive below after flushing these
    // writes. No TLB invalidate is needed since the big PTE is active.
    //
    // Mask computation: big_before && !big_after
    bitmap_andnot(big_ptes_split, gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    block_gpu_pte_big_split_write_4k(block,
                                     block_context,
                                     gpu,
                                     UVM_ID_INVALID,
                                     UVM_PROT_NONE,
                                     big_ptes_split,
                                     pages_to_unmap,
                                     pte_batch);

    // Case 4: Clear the 4k PTEs which weren't covered by a big PTE before, and
    // remain uncovered after the unmap.
    //
    // Mask computation: !big_before && !big_after
    bitmap_or(big_ptes_before_or_after, gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    uvm_page_mask_init_from_big_ptes(block, gpu, &block_context->scratch_page_mask, big_ptes_before_or_after);
    if (uvm_page_mask_andnot(&block_context->scratch_page_mask, pages_to_unmap, &block_context->scratch_page_mask))
        block_gpu_pte_clear_4k(block, gpu, &block_context->scratch_page_mask, 0, pte_batch, tlb_batch);

    // Case 5: If the big page table is newly-allocated, make sure that all big
    // PTEs we aren't otherwise writing (that is, those which cover 4k PTEs) are
    // all initialized to invalid.
    //
    // The similar case of making newly-allocated big PTEs unmapped when no
    // lower 4k table is present is handled by having
    // block_gpu_compute_new_pte_state set new_pte_state->big_ptes
    // appropriately.
    if (gpu_state->page_table_range_big.table && !gpu_state->initialized_big) {
        // TODO: Bug 1766424: If we have the 4k page table already, we could
        //       attempt to merge all uncovered big PTE regions when first
        //       allocating the big table. That's probably not worth doing.
        UVM_ASSERT(gpu_state->page_table_range_4k.table);
        UVM_ASSERT(bitmap_empty(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
        bitmap_complement(big_ptes_mask, new_pte_state->big_ptes, uvm_va_block_num_big_pages(block, big_page_size));
        block_gpu_pte_clear_big(block, gpu, big_ptes_mask, 0, pte_batch, tlb_batch);
        gpu_state->initialized_big = true;
    }

    // Case 3 and step 1 of case 1: Unmap both currently-big PTEs which are
    // getting wholly unmapped, and those currently-big PTEs which are being
    // split to 4k. We can't directly transition from a valid big PTE to valid
    // lower PTEs, because that could cause the GPU TLBs to cache the same VA in
    // different cache lines. That could cause memory ordering to not be
    // maintained.
    //
    // Mask computation: (big_before && big_after && covered) ||
    //                   (big_before && !big_after)
    bitmap_and(big_ptes_mask, gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    bitmap_and(big_ptes_mask, big_ptes_mask, new_pte_state->big_ptes_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    bitmap_or(big_ptes_mask, big_ptes_mask, big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    block_gpu_pte_clear_big(block, gpu, big_ptes_mask, unmapped_pte_val, pte_batch, tlb_batch);

    // Case 2: Merge the new big PTEs and end the batches, now that we've done
    // all of the independent PTE writes we can.
    //
    // Mask computation: !big_before && big_after
    if (bitmap_andnot(big_ptes_mask, new_pte_state->big_ptes, gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK)) {
        // This writes the newly-big PTEs to unmapped and ends the PTE and TLB
        // batches.
        block_gpu_pte_merge_big_and_end(block,
                                        block_context,
                                        gpu,
                                        big_ptes_mask,
                                        push,
                                        pte_batch,
                                        tlb_batch,
                                        tlb_membar);
    }
    else {
        // End the batches. We have to commit the membars and TLB invalidates
        // before we finish splitting formerly-big PTEs.
        uvm_pte_batch_end(pte_batch);
        uvm_tlb_batch_end(tlb_batch, push, tlb_membar);
    }

    if (!bitmap_empty(big_ptes_split, MAX_BIG_PAGES_PER_UVM_VA_BLOCK) ||
        block_gpu_needs_to_activate_table(block, gpu)) {
        uvm_pte_batch_begin(push, pte_batch);
        uvm_tlb_batch_begin(tree, tlb_batch);

        // Case 1 (step 2): Finish splitting our big PTEs, if we have any, by
        // switching them from unmapped to invalid. This causes the GPU MMU to
        // start reading the 4k PTEs instead of stopping at the unmapped big
        // PTEs.
        block_gpu_pte_clear_big(block, gpu, big_ptes_split, 0, pte_batch, tlb_batch);

        uvm_pte_batch_end(pte_batch);

        // Finally, activate the page tables if they're inactive
        if (block_gpu_needs_to_activate_table(block, gpu))
            block_gpu_write_pde(block, gpu, push, tlb_batch);

        uvm_tlb_batch_end(tlb_batch, push, UVM_MEMBAR_NONE);
    }

    // Update gpu_state
    bitmap_copy(gpu_state->big_ptes, new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
}

// When PTE state is about to change (for example due to a map/unmap/revoke
// operation), this function decides how to split and merge the PTEs in response
// to that operation.
//
// The operation is described with the two page masks:
//
// - pages_changing indicates which pages will have their PTE mappings changed
//   on the GPU in some way as a result of the operation (for example, which
//   pages will actually have their mapping permissions upgraded).
//
// - page_mask_after indicates which pages on this GPU will have exactly the
//   same PTE attributes (permissions, residency) as pages_changing after the
//   operation is applied.
//
// PTEs are merged eagerly.
static void block_gpu_compute_new_pte_state(uvm_va_block_t *block,
                                            uvm_gpu_t *gpu,
                                            uvm_processor_id_t resident_id,
                                            const uvm_page_mask_t *pages_changing,
                                            const uvm_page_mask_t *page_mask_after,
                                            uvm_va_block_new_pte_state_t *new_pte_state)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_va_block_region_t big_region_all, big_page_region, region;
    NvU64 big_page_size;
    uvm_page_index_t page_index;
    size_t big_page_index;
    DECLARE_BITMAP(big_ptes_not_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    bool can_make_new_big_ptes;

    memset(new_pte_state, 0, sizeof(*new_pte_state));
    new_pte_state->needs_4k = true;

    // TODO: Bug 1676485: Force a specific page size for perf testing

    if (gpu_state->force_4k_ptes)
        return;

    // Limit HMM GPU allocations to PAGE_SIZE since migrate_vma_*(),
    // hmm_range_fault(), and make_device_exclusive_range() don't handle folios
    // yet. Also, it makes mremap() difficult since the new address may not
    // align with the GPU block size otherwise.
    // If PAGE_SIZE is 64K, the code following this check is OK since 64K
    // big_pages is supported on all HMM supported GPUs (Turing+).
    // TODO: Bug 3368756: add support for transparent huge pages (THP).
    if (uvm_va_block_is_hmm(block) && PAGE_SIZE == UVM_PAGE_SIZE_4K)
        return;

    UVM_ASSERT(uvm_page_mask_subset(pages_changing, page_mask_after));

    // If all pages in the 2M mask have the same attributes after the
    // operation is applied, we can use a 2M PTE.
    if (block_gpu_supports_2m(block, gpu) && uvm_page_mask_full(page_mask_after) &&
        (UVM_ID_IS_INVALID(resident_id) ||
         is_block_phys_contig(block, resident_id, block_get_page_node_residency(block, 0)))) {
        new_pte_state->pte_is_2m = true;
        new_pte_state->needs_4k = false;
        return;
    }

    // Find big PTEs with matching attributes

    // Can this block fit any big pages?
    big_page_size = uvm_va_block_gpu_big_page_size(block, gpu);
    big_region_all = uvm_va_block_big_page_region_all(block, big_page_size);
    if (big_region_all.first >= big_region_all.outer)
        return;

    new_pte_state->needs_4k = false;

    can_make_new_big_ptes = true;

    // Big pages can be used when mapping sysmem if the GPU supports it (Pascal+).
    if (UVM_ID_IS_CPU(resident_id) && !gpu->parent->can_map_sysmem_with_large_pages)
        can_make_new_big_ptes = false;

    // We must not fail during teardown: unmap (resident_id == UVM_ID_INVALID)
    // with no splits required. That means we should avoid allocating PTEs
    // which are only needed for merges.
    //
    // This only matters if we're merging to big PTEs. If we're merging to 2M,
    // then we must already have the 2M level (since it has to be allocated
    // before the lower levels).
    //
    // If pte_is_2m already and we don't have a big table, we're splitting so we
    // have to allocate.
    if (UVM_ID_IS_INVALID(resident_id) && !gpu_state->page_table_range_big.table && !gpu_state->pte_is_2m)
        can_make_new_big_ptes = false;

    for_each_va_block_page_in_region_mask(page_index, pages_changing, big_region_all) {
        uvm_cpu_chunk_t *chunk = NULL;
        int nid = NUMA_NO_NODE;

        if (UVM_ID_IS_CPU(resident_id)) {
            nid = block_get_page_node_residency(block, page_index);
            UVM_ASSERT(nid != NUMA_NO_NODE);
            chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);
        }

        big_page_index = uvm_va_block_big_page_index(block, page_index, big_page_size);
        big_page_region = uvm_va_block_big_page_region(block, big_page_index, big_page_size);

        __set_bit(big_page_index, new_pte_state->big_ptes_covered);

        // When mapping sysmem, we can use big pages only if we are mapping all
        // pages in the big page subregion and the CPU pages backing the
        // subregion are physically contiguous.
        if (can_make_new_big_ptes &&
            uvm_page_mask_region_full(page_mask_after, big_page_region) &&
            (!UVM_ID_IS_CPU(resident_id) ||
             (uvm_cpu_chunk_get_size(chunk) >= big_page_size &&
              uvm_va_block_cpu_is_region_resident_on(block, nid, big_page_region))))
            __set_bit(big_page_index, new_pte_state->big_ptes);

        if (!test_bit(big_page_index, new_pte_state->big_ptes))
            new_pte_state->needs_4k = true;

        // Skip to the end of the region
        page_index = big_page_region.outer - 1;
    }

    if (!new_pte_state->needs_4k) {
        // All big page regions in pages_changing will be big PTEs. Now check if
        // there are any unaligned pages outside of big_region_all which are
        // changing.
        region = uvm_va_block_region(0, big_region_all.first);
        if (!uvm_page_mask_region_empty(pages_changing, region)) {
            new_pte_state->needs_4k = true;
        }
        else {
            region = uvm_va_block_region(big_region_all.outer, uvm_va_block_num_cpu_pages(block));
            if (!uvm_page_mask_region_empty(pages_changing, region))
                new_pte_state->needs_4k = true;
        }
    }

    // Now add in the PTEs which should be big but weren't covered by this
    // operation.
    //
    // Note that we can't assume that a given page table range has been
    // initialized if it's present here, since it could have been allocated by a
    // thread which had to restart its operation due to allocation retry.
    if (gpu_state->pte_is_2m || (block_gpu_supports_2m(block, gpu) && !gpu_state->page_table_range_2m.table)) {
        // We're splitting a 2M PTE so all of the uncovered big PTE regions will
        // become big PTEs which inherit the 2M permissions. If we haven't
        // allocated the 2M table yet, it will start as a 2M PTE until the lower
        // levels are allocated, so it's the same split case regardless of
        // whether this operation will need to retry a later allocation.
        bitmap_complement(big_ptes_not_covered, new_pte_state->big_ptes_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    }
    else if (!gpu_state->page_table_range_4k.table && !new_pte_state->needs_4k) {
        // If we don't have 4k PTEs and we won't be allocating them for this
        // operation, all of our PTEs need to be big.
        UVM_ASSERT(!bitmap_empty(new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
        bitmap_zero(big_ptes_not_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
        bitmap_set(big_ptes_not_covered, 0, uvm_va_block_num_big_pages(block, big_page_size));
    }
    else {
        // Otherwise, add in all of the currently-big PTEs which are unchanging.
        // They won't be written, but they need to be carried into the new
        // gpu_state->big_ptes when it's updated.
        bitmap_andnot(big_ptes_not_covered,
                      gpu_state->big_ptes,
                      new_pte_state->big_ptes_covered,
                      MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    }

    bitmap_or(new_pte_state->big_ptes, new_pte_state->big_ptes, big_ptes_not_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
}

// Wrapper around uvm_page_tree_get_ptes() and uvm_page_tree_alloc_table() that
// handles allocation retry. If the block lock has been unlocked and relocked as
// part of the allocation, NV_ERR_MORE_PROCESSING_REQUIRED is returned to signal
// to the caller that the operation likely needs to be restarted. If that
// happens, the pending tracker is added to the block's tracker.
static NV_STATUS block_alloc_pt_range_with_retry(uvm_va_block_t *va_block,
                                                 uvm_gpu_t *gpu,
                                                 NvU64 page_size,
                                                 uvm_page_table_range_t *page_table_range,
                                                 uvm_tracker_t *pending_tracker)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    uvm_gpu_va_space_t *gpu_va_space = uvm_va_block_get_gpu_va_space(va_block, gpu);
    uvm_page_tree_t *page_tables = &gpu_va_space->page_tables;
    uvm_va_block_test_t *va_block_test = uvm_va_block_get_test(va_block);
    uvm_page_table_range_t local_range;
    NV_STATUS status;

    // Blocks may contain large PTEs without starting on a PTE boundary or
    // having an aligned size. Cover the PTEs of this size in the block's
    // interior so we match uvm_va_block_gpu_state_t::big_ptes.
    NvU64 start = UVM_ALIGN_UP(va_block->start, page_size);
    NvU64 size  = UVM_ALIGN_DOWN(va_block->end + 1, page_size) - start;

    // VA blocks which can use the 2MB level as either a PTE or a PDE need to
    // account for the PDE specially, so they must use uvm_page_tree_alloc_table
    // to allocate the lower levels.
    bool use_alloc_table = block_gpu_supports_2m(va_block, gpu) && page_size < UVM_PAGE_SIZE_2M;

    UVM_ASSERT(page_table_range->table == NULL);

    if (va_block_test && va_block_test->page_table_allocation_retry_force_count > 0) {
        --va_block_test->page_table_allocation_retry_force_count;
        status = NV_ERR_NO_MEMORY;
    }
    else if (use_alloc_table) {
        // Pascal+: 4k/64k tables under a 2M entry
        UVM_ASSERT(gpu_state->page_table_range_2m.table);
        status = uvm_page_tree_alloc_table(page_tables,
                                           page_size,
                                           UVM_PMM_ALLOC_FLAGS_NONE,
                                           &gpu_state->page_table_range_2m,
                                           page_table_range);
    }
    else {
        // 4k/big tables on pre-Pascal, and the 2M entry on Pascal+
        status = uvm_page_tree_get_ptes(page_tables,
                                        page_size,
                                        start,
                                        size,
                                        UVM_PMM_ALLOC_FLAGS_NONE,
                                        page_table_range);
    }

    if (status == NV_OK)
        goto allocated;

    if (status != NV_ERR_NO_MEMORY)
        return status;

    // Before unlocking the block lock, any pending work on the block has to be
    // added to the block's tracker.
    if (pending_tracker) {
        status = uvm_tracker_add_tracker_safe(&va_block->tracker, pending_tracker);
        if (status != NV_OK)
            return status;
    }

    // Unlock the va block and retry with eviction enabled
    uvm_mutex_unlock(&va_block->lock);

    if (use_alloc_table) {
        // Although we don't hold the block lock here, it's safe to pass
        // gpu_state->page_table_range_2m to the page tree code because we know
        // that the 2m range has already been allocated, and that it can't go
        // away while we have the va_space lock held.
        status = uvm_page_tree_alloc_table(page_tables,
                                           page_size,
                                           UVM_PMM_ALLOC_FLAGS_EVICT,
                                           &gpu_state->page_table_range_2m,
                                           &local_range);
    }
    else {
        status = uvm_page_tree_get_ptes(page_tables,
                                        page_size,
                                        start,
                                        size,
                                        UVM_PMM_ALLOC_FLAGS_EVICT,
                                        &local_range);
    }

    uvm_mutex_lock(&va_block->lock);

    if (status != NV_OK)
        return status;

    status = NV_ERR_MORE_PROCESSING_REQUIRED;

    if (page_table_range->table) {
        // A different caller allocated the page tables in the meantime, release the
        // local copy.
        uvm_page_tree_put_ptes(page_tables, &local_range);
        return status;
    }

    *page_table_range = local_range;

allocated:
    // Mark the 2M PTE as active when we first allocate it, since we don't have
    // any PTEs below it yet.
    if (page_size == UVM_PAGE_SIZE_2M) {
        UVM_ASSERT(!gpu_state->pte_is_2m);
        gpu_state->pte_is_2m = true;
    }
    else if (page_size != UVM_PAGE_SIZE_4K) {
        // uvm_page_tree_get_ptes initializes big PTEs to invalid.
        // uvm_page_tree_alloc_table does not, so we'll have to do it later.
        if (use_alloc_table)
            UVM_ASSERT(!gpu_state->initialized_big);
        else
            gpu_state->initialized_big = true;
    }

    return status;
}

// Helper which allocates all page table ranges necessary for the given page
// sizes. See block_alloc_pt_range_with_retry.
static NV_STATUS block_alloc_ptes_with_retry(uvm_va_block_t *va_block,
                                             uvm_gpu_t *gpu,
                                             NvU64 page_sizes,
                                             uvm_tracker_t *pending_tracker)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    uvm_gpu_va_space_t *gpu_va_space = uvm_va_block_get_gpu_va_space(va_block, gpu);
    uvm_page_table_range_t *range;
    NvU64 page_size;
    NV_STATUS status, final_status = NV_OK;

    UVM_ASSERT(gpu_state);

    // Blocks which can map 2M PTE/PDEs must always allocate the 2MB level first
    // in order to allocate the levels below.
    if (block_gpu_supports_2m(va_block, gpu))
        page_sizes |= UVM_PAGE_SIZE_2M;

    UVM_ASSERT((page_sizes & gpu_va_space->page_tables.hal->page_sizes()) == page_sizes);

    for_each_chunk_size_rev(page_size, page_sizes) {
        if (page_size == UVM_PAGE_SIZE_2M)
            range = &gpu_state->page_table_range_2m;
        else if (page_size == UVM_PAGE_SIZE_4K)
            range = &gpu_state->page_table_range_4k;
        else
            range = &gpu_state->page_table_range_big;

        if (range->table)
            continue;

        if (page_size == UVM_PAGE_SIZE_2M) {
            UVM_ASSERT(!gpu_state->pte_is_2m);
            UVM_ASSERT(!gpu_state->page_table_range_big.table);
            UVM_ASSERT(!gpu_state->page_table_range_4k.table);
        }
        else if (page_size != UVM_PAGE_SIZE_4K) {
            UVM_ASSERT(uvm_va_block_num_big_pages(va_block, uvm_va_block_gpu_big_page_size(va_block, gpu)) > 0);
            UVM_ASSERT(bitmap_empty(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
        }

        status = block_alloc_pt_range_with_retry(va_block, gpu, page_size, range, pending_tracker);

        // Keep going to allocate the remaining levels even if the allocation
        // requires a retry, since we'll likely still need them when we retry
        // anyway.
        if (status == NV_ERR_MORE_PROCESSING_REQUIRED)
            final_status = NV_ERR_MORE_PROCESSING_REQUIRED;
        else if (status != NV_OK)
            return status;
    }

    return final_status;
}

static NV_STATUS block_alloc_ptes_new_state(uvm_va_block_t *va_block,
                                            uvm_gpu_t *gpu,
                                            uvm_va_block_new_pte_state_t *new_pte_state,
                                            uvm_tracker_t *pending_tracker)
{
    NvU64 page_sizes = 0;

    if (new_pte_state->pte_is_2m) {
        page_sizes |= UVM_PAGE_SIZE_2M;
    }
    else {
        if (!bitmap_empty(new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK))
            page_sizes |= uvm_va_block_gpu_big_page_size(va_block, gpu);

        if (new_pte_state->needs_4k)
            page_sizes |= UVM_PAGE_SIZE_4K;
        else
            UVM_ASSERT(!bitmap_empty(new_pte_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK));
    }

    return block_alloc_ptes_with_retry(va_block, gpu, page_sizes, pending_tracker);
}

// Make sure that GMMU PDEs down to PDE1 are populated for the given VA block.
// This is currently used on ATS systems to prevent GPUs from inadvertently
// accessing sysmem via ATS because there is no PDE1 in the GMMU page tables,
// which is where the NOATS bit resides.
//
// The current implementation simply pre-allocates the PTEs for the VA Block,
// which is wasteful because the GPU may never need them.
//
// TODO: Bug 2064188: Change the MMU code to be able to directly refcount PDE1
// page table entries without having to request PTEs.
static NV_STATUS block_pre_populate_pde1_gpu(uvm_va_block_t *block,
                                             uvm_gpu_va_space_t *gpu_va_space,
                                             uvm_tracker_t *pending_tracker)
{
    NvU64 page_sizes;
    NvU64 big_page_size;
    uvm_gpu_t *gpu;
    uvm_va_block_gpu_state_t *gpu_state;

    UVM_ASSERT(block);
    UVM_ASSERT(gpu_va_space);
    UVM_ASSERT(gpu_va_space->ats.enabled);
    UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_ACTIVE);

    gpu = gpu_va_space->gpu;
    big_page_size = gpu_va_space->page_tables.big_page_size;

    gpu_state = block_gpu_state_get_alloc(block, gpu);
    if (!gpu_state)
        return NV_ERR_NO_MEMORY;

    // If the VA Block supports 2M pages, allocate the 2M PTE only, as it
    // requires less memory
    if (block_gpu_supports_2m(block, gpu))
        page_sizes = UVM_PAGE_SIZE_2M;
    else if (uvm_va_block_num_big_pages(block, big_page_size) > 0)
        page_sizes = big_page_size;
    else
        page_sizes = UVM_PAGE_SIZE_4K;

    return block_alloc_ptes_with_retry(block, gpu, page_sizes, pending_tracker);
}

static NV_STATUS block_pre_populate_pde1_all_gpus(uvm_va_block_t *block, uvm_tracker_t *pending_tracker)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    NV_STATUS status = NV_OK;

    // Pre-populate PDEs down to PDE1 for all GPU VA spaces on ATS systems. See
    // comments in block_pre_populate_pde1_gpu.
    if (g_uvm_global.ats.enabled && !block->cpu.ever_mapped) {
        uvm_gpu_va_space_t *gpu_va_space;

        for_each_gpu_va_space(gpu_va_space, va_space) {
            // We only care about systems where ATS is supported and the application
            // enabled it.
            if (!gpu_va_space->ats.enabled)
                continue;

            status = block_pre_populate_pde1_gpu(block, gpu_va_space, pending_tracker);
            if (status != NV_OK)
                break;
        }
    }

    return status;
}

static NV_STATUS block_unmap_gpu(uvm_va_block_t *block,
                                 uvm_va_block_context_t *block_context,
                                 uvm_gpu_t *gpu,
                                 const uvm_page_mask_t *unmap_page_mask,
                                 uvm_tracker_t *out_tracker)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_pte_bits_gpu_t pte_bit;
    uvm_push_t push;
    uvm_membar_t tlb_membar;
    bool only_local_mappings;
    uvm_page_mask_t *pages_to_unmap = &block_context->mapping.page_mask;
    NV_STATUS status;
    uvm_va_block_new_pte_state_t *new_pte_state = &block_context->mapping.new_pte_state;
    bool mask_empty;
    uvm_processor_mask_t *non_uvm_lite_gpus = &block_context->mapping.non_uvm_lite_gpus;

    // We have to check gpu_state before looking at any VA space state like our
    // gpu_va_space, because we could be on the eviction path where we don't
    // have a lock on that state. However, since remove_gpu_va_space walks each
    // block to unmap the GPU before destroying the gpu_va_space, we're
    // guaranteed that if this GPU has page tables, the gpu_va_space can't go
    // away while we're holding the block lock.
    if (!block_gpu_has_page_tables(block, gpu))
        return NV_OK;

    if (!uvm_page_mask_and(pages_to_unmap, unmap_page_mask, &gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ]))
        return NV_OK;

    // block_gpu_compute_new_pte_state needs a mask of pages which will have
    // matching attributes after the operation is performed. In the case of
    // unmap, those are the pages with unset bits.
    uvm_page_mask_andnot(&block_context->scratch_page_mask, &gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ], pages_to_unmap);
    uvm_page_mask_complement(&block_context->scratch_page_mask, &block_context->scratch_page_mask);
    block_gpu_compute_new_pte_state(block,
                                    gpu,
                                    UVM_ID_INVALID,
                                    pages_to_unmap,
                                    &block_context->scratch_page_mask,
                                    new_pte_state);

    status = block_alloc_ptes_new_state(block, gpu, new_pte_state, out_tracker);
    if (status != NV_OK)
        return status;

    only_local_mappings = !block_has_remote_mapping_gpu(block, &block_context->scratch_page_mask, gpu->id, pages_to_unmap);
    tlb_membar = uvm_hal_downgrade_membar_type(gpu, only_local_mappings);

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_MEMOPS,
                                    &block->tracker,
                                    &push,
                                    "Unmapping pages in block [0x%llx, 0x%llx)",
                                    block->start,
                                    block->end + 1);
    if (status != NV_OK)
        return status;

    if (new_pte_state->pte_is_2m) {
        // We're either unmapping a whole valid 2M PTE, or we're unmapping all
        // remaining pages in a split 2M PTE.
        block_gpu_unmap_to_2m(block, block_context, gpu, &push, tlb_membar);
    }
    else if (gpu_state->pte_is_2m) {
        // The block is currently mapped as a valid 2M PTE and we're unmapping
        // some pages within the 2M, so we have to split it into the appropriate
        // mix of big and 4k PTEs.
        block_gpu_unmap_split_2m(block, block_context, gpu, pages_to_unmap, &push, tlb_membar);
    }
    else {
        // We're unmapping some pre-existing mix of big and 4K PTEs into some
        // other mix of big and 4K PTEs.
        block_gpu_unmap_big_and_4k(block, block_context, gpu, pages_to_unmap, &push, tlb_membar);
    }

    uvm_push_end(&push);

    if (!uvm_processor_mask_test(block_get_uvm_lite_gpus(block), gpu->id)) {

        uvm_processor_mask_andnot(non_uvm_lite_gpus, &block->mapped, block_get_uvm_lite_gpus(block));

        UVM_ASSERT(uvm_processor_mask_test(non_uvm_lite_gpus, gpu->id));

        // If the GPU is the only non-UVM-Lite processor with mappings, we can
        // safely mark pages as fully unmapped
        if (uvm_processor_mask_get_count(non_uvm_lite_gpus) == 1 && !uvm_va_block_is_hmm(block))
            uvm_page_mask_andnot(&block->maybe_mapped_pages, &block->maybe_mapped_pages, pages_to_unmap);
    }

    // Clear block PTE state
    for (pte_bit = 0; pte_bit < UVM_PTE_BITS_GPU_MAX; pte_bit++) {
        mask_empty = !uvm_page_mask_andnot(&gpu_state->pte_bits[pte_bit],
                                           &gpu_state->pte_bits[pte_bit],
                                           pages_to_unmap);
        if (pte_bit == UVM_PTE_BITS_GPU_READ && mask_empty)
            uvm_processor_mask_clear(&block->mapped, gpu->id);
    }

    UVM_ASSERT(block_check_mappings(block, block_context));

    return uvm_tracker_add_push_safe(out_tracker, &push);
}

NV_STATUS uvm_va_block_unmap(uvm_va_block_t *va_block,
                             uvm_va_block_context_t *va_block_context,
                             uvm_processor_id_t id,
                             uvm_va_block_region_t region,
                             const uvm_page_mask_t *unmap_page_mask,
                             uvm_tracker_t *out_tracker)
{
    uvm_page_mask_t *region_page_mask = &va_block_context->mapping.map_running_page_mask;

    UVM_ASSERT(!uvm_va_block_is_dead(va_block));
    uvm_assert_mutex_locked(&va_block->lock);

    if (UVM_ID_IS_CPU(id)) {
       block_unmap_cpu(va_block, va_block_context, region, unmap_page_mask);
       return NV_OK;
    }

    uvm_page_mask_init_from_region(region_page_mask, region, unmap_page_mask);

    return block_unmap_gpu(va_block, va_block_context, block_get_gpu(va_block, id), region_page_mask, out_tracker);
}

// This function essentially works as a wrapper around vm_insert_page (hence
// the similar function prototype). This is needed since vm_insert_page
// doesn't take permissions as input, but uses vma->vm_page_prot instead.
// Since we may have multiple VA blocks under one VMA which need to map
// with different permissions, we have to manually change vma->vm_page_prot for
// each call to vm_insert_page. Multiple faults under one VMA in separate
// blocks can be serviced concurrently, so the VMA wrapper lock is used
// to protect access to vma->vm_page_prot.
static NV_STATUS uvm_cpu_insert_page(struct vm_area_struct *vma,
                                     NvU64 addr,
                                     struct page *page,
                                     uvm_prot_t new_prot)
{
    uvm_vma_wrapper_t *vma_wrapper;
    unsigned long target_flags;
    pgprot_t target_pgprot;
    int ret;

    UVM_ASSERT(vma);
    UVM_ASSERT(vma->vm_private_data);

    vma_wrapper = vma->vm_private_data;
    target_flags = vma->vm_flags;

    if (new_prot == UVM_PROT_READ_ONLY)
        target_flags &= ~VM_WRITE;

    target_pgprot = vm_get_page_prot(target_flags);

    // Take VMA wrapper lock to check vma->vm_page_prot
    uvm_down_read(&vma_wrapper->lock);

    // Take a write lock if we need to modify the VMA vm_page_prot
    // - vma->vm_page_prot creates writable PTEs but new prot is RO
    // - vma->vm_page_prot creates read-only PTEs but new_prot is RW
    if (pgprot_val(vma->vm_page_prot) != pgprot_val(target_pgprot)) {
        uvm_up_read(&vma_wrapper->lock);
        uvm_down_write(&vma_wrapper->lock);

        vma->vm_page_prot = target_pgprot;

        uvm_downgrade_write(&vma_wrapper->lock);
    }

    ret = vm_insert_page(vma, addr, page);
    uvm_up_read(&vma_wrapper->lock);
    if (ret) {
        UVM_ASSERT_MSG(ret == -ENOMEM, "ret: %d\n", ret);
        return errno_to_nv_status(ret);
    }

    return NV_OK;
}

static uvm_prot_t compute_logical_prot(uvm_va_block_t *va_block,
                                       struct vm_area_struct *hmm_vma,
                                       uvm_page_index_t page_index)
{
    uvm_prot_t logical_prot;

    if (uvm_va_block_is_hmm(va_block)) {
        NvU64 addr = uvm_va_block_cpu_page_address(va_block, page_index);

        logical_prot = uvm_hmm_compute_logical_prot(va_block, hmm_vma, addr);
    }
    else {
        uvm_va_range_t *va_range = va_block->va_range;

        UVM_ASSERT(va_range->type == UVM_VA_RANGE_TYPE_MANAGED);

        // Zombified VA ranges no longer have a vma, so they have no permissions
        if (uvm_va_range_is_managed_zombie(va_range)) {
            logical_prot = UVM_PROT_NONE;
        }
        else {
            struct vm_area_struct *vma;

            vma = uvm_va_range_vma(va_range);

            if (!(vma->vm_flags & VM_READ))
                logical_prot = UVM_PROT_NONE;
            else if (!(vma->vm_flags & VM_WRITE))
                logical_prot = UVM_PROT_READ_ONLY;
            else
                logical_prot = UVM_PROT_READ_WRITE_ATOMIC;
        }
    }

    return logical_prot;
}

static struct page *block_page_get(uvm_va_block_t *block, block_phys_page_t block_page)
{
    struct page *page;

    if (UVM_ID_IS_CPU(block_page.processor)) {
        page = uvm_va_block_get_cpu_page(block, block_page.page_index);
    }
    else {
        uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
        uvm_gpu_t *gpu = uvm_va_space_get_gpu(va_space, block_page.processor);
        size_t chunk_offset;
        uvm_gpu_chunk_t *chunk = block_phys_page_chunk(block, block_page, &chunk_offset);

        UVM_ASSERT(gpu->mem_info.numa.enabled);
        page = uvm_gpu_chunk_to_page(&gpu->pmm, chunk) + chunk_offset / PAGE_SIZE;
    }

    UVM_ASSERT(page);
    return page;
}

// Creates or upgrades a CPU mapping for the given page, updating the block's
// mapping and pte_bits bitmaps as appropriate. Upon successful return, the page
// will be mapped with at least new_prot permissions.
//
// This never downgrades mappings, so new_prot must not be UVM_PROT_NONE. Use
// block_unmap_cpu or uvm_va_block_revoke_prot instead.
//
// If the existing mapping is >= new_prot already, this is a no-op.
//
// It is the caller's responsibility to:
//  - Revoke mappings from other processors as appropriate so the CPU can map
//    with new_prot permissions
//  - Guarantee that vm_insert_page is safe to use (vma->vm_mm has a reference
//    and mmap_lock is held in at least read mode)
//  - For HMM blocks that vma is valid and safe to use, vma->vm_mm has a
//    reference and mmap_lock is held in at least read mode
//  - Ensure that the struct page corresponding to the physical memory being
//    mapped exists
//  - Manage the block's residency bitmap
//  - Ensure that the block hasn't been killed (block->va_range is present)
//  - Update the pte/mapping tracking state on success
static NV_STATUS block_map_cpu_page_to(uvm_va_block_t *block,
                                       struct vm_area_struct *hmm_vma,
                                       uvm_processor_id_t resident_id,
                                       uvm_page_index_t page_index,
                                       uvm_prot_t new_prot)
{
    uvm_prot_t curr_prot = block_page_prot_cpu(block, page_index);
    uvm_va_range_t *va_range = block->va_range;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    struct vm_area_struct *vma;
    NV_STATUS status;
    NvU64 addr;
    struct page *page;
    int nid = NUMA_NO_NODE;

    UVM_ASSERT((uvm_va_block_is_hmm(block) && hmm_vma) || va_range->type == UVM_VA_RANGE_TYPE_MANAGED);
    UVM_ASSERT(new_prot != UVM_PROT_NONE);
    UVM_ASSERT(new_prot < UVM_PROT_MAX);
    UVM_ASSERT(uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(resident_id)], UVM_ID_CPU));

    uvm_assert_mutex_locked(&block->lock);
    if (UVM_ID_IS_CPU(resident_id))
        UVM_ASSERT(uvm_page_mask_test(&block->cpu.allocated, page_index));

    // For the CPU, write implies atomic
    if (new_prot == UVM_PROT_READ_WRITE)
        new_prot = UVM_PROT_READ_WRITE_ATOMIC;

    // Only upgrades are supported in this function
    UVM_ASSERT(curr_prot <= new_prot);

    if (new_prot == curr_prot)
        return NV_OK;

    // Check for existing VMA permissions. They could have been modified after
    // the initial mmap by mprotect.
    if (new_prot > compute_logical_prot(block, hmm_vma, page_index))
        return NV_ERR_INVALID_ACCESS_TYPE;

    if (uvm_va_block_is_hmm(block)) {
        // Do not map CPU pages because they belong to the Linux kernel.
        return NV_OK;
    }

    UVM_ASSERT(va_range);

    if (UVM_ID_IS_CPU(resident_id)) {
        if (UVM_ID_IS_CPU(uvm_va_range_get_policy(va_range)->preferred_location)) {
            // Add the page's range group range to the range group's migrated list.
            uvm_range_group_range_t *rgr = uvm_range_group_range_find(va_space,
                                                                      uvm_va_block_cpu_page_address(block, page_index));
            if (rgr != NULL) {
                uvm_spin_lock(&rgr->range_group->migrated_ranges_lock);
                if (list_empty(&rgr->range_group_migrated_list_node))
                    list_move_tail(&rgr->range_group_migrated_list_node, &rgr->range_group->migrated_ranges);
                uvm_spin_unlock(&rgr->range_group->migrated_ranges_lock);
            }
        }

        nid = block_get_page_node_residency(block, page_index);
        UVM_ASSERT(nid != NUMA_NO_NODE);
    }

    // It's possible here that current->mm != vma->vm_mm. That can happen for
    // example due to access_process_vm (ptrace) or get_user_pages from another
    // driver.
    //
    // In such cases the caller has taken care of ref counting vma->vm_mm for
    // us, so we can safely operate on the vma but we can't use
    // uvm_va_range_vma_current.
    vma = uvm_va_range_vma(va_range);
    uvm_assert_mmap_lock_locked(vma->vm_mm);
    UVM_ASSERT(!uvm_va_space_mm_enabled(va_space) || va_space->va_space_mm.mm == vma->vm_mm);

    // Add the mapping
    addr = uvm_va_block_cpu_page_address(block, page_index);

    // This unmap handles upgrades as vm_insert_page returns -EBUSY when
    // there's already a mapping present at fault_addr, so we have to unmap
    // first anyway when upgrading from RO -> RW.
    if (curr_prot != UVM_PROT_NONE)
        unmap_mapping_range(va_space->mapping, addr, PAGE_SIZE, 1);

    // Don't map the CPU until prior copies and GPU PTE updates finish,
    // otherwise we might not stay coherent.
    status = uvm_tracker_wait(&block->tracker);
    if (status != NV_OK)
        return status;

    page = block_page_get(block, block_phys_page(resident_id, nid, page_index));
    return uvm_cpu_insert_page(vma, addr, page, new_prot);
}

// Maps the CPU to the given pages which are resident on resident_id.
// map_page_mask is an in/out parameter: the pages which are mapped to
// resident_id are removed from the mask before returning.
//
// Caller must ensure that:
// -  Pages in map_page_mask must not be set in the corresponding cpu.pte_bits
// mask for the requested protection.
static NV_STATUS block_map_cpu_to(uvm_va_block_t *block,
                                  uvm_va_block_context_t *block_context,
                                  uvm_processor_id_t resident_id,
                                  uvm_va_block_region_t region,
                                  uvm_page_mask_t *map_page_mask,
                                  uvm_prot_t new_prot,
                                  uvm_tracker_t *out_tracker)
{
    NV_STATUS status = NV_OK;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_page_index_t page_index;
    uvm_page_mask_t *pages_to_map = &block_context->mapping.page_mask;
    const uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(block, resident_id, NUMA_NO_NODE);
    uvm_pte_bits_cpu_t prot_pte_bit = get_cpu_pte_bit_index(new_prot);
    uvm_pte_bits_cpu_t pte_bit;

    UVM_ASSERT(uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(resident_id)], UVM_ID_CPU));

    // TODO: Bug 1766424: Check if optimizing the unmap_mapping_range calls
    //       within block_map_cpu_page_to by doing them once here is helpful.

    UVM_ASSERT(!uvm_page_mask_and(&block_context->scratch_page_mask,
                                  map_page_mask,
                                  &block->cpu.pte_bits[prot_pte_bit]));

    // The pages which will actually change are those in the input page mask
    // which are resident on the target.
    if (!uvm_page_mask_and(pages_to_map, map_page_mask, resident_mask))
        return NV_OK;

    status = block_pre_populate_pde1_all_gpus(block, out_tracker);
    if (status != NV_OK)
        return status;

    block->cpu.ever_mapped = true;

    for_each_va_block_page_in_region_mask(page_index, pages_to_map, region) {
        status = block_map_cpu_page_to(block,
                                       block_context->hmm.vma,
                                       resident_id,
                                       page_index,
                                       new_prot);
        if (status != NV_OK)
            break;

        uvm_processor_mask_set(&block->mapped, UVM_ID_CPU);
    }

    // If there was some error, shrink the region so that we only update the
    // pte/mapping tracking bits for the pages that succeeded
    if (status != NV_OK) {
        region = uvm_va_block_region(region.first, page_index);
        uvm_page_mask_region_clear_outside(pages_to_map, region);
    }

    // If pages are mapped from a remote residency, notify the remote mapping
    // events to tools. We skip event notification if the cause is Invalid. We
    // use it to signal that this function is being called from the revocation
    // path to avoid reporting duplicate events.
    if (UVM_ID_IS_GPU(resident_id) &&
        va_space->tools.enabled &&
        block_context->mapping.cause != UvmEventMapRemoteCauseInvalid) {
        uvm_va_block_region_t subregion;
        for_each_va_block_subregion_in_mask(subregion, pages_to_map, region) {
            uvm_tools_record_map_remote(block,
                                        NULL,
                                        UVM_ID_CPU,
                                        resident_id,
                                        uvm_va_block_region_start(block, subregion),
                                        uvm_va_block_region_size(subregion),
                                        block_context->mapping.cause);
        }
    }

    // Update CPU mapping state
    for (pte_bit = 0; pte_bit <= prot_pte_bit; pte_bit++)
        uvm_page_mask_or(&block->cpu.pte_bits[pte_bit], &block->cpu.pte_bits[pte_bit], pages_to_map);

    if (!uvm_va_block_is_hmm(block))
        uvm_page_mask_or(&block->maybe_mapped_pages, &block->maybe_mapped_pages, pages_to_map);

    UVM_ASSERT(block_check_mappings(block, block_context));

    // Remove all pages that were newly-mapped from the input mask
    uvm_page_mask_andnot(map_page_mask, map_page_mask, pages_to_map);

    return status;
}

// Maps the GPU to the given pages which are resident on resident_id.
// map_page_mask is an in/out parameter: the pages which are mapped
// to resident_id are removed from the mask before returning.
//
// Caller must ensure that:
// -  Pages in map_page_mask must not be set in the corresponding pte_bits mask
// for the requested protection on the mapping GPU.
static NV_STATUS block_map_gpu_to(uvm_va_block_t *va_block,
                                  uvm_va_block_context_t *block_context,
                                  uvm_gpu_t *gpu,
                                  uvm_processor_id_t resident_id,
                                  int resident_nid,
                                  uvm_page_mask_t *map_page_mask,
                                  uvm_prot_t new_prot,
                                  uvm_tracker_t *out_tracker)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_push_t push;
    NV_STATUS status;
    uvm_page_mask_t *pages_to_map = &block_context->mapping.page_mask;
    const uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(va_block, resident_id, resident_nid);
    uvm_pte_bits_gpu_t pte_bit;
    uvm_pte_bits_gpu_t prot_pte_bit = get_gpu_pte_bit_index(new_prot);
    uvm_va_block_new_pte_state_t *new_pte_state = &block_context->mapping.new_pte_state;
    block_pte_op_t pte_op;

    UVM_ASSERT(map_page_mask);
    UVM_ASSERT(uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(resident_id)], gpu->id));

    if (uvm_processor_mask_test(block_get_uvm_lite_gpus(va_block), gpu->id)) {
        uvm_va_policy_t *policy = uvm_va_range_get_policy(va_block->va_range);
        UVM_ASSERT(uvm_va_policy_preferred_location_equal(policy, resident_id, policy->preferred_nid));
    }

    UVM_ASSERT(!uvm_page_mask_and(&block_context->scratch_page_mask,
                                  map_page_mask,
                                  &gpu_state->pte_bits[prot_pte_bit]));

    // The pages which will actually change are those in the input page mask
    // which are resident on the target.
    if (!uvm_page_mask_and(pages_to_map, map_page_mask, resident_mask))
        return NV_OK;

    UVM_ASSERT(block_check_mapping_residency(va_block, block_context, gpu, resident_id, pages_to_map));

    // For PTE merge/split computation, compute all resident pages which will
    // have exactly new_prot after performing the mapping.
    uvm_page_mask_or(&block_context->scratch_page_mask, &gpu_state->pte_bits[prot_pte_bit], pages_to_map);
    if (prot_pte_bit < UVM_PTE_BITS_GPU_ATOMIC) {
        uvm_page_mask_andnot(&block_context->scratch_page_mask,
                             &block_context->scratch_page_mask,
                             &gpu_state->pte_bits[prot_pte_bit + 1]);
    }
    uvm_page_mask_and(&block_context->scratch_page_mask, &block_context->scratch_page_mask, resident_mask);

    block_gpu_compute_new_pte_state(va_block,
                                    gpu,
                                    resident_id,
                                    pages_to_map,
                                    &block_context->scratch_page_mask,
                                    new_pte_state);

    status = block_alloc_ptes_new_state(va_block, gpu, new_pte_state, out_tracker);
    if (status != NV_OK)
        return status;

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_MEMOPS,
                                    &va_block->tracker,
                                    &push,
                                    "Mapping pages in block [0x%llx, 0x%llx) as %s",
                                    va_block->start,
                                    va_block->end + 1,
                                    uvm_prot_string(new_prot));
    if (status != NV_OK)
        return status;

    pte_op = BLOCK_PTE_OP_MAP;
    if (new_pte_state->pte_is_2m) {
        // We're either modifying permissions of a pre-existing 2M PTE, or all
        // permissions match so we can merge to a new 2M PTE.
        block_gpu_map_to_2m(va_block, block_context, gpu, resident_id, new_prot, &push, pte_op);
    }
    else if (gpu_state->pte_is_2m) {
        // Permissions on a subset of the existing 2M PTE are being upgraded, so
        // we have to split it into the appropriate mix of big and 4k PTEs.
        block_gpu_map_split_2m(va_block, block_context, gpu, resident_id, pages_to_map, new_prot, &push, pte_op);
    }
    else {
        // We're upgrading permissions on some pre-existing mix of big and 4K
        // PTEs into some other mix of big and 4K PTEs.
        block_gpu_map_big_and_4k(va_block, block_context, gpu, resident_id, pages_to_map, new_prot, &push, pte_op);
    }

    // If we are mapping remotely, record the event
    if (va_space->tools.enabled && !uvm_id_equal(resident_id, gpu->id)) {
        uvm_va_block_region_t subregion, region = uvm_va_block_region_from_block(va_block);

        UVM_ASSERT(block_context->mapping.cause != UvmEventMapRemoteCauseInvalid);

        for_each_va_block_subregion_in_mask(subregion, pages_to_map, region) {
            uvm_tools_record_map_remote(va_block,
                                        &push,
                                        gpu->id,
                                        resident_id,
                                        uvm_va_block_region_start(va_block, subregion),
                                        uvm_va_block_region_size(subregion),
                                        block_context->mapping.cause);
        }
    }

    uvm_push_end(&push);

    // Update GPU mapping state
    for (pte_bit = 0; pte_bit <= prot_pte_bit; pte_bit++)
        uvm_page_mask_or(&gpu_state->pte_bits[pte_bit], &gpu_state->pte_bits[pte_bit], pages_to_map);

    uvm_processor_mask_set(&va_block->mapped, gpu->id);

    // If we are mapping a UVM-Lite GPU or HMM va_block, do not update
    // maybe_mapped_pages.
    if (!uvm_processor_mask_test(block_get_uvm_lite_gpus(va_block), gpu->id) &&
        !uvm_va_block_is_hmm(va_block))
        uvm_page_mask_or(&va_block->maybe_mapped_pages, &va_block->maybe_mapped_pages, pages_to_map);

    // Remove all pages resident on this processor from the input mask, which
    // were newly-mapped.
    uvm_page_mask_andnot(map_page_mask, map_page_mask, pages_to_map);

    UVM_ASSERT(block_check_mappings(va_block, block_context));

    return uvm_tracker_add_push_safe(out_tracker, &push);
}

// allowed_nid_mask is only valid if the CPU is set in allowed_mask.
static void map_get_allowed_destinations(uvm_va_block_t *block,
                                         uvm_va_block_context_t *va_block_context,
                                         const uvm_va_policy_t *policy,
                                         uvm_processor_id_t id,
                                         uvm_processor_mask_t *allowed_mask,
                                         nodemask_t *allowed_nid_mask)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

    *allowed_nid_mask = node_possible_map;

    if (uvm_processor_mask_test(block_get_uvm_lite_gpus(block), id)) {
        // UVM-Lite can only map resident pages on the preferred location
        uvm_processor_mask_zero(allowed_mask);
        uvm_processor_mask_set(allowed_mask, policy->preferred_location);
        if (UVM_ID_IS_CPU(policy->preferred_location) &&
            !uvm_va_policy_preferred_location_equal(policy, UVM_ID_CPU, NUMA_NO_NODE)) {
            nodes_clear(*allowed_nid_mask);
            node_set(policy->preferred_nid, *allowed_nid_mask);
        }
    }
    else if ((uvm_va_policy_is_read_duplicate(policy, va_space) ||
              (uvm_id_equal(policy->preferred_location, id) &&
               !is_uvm_fault_force_sysmem_set() &&
               !uvm_hmm_must_use_sysmem(block, va_block_context->hmm.vma))) &&
             uvm_va_space_processor_has_memory(va_space, id)) {
        // When operating under read-duplication we should only map the local
        // processor to cause fault-and-duplicate of remote pages.
        //
        // The same holds when this processor is the preferred location: only
        // create local mappings to force remote pages to fault-and-migrate.
        uvm_processor_mask_zero(allowed_mask);
        uvm_processor_mask_set(allowed_mask, id);
    }
    else {
        // Common case: Just map wherever the memory happens to reside
        uvm_processor_mask_and(allowed_mask, &block->resident, &va_space->can_access[uvm_id_value(id)]);
        return;
    }

    // Clamp to resident and accessible processors
    uvm_processor_mask_and(allowed_mask, allowed_mask, &block->resident);
    uvm_processor_mask_and(allowed_mask, allowed_mask, &va_space->can_access[uvm_id_value(id)]);
}

NV_STATUS uvm_va_block_map(uvm_va_block_t *va_block,
                           uvm_va_block_context_t *va_block_context,
                           uvm_processor_id_t id,
                           uvm_va_block_region_t region,
                           const uvm_page_mask_t *map_page_mask,
                           uvm_prot_t new_prot,
                           UvmEventMapRemoteCause cause,
                           uvm_tracker_t *out_tracker)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_gpu_t *gpu = NULL;
    uvm_processor_mask_t *allowed_destinations;
    uvm_processor_id_t resident_id;
    const uvm_page_mask_t *pte_mask;
    uvm_page_mask_t *running_page_mask = &va_block_context->mapping.map_running_page_mask;
    NV_STATUS status = NV_OK;
    const uvm_va_policy_t *policy = uvm_va_policy_get_region(va_block, region);
    nodemask_t *allowed_nid_destinations;

    va_block_context->mapping.cause = cause;

    UVM_ASSERT(new_prot != UVM_PROT_NONE);
    UVM_ASSERT(new_prot < UVM_PROT_MAX);
    uvm_assert_mutex_locked(&va_block->lock);

    // Mapping is not supported on the eviction path that doesn't hold the VA
    // space lock.
    uvm_assert_rwsem_locked(&va_space->lock);

    if (UVM_ID_IS_CPU(id)) {
        uvm_pte_bits_cpu_t prot_pte_bit;

        // Check if the current thread is allowed to call vm_insert_page
        if (!uvm_va_block_is_hmm(va_block) && !uvm_va_range_vma_check(va_block->va_range, va_block_context->mm))
            return NV_OK;

        prot_pte_bit = get_cpu_pte_bit_index(new_prot);
        pte_mask = &va_block->cpu.pte_bits[prot_pte_bit];
    }
    else {
        uvm_va_block_gpu_state_t *gpu_state;
        uvm_pte_bits_gpu_t prot_pte_bit;

        gpu = uvm_va_space_get_gpu(va_space, id);

        // Although this GPU UUID is registered in the VA space, it might not have a
        // GPU VA space registered.
        if (!uvm_gpu_va_space_get(va_space, gpu))
            return NV_OK;

        gpu_state = block_gpu_state_get_alloc(va_block, gpu);
        if (!gpu_state)
            return NV_ERR_NO_MEMORY;

        prot_pte_bit = get_gpu_pte_bit_index(new_prot);
        pte_mask = &gpu_state->pte_bits[prot_pte_bit];
    }

    uvm_page_mask_init_from_region(running_page_mask, region, map_page_mask);

    if (!uvm_page_mask_andnot(running_page_mask, running_page_mask, pte_mask))
        return NV_OK;

    allowed_destinations = uvm_processor_mask_cache_alloc();
    if (!allowed_destinations)
        return NV_ERR_NO_MEMORY;

    allowed_nid_destinations = uvm_kvmalloc(sizeof(*allowed_nid_destinations));
    if (!allowed_nid_destinations) {
        uvm_processor_mask_cache_free(allowed_destinations);
        return NV_ERR_NO_MEMORY;
    }

    // Map per resident location so we can more easily detect physically-
    // contiguous mappings.
    map_get_allowed_destinations(va_block,
                                 va_block_context,
                                 policy,
                                 id,
                                 allowed_destinations,
                                 allowed_nid_destinations);
    for_each_closest_id(resident_id, allowed_destinations, id, va_space) {
        if (UVM_ID_IS_CPU(id)) {
            status = block_map_cpu_to(va_block,
                                      va_block_context,
                                      resident_id,
                                      region,
                                      running_page_mask,
                                      new_prot,
                                      out_tracker);
        }
        else if (UVM_ID_IS_CPU(resident_id)) {
            int nid;

            // map_get_allowed_distinations() will set the mask of CPU NUMA
            // nodes that should be mapped.
            for_each_node_mask(nid, *allowed_nid_destinations) {
                status = block_map_gpu_to(va_block,
                                          va_block_context,
                                          gpu,
                                          resident_id,
                                          nid,
                                          running_page_mask,
                                          new_prot,
                                          out_tracker);
                if (status != NV_OK)
                    break;
            }
        }
        else {
            status = block_map_gpu_to(va_block,
                                      va_block_context,
                                      gpu,
                                      resident_id,
                                      NUMA_NO_NODE,
                                      running_page_mask,
                                      new_prot,
                                      out_tracker);
        }

        if (status != NV_OK)
            break;

        // If we've mapped all requested pages, we're done
        if (uvm_page_mask_region_empty(running_page_mask, region))
            break;
    }

    uvm_processor_mask_cache_free(allowed_destinations);
    uvm_kvfree(allowed_nid_destinations);

    return status;
}

// Revokes the given pages mapped by cpu. This is implemented by unmapping all
// pages and mapping them later with the lower permission. This is required
// because vm_insert_page can only be used for upgrades from Invalid.
//
// Caller must ensure that:
// -  Pages in revoke_page_mask must be set in the
// cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE] mask.
static NV_STATUS block_revoke_cpu_write(uvm_va_block_t *block,
                                        uvm_va_block_context_t *block_context,
                                        uvm_va_block_region_t region,
                                        const uvm_page_mask_t *revoke_page_mask,
                                        uvm_tracker_t *out_tracker)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_va_block_region_t subregion;

    UVM_ASSERT(revoke_page_mask);

    UVM_ASSERT(uvm_page_mask_subset(revoke_page_mask, &block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE]));

    block_unmap_cpu(block, block_context, region, revoke_page_mask);

    // Coalesce revocation event notification
    for_each_va_block_subregion_in_mask(subregion, revoke_page_mask, region) {
        uvm_perf_event_notify_revocation(&va_space->perf_events,
                                         block,
                                         UVM_ID_CPU,
                                         uvm_va_block_region_start(block, subregion),
                                         uvm_va_block_region_size(subregion),
                                         UVM_PROT_READ_WRITE_ATOMIC,
                                         UVM_PROT_READ_ONLY);
    }

    // uvm_va_block_map will skip this remap if we aren't holding the right mm
    // lock.
    return uvm_va_block_map(block,
                            block_context,
                            UVM_ID_CPU,
                            region,
                            revoke_page_mask,
                            UVM_PROT_READ_ONLY,
                            UvmEventMapRemoteCauseInvalid,
                            out_tracker);
}

static void block_revoke_prot_gpu_perf_notify(uvm_va_block_t *block,
                                              uvm_va_block_context_t *block_context,
                                              uvm_gpu_t *gpu,
                                              uvm_prot_t prot_revoked,
                                              const uvm_page_mask_t *pages_revoked)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, gpu->id);
    uvm_va_block_region_t subregion, region = uvm_va_block_region_from_block(block);
    uvm_pte_bits_gpu_t pte_bit;

    for (pte_bit = UVM_PTE_BITS_GPU_ATOMIC; pte_bit >= get_gpu_pte_bit_index(prot_revoked); pte_bit--) {
        uvm_prot_t old_prot;

        if (!uvm_page_mask_and(&block_context->scratch_page_mask, &gpu_state->pte_bits[pte_bit], pages_revoked))
            continue;

        if (pte_bit == UVM_PTE_BITS_GPU_ATOMIC)
            old_prot = UVM_PROT_READ_WRITE_ATOMIC;
        else
            old_prot = UVM_PROT_READ_WRITE;

        for_each_va_block_subregion_in_mask(subregion, &block_context->scratch_page_mask, region) {
            uvm_perf_event_notify_revocation(&va_space->perf_events,
                                             block,
                                             gpu->id,
                                             uvm_va_block_region_start(block, subregion),
                                             uvm_va_block_region_size(subregion),
                                             old_prot,
                                             prot_revoked - 1);
        }
    }
}

// Revokes the given pages mapped by gpu which are resident on resident_id.
// revoke_page_mask is an in/out parameter: the pages which have the appropriate
// permissions and are mapped to resident_id are removed from the mask before
// returning.
//
// Caller must ensure that:
// -  Pages in map_page_mask must be set in the corresponding pte_bits mask for
// the protection to be revoked on the mapping GPU.
static NV_STATUS block_revoke_prot_gpu_to(uvm_va_block_t *va_block,
                                          uvm_va_block_context_t *block_context,
                                          uvm_gpu_t *gpu,
                                          uvm_processor_id_t resident_id,
                                          uvm_page_mask_t *revoke_page_mask,
                                          uvm_prot_t prot_to_revoke,
                                          uvm_tracker_t *out_tracker)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    uvm_push_t push;
    NV_STATUS status;
    uvm_pte_bits_gpu_t pte_bit;
    uvm_pte_bits_gpu_t prot_pte_bit = get_gpu_pte_bit_index(prot_to_revoke);
    uvm_prot_t new_prot = prot_to_revoke - 1;
    uvm_va_block_new_pte_state_t *new_pte_state = &block_context->mapping.new_pte_state;
    block_pte_op_t pte_op;
    const uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(va_block, resident_id, NUMA_NO_NODE);
    uvm_page_mask_t *pages_to_revoke = &block_context->mapping.page_mask;

    UVM_ASSERT(revoke_page_mask);
    UVM_ASSERT(uvm_page_mask_subset(revoke_page_mask, &gpu_state->pte_bits[prot_pte_bit]));

    // The pages which will actually change are those in the input page mask
    // which are resident on the target.
    if (!uvm_page_mask_and(pages_to_revoke, revoke_page_mask, resident_mask))
        return NV_OK;

    UVM_ASSERT(block_check_mapping_residency(va_block, block_context, gpu, resident_id, pages_to_revoke));

    // For PTE merge/split computation, compute all resident pages which will
    // have exactly prot_to_revoke-1 after performing the revocation.
    uvm_page_mask_andnot(&block_context->scratch_page_mask, &gpu_state->pte_bits[prot_pte_bit], pages_to_revoke);
    uvm_page_mask_andnot(&block_context->scratch_page_mask,
                         &gpu_state->pte_bits[prot_pte_bit - 1],
                         &block_context->scratch_page_mask);
    uvm_page_mask_and(&block_context->scratch_page_mask, &block_context->scratch_page_mask, resident_mask);

    block_gpu_compute_new_pte_state(va_block,
                                    gpu,
                                    resident_id,
                                    pages_to_revoke,
                                    &block_context->scratch_page_mask,
                                    new_pte_state);

    status = block_alloc_ptes_new_state(va_block, gpu, new_pte_state, out_tracker);
    if (status != NV_OK)
        return status;

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_MEMOPS,
                                    &va_block->tracker,
                                    &push,
                                    "Revoking %s access privileges in block [0x%llx, 0x%llx) ",
                                    uvm_prot_string(prot_to_revoke),
                                    va_block->start,
                                    va_block->end + 1);
    if (status != NV_OK)
        return status;

    pte_op = BLOCK_PTE_OP_REVOKE;
    if (new_pte_state->pte_is_2m) {
        // We're either modifying permissions of a pre-existing 2M PTE, or all
        // permissions match so we can merge to a new 2M PTE.
        block_gpu_map_to_2m(va_block, block_context, gpu, resident_id, new_prot, &push, pte_op);
    }
    else if (gpu_state->pte_is_2m) {
        // Permissions on a subset of the existing 2M PTE are being downgraded,
        // so we have to split it into the appropriate mix of big and 4k PTEs.
        block_gpu_map_split_2m(va_block, block_context, gpu, resident_id, pages_to_revoke, new_prot, &push, pte_op);
    }
    else {
        // We're downgrading permissions on some pre-existing mix of big and 4K
        // PTEs into some other mix of big and 4K PTEs.
        block_gpu_map_big_and_4k(va_block, block_context, gpu, resident_id, pages_to_revoke, new_prot, &push, pte_op);
    }

    uvm_push_end(&push);

    block_revoke_prot_gpu_perf_notify(va_block, block_context, gpu, prot_to_revoke, pages_to_revoke);

    // Update GPU mapping state
    for (pte_bit = UVM_PTE_BITS_GPU_ATOMIC; pte_bit >= prot_pte_bit; pte_bit--)
        uvm_page_mask_andnot(&gpu_state->pte_bits[pte_bit], &gpu_state->pte_bits[pte_bit], pages_to_revoke);

    // Remove all pages resident on this processor from the input mask, which
    // pages which were revoked and pages which already had the correct
    // permissions.
    uvm_page_mask_andnot(revoke_page_mask, revoke_page_mask, pages_to_revoke);

    UVM_ASSERT(block_check_mappings(va_block, block_context));

    return uvm_tracker_add_push_safe(out_tracker, &push);
}

NV_STATUS uvm_va_block_revoke_prot(uvm_va_block_t *va_block,
                                   uvm_va_block_context_t *va_block_context,
                                   uvm_processor_id_t id,
                                   uvm_va_block_region_t region,
                                   const uvm_page_mask_t *revoke_page_mask,
                                   uvm_prot_t prot_to_revoke,
                                   uvm_tracker_t *out_tracker)
{
    uvm_gpu_t *gpu;
    uvm_va_block_gpu_state_t *gpu_state;
    uvm_processor_mask_t *resident_procs;
    uvm_processor_id_t resident_id;
    uvm_page_mask_t *running_page_mask = &va_block_context->mapping.revoke_running_page_mask;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_pte_bits_gpu_t prot_pte_bit;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(prot_to_revoke > UVM_PROT_READ_ONLY);
    UVM_ASSERT(prot_to_revoke < UVM_PROT_MAX);
    uvm_assert_mutex_locked(&va_block->lock);

    if (UVM_ID_IS_CPU(id)) {
        if (prot_to_revoke == UVM_PROT_READ_WRITE_ATOMIC)
            return NV_OK;

        if (uvm_va_block_is_hmm(va_block)) {
            // Linux is responsible for CPU page table updates.
            uvm_page_mask_region_clear(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE], region);
            return NV_OK;
        }

        uvm_page_mask_init_from_region(running_page_mask, region, revoke_page_mask);

        if (uvm_page_mask_and(running_page_mask, running_page_mask, &va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE]))
            return block_revoke_cpu_write(va_block, va_block_context, region, running_page_mask, out_tracker);

        return NV_OK;
    }

    gpu = uvm_va_space_get_gpu(va_space, id);

    // UVM-Lite GPUs should never have access revoked
    UVM_ASSERT_MSG(!uvm_processor_mask_test(block_get_uvm_lite_gpus(va_block), gpu->id),
                   "GPU %s\n", uvm_gpu_name(gpu));

    // Return early if there are no mappings for the GPU present in the block
    if (!uvm_processor_mask_test(&va_block->mapped, gpu->id))
        return NV_OK;

    gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    prot_pte_bit = get_gpu_pte_bit_index(prot_to_revoke);

    uvm_page_mask_init_from_region(running_page_mask, region, revoke_page_mask);

    if (!uvm_page_mask_and(running_page_mask, running_page_mask, &gpu_state->pte_bits[prot_pte_bit]))
        return NV_OK;

    resident_procs = uvm_processor_mask_cache_alloc();
    if (!resident_procs)
        return NV_ERR_NO_MEMORY;

    // Revoke per resident location so we can more easily detect physically-
    // contiguous mappings.
    uvm_processor_mask_copy(resident_procs, &va_block->resident);

    for_each_closest_id(resident_id, resident_procs, gpu->id, va_space) {
        NV_STATUS status = block_revoke_prot_gpu_to(va_block,
                                                    va_block_context,
                                                    gpu,
                                                    resident_id,
                                                    running_page_mask,
                                                    prot_to_revoke,
                                                    out_tracker);
        if (status != NV_OK)
            break;

        // If we've revoked all requested pages, we're done
        if (uvm_page_mask_region_empty(running_page_mask, region))
            break;
    }

    uvm_processor_mask_cache_free(resident_procs);

    return status;
}

NV_STATUS uvm_va_block_map_mask(uvm_va_block_t *va_block,
                                uvm_va_block_context_t *va_block_context,
                                const uvm_processor_mask_t *map_processor_mask,
                                uvm_va_block_region_t region,
                                const uvm_page_mask_t *map_page_mask,
                                uvm_prot_t new_prot,
                                UvmEventMapRemoteCause cause)
{
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;
    uvm_processor_id_t id;

    for_each_id_in_mask(id, map_processor_mask) {
        status = uvm_va_block_map(va_block,
                                  va_block_context,
                                  id,
                                  region,
                                  map_page_mask,
                                  new_prot,
                                  cause,
                                  &local_tracker);
        if (status != NV_OK)
            break;
    }

    // Regardless of error, add the successfully-pushed mapping operations into
    // the block's tracker. Note that we can't overwrite the tracker because we
    // aren't guaranteed that the map actually pushed anything (in which case it
    // would've acquired the block tracker first).
    tracker_status = uvm_tracker_add_tracker_safe(&va_block->tracker, &local_tracker);
    uvm_tracker_deinit(&local_tracker);

    return status == NV_OK ? tracker_status : status;
}

NV_STATUS uvm_va_block_unmap_mask(uvm_va_block_t *va_block,
                                  uvm_va_block_context_t *va_block_context,
                                  const uvm_processor_mask_t *unmap_processor_mask,
                                  uvm_va_block_region_t region,
                                  const uvm_page_mask_t *unmap_page_mask)
{
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;
    uvm_processor_id_t id;

    // Watch out, unmap_mask could change during iteration since it could be
    // va_block->mapped.
    for_each_id_in_mask(id, unmap_processor_mask) {
        // Errors could either be a system-fatal error (ECC) or an allocation
        // retry due to PTE splitting. In either case we should stop after
        // hitting the first one.
        status = uvm_va_block_unmap(va_block, va_block_context, id, region, unmap_page_mask, &local_tracker);
        if (status != NV_OK)
            break;
    }

    // See the comment in uvm_va_block_map_mask for adding to the tracker.
    tracker_status = uvm_tracker_add_tracker_safe(&va_block->tracker, &local_tracker);
    uvm_tracker_deinit(&local_tracker);

    return status == NV_OK ? tracker_status : status;
}

NV_STATUS uvm_va_block_revoke_prot_mask(uvm_va_block_t *va_block,
                                        uvm_va_block_context_t *va_block_context,
                                        const uvm_processor_mask_t *revoke_processor_mask,
                                        uvm_va_block_region_t region,
                                        const uvm_page_mask_t *revoke_page_mask,
                                        uvm_prot_t prot_to_revoke)
{
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;
    uvm_processor_id_t id;

    for_each_id_in_mask(id, revoke_processor_mask) {
        status = uvm_va_block_revoke_prot(va_block,
                                          va_block_context,
                                          id,
                                          region,
                                          revoke_page_mask,
                                          prot_to_revoke,
                                          &local_tracker);
        if (status != NV_OK)
            break;
    }

    // See the comment in uvm_va_block_map_mask for adding to the tracker.
    tracker_status = uvm_tracker_add_tracker_safe(&va_block->tracker, &local_tracker);
    uvm_tracker_deinit(&local_tracker);

    return status == NV_OK ? tracker_status : status;
}

// Updates the read_duplicated_pages mask in the block when the state of GPU id
// is being destroyed
static void update_read_duplicated_pages_mask(uvm_va_block_t *block,
                                              uvm_gpu_id_t id,
                                              uvm_va_block_gpu_state_t *gpu_state)
{
    uvm_gpu_id_t running_id;
    bool first = true;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_va_block_context_t *block_context = uvm_va_space_block_context(va_space, NULL);
    uvm_page_mask_t *running_page_mask = &block_context->update_read_duplicated_pages.running_page_mask;
    uvm_page_mask_t *tmp_page_mask = &block_context->scratch_page_mask;

    uvm_page_mask_zero(&block->read_duplicated_pages);

    for_each_id_in_mask(running_id, &block->resident) {
        const uvm_page_mask_t *running_residency_mask;

        if (uvm_id_equal(running_id, id))
            continue;

        running_residency_mask = uvm_va_block_resident_mask_get(block, running_id, NUMA_NO_NODE);

        if (first) {
            uvm_page_mask_copy(running_page_mask, running_residency_mask);
            first = false;
            continue;
        }

        if (uvm_page_mask_and(tmp_page_mask, running_page_mask, running_residency_mask))
            uvm_page_mask_or(&block->read_duplicated_pages, &block->read_duplicated_pages, tmp_page_mask);

        uvm_page_mask_or(running_page_mask, running_page_mask, running_residency_mask);
    }
}

// Unmaps all GPU mappings under this block, frees the page tables, and frees
// all the GPU chunks. This simply drops the chunks on the floor, so the caller
// must take care of copying the data elsewhere if it needs to remain intact.
//
// This serializes on the block tracker since it must unmap page tables.
static void block_destroy_gpu_state(uvm_va_block_t *block, uvm_va_block_context_t *block_context, uvm_gpu_id_t id)
{
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, id);
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_gpu_t *gpu;

    if (!gpu_state)
        return;

    uvm_assert_mutex_locked(&block->lock);

    // Unmap PTEs and free page tables
    gpu = uvm_va_space_get_gpu(va_space, id);
    gpu_va_space = uvm_gpu_va_space_get(va_space, gpu);
    if (gpu_va_space)
        uvm_va_block_remove_gpu_va_space(block, gpu_va_space, block_context);

    UVM_ASSERT(!uvm_processor_mask_test(&block->mapped, id));

    // No processor should have this GPU mapped at this point
    UVM_ASSERT(block_check_processor_not_mapped(block, block_context, id));

    if (gpu_state->chunks) {
        size_t i, num_chunks;

        update_read_duplicated_pages_mask(block, id, gpu_state);
        uvm_page_mask_zero(&gpu_state->resident);
        block_clear_resident_processor(block, id);

        num_chunks = block_num_gpu_chunks(block, gpu);
        for (i = 0; i < num_chunks; i++) {
            uvm_gpu_chunk_t *chunk = gpu_state->chunks[i];
            if (!chunk)
                continue;

            uvm_mmu_chunk_unmap(chunk, &block->tracker);
            uvm_pmm_gpu_free(&gpu->pmm, chunk, &block->tracker);
        }

        uvm_kvfree(gpu_state->chunks);
    }
    else {
        UVM_ASSERT(!uvm_processor_mask_test(&block->resident, id));
    }


    // Pending operations may still need the DMA memory to be mapped.
    uvm_tracker_wait(&block->tracker);

    block_gpu_unmap_phys_all_cpu_pages(block, gpu);
    uvm_processor_mask_clear(&block->evicted_gpus, id);

    kmem_cache_free(g_uvm_va_block_gpu_state_cache, gpu_state);
    block->gpus[uvm_id_gpu_index(id)] = NULL;
}

static void block_put_ptes_safe(uvm_page_tree_t *tree, uvm_page_table_range_t *range)
{
    if (range->table) {
        uvm_page_tree_put_ptes(tree, range);
        memset(range, 0, sizeof(*range));
    }
}

NV_STATUS uvm_va_block_add_gpu_va_space(uvm_va_block_t *va_block, uvm_gpu_va_space_t *gpu_va_space)
{
    uvm_assert_mutex_locked(&va_block->lock);

    if (!gpu_va_space->ats.enabled || !va_block->cpu.ever_mapped)
        return NV_OK;

    // Pre-populate PDEs down to PDE1 for all GPU VA spaces on ATS systems. See
    // comments in pre_populate_pde1_gpu.
    return block_pre_populate_pde1_gpu(va_block, gpu_va_space, NULL);
}

void uvm_va_block_remove_gpu_va_space(uvm_va_block_t *va_block,
                                      uvm_gpu_va_space_t *gpu_va_space,
                                      uvm_va_block_context_t *block_context)
{
    uvm_pte_batch_t *pte_batch = &block_context->mapping.pte_batch;
    uvm_tlb_batch_t *tlb_batch = &block_context->mapping.tlb_batch;
    uvm_gpu_t *gpu = gpu_va_space->gpu;
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    uvm_va_block_region_t region = uvm_va_block_region_from_block(va_block);
    uvm_push_t push;
    NV_STATUS status;

    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();

    if (!gpu_state)
        return;

    uvm_assert_mutex_locked(&va_block->lock);

    // Unmapping the whole block won't cause a page table split, so this should
    // only fail if we have a system-fatal error.
    status = uvm_va_block_unmap(va_block, block_context, gpu->id, region, NULL, &local_tracker);
    if (status != NV_OK) {
        UVM_ASSERT(status == uvm_global_get_status());
        return; // Just leak
    }

    UVM_ASSERT(!uvm_processor_mask_test(&va_block->mapped, gpu->id));

    // Reset the page tables if other allocations could reuse them
    if (!block_gpu_supports_2m(va_block, gpu) &&
        !bitmap_empty(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK)) {

        status = uvm_push_begin_acquire(gpu->channel_manager,
                                        UVM_CHANNEL_TYPE_MEMOPS,
                                        &local_tracker,
                                        &push,
                                        "Resetting PTEs for block [0x%llx, 0x%llx)",
                                        va_block->start,
                                        va_block->end + 1);
        if (status != NV_OK) {
            UVM_ASSERT(status == uvm_global_get_status());
            return; // Just leak
        }

        uvm_pte_batch_begin(&push, pte_batch);
        uvm_tlb_batch_begin(&gpu_va_space->page_tables, tlb_batch);

        // When the big PTEs is active, the 4k PTEs under it are garbage. Make
        // them invalid so the page tree code can reuse them for other
        // allocations on this VA. These don't need TLB invalidates since the
        // big PTEs above them are active.
        if (gpu_state->page_table_range_4k.table) {
            uvm_page_mask_init_from_big_ptes(va_block, gpu, &block_context->scratch_page_mask, gpu_state->big_ptes);
            block_gpu_pte_clear_4k(va_block, gpu, &block_context->scratch_page_mask, 0, pte_batch, NULL);
        }

        // We unmapped all big PTEs above, which means they have the unmapped
        // pattern so the GPU MMU won't read 4k PTEs under them. Set them to
        // invalid to activate the 4ks below so new allocations using just those
        // 4k PTEs will work.
        block_gpu_pte_clear_big(va_block, gpu, gpu_state->big_ptes, 0, pte_batch, tlb_batch);

        uvm_pte_batch_end(pte_batch);
        uvm_tlb_batch_end(tlb_batch, &push, UVM_MEMBAR_NONE);

        uvm_push_end(&push);
        uvm_tracker_overwrite_with_push(&local_tracker, &push);
    }

    // The unmap must finish before we free the page tables
    status = uvm_tracker_wait_deinit(&local_tracker);
    if (status != NV_OK)
        return; // System-fatal error, just leak

    // Note that if the PTE is currently 2M with lower tables allocated but not
    // in use, calling put_ptes on those lower ranges will re-write the 2M entry
    // to be a PDE.
    block_put_ptes_safe(&gpu_va_space->page_tables, &gpu_state->page_table_range_4k);
    block_put_ptes_safe(&gpu_va_space->page_tables, &gpu_state->page_table_range_big);
    block_put_ptes_safe(&gpu_va_space->page_tables, &gpu_state->page_table_range_2m);

    gpu_state->pte_is_2m = false;
    gpu_state->initialized_big = false;
    gpu_state->activated_big = false;
    gpu_state->activated_4k = false;
    bitmap_zero(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    UVM_ASSERT(block_check_mappings(va_block, block_context));
}

void uvm_va_block_disable_peer(uvm_va_block_t *va_block, uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    NV_STATUS status;
    uvm_tracker_t tracker = UVM_TRACKER_INIT();
    uvm_va_block_context_t *block_context = uvm_va_space_block_context(va_space, NULL);
    uvm_page_mask_t *unmap_page_mask = &block_context->caller_page_mask;
    const uvm_page_mask_t *resident0;
    const uvm_page_mask_t *resident1;

    uvm_assert_mutex_locked(&va_block->lock);

    // If either of the GPUs doesn't have GPU state then nothing could be mapped
    // between them.
    if (!uvm_va_block_gpu_state_get(va_block, gpu0->id) || !uvm_va_block_gpu_state_get(va_block, gpu1->id))
        return;

    resident0 = uvm_va_block_resident_mask_get(va_block, gpu0->id, NUMA_NO_NODE);
    resident1 = uvm_va_block_resident_mask_get(va_block, gpu1->id, NUMA_NO_NODE);

    // Unmap all pages resident on gpu1, but not on gpu0, from gpu0
    if (uvm_page_mask_andnot(unmap_page_mask, resident1, resident0)) {
        status = block_unmap_gpu(va_block, block_context, gpu0, unmap_page_mask, &tracker);
        if (status != NV_OK) {
            // Since all PTEs unmapped by this call have the same aperture, page
            // splits should never be required so any failure should be the
            // result of a system-fatal error.
            UVM_ASSERT_MSG(status == uvm_global_get_status(),
                           "Unmapping failed: %s, GPU %s\n",
                           nvstatusToString(status),
                           uvm_gpu_name(gpu0));
        }
    }

    // Unmap all pages resident on gpu0, but not on gpu1, from gpu1
    if (uvm_page_mask_andnot(unmap_page_mask, resident0, resident1)) {
        status = block_unmap_gpu(va_block, block_context, gpu1, unmap_page_mask, &tracker);
        if (status != NV_OK) {
            UVM_ASSERT_MSG(status == uvm_global_get_status(),
                           "Unmapping failed: %s, GPU %s\n",
                           nvstatusToString(status),
                           uvm_gpu_name(gpu0));
        }
    }

    status = uvm_tracker_add_tracker_safe(&va_block->tracker, &tracker);
    if (status != NV_OK)
        UVM_ASSERT(status == uvm_global_get_status());

    status = uvm_tracker_wait_deinit(&tracker);
    if (status != NV_OK)
        UVM_ASSERT(status == uvm_global_get_status());
}

void uvm_va_block_unmap_preferred_location_uvm_lite(uvm_va_block_t *va_block, uvm_gpu_t *gpu)
{
    NV_STATUS status;
    uvm_va_range_t *va_range = va_block->va_range;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_va_block_context_t *block_context = uvm_va_space_block_context(va_space, NULL);
    uvm_va_block_region_t region = uvm_va_block_region_from_block(va_block);

    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(uvm_processor_mask_test(&va_range->uvm_lite_gpus, gpu->id));

    // If the GPU doesn't have GPU state then nothing could be mapped.
    if (!uvm_va_block_gpu_state_get(va_block, gpu->id))
        return;

    // In UVM-Lite mode, mappings to the preferred location are not tracked
    // directly, so just unmap the whole block.
    status = uvm_va_block_unmap(va_block, block_context, gpu->id, region, NULL, &va_block->tracker);
    if (status != NV_OK) {
        // Unmapping the whole block should not cause page splits so any failure
        // should be the result of a system-fatal error.
        UVM_ASSERT_MSG(status == uvm_global_get_status(),
                       "Unmapping failed: %s, GPU %s\n",
                       nvstatusToString(status), uvm_gpu_name(gpu));
    }

    status = uvm_tracker_wait(&va_block->tracker);
    if (status != NV_OK) {
        UVM_ASSERT_MSG(status == uvm_global_get_status(),
                       "Unmapping failed: %s, GPU %s\n",
                       nvstatusToString(status), uvm_gpu_name(gpu));
    }
}

// Evict pages from the GPU by moving each resident region to the CPU
//
// Notably the caller needs to support allocation-retry as
// uvm_va_block_migrate_locked() requires that.
static NV_STATUS block_evict_pages_from_gpu(uvm_va_block_t *va_block, uvm_gpu_t *gpu, struct mm_struct *mm)
{
    NV_STATUS status = NV_OK;
    const uvm_page_mask_t *resident = uvm_va_block_resident_mask_get(va_block, gpu->id, NUMA_NO_NODE);
    uvm_va_block_region_t region = uvm_va_block_region_from_block(va_block);
    uvm_va_block_region_t subregion;
    uvm_service_block_context_t *service_context;

    service_context = uvm_service_block_context_alloc(mm);
    if (!service_context)
        return NV_ERR_NO_MEMORY;

    // Move all subregions resident on the GPU to the CPU
    for_each_va_block_subregion_in_mask(subregion, resident, region) {
        if (uvm_va_block_is_hmm(va_block)) {
            status = uvm_hmm_va_block_evict_pages_from_gpu(va_block, gpu, service_context, resident, subregion);
        }
        else {
            status = uvm_va_block_migrate_locked(va_block,
                                                 NULL,
                                                 service_context,
                                                 subregion,
                                                 UVM_ID_CPU,
                                                 UVM_MIGRATE_MODE_MAKE_RESIDENT_AND_MAP,
                                                 NULL);
        }

        if (status != NV_OK)
            break;
    }

    if (status == NV_OK)
        UVM_ASSERT(!uvm_processor_mask_test(&va_block->resident, gpu->id));

    uvm_service_block_context_free(service_context);

    return status;
}

void uvm_va_block_unregister_gpu_locked(uvm_va_block_t *va_block, uvm_gpu_t *gpu, struct mm_struct *mm)
{
    NV_STATUS status;
    uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_va_block_context_t *va_block_context = uvm_va_space_block_context(va_space, mm);

    uvm_assert_mutex_locked(&va_block->lock);

    if (!gpu_state)
        return;

    // The mappings should've already been torn down by GPU VA space unregister
    UVM_ASSERT(!uvm_processor_mask_test(&va_block->mapped, gpu->id));
    UVM_ASSERT(uvm_page_mask_empty(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ]));
    UVM_ASSERT(!block_gpu_has_page_tables(va_block, gpu));

    // Use UVM_VA_BLOCK_RETRY_LOCKED() as the va block lock is already taken and
    // we don't rely on any state of the block across the call.
    // TODO: Bug 4494289: Prevent setting the global error on allocation
    // failures.
    status = UVM_VA_BLOCK_RETRY_LOCKED(va_block, NULL, block_evict_pages_from_gpu(va_block, gpu, mm));
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to evict GPU pages on GPU unregister: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        uvm_global_set_fatal_error(status);
    }

    // This function will copy the block's tracker into each chunk then free the
    // chunk to PMM. If we do this before waiting for the block tracker below
    // we'll populate PMM's free chunks with tracker entries, which gives us
    // better testing coverage of chunk synchronization on GPU unregister.
    block_destroy_gpu_state(va_block, va_block_context, gpu->id);

    // Any time a GPU is unregistered we need to make sure that there are no
    // pending (direct or indirect) tracker entries for that GPU left in the
    // block's tracker. The only way to ensure that is to wait for the whole
    // tracker.
    status = uvm_tracker_wait(&va_block->tracker);
    if (status != NV_OK)
        UVM_ASSERT(status == uvm_global_get_status());
}

void uvm_va_block_unregister_gpu(uvm_va_block_t *va_block, uvm_gpu_t *gpu, struct mm_struct *mm)
{
    // Take the lock internally to not expose the caller to allocation-retry.
    uvm_mutex_lock(&va_block->lock);

    uvm_va_block_unregister_gpu_locked(va_block, gpu, mm);

    uvm_mutex_unlock(&va_block->lock);
}

static void block_mark_region_cpu_dirty(uvm_va_block_t *va_block, uvm_va_block_region_t region)
{
    uvm_page_index_t page_index;
    uvm_page_mask_t *resident_mask;

    uvm_assert_mutex_locked(&va_block->lock);
    resident_mask = uvm_va_block_resident_mask_get(va_block, UVM_ID_CPU, NUMA_NO_NODE);
    for_each_va_block_page_in_region_mask(page_index, resident_mask, region) {
        int nid = block_get_page_node_residency(va_block, page_index);
        UVM_ASSERT(nid != NUMA_NO_NODE);
        block_mark_cpu_page_dirty(va_block, page_index, nid);
    }
}

// Tears down everything within the block, but doesn't free the block itself.
// Note that when uvm_va_block_kill is called, this is called twice: once for
// the initial kill itself, then again when the block's ref count is eventually
// destroyed. block->va_range is used to track whether the block has already
// been killed.
static void block_kill(uvm_va_block_t *block)
{
    uvm_va_space_t *va_space;
    uvm_perf_event_data_t event_data;
    uvm_cpu_chunk_t *chunk;
    uvm_gpu_id_t id;
    NV_STATUS status;
    uvm_va_block_region_t region = uvm_va_block_region_from_block(block);
    uvm_page_index_t page_index;
    uvm_page_index_t next_page_index;
    int nid;
    uvm_va_block_context_t *block_context;

    if (uvm_va_block_is_dead(block))
        return;

    va_space = uvm_va_block_get_va_space(block);
    event_data.block_destroy.block = block;
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_BLOCK_DESTROY, &event_data);

    block_context = uvm_va_space_block_context(va_space, NULL);

    // Unmap all processors in parallel first. Unmapping the whole block won't
    // cause a page table split, so this should only fail if we have a system-
    // fatal error.
    if (!uvm_processor_mask_empty(&block->mapped)) {
        // HMM CPU mappings are controlled by Linux so no need to unmap.
        // Remote GPU mappings will be removed below.
        if (uvm_va_block_is_hmm(block) && uvm_processor_mask_test(&block->mapped, UVM_ID_CPU)) {
            uvm_page_mask_zero(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE]);
            uvm_page_mask_zero(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ]);
            uvm_processor_mask_clear(&block->mapped, UVM_ID_CPU);
        }

        // We could only be killed with mapped GPU state by VA range free or VA
        // space teardown, so it's safe to use the va_space's block_context
        // because both of those have the VA space lock held in write mode.
        status = uvm_va_block_unmap_mask(block, block_context, &block->mapped, region, NULL);
        UVM_ASSERT(status == uvm_global_get_status());
    }

    UVM_ASSERT(uvm_processor_mask_empty(&block->mapped));

    // Free the GPU page tables and chunks
    for_each_gpu_id(id)
        block_destroy_gpu_state(block, block_context, id);

    // Wait for the GPU PTE unmaps before freeing CPU memory
    uvm_tracker_wait_deinit(&block->tracker);

    // No processor should have the CPU mapped at this point
    UVM_ASSERT(block_check_processor_not_mapped(block, block_context, UVM_ID_CPU));

    // Free CPU pages
    for_each_possible_uvm_node(nid) {
        uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, nid);

        for_each_cpu_chunk_in_block_safe(chunk, page_index, next_page_index, block, nid) {
            // be conservative.
            // Tell the OS we wrote to the page because we sometimes clear the dirty
            // bit after writing to it. HMM dirty flags are managed by the kernel.
            if (!uvm_va_block_is_hmm(block))
                uvm_cpu_chunk_mark_dirty(chunk, 0);

            uvm_cpu_chunk_remove_from_block(block, nid, page_index);
            uvm_cpu_chunk_free(chunk);
        }

        UVM_ASSERT(uvm_page_mask_empty(&node_state->allocated));
        UVM_ASSERT(node_state->chunks == 0);
    }

    // While a per-NUMA node_state array is in use, all of its elements are
    // expected to be valid. Therefore the teardown of these elements must occur
    // as a single "transaction". This teardown must take place after freeing
    // the CPU pages (see the "Free CPU pages" loop above). This is because as
    // part of removing chunks from VA blocks, the per-page allocated bitmap is
    // recomputed using the per-NUMA node_state array elements.
    for_each_possible_uvm_node(nid) {
        uvm_va_block_cpu_node_state_t *node_state;

        node_state = block_node_state_get(block, nid);
        kmem_cache_free(g_uvm_va_block_cpu_node_state_cache, node_state);
    }
    uvm_kvfree((void *)block->cpu.node_state);
    block->cpu.node_state = NULL;

    // Clearing the resident bit isn't strictly necessary since this block
    // is getting destroyed, but it keeps state consistent for assertions.
    uvm_page_mask_zero(&block->cpu.resident);
    block_clear_resident_processor(block, UVM_ID_CPU);

    if (uvm_va_block_is_hmm(block))
        uvm_va_policy_clear(block, block->start, block->end);

    block->va_range = NULL;
#if UVM_IS_CONFIG_HMM()
    block->hmm.va_space = NULL;
#endif
}

// Called when the block's ref count drops to 0
void uvm_va_block_destroy(nv_kref_t *nv_kref)
{
    uvm_va_block_t *block = container_of(nv_kref, uvm_va_block_t, kref);

    // Nobody else should have a reference when freeing
    uvm_assert_mutex_unlocked(&block->lock);

    uvm_mutex_lock(&block->lock);
    block_kill(block);
    uvm_mutex_unlock(&block->lock);
    uvm_va_block_free(block);
}

void uvm_va_block_kill(uvm_va_block_t *va_block)
{
    uvm_mutex_lock(&va_block->lock);
    block_kill(va_block);
    uvm_mutex_unlock(&va_block->lock);

    // May call block_kill again
    uvm_va_block_release(va_block);
}

static void block_gpu_release_region(uvm_va_block_t *va_block,
                                     uvm_gpu_id_t gpu_id,
                                     uvm_va_block_gpu_state_t *gpu_state,
                                     uvm_page_mask_t *page_mask,
                                     uvm_va_block_region_t region)
{
    uvm_page_index_t page_index;

    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        uvm_gpu_chunk_t *gpu_chunk = gpu_state->chunks[page_index];

        if (!gpu_chunk)
            continue;

        uvm_mmu_chunk_unmap(gpu_chunk, &va_block->tracker);

        // The GPU chunk will be freed when the device private reference drops.
        if (uvm_page_mask_test_and_clear(&gpu_state->resident, page_index) &&
            uvm_page_mask_empty(&gpu_state->resident))
            block_clear_resident_processor(va_block, gpu_id);

        gpu_state->chunks[page_index] = NULL;
    }
}

void uvm_va_block_munmap_region(uvm_va_block_t *va_block,
                                uvm_va_block_region_t region)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_perf_event_data_t event_data;
    uvm_gpu_id_t gpu_id;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mutex_locked(&va_block->lock);

    // Reset thrashing state for the region.
    event_data.block_munmap.block = va_block;
    event_data.block_munmap.region = region;
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_BLOCK_MUNMAP, &event_data);

    // Release any remaining vidmem chunks in the given region.
    for_each_gpu_id(gpu_id) {
        uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(va_block, gpu_id);

        if (!gpu_state)
            continue;

        uvm_page_mask_region_clear(&gpu_state->evicted, region);
        if (uvm_page_mask_empty(&gpu_state->evicted))
            uvm_processor_mask_clear(&va_block->evicted_gpus, gpu_id);

        if (gpu_state->chunks) {
            block_gpu_release_region(va_block, gpu_id, gpu_state, NULL, region);

            // TODO: bug 3660922: Need to update the read duplicated pages mask
            // when read duplication is supported for HMM.
        }
        else {
            UVM_ASSERT(!uvm_processor_mask_test(&va_block->resident, gpu_id));
        }
    }

    uvm_va_policy_clear(va_block,
                        uvm_va_block_region_start(va_block, region),
                        uvm_va_block_region_end(va_block, region));
}

static NV_STATUS block_split_presplit_ptes_gpu(uvm_va_block_t *existing, uvm_va_block_t *new, uvm_gpu_t *gpu)
{
    uvm_va_block_gpu_state_t *existing_gpu_state = uvm_va_block_gpu_state_get(existing, gpu->id);
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(existing);
    uvm_va_block_context_t *block_context = uvm_va_space_block_context(va_space, NULL);
    NvU64 big_page_size = uvm_va_block_gpu_big_page_size(existing, gpu);
    NvU64 alloc_sizes;
    DECLARE_BITMAP(new_big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
    uvm_page_index_t new_start_page_index = uvm_va_block_cpu_page_index(existing, new->start);
    size_t big_page_index;
    uvm_push_t push;
    NV_STATUS status;

    // We only have to split to big PTEs if we're currently a 2M PTE
    if (existing_gpu_state->pte_is_2m) {
        // We can skip the split if the 2M PTE is invalid and we have no lower
        // PTEs.
        if (block_page_prot_gpu(existing, gpu, 0) == UVM_PROT_NONE &&
            !existing_gpu_state->page_table_range_big.table &&
            !existing_gpu_state->page_table_range_4k.table)
            return NV_OK;

        alloc_sizes = big_page_size;
        bitmap_fill(new_big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

        if (!IS_ALIGNED(new->start, big_page_size)) {
            alloc_sizes |= UVM_PAGE_SIZE_4K;

            big_page_index = uvm_va_block_big_page_index(existing, new_start_page_index, big_page_size);
            __clear_bit(big_page_index, new_big_ptes);
        }

        status = block_alloc_ptes_with_retry(existing, gpu, alloc_sizes, NULL);
        if (status != NV_OK)
            return status;

        status = uvm_push_begin_acquire(gpu->channel_manager,
                                        UVM_CHANNEL_TYPE_MEMOPS,
                                        &existing->tracker,
                                        &push,
                                        "Splitting 2M PTE, existing [0x%llx, 0x%llx) new [0x%llx, 0x%llx)",
                                        existing->start, existing->end + 1,
                                        new->start, new->end + 1);
        if (status != NV_OK)
            return status;

        block_gpu_split_2m(existing, block_context, gpu, new_big_ptes, &push);
    }
    else {
        big_page_index = uvm_va_block_big_page_index(existing, new_start_page_index, big_page_size);

        // If the split point is on a big page boundary, or if the split point
        // is not currently covered by a big PTE, we don't have to split
        // anything.
        if (IS_ALIGNED(new->start, big_page_size) ||
            big_page_index == MAX_BIG_PAGES_PER_UVM_VA_BLOCK ||
            !test_bit(big_page_index, existing_gpu_state->big_ptes))
            return NV_OK;

        status = block_alloc_ptes_with_retry(existing, gpu, UVM_PAGE_SIZE_4K, NULL);
        if (status != NV_OK)
            return status;

        bitmap_zero(new_big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
        __set_bit(big_page_index, new_big_ptes);

        status = uvm_push_begin_acquire(gpu->channel_manager,
                                        UVM_CHANNEL_TYPE_MEMOPS,
                                        &existing->tracker,
                                        &push,
                                        "Splitting big PTE, existing [0x%llx, 0x%llx) new [0x%llx, 0x%llx)",
                                        existing->start, existing->end + 1,
                                        new->start, new->end + 1);
        if (status != NV_OK)
            return status;

        block_gpu_split_big(existing, block_context, gpu, new_big_ptes, &push);
    }

    uvm_push_end(&push);

    // Adding this push to existing block tracker will cause all GPU PTE splits
    // to serialize on each other, but it's simpler than maintaining a separate
    // tracker and this path isn't performance-critical.
    return uvm_tracker_add_push_safe(&existing->tracker, &push);
}

static NV_STATUS block_split_presplit_ptes(uvm_va_block_t *existing, uvm_va_block_t *new)
{
    uvm_gpu_t *gpu;
    uvm_gpu_id_t id;
    NV_STATUS status;

    for_each_gpu_id(id) {
        if (!uvm_va_block_gpu_state_get(existing, id))
            continue;

        gpu = block_get_gpu(existing, id);

        if (block_gpu_has_page_tables(existing, gpu)) {
            status = block_split_presplit_ptes_gpu(existing, new, gpu);
            if (status != NV_OK)
                return status;
        }
    }

    return NV_OK;
}

typedef struct
{
    // Number of chunks contained by this VA block
    size_t num_chunks;

    // Index of the "interesting" chunk, either adjacent to or spanning the
    // split point depending on which block this is.
    size_t chunk_index;

    // Size of the chunk referenced by chunk_index
    uvm_chunk_size_t chunk_size;
} block_gpu_chunk_split_state_t;

static void block_gpu_chunk_get_split_state(uvm_va_block_t *block,
                                            block_gpu_chunk_split_state_t *state,
                                            NvU64 start,
                                            NvU64 end,
                                            uvm_page_index_t page_index,
                                            uvm_gpu_t *gpu)
{
    NvU64 size = end - start + 1;
    state->num_chunks = block_num_gpu_chunks_range(block, start, size, gpu);
    state->chunk_index = block_gpu_chunk_index_range(block, start, size, gpu, page_index, &state->chunk_size);
}

// Perform any chunk splitting and array growing required for this block split,
// but don't actually move chunk pointers anywhere.
static NV_STATUS block_presplit_gpu_chunks(uvm_va_block_t *existing, uvm_va_block_t *new, uvm_gpu_t *gpu)
{
    uvm_va_block_gpu_state_t *existing_gpu_state = uvm_va_block_gpu_state_get(existing, gpu->id);
    uvm_gpu_chunk_t **temp_chunks;
    uvm_gpu_chunk_t *original_chunk, *curr_chunk;
    uvm_page_index_t split_page_index = uvm_va_block_cpu_page_index(existing, new->start);
    uvm_chunk_sizes_mask_t split_sizes;
    uvm_chunk_size_t subchunk_size;
    NV_STATUS status;
    block_gpu_chunk_split_state_t existing_before_state, existing_after_state, new_state;

    block_gpu_chunk_get_split_state(existing,
                                    &existing_before_state,
                                    existing->start,
                                    existing->end,
                                    split_page_index,
                                    gpu);
    block_gpu_chunk_get_split_state(existing,
                                    &existing_after_state,
                                    existing->start,
                                    new->start - 1,
                                    split_page_index - 1,
                                    gpu);
    block_gpu_chunk_get_split_state(new,
                                    &new_state,
                                    new->start,
                                    new->end,
                                    0,
                                    gpu);

    // Even though we're splitting existing, we could wind up requiring a larger
    // chunks array if we split a large chunk into many smaller ones.
    if (existing_after_state.num_chunks > existing_before_state.num_chunks) {
        temp_chunks = uvm_kvrealloc(existing_gpu_state->chunks,
                                    existing_after_state.num_chunks * sizeof(existing_gpu_state->chunks[0]));
        if (!temp_chunks)
            return NV_ERR_NO_MEMORY;
        existing_gpu_state->chunks = temp_chunks;
    }

    original_chunk = existing_gpu_state->chunks[existing_before_state.chunk_index];

    // If the chunk covering the split point is not populated, we're done. We've
    // already grown the array to cover any new chunks which may be populated
    // later.
    if (!original_chunk)
        return NV_OK;

    // Figure out the splits we need to perform. Remove all sizes >= the current
    // size, and all sizes < the target size. Note that the resulting mask will
    // be 0 if the sizes match (we're already splitting at a chunk boundary).
    UVM_ASSERT(uvm_gpu_chunk_get_size(original_chunk) == existing_before_state.chunk_size);
    UVM_ASSERT(existing_before_state.chunk_size >= new_state.chunk_size);
    split_sizes = gpu->parent->mmu_user_chunk_sizes;
    split_sizes &= existing_before_state.chunk_size - 1;
    split_sizes &= ~(new_state.chunk_size - 1);

    // Keep splitting the chunk covering the split point until we hit the target
    // size.
    curr_chunk = original_chunk;
    for_each_chunk_size_rev(subchunk_size, split_sizes) {
        size_t last_index, num_subchunks;

        status = uvm_pmm_gpu_split_chunk(&gpu->pmm, curr_chunk, subchunk_size, NULL);
        if (status != NV_OK)
            goto error;

        if (subchunk_size == new_state.chunk_size)
            break;

        // Compute the last subchunk index prior to the split point. Divide the
        // entire address space into units of subchunk_size, then mod by the
        // number of subchunks within the parent.
        last_index = (size_t)uvm_div_pow2_64(new->start - 1, subchunk_size);
        num_subchunks = (size_t)uvm_div_pow2_64(uvm_gpu_chunk_get_size(curr_chunk), subchunk_size);
        UVM_ASSERT(num_subchunks > 1);
        last_index &= num_subchunks - 1;

        uvm_pmm_gpu_get_subchunks(&gpu->pmm, curr_chunk, last_index, 1, &curr_chunk);
        UVM_ASSERT(uvm_gpu_chunk_get_size(curr_chunk) == subchunk_size);
    }

    // Note that existing's chunks array still has a pointer to original_chunk,
    // not to any newly-split subchunks. If a subsequent split failure occurs on
    // a later GPU we'll have to merge it back. Once we're past the preallocate
    // stage we'll remove it from the chunks array and move the new split chunks
    // in.

    return NV_OK;

error:
    // On error we need to leave the chunk in its initial state
    uvm_pmm_gpu_merge_chunk(&gpu->pmm, original_chunk);

    return status;
}

static NV_STATUS block_split_cpu_chunk_to_64k(uvm_va_block_t *block, int nid)
{
    uvm_cpu_chunk_storage_mixed_t *mixed;
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, 0);
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, nid);
    NV_STATUS status;

    UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_2M);
    UVM_ASSERT(uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_CHUNK);

    mixed = uvm_kvmalloc_zero(sizeof(*mixed));
    if (!mixed)
        return NV_ERR_NO_MEMORY;

    status = uvm_cpu_chunk_split(chunk, (uvm_cpu_chunk_t **)&mixed->slots);
    if (status != NV_OK) {
        uvm_kvfree(mixed);
        return status;
    }

    bitmap_fill(mixed->big_chunks, MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK);
    node_state->chunks = (unsigned long)mixed | UVM_CPU_CHUNK_STORAGE_MIXED;

    return status;
}

static NV_STATUS block_split_cpu_chunk_to_4k(uvm_va_block_t *block, uvm_page_index_t page_index, int nid)
{
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, nid);
    uvm_cpu_chunk_storage_mixed_t *mixed;
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);
    uvm_cpu_chunk_t **small_chunks;
    size_t slot_index;
    NV_STATUS status;

    UVM_ASSERT(chunk);
    UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_64K);
    UVM_ASSERT(uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_MIXED);

    mixed = uvm_cpu_storage_get_ptr(node_state);
    slot_index = compute_slot_index(block, page_index);
    small_chunks = uvm_kvmalloc_zero(sizeof(*small_chunks) * MAX_SMALL_CHUNKS_PER_BIG_SLOT);
    if (!small_chunks)
        return NV_ERR_NO_MEMORY;

    status = uvm_cpu_chunk_split(chunk, small_chunks);
    if (status != NV_OK) {
        uvm_kvfree(small_chunks);
        return status;
    }

    mixed->slots[slot_index] = small_chunks;
    clear_bit(slot_index, mixed->big_chunks);

    return status;
}

static NV_STATUS block_split_cpu_chunk_one(uvm_va_block_t *block, uvm_page_index_t page_index, int nid)
{
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);
    uvm_chunk_size_t chunk_size = uvm_cpu_chunk_get_size(chunk);
    uvm_chunk_size_t new_size;
    uvm_gpu_t *gpu;
    NvU64 gpu_mapping_addr;
    uvm_processor_mask_t *gpu_split_mask;
    uvm_gpu_id_t id;
    NV_STATUS status;

    gpu_split_mask = uvm_processor_mask_cache_alloc();
    if (!gpu_split_mask)
        return NV_ERR_NO_MEMORY;

    if (chunk_size == UVM_CHUNK_SIZE_2M)
        new_size = UVM_CHUNK_SIZE_64K;
    else
        new_size = UVM_CHUNK_SIZE_4K;

    UVM_ASSERT(IS_ALIGNED(chunk_size, new_size));

    uvm_processor_mask_zero(gpu_split_mask);
    for_each_gpu_id(id) {
        if (!uvm_va_block_gpu_state_get(block, id))
            continue;

        gpu = block_get_gpu(block, id);

        // If the parent chunk has not been mapped, there is nothing to split.
        gpu_mapping_addr = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu);
        if (gpu_mapping_addr == 0)
            continue;

        status = uvm_pmm_sysmem_mappings_split_gpu_mappings(&gpu->pmm_reverse_sysmem_mappings,
                                                            gpu_mapping_addr,
                                                            new_size);
        if (status != NV_OK)
            goto merge;

        uvm_processor_mask_set(gpu_split_mask, id);
    }

    if (new_size == UVM_CHUNK_SIZE_64K)
        status = block_split_cpu_chunk_to_64k(block, nid);
    else
        status = block_split_cpu_chunk_to_4k(block, page_index, nid);

    if (status != NV_OK) {
merge:
        for_each_gpu_id_in_mask(id, gpu_split_mask) {
            gpu = block_get_gpu(block, id);
            gpu_mapping_addr = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu);
            uvm_pmm_sysmem_mappings_merge_gpu_mappings(&gpu->pmm_reverse_sysmem_mappings,
                                                       gpu_mapping_addr,
                                                       chunk_size);
        }
    }

    uvm_processor_mask_cache_free(gpu_split_mask);

    return status;
}

static NV_STATUS block_prealloc_cpu_chunk_storage(uvm_va_block_t *existing, uvm_va_block_t *new, int nid)
{
    uvm_cpu_chunk_storage_mixed_t *existing_mixed;
    uvm_cpu_chunk_storage_mixed_t *new_mixed = NULL;
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(existing, nid);
    uvm_va_block_cpu_node_state_t *new_node_state = block_node_state_get(new, nid);
    size_t slot_offset;
    size_t existing_slot;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_MIXED);
    existing_mixed = uvm_cpu_storage_get_ptr(node_state);

    // Pre-allocate chunk storage for the new block. By definition, the new block
    // will contain either 64K and/or 4K chunks.
    //
    // We do this here so there are no failures in block_split_cpu().
    new_mixed = uvm_kvmalloc_zero(sizeof(*new_mixed));
    if (!new_mixed)
        return NV_ERR_NO_MEMORY;

    slot_offset = compute_slot_index(existing, uvm_va_block_cpu_page_index(existing, new->start));
    existing_slot = slot_offset;
    for_each_clear_bit_from(existing_slot, existing_mixed->big_chunks, MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK) {
        size_t new_slot = existing_slot - slot_offset;

        if (existing_mixed->slots[existing_slot]) {
            uvm_cpu_chunk_t **small_chunks = uvm_kvmalloc_zero(sizeof(*small_chunks) * MAX_SMALL_CHUNKS_PER_BIG_SLOT);

            if (!small_chunks) {
                status = NV_ERR_NO_MEMORY;
                goto done;
            }

            new_mixed->slots[new_slot] = small_chunks;
        }
    }

    new_node_state->chunks = (unsigned long)new_mixed | UVM_CPU_CHUNK_STORAGE_MIXED;
    UVM_ASSERT(status == NV_OK);

done:
    if (status != NV_OK) {
        for (; existing_slot > slot_offset; existing_slot--)
            uvm_kvfree(new_mixed->slots[existing_slot - slot_offset]);

        uvm_kvfree(new_mixed);
    }

    return status;
}

static void block_free_cpu_chunk_storage(uvm_va_block_t *block, int nid)
{
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, nid);

    if (node_state->chunks) {
        uvm_cpu_chunk_storage_mixed_t *mixed;
        size_t slot_index;

        UVM_ASSERT(uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_MIXED);
        mixed = uvm_cpu_storage_get_ptr(node_state);
        for (slot_index = 0; slot_index < MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK; slot_index++)
            uvm_kvfree(mixed->slots[slot_index]);

        uvm_kvfree(mixed);
        node_state->chunks = 0;
    }
}

// Perform any CPU chunk splitting that may be required for this block split.
// Just like block_presplit_gpu_chunks, no chunks are moved to the new block.
static NV_STATUS block_presplit_cpu_chunks(uvm_va_block_t *existing, uvm_va_block_t *new)
{
    uvm_page_index_t page_index = uvm_va_block_cpu_page_index(existing, new->start);
    uvm_cpu_chunk_t *splitting_chunk;
    uvm_chunk_sizes_mask_t split_sizes = uvm_cpu_chunk_get_allocation_sizes();
    uvm_chunk_size_t subchunk_size;
    NV_STATUS status = NV_OK;
    int nid;

    UVM_ASSERT(!IS_ALIGNED(new->start, UVM_VA_BLOCK_SIZE));

    for_each_possible_uvm_node(nid) {
        splitting_chunk = uvm_cpu_chunk_get_chunk_for_page(existing, nid, page_index);

        // If the page covering the split point has not been populated, there is no
        // need to split.
        if (!splitting_chunk)
            continue;

        // If the split point is aligned on the chunk size, there is no need to
        // split.
        if (IS_ALIGNED(new->start, uvm_cpu_chunk_get_size(splitting_chunk)))
            continue;

        // Remove all sizes above the chunk's current size.
        split_sizes &= uvm_cpu_chunk_get_size(splitting_chunk) - 1;
        // Remove all sizes below the alignment of the new block's start.
        split_sizes &= ~(IS_ALIGNED(new->start, UVM_CHUNK_SIZE_64K) ? UVM_CHUNK_SIZE_64K - 1 : 0);

        for_each_chunk_size_rev(subchunk_size, split_sizes) {
            status = block_split_cpu_chunk_one(existing, page_index, nid);
            if (status != NV_OK)
                return status;
        }

        status = block_prealloc_cpu_chunk_storage(existing, new, nid);
        if (status != NV_OK)
            break;
    }

    return status;
}

static void block_merge_cpu_chunks_to_64k(uvm_va_block_t *block, uvm_page_index_t page_index, int nid)
{
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, nid);
    uvm_cpu_chunk_storage_mixed_t *mixed = uvm_cpu_storage_get_ptr(node_state);
    size_t slot_index = compute_slot_index(block, page_index);
    uvm_cpu_chunk_t **small_chunks = mixed->slots[slot_index];
    uvm_cpu_chunk_t *merged_chunk;

    UVM_ASSERT(uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_MIXED);
    UVM_ASSERT(small_chunks);
    UVM_ASSERT(!test_bit(slot_index, mixed->big_chunks));

    merged_chunk = uvm_cpu_chunk_merge(small_chunks);
    mixed->slots[slot_index] = merged_chunk;
    set_bit(slot_index, mixed->big_chunks);
    uvm_kvfree(small_chunks);
}

static void block_merge_cpu_chunks_to_2m(uvm_va_block_t *block, uvm_page_index_t page_index, int nid)
{
    uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(block, nid);
    uvm_cpu_chunk_storage_mixed_t *mixed = uvm_cpu_storage_get_ptr(node_state);
    uvm_cpu_chunk_t **big_chunks = (uvm_cpu_chunk_t **)&mixed->slots;
    uvm_cpu_chunk_t *merged_chunk;

    UVM_ASSERT(uvm_cpu_storage_get_type(node_state) == UVM_CPU_CHUNK_STORAGE_MIXED);
    UVM_ASSERT(bitmap_full(mixed->big_chunks, MAX_BIG_CPU_CHUNK_SLOTS_PER_UVM_VA_BLOCK));

    merged_chunk = uvm_cpu_chunk_merge(big_chunks);
    node_state->chunks = (unsigned long)merged_chunk | UVM_CPU_CHUNK_STORAGE_CHUNK;
    uvm_kvfree(mixed);
}

static void block_merge_cpu_chunks_one(uvm_va_block_t *block, uvm_page_index_t page_index, int nid)
{
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);
    uvm_gpu_id_t id;

    if (!chunk)
        return;

    if (uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_4K) {
        block_merge_cpu_chunks_to_64k(block, page_index, nid);
    }
    else {
        UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == UVM_CHUNK_SIZE_64K);
        block_merge_cpu_chunks_to_2m(block, page_index, nid);
    }

    chunk = uvm_cpu_chunk_get_chunk_for_page(block, nid, page_index);

    for_each_gpu_id(id) {
        NvU64 gpu_mapping_addr;
        uvm_gpu_t *gpu;

        if (!uvm_va_block_gpu_state_get(block, id))
            continue;

        gpu = block_get_gpu(block, id);
        gpu_mapping_addr = uvm_cpu_chunk_get_gpu_phys_addr(chunk, gpu);
        if (gpu_mapping_addr == 0)
            continue;

        uvm_pmm_sysmem_mappings_merge_gpu_mappings(&gpu->pmm_reverse_sysmem_mappings,
                                                   gpu_mapping_addr,
                                                   uvm_cpu_chunk_get_size(chunk));
    }
}

static void block_merge_cpu_chunks(uvm_va_block_t *existing, uvm_va_block_t *new)
{
    uvm_page_index_t page_index = uvm_va_block_cpu_page_index(existing, new->start);
    uvm_chunk_sizes_mask_t merge_sizes = uvm_cpu_chunk_get_allocation_sizes();
    uvm_chunk_size_t largest_size;
    size_t block_size = uvm_va_block_size(existing);
    int nid;

    // Since block sizes are not always powers of 2, use the largest power of 2
    // less than or equal to the block size since we can't merge to a size
    // larger than the block's size.
    largest_size = rounddown_pow_of_two(block_size);

    for_each_possible_uvm_node(nid) {
        uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_chunk_for_page(existing, nid, page_index);
        uvm_va_block_cpu_node_state_t *node_state = block_node_state_get(existing, nid);
        uvm_chunk_size_t chunk_size;
        uvm_chunk_size_t merge_size;

        if (!chunk || uvm_cpu_chunk_is_physical(chunk))
            continue;

        chunk_size = uvm_cpu_chunk_get_size(chunk);

        // Remove all CPU chunk sizes above the size of the existing VA block.
        merge_sizes &= (largest_size | (largest_size - 1));

        // Remove all CPU chunk sizes smaller than the size of the chunk being merged up.
        merge_sizes &= ~(chunk_size | (chunk_size - 1));

        for_each_chunk_size(merge_size, merge_sizes) {
            uvm_va_block_region_t chunk_region;

            // The block has to fully contain the VA range after the merge.
            if (!uvm_va_block_contains_address(existing, UVM_ALIGN_DOWN(new->start, merge_size)) ||
                !uvm_va_block_contains_address(existing, UVM_ALIGN_DOWN(new->start, merge_size) + merge_size - 1))
                break;

            chunk_region = uvm_va_block_chunk_region(existing, merge_size, page_index);

            // If not all pages in the region covered by the chunk are allocated,
            // we can't merge.
            if (!uvm_page_mask_region_full(&node_state->allocated, chunk_region))
                break;

            block_merge_cpu_chunks_one(existing, chunk_region.first, nid);
            chunk = uvm_cpu_chunk_get_chunk_for_page(existing, nid, page_index);
            if (uvm_cpu_chunk_is_physical(chunk))
                break;
        }

        block_free_cpu_chunk_storage(new, nid);
    }
}

// Pre-allocate everything which doesn't require retry on both existing and new
// which will be needed to handle a split. If this fails, existing must remain
// functionally unmodified.
static NV_STATUS block_split_preallocate_no_retry(uvm_va_block_t *existing, uvm_va_block_t *new)
{
    NV_STATUS status;
    uvm_gpu_t *gpu;
    uvm_gpu_id_t id;
    uvm_page_index_t split_page_index;
    uvm_va_block_test_t *block_test;

    status = block_presplit_cpu_chunks(existing, new);
    if (status != NV_OK)
        goto error;

    for_each_gpu_id(id) {
        if (!uvm_va_block_gpu_state_get(existing, id))
            continue;

        gpu = block_get_gpu(existing, id);

        status = block_presplit_gpu_chunks(existing, new, gpu);
        if (status != NV_OK)
            goto error;

        if (!block_gpu_state_get_alloc(new, gpu)) {
            status = NV_ERR_NO_MEMORY;
            goto error;
        }
    }

    block_test = uvm_va_block_get_test(existing);
    if (block_test && block_test->inject_split_error) {
        block_test->inject_split_error = false;
        if (!uvm_va_block_is_hmm(existing)) {
            UVM_ASSERT(existing->va_range->inject_split_error);
            existing->va_range->inject_split_error = false;
        }
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    if (uvm_va_block_is_hmm(existing)) {
        uvm_va_policy_node_t *node = uvm_va_policy_node_find(existing, new->start);

        if (node && node->node.start != new->start) {
            status = uvm_va_policy_node_split(existing, node, new->start - 1, NULL);
            if (status != NV_OK)
                goto error;
        }
    }

    return NV_OK;

error:
    // Merge back the chunks we split
    split_page_index = uvm_va_block_cpu_page_index(existing, new->start);

    for_each_gpu_id(id) {
        uvm_gpu_chunk_t *chunk;
        size_t chunk_index;
        uvm_va_block_gpu_state_t *existing_gpu_state = uvm_va_block_gpu_state_get(existing, id);

        if (!existing_gpu_state)
            continue;

        // If the chunk spanning the split point was split, merge it back
        gpu = block_get_gpu(existing, id);
        chunk_index = block_gpu_chunk_index(existing, gpu, split_page_index, NULL);
        chunk = existing_gpu_state->chunks[chunk_index];
        if (!chunk || chunk->state != UVM_PMM_GPU_CHUNK_STATE_IS_SPLIT)
            continue;

        uvm_pmm_gpu_merge_chunk(&gpu->pmm, chunk);

        // We could attempt to shrink the chunks array back down, but it doesn't
        // hurt much to have it larger than necessary, and we'd have to handle
        // the shrink call failing anyway on this error path.

    }

    block_merge_cpu_chunks(existing, new);

    return status;
}

// Re-calculate the block's top-level processor masks:
//   - block->mapped
//   - block->resident
//
// This is called on block split.
static void block_set_processor_masks(uvm_va_block_t *block)
{
    size_t num_pages = uvm_va_block_num_cpu_pages(block);
    uvm_va_block_region_t block_region = uvm_va_block_region(0, num_pages);
    uvm_gpu_id_t id;

    if (uvm_page_mask_region_empty(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ], block_region)) {
        UVM_ASSERT(uvm_page_mask_region_empty(&block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE], block_region));
        uvm_processor_mask_clear(&block->mapped, UVM_ID_CPU);
    }
    else {
        uvm_processor_mask_set(&block->mapped, UVM_ID_CPU);
    }

    if (uvm_page_mask_region_empty(uvm_va_block_resident_mask_get(block, UVM_ID_CPU, NUMA_NO_NODE), block_region)) {
        uvm_va_space_t *va_space = uvm_va_block_get_va_space(block);

        if (uvm_processor_mask_get_gpu_count(&va_space->can_access[UVM_ID_CPU_VALUE]) == 0)
            UVM_ASSERT(!uvm_processor_mask_test(&block->mapped, UVM_ID_CPU));

        block_clear_resident_processor(block, UVM_ID_CPU);
    }
    else {
        block_set_resident_processor(block, UVM_ID_CPU);
    }

    for_each_gpu_id(id) {
        uvm_va_block_gpu_state_t *gpu_state = uvm_va_block_gpu_state_get(block, id);
        if (!gpu_state)
            continue;

        if (uvm_page_mask_region_empty(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_READ], block_region)) {
            UVM_ASSERT(uvm_page_mask_region_empty(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_WRITE], block_region));
            UVM_ASSERT(uvm_page_mask_region_empty(&gpu_state->pte_bits[UVM_PTE_BITS_GPU_ATOMIC], block_region));
            uvm_processor_mask_clear(&block->mapped, id);
        }
        else {
            uvm_processor_mask_set(&block->mapped, id);
        }

        if (uvm_page_mask_region_empty(&gpu_state->resident, block_region))
            block_clear_resident_processor(block, id);
        else
            block_set_resident_processor(block, id);

        if (uvm_page_mask_region_empty(&gpu_state->evicted, block_region))
            uvm_processor_mask_clear(&block->evicted_gpus, id);
        else
            uvm_processor_mask_set(&block->evicted_gpus, id);
    }
}

// Split a PAGES_PER_UVM_VA_BLOCK sized bitmap into new and existing parts
// corresponding to a block split.
static void block_split_page_mask(uvm_page_mask_t *existing_mask,
                                  size_t existing_pages,
                                  uvm_page_mask_t *new_mask,
                                  size_t new_pages)
{
    UVM_ASSERT_MSG(existing_pages + new_pages <= PAGES_PER_UVM_VA_BLOCK, "existing %zu new %zu\n",
                   existing_pages, new_pages);

    // The new block is always in the upper region of existing, so shift the bit
    // vectors down.
    //
    // Note that bitmap_shift_right requires both dst and src to be the same
    // size. That's ok since we don't scale them by block size.
    uvm_page_mask_shift_right(new_mask, existing_mask, existing_pages);
    uvm_page_mask_region_clear(existing_mask, uvm_va_block_region(existing_pages, existing_pages + new_pages));
}

// Split the CPU state within the existing block. existing's start is correct
// but its end has not yet been adjusted.
static void block_split_cpu(uvm_va_block_t *existing, uvm_va_block_t *new)
{
    size_t existing_pages, new_pages = uvm_va_block_num_cpu_pages(new);
    uvm_pte_bits_cpu_t pte_bit;
    uvm_va_block_region_t block_region = uvm_va_block_region_from_block(existing);
    uvm_page_index_t split_page_index = uvm_va_block_cpu_page_index(existing, new->start);
    uvm_page_index_t page_index;
    uvm_page_index_t next_page_index;
    uvm_cpu_chunk_t *chunk;
    uvm_va_range_t *existing_va_range = existing->va_range;
    int nid;

    if (existing_va_range) {
        UVM_ASSERT(existing->va_range->type == UVM_VA_RANGE_TYPE_MANAGED);
        UVM_ASSERT(existing->va_range->type == new->va_range->type);
    }

    UVM_ASSERT(existing->start < new->start);
    UVM_ASSERT(existing->end == new->end);

    UVM_ASSERT(PAGE_ALIGNED(new->start));
    UVM_ASSERT(PAGE_ALIGNED(existing->start));

    existing_pages = (new->start - existing->start) / PAGE_SIZE;

    // We don't have to unmap the CPU since its virtual -> physical mappings
    // don't change.

    for_each_possible_uvm_node(nid) {
        uvm_page_mask_t *existing_resident_mask = uvm_va_block_resident_mask_get(existing, UVM_ID_CPU, nid);
        uvm_page_mask_t *new_resident_mask = uvm_va_block_resident_mask_get(new, UVM_ID_CPU, nid);

        for_each_cpu_chunk_in_block_region_safe(chunk,
                                                page_index,
                                                next_page_index,
                                                existing,
                                                nid,
                                                uvm_va_block_region(split_page_index, block_region.outer)) {
            uvm_page_index_t new_chunk_page_index;
            NV_STATUS status;

            uvm_cpu_chunk_remove_from_block(existing, nid, page_index);

            // The chunk has to be adjusted for the new block before inserting it.
            new_chunk_page_index = page_index - split_page_index;

            // This should never fail because all necessary storage was allocated
            // in block_presplit_cpu_chunks().
            status = uvm_cpu_chunk_insert_in_block(new, chunk, new_chunk_page_index);
            UVM_ASSERT(status == NV_OK);
        }

        block_split_page_mask(existing_resident_mask, existing_pages, new_resident_mask, new_pages);
    }

    block_split_page_mask(&existing->cpu.resident, existing_pages, &new->cpu.resident, new_pages);
    new->cpu.ever_mapped = existing->cpu.ever_mapped;

    for (pte_bit = 0; pte_bit < UVM_PTE_BITS_CPU_MAX; pte_bit++)
        block_split_page_mask(&existing->cpu.pte_bits[pte_bit], existing_pages, &new->cpu.pte_bits[pte_bit], new_pages);
}

// Fill out the blocks' chunks arrays with the chunks split by
// block_presplit_gpu_chunks.
static void block_copy_split_gpu_chunks(uvm_va_block_t *existing, uvm_va_block_t *new, uvm_gpu_t *gpu)
{
    uvm_va_block_gpu_state_t *existing_gpu_state = uvm_va_block_gpu_state_get(existing, gpu->id);
    uvm_va_block_gpu_state_t *new_gpu_state = uvm_va_block_gpu_state_get(new, gpu->id);
    uvm_gpu_chunk_t **temp_chunks;
    uvm_gpu_chunk_t *original_chunk;
    block_gpu_chunk_split_state_t existing_before_state, existing_after_state, new_state;
    size_t num_pre_chunks, num_post_chunks, num_split_chunks_existing, num_split_chunks_new;
    uvm_page_index_t split_page_index = uvm_va_block_cpu_page_index(existing, new->start);
    size_t i;

    block_gpu_chunk_get_split_state(existing,
                                    &existing_before_state,
                                    existing->start,
                                    existing->end,
                                    split_page_index,
                                    gpu);
    block_gpu_chunk_get_split_state(existing,
                                    &existing_after_state,
                                    existing->start,
                                    new->start - 1,
                                    split_page_index - 1,
                                    gpu);
    block_gpu_chunk_get_split_state(new,
                                    &new_state,
                                    new->start,
                                    new->end,
                                    0,
                                    gpu);

    // General case (B is original_chunk):
    //                                          split
    //                                            v
    //  existing (before) [------ A -----][------ B -----][------ C -----]
    //  existing (after)  [------ A -----][- B0 -]
    //  new                                       [- B1 -][------ C -----]
    //
    // Note that the logic below also handles the case of the split happening at
    // a chunk boundary. That case behaves as though there is no B0 chunk.

    // Number of chunks to the left and right of original_chunk (A and C above).
    // Either or both of these may be 0.
    num_pre_chunks  = existing_before_state.chunk_index;
    num_post_chunks = existing_before_state.num_chunks - num_pre_chunks - 1;

    // Number of subchunks under existing's portion of original_chunk (B0 above)
    num_split_chunks_existing = existing_after_state.num_chunks - num_pre_chunks;

    // Number of subchunks under new's portion of original_chunk (B1 above)
    num_split_chunks_new = new_state.num_chunks - num_post_chunks;

    UVM_ASSERT(num_pre_chunks + num_split_chunks_existing > 0);
    UVM_ASSERT(num_split_chunks_new > 0);

    // Copy post chunks from the end of existing into new (C above)
    memcpy(&new_gpu_state->chunks[num_split_chunks_new],
           &existing_gpu_state->chunks[existing_before_state.chunk_index + 1],
           num_post_chunks * sizeof(new_gpu_state->chunks[0]));

    // Save off the original split chunk since we may overwrite the array
    original_chunk = existing_gpu_state->chunks[existing_before_state.chunk_index];

    // Fill out the new pointers
    if (original_chunk) {
        // Note that if the split happened at a chunk boundary, original_chunk
        // will not be split. In that case, num_split_chunks_existing will be 0
        // and num_split_chunks_new will be 1, so the left copy will be skipped
        // and the right copy will pick up the chunk.

        // Copy left newly-split chunks into existing (B0 above). The array was
        // re-sized in block_presplit_gpu_chunks as necessary.
        size_t num_subchunks;

        num_subchunks = uvm_pmm_gpu_get_subchunks(&gpu->pmm,
                                                  original_chunk,
                                                  0, // start_index
                                                  num_split_chunks_existing,
                                                  &existing_gpu_state->chunks[existing_before_state.chunk_index]);
        UVM_ASSERT(num_subchunks == num_split_chunks_existing);

        // Copy right newly-split chunks into new (B1 above), overwriting the
        // pointer to the original chunk.
        num_subchunks = uvm_pmm_gpu_get_subchunks(&gpu->pmm,
                                                  original_chunk,
                                                  num_split_chunks_existing, // start_index
                                                  num_split_chunks_new,
                                                  &new_gpu_state->chunks[0]);
        UVM_ASSERT(num_subchunks == num_split_chunks_new);
    }
    else {
        // If the chunk wasn't already populated we don't need to copy pointers
        // anywhere, but we need to clear out stale pointers from existing's
        // array covering the new elements. new's chunks array was already zero-
        // initialized.
        memset(&existing_gpu_state->chunks[existing_before_state.chunk_index],
               0,
               num_split_chunks_existing * sizeof(existing_gpu_state->chunks[0]));
    }

    // Since we update the reverse map information, protect it against a
    // concurrent lookup
    uvm_spin_lock(&gpu->pmm.list_lock);

    // Update the reverse map of all the chunks that are now under the new block
    for (i = 0; i < new_state.num_chunks; ++i) {
        if (new_gpu_state->chunks[i]) {
            UVM_ASSERT(new_gpu_state->chunks[i]->va_block == existing);
            new_gpu_state->chunks[i]->va_block = new;

            // Adjust the page_index within the VA block for the new subchunks in
            // the new VA block
            UVM_ASSERT(new_gpu_state->chunks[i]->va_block_page_index >= split_page_index);
            new_gpu_state->chunks[i]->va_block_page_index -= split_page_index;
        }
    }

    uvm_spin_unlock(&gpu->pmm.list_lock);

    // Attempt to shrink existing's chunk allocation. If the realloc fails, just
    // keep on using the old larger one.
    if (existing_after_state.num_chunks < existing_before_state.num_chunks) {
        temp_chunks = uvm_kvrealloc(existing_gpu_state->chunks,
                                    existing_after_state.num_chunks * sizeof(existing_gpu_state->chunks[0]));
        if (temp_chunks)
            existing_gpu_state->chunks = temp_chunks;
    }
}

static void block_split_gpu(uvm_va_block_t *existing, uvm_va_block_t *new, uvm_gpu_id_t gpu_id)
{
    uvm_va_block_gpu_state_t *existing_gpu_state = uvm_va_block_gpu_state_get(existing, gpu_id);
    uvm_va_block_gpu_state_t *new_gpu_state = uvm_va_block_gpu_state_get(new, gpu_id);
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(existing);
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_gpu_t *gpu;
    size_t new_pages = uvm_va_block_num_cpu_pages(new);
    size_t existing_pages, existing_pages_4k, existing_pages_big, new_pages_big;
    uvm_pte_bits_gpu_t pte_bit;
    uvm_cpu_chunk_t *cpu_chunk;
    uvm_page_index_t page_index;
    int nid;

    if (!existing_gpu_state)
        return;

    gpu = uvm_va_space_get_gpu(va_space, gpu_id);
    UVM_ASSERT(new_gpu_state);

    new_gpu_state->force_4k_ptes = existing_gpu_state->force_4k_ptes;

    UVM_ASSERT(PAGE_ALIGNED(new->start));
    UVM_ASSERT(PAGE_ALIGNED(existing->start));
    existing_pages = (new->start - existing->start) / PAGE_SIZE;

    for_each_possible_uvm_node(nid) {
        for_each_cpu_chunk_in_block(cpu_chunk, page_index, new, nid) {
            uvm_pmm_sysmem_mappings_reparent_gpu_mapping(&gpu->pmm_reverse_sysmem_mappings,
                                                         uvm_cpu_chunk_get_gpu_phys_addr(cpu_chunk, gpu),
                                                         new);
        }
    }

    block_copy_split_gpu_chunks(existing, new, gpu);

    block_split_page_mask(&existing_gpu_state->resident,
                          existing_pages,
                          &new_gpu_state->resident,
                          new_pages);

    for (pte_bit = 0; pte_bit < UVM_PTE_BITS_GPU_MAX; pte_bit++) {
        block_split_page_mask(&existing_gpu_state->pte_bits[pte_bit], existing_pages,
                              &new_gpu_state->pte_bits[pte_bit], new_pages);
    }

    // Adjust page table ranges.
    gpu_va_space = uvm_gpu_va_space_get(va_space, gpu);
    if (gpu_va_space) {
        if (existing_gpu_state->page_table_range_big.table) {
            NvU64 big_page_size = uvm_va_block_gpu_big_page_size(existing, gpu);

            // existing's end has not been adjusted yet
            existing_pages_big = range_num_big_pages(existing->start, new->start - 1, big_page_size);

            // Take references on all big pages covered by new
            new_pages_big = uvm_va_block_num_big_pages(new, big_page_size);
            if (new_pages_big) {
                uvm_page_table_range_get_upper(&gpu_va_space->page_tables,
                                               &existing_gpu_state->page_table_range_big,
                                               &new_gpu_state->page_table_range_big,
                                               new_pages_big);

                // If the split point is within a big page region, we might have
                // a gap since neither existing nor new can use it anymore.
                // Get the top N bits from existing's mask to handle that.
                bitmap_shift_right(new_gpu_state->big_ptes,
                                   existing_gpu_state->big_ptes,
                                   uvm_va_block_num_big_pages(existing, big_page_size) - new_pages_big,
                                   MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

                new_gpu_state->initialized_big = existing_gpu_state->initialized_big;
            }

            // Drop existing's references on the big PTEs it no longer covers
            // now that new has references on them. Note that neither existing
            // nor new might have big PTEs after the split. In that case, this
            // shrink will free the entire old range.
            uvm_page_table_range_shrink(&gpu_va_space->page_tables,
                                        &existing_gpu_state->page_table_range_big,
                                        existing_pages_big);

            if (existing_pages_big == 0) {
                memset(&existing_gpu_state->page_table_range_big, 0, sizeof(existing_gpu_state->page_table_range_big));
                existing_gpu_state->initialized_big = false;
            }

            bitmap_clear(existing_gpu_state->big_ptes,
                         existing_pages_big,
                         MAX_BIG_PAGES_PER_UVM_VA_BLOCK - existing_pages_big);
        }

        if (existing_gpu_state->page_table_range_4k.table) {
            // Since existing and new share the same PDE we just need to bump
            // the ref-count on new's sub-range.
            uvm_page_table_range_get_upper(&gpu_va_space->page_tables,
                                           &existing_gpu_state->page_table_range_4k,
                                           &new_gpu_state->page_table_range_4k,
                                           uvm_va_block_size(new) / UVM_PAGE_SIZE_4K);

            // Drop existing's references on the PTEs it no longer covers now
            // that new has references on them.
            existing_pages_4k = existing_pages * (PAGE_SIZE / UVM_PAGE_SIZE_4K);
            uvm_page_table_range_shrink(&gpu_va_space->page_tables,
                                        &existing_gpu_state->page_table_range_4k,
                                        existing_pages_4k);
        }

        // We have to set this explicitly to handle the case of splitting an
        // invalid, active 2M PTE with no lower page tables allocated.
        if (existing_gpu_state->pte_is_2m) {
            UVM_ASSERT(!existing_gpu_state->page_table_range_big.table);
            UVM_ASSERT(!existing_gpu_state->page_table_range_4k.table);
            existing_gpu_state->pte_is_2m = false;
        }

        // existing can't possibly cover 2MB after a split, so drop any 2M PTE
        // references it has. We've taken the necessary references on the lower
        // tables above.
        block_put_ptes_safe(&gpu_va_space->page_tables, &existing_gpu_state->page_table_range_2m);
        existing_gpu_state->activated_big = false;
        existing_gpu_state->activated_4k = false;
    }

    block_split_page_mask(&existing_gpu_state->evicted, existing_pages, &new_gpu_state->evicted, new_pages);
}

NV_STATUS uvm_va_block_split(uvm_va_block_t *existing_va_block,
                             NvU64 new_end,
                             uvm_va_block_t **new_va_block,
                             uvm_va_range_t *new_va_range)
{
    uvm_va_space_t *va_space;
    uvm_va_block_t *new_block = NULL;
    NV_STATUS status;

    va_space = new_va_range->va_space;
    UVM_ASSERT(existing_va_block->va_range);
    UVM_ASSERT(existing_va_block->va_range->va_space == va_space);
    UVM_ASSERT(!uvm_va_block_is_hmm(existing_va_block));

    // External range types can't be split
    UVM_ASSERT(existing_va_block->va_range->type == UVM_VA_RANGE_TYPE_MANAGED);
    UVM_ASSERT(new_va_range->type == UVM_VA_RANGE_TYPE_MANAGED);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    UVM_ASSERT(new_end > existing_va_block->start);
    UVM_ASSERT(new_end < existing_va_block->end);
    UVM_ASSERT(PAGE_ALIGNED(new_end + 1));

    status = uvm_va_block_create(new_va_range, new_end + 1, existing_va_block->end, &new_block);
    if (status != NV_OK)
        return status;

    // We're protected from other splits and faults by the va_space lock being
    // held in write mode, but that doesn't stop the reverse mapping (eviction
    // path) from inspecting the existing block. Stop those threads by taking
    // the block lock. When a reverse mapping thread takes this lock after the
    // split has been performed, it will have to re-inspect state and may see
    // that it should use the newly-split block instead.
    uvm_mutex_lock(&existing_va_block->lock);

    status = uvm_va_block_split_locked(existing_va_block, new_end, new_block, new_va_range);

    uvm_mutex_unlock(&existing_va_block->lock);

    if (status != NV_OK)
        uvm_va_block_release(new_block);
    else if (new_va_block)
        *new_va_block = new_block;

    return status;
}

NV_STATUS uvm_va_block_split_locked(uvm_va_block_t *existing_va_block,
                                    NvU64 new_end,
                                    uvm_va_block_t *new_block,
                                    uvm_va_range_t *new_va_range)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(existing_va_block);
    uvm_gpu_id_t id;
    NV_STATUS status;
    uvm_perf_event_data_t event_data;
    uvm_va_block_context_t *va_block_context;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    va_block_context = uvm_va_space_block_context(va_space, NULL);

    UVM_ASSERT(block_check_chunks(existing_va_block));

    // As soon as we update existing's reverse mappings to point to the newly-
    // split block, the eviction path could try to operate on the new block.
    // Lock that out too until new is ready.
    //
    // Note that we usually shouldn't nest block locks, but it's ok here because
    // we just created new_block so no other thread could possibly take it out
    // of order with existing's lock.
    uvm_mutex_lock_nested(&new_block->lock);

    // The split has to be transactional, meaning that if we fail, the existing
    // block must not be modified. Handle that by pre-allocating everything we
    // might need under both existing and new at the start so we only have a
    // single point of failure.

    // Since pre-allocation might require allocating new PTEs, we have to handle
    // allocation retry which might drop existing's block lock. The
    // preallocation is split into two steps for that: the first part which
    // allocates and splits PTEs can handle having the block lock dropped then
    // re-taken. It won't modify existing_va_block other than adding new PTE
    // allocations and splitting existing PTEs, which is always safe.
    status = UVM_VA_BLOCK_RETRY_LOCKED(existing_va_block,
                                       NULL,
                                       block_split_presplit_ptes(existing_va_block, new_block));
    if (status != NV_OK)
        goto out;

    // Pre-allocate, stage two. This modifies existing_va_block in ways which
    // violate many assumptions (such as changing chunk size), but it will put
    // things back into place on a failure without dropping the block lock.
    status = block_split_preallocate_no_retry(existing_va_block, new_block);
    if (status != NV_OK)
        goto out;

    // We'll potentially be freeing page tables, so we need to wait for any
    // outstanding work before we start
    status = uvm_tracker_wait(&existing_va_block->tracker);
    if (status != NV_OK)
        goto out;

    // Update existing's state only once we're past all failure points

    event_data.block_shrink.block = existing_va_block;
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_BLOCK_SHRINK, &event_data);

    block_split_cpu(existing_va_block, new_block);

    for_each_gpu_id(id)
        block_split_gpu(existing_va_block, new_block, id);

    // Update the size of the existing block first so that
    // block_set_processor_masks can use block_{set,clear}_resident_processor
    // that relies on the size to be correct.
    existing_va_block->end = new_end;

    block_split_page_mask(&existing_va_block->read_duplicated_pages,
                          uvm_va_block_num_cpu_pages(existing_va_block),
                          &new_block->read_duplicated_pages,
                          uvm_va_block_num_cpu_pages(new_block));

    if (!uvm_va_block_is_hmm(existing_va_block)) {
        block_split_page_mask(&existing_va_block->maybe_mapped_pages,
                              uvm_va_block_num_cpu_pages(existing_va_block),
                              &new_block->maybe_mapped_pages,
                              uvm_va_block_num_cpu_pages(new_block));
    }

    block_set_processor_masks(existing_va_block);
    block_set_processor_masks(new_block);

    if (uvm_va_block_is_hmm(existing_va_block)) {
        uvm_hmm_va_block_split_tree(existing_va_block, new_block);
        uvm_va_policy_node_split_move(existing_va_block, new_block);
    }

out:
    // Run checks on existing_va_block even on failure, since an error must
    // leave the block in a consistent state.
    UVM_ASSERT(block_check_chunks(existing_va_block));
    UVM_ASSERT(block_check_mappings(existing_va_block, va_block_context));
    if (status == NV_OK) {
        UVM_ASSERT(block_check_chunks(new_block));
        UVM_ASSERT(block_check_mappings(new_block, va_block_context));
    }
    else {
        int nid;

        for_each_possible_uvm_node(nid)
            block_free_cpu_chunk_storage(new_block, nid);
    }

    uvm_mutex_unlock_nested(&new_block->lock);

    return status;
}

static bool block_region_might_read_duplicate(uvm_va_block_t *va_block,
                                              uvm_va_block_region_t region)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_va_range_t *va_range = va_block->va_range;

    if (!uvm_va_space_can_read_duplicate(va_space, NULL))
        return false;

    // TODO: Bug 3660922: need to implement HMM read duplication support.
    if (uvm_va_block_is_hmm(va_block) ||
        uvm_va_range_get_policy(va_range)->read_duplication == UVM_READ_DUPLICATION_DISABLED)
        return false;

    if (uvm_va_range_get_policy(va_range)->read_duplication == UVM_READ_DUPLICATION_UNSET
        && uvm_page_mask_region_weight(&va_block->read_duplicated_pages, region) == 0)
        return false;

    return true;
}

// Returns the new access permission for the processor that faulted or
// triggered access counter notifications on the given page
//
// TODO: Bug 1766424: this function works on a single page at a time. This
//       could be changed in the future to optimize multiple faults/counters on
//       contiguous pages.
static uvm_prot_t compute_new_permission(uvm_va_block_t *va_block,
                                         uvm_va_block_context_t *va_block_context,
                                         uvm_page_index_t page_index,
                                         uvm_processor_id_t fault_processor_id,
                                         uvm_processor_id_t new_residency,
                                         uvm_fault_access_type_t access_type)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_prot_t logical_prot, new_prot;
    uvm_processor_mask_t *revoke_processors = &va_block_context->scratch_processor_mask;
    struct vm_area_struct *hmm_vma = va_block_context->hmm.vma;

    // TODO: Bug 1766432: Refactor into policies. Current policy is
    //       query_promote: upgrade access privileges to avoid future faults IF
    //       they don't trigger further revocations.
    new_prot = uvm_fault_access_type_to_prot(access_type);
    logical_prot = compute_logical_prot(va_block, hmm_vma, page_index);

    UVM_ASSERT(logical_prot >= new_prot);

    if ((logical_prot > UVM_PROT_READ_ONLY) &&
        (new_prot == UVM_PROT_READ_ONLY) &&
        !block_region_might_read_duplicate(va_block, uvm_va_block_region_for_page(page_index))) {

        block_page_authorized_processors(va_block,
                                         page_index,
                                         UVM_PROT_READ_WRITE_ATOMIC,
                                         revoke_processors);

        uvm_processor_mask_andnot(revoke_processors,
                                  revoke_processors,
                                  &va_space->has_native_atomics[uvm_id_value(new_residency)]);

        // Only check if there are no faultable processors in the revoke
        // processors mask.
        uvm_processor_mask_and(revoke_processors, revoke_processors, &va_space->faultable_processors);

        if (uvm_processor_mask_empty(revoke_processors))
            new_prot = UVM_PROT_READ_WRITE;
    }

    if (logical_prot == UVM_PROT_READ_WRITE_ATOMIC && new_prot == UVM_PROT_READ_WRITE) {
        if (uvm_processor_mask_test(&va_space->has_native_atomics[uvm_id_value(new_residency)], fault_processor_id))
            new_prot = UVM_PROT_READ_WRITE_ATOMIC;
    }

    return new_prot;
}

static NV_STATUS do_block_add_mappings_after_migration(uvm_va_block_t *va_block,
                                                       uvm_va_block_context_t *va_block_context,
                                                       uvm_processor_id_t new_residency,
                                                       uvm_processor_id_t processor_id,
                                                       const uvm_processor_mask_t *map_processors,
                                                       uvm_va_block_region_t region,
                                                       const uvm_page_mask_t *map_page_mask,
                                                       uvm_prot_t max_prot,
                                                       const uvm_processor_mask_t *thrashing_processors,
                                                       uvm_tracker_t *tracker)
{
    NV_STATUS status;
    uvm_processor_id_t map_processor_id;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_prot_t new_map_prot = max_prot;
    uvm_processor_mask_t *map_processors_local;
    uvm_processor_mask_t *native_atomics_mask = &va_space->has_native_atomics[uvm_id_value(new_residency)];

    map_processors_local = uvm_processor_mask_cache_alloc();
    if (!map_processors_local)
        return NV_ERR_NO_MEMORY;

    uvm_processor_mask_copy(map_processors_local, map_processors);

    // Handle atomic mappings separately
    if (max_prot == UVM_PROT_READ_WRITE_ATOMIC) {
        if (uvm_processor_mask_test(native_atomics_mask, processor_id)) {

            for_each_id_in_mask(map_processor_id, map_processors_local) {
                UvmEventMapRemoteCause cause = UvmEventMapRemoteCausePolicy;

                // Skip processors without native atomics to the residency.
                if (!uvm_processor_mask_test(native_atomics_mask, map_processor_id))
                    continue;

                if (thrashing_processors && uvm_processor_mask_test(thrashing_processors, map_processor_id))
                    cause = UvmEventMapRemoteCauseThrashing;

                status = uvm_va_block_map(va_block,
                                          va_block_context,
                                          map_processor_id,
                                          region,
                                          map_page_mask,
                                          UVM_PROT_READ_WRITE_ATOMIC,
                                          cause,
                                          tracker);
                if (status != NV_OK)
                    goto out;
            }

            // Filter out these mapped processors for the next steps
            uvm_processor_mask_andnot(map_processors_local, map_processors_local, native_atomics_mask);

            new_map_prot = UVM_PROT_READ_WRITE;
        }
        else {
            if (UVM_ID_IS_CPU(processor_id))
                new_map_prot = UVM_PROT_READ_WRITE;
            else
                new_map_prot = UVM_PROT_READ_ONLY;
        }
    }

    // Map the rest of processors
    for_each_id_in_mask(map_processor_id, map_processors_local) {
        UvmEventMapRemoteCause cause = UvmEventMapRemoteCausePolicy;
        uvm_prot_t final_map_prot;
        bool map_processor_has_enabled_system_wide_atomics =
            uvm_processor_mask_test(&va_space->system_wide_atomics_enabled_processors, map_processor_id);

        // Write mappings from processors with disabled system-wide atomics are treated like atomics
        if (new_map_prot == UVM_PROT_READ_WRITE && !map_processor_has_enabled_system_wide_atomics)
            final_map_prot = UVM_PROT_READ_WRITE_ATOMIC;
        else
            final_map_prot = new_map_prot;

        if (thrashing_processors && uvm_processor_mask_test(thrashing_processors, map_processor_id))
            cause = UvmEventMapRemoteCauseThrashing;

        status = uvm_va_block_map(va_block,
                                  va_block_context,
                                  map_processor_id,
                                  region,
                                  map_page_mask,
                                  final_map_prot,
                                  cause,
                                  tracker);
        if (status != NV_OK)
            goto out;
    }

out:
    uvm_processor_mask_cache_free(map_processors_local);

    return NV_OK;
}

NV_STATUS uvm_va_block_add_mappings_after_migration(uvm_va_block_t *va_block,
                                                    uvm_va_block_context_t *va_block_context,
                                                    uvm_processor_id_t new_residency,
                                                    uvm_processor_id_t processor_id,
                                                    uvm_va_block_region_t region,
                                                    const uvm_page_mask_t *map_page_mask,
                                                    uvm_prot_t max_prot,
                                                    const uvm_processor_mask_t *thrashing_processors)
{
    NV_STATUS tracker_status, status = NV_OK;
    uvm_processor_mask_t *map_other_processors = NULL;
    uvm_processor_mask_t *map_uvm_lite_gpus = NULL;
    uvm_processor_id_t map_processor_id;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    const uvm_page_mask_t *final_page_mask = map_page_mask;
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    const uvm_va_policy_t *policy = uvm_va_policy_get_region(va_block, region);
    uvm_processor_id_t preferred_location;

    uvm_assert_mutex_locked(&va_block->lock);

    map_other_processors = uvm_processor_mask_cache_alloc();
    if (!map_other_processors) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    map_uvm_lite_gpus = uvm_processor_mask_cache_alloc();
    if (!map_uvm_lite_gpus) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    // Read duplication takes precedence over SetAccessedBy.
    //
    // Exclude ranges with read duplication set...
    if (uvm_va_policy_is_read_duplicate(policy, va_space)) {
        status = NV_OK;
        goto out;
    }

    // ... and pages read-duplicated by performance heuristics
    if (policy->read_duplication == UVM_READ_DUPLICATION_UNSET) {
        if (map_page_mask) {
            uvm_page_mask_andnot(&va_block_context->mapping.filtered_page_mask,
                                 map_page_mask,
                                 &va_block->read_duplicated_pages);
        }
        else {
            uvm_page_mask_complement(&va_block_context->mapping.filtered_page_mask, &va_block->read_duplicated_pages);
        }
        final_page_mask = &va_block_context->mapping.filtered_page_mask;
    }

    // Add mappings for accessed_by processors and the given processor mask
    if (thrashing_processors)
        uvm_processor_mask_or(map_other_processors, &policy->accessed_by, thrashing_processors);
    else
        uvm_processor_mask_copy(map_other_processors, &policy->accessed_by);

    // Only processors that can access the new location must be considered
    uvm_processor_mask_and(map_other_processors,
                           map_other_processors,
                           &va_space->accessible_from[uvm_id_value(new_residency)]);

    // Exclude caller processor as it must have already been mapped
    uvm_processor_mask_clear(map_other_processors, processor_id);

    // Exclude preferred location so it won't get remote mappings
    preferred_location = policy->preferred_location;
    if (UVM_ID_IS_VALID(preferred_location) &&
        !uvm_id_equal(new_residency, preferred_location) &&
        uvm_va_space_processor_has_memory(va_space, preferred_location)) {
        uvm_processor_mask_clear(map_other_processors, preferred_location);
    }

    // Map the UVM-Lite GPUs if the new location is the preferred location. This
    // will only create mappings on first touch. After that they're persistent
    // so uvm_va_block_map will be a no-op.
    uvm_processor_mask_and(map_uvm_lite_gpus, map_other_processors, block_get_uvm_lite_gpus(va_block));
    if (!uvm_processor_mask_empty(map_uvm_lite_gpus) &&
        uvm_va_policy_preferred_location_equal(policy, new_residency, va_block_context->make_resident.dest_nid)) {
        for_each_id_in_mask (map_processor_id, map_uvm_lite_gpus) {
            status = uvm_va_block_map(va_block,
                                      va_block_context,
                                      map_processor_id,
                                      region,
                                      final_page_mask,
                                      UVM_PROT_READ_WRITE_ATOMIC,
                                      UvmEventMapRemoteCauseCoherence,
                                      &local_tracker);
            if (status != NV_OK)
                goto out;
        }
    }

    uvm_processor_mask_andnot(map_other_processors, map_other_processors, block_get_uvm_lite_gpus(va_block));

    // We can't map non-migratable pages to the CPU. If we have any, build a
    // new mask of migratable pages and map the CPU separately.
    if (uvm_processor_mask_test(map_other_processors, UVM_ID_CPU) &&
        !uvm_range_group_all_migratable(va_space,
                                        uvm_va_block_region_start(va_block, region),
                                        uvm_va_block_region_end(va_block, region))) {
        uvm_page_mask_t *migratable_mask = &va_block_context->mapping.migratable_mask;

        uvm_range_group_migratable_page_mask(va_block, region, migratable_mask);
        if (uvm_page_mask_and(migratable_mask, migratable_mask, final_page_mask)) {
            status = do_block_add_mappings_after_migration(va_block,
                                                           va_block_context,
                                                           new_residency,
                                                           processor_id,
                                                           &g_uvm_processor_mask_cpu,
                                                           region,
                                                           migratable_mask,
                                                           max_prot,
                                                           thrashing_processors,
                                                           &local_tracker);
            if (status != NV_OK)
                goto out;
        }

        uvm_processor_mask_clear(map_other_processors, UVM_ID_CPU);
    }

    status = do_block_add_mappings_after_migration(va_block,
                                                   va_block_context,
                                                   new_residency,
                                                   processor_id,
                                                   map_other_processors,
                                                   region,
                                                   final_page_mask,
                                                   max_prot,
                                                   thrashing_processors,
                                                   &local_tracker);
    if (status != NV_OK)
        goto out;

out:
    tracker_status = uvm_tracker_add_tracker_safe(&va_block->tracker, &local_tracker);
    uvm_tracker_deinit(&local_tracker);
    uvm_processor_mask_cache_free(map_other_processors);
    uvm_processor_mask_cache_free(map_uvm_lite_gpus);

    return status == NV_OK ? tracker_status : status;
}

uvm_prot_t uvm_va_block_page_compute_highest_permission(uvm_va_block_t *va_block,
                                                        uvm_va_block_context_t *va_block_context,
                                                        uvm_processor_id_t processor_id,
                                                        uvm_page_index_t page_index)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_processor_mask_t *resident_processors = &va_block_context->scratch_processor_mask;
    NvU32 resident_processors_count;

    if (uvm_processor_mask_test(block_get_uvm_lite_gpus(va_block), processor_id))
        return UVM_PROT_READ_WRITE_ATOMIC;

    uvm_va_block_page_resident_processors(va_block, page_index, resident_processors);
    resident_processors_count = uvm_processor_mask_get_count(resident_processors);

    if (resident_processors_count == 0) {
        return UVM_PROT_NONE;
    }
    else if (resident_processors_count > 1) {
        // If there are many copies, we can only map READ ONLY
        //
        // The block state doesn't track the mapping target (aperture) of each
        // individual PTE, just the permissions and where the data is resident.
        // If the data is resident in multiple places, then we have a problem
        // since we can't know where the PTE points. This means we won't know
        // what needs to be unmapped for cases like UvmUnregisterGpu and
        // UvmDisablePeerAccess.
        //
        // The simple way to solve this is to enforce that a read-duplication
        // mapping always points to local memory.
        if (uvm_processor_mask_test(resident_processors, processor_id))
            return UVM_PROT_READ_ONLY;

        return UVM_PROT_NONE;
    }
    else {
        uvm_processor_id_t atomic_id;
        uvm_processor_id_t residency;
        uvm_processor_mask_t *atomic_mappings;
        uvm_processor_mask_t *write_mappings;

        // Search the id of the processor with the only resident copy
        residency = uvm_processor_mask_find_first_id(resident_processors);
        UVM_ASSERT(UVM_ID_IS_VALID(residency));

        // If we cannot map the processor with the resident copy, exit
        if (!uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(residency)], processor_id))
            return UVM_PROT_NONE;

        // Fast path: if the page is not mapped anywhere else, it can be safely
        // mapped with RWA permission
        if (!uvm_page_mask_test(&va_block->maybe_mapped_pages, page_index) &&
            !uvm_va_block_is_hmm(va_block))
            return UVM_PROT_READ_WRITE_ATOMIC;

        atomic_mappings = &va_block_context->scratch_processor_mask;

        block_page_authorized_processors(va_block, page_index, UVM_PROT_READ_WRITE_ATOMIC, atomic_mappings);

        // Exclude processors with system-wide atomics disabled from atomic_mappings
        uvm_processor_mask_and(atomic_mappings, atomic_mappings, &va_space->system_wide_atomics_enabled_processors);

        // Exclude the processor for which the mapping protections are being computed
        uvm_processor_mask_clear(atomic_mappings, processor_id);

        // If there is any processor with atomic mapping, check if it has native atomics to the processor
        // with the resident copy. If it does not, we can only map READ ONLY
        atomic_id = uvm_processor_mask_find_first_id(atomic_mappings);
        if (UVM_ID_IS_VALID(atomic_id) &&
            !uvm_processor_mask_test(&va_space->has_native_atomics[uvm_id_value(residency)], atomic_id)) {
            return UVM_PROT_READ_ONLY;
        }

        write_mappings = &va_block_context->scratch_processor_mask;

        block_page_authorized_processors(va_block, page_index, UVM_PROT_READ_WRITE, write_mappings);

        // Exclude the processor for which the mapping protections are being computed
        uvm_processor_mask_clear(write_mappings, processor_id);

        // At this point, any processor with atomic mappings either has native
        // atomics support to the processor with the resident copy or has
        // disabled system-wide atomics. If the requesting processor has
        // disabled system-wide atomics or has native atomics to that processor,
        // we can map with ATOMIC privileges. Likewise, if there are no other
        // processors with WRITE or ATOMIC mappings, we can map with ATOMIC
        // privileges. For HMM, don't allow GPU atomic access to remote mapped
        // system memory even if there are no write mappings since CPU access
        // can be upgraded without notification.
        if (!uvm_processor_mask_test(&va_space->system_wide_atomics_enabled_processors, processor_id) ||
            uvm_processor_mask_test(&va_space->has_native_atomics[uvm_id_value(residency)], processor_id) ||
            (uvm_processor_mask_empty(write_mappings) && !uvm_va_block_is_hmm(va_block))) {
            return UVM_PROT_READ_WRITE_ATOMIC;
        }

        return UVM_PROT_READ_WRITE;
    }
}

NV_STATUS uvm_va_block_add_mappings(uvm_va_block_t *va_block,
                                    uvm_va_block_context_t *va_block_context,
                                    uvm_processor_id_t processor_id,
                                    uvm_va_block_region_t region,
                                    const uvm_page_mask_t *page_mask,
                                    UvmEventMapRemoteCause cause)
{
    uvm_va_range_t *va_range = va_block->va_range;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    NV_STATUS status = NV_OK;
    uvm_page_index_t page_index;
    uvm_range_group_range_iter_t iter;
    uvm_prot_t prot_to_map;

    if (UVM_ID_IS_CPU(processor_id) && !uvm_va_block_is_hmm(va_block)) {
        if (!uvm_va_range_vma_check(va_range, va_block_context->mm))
            return NV_OK;

        uvm_range_group_range_migratability_iter_first(va_space,
                                                       uvm_va_block_region_start(va_block, region),
                                                       uvm_va_block_region_end(va_block, region),
                                                       &iter);
    }

    for (prot_to_map = UVM_PROT_READ_ONLY; prot_to_map <= UVM_PROT_READ_WRITE_ATOMIC; ++prot_to_map)
        va_block_context->mask_by_prot[prot_to_map - 1].count = 0;

    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        // Read duplication takes precedence over SetAccessedBy. Exclude pages
        // read-duplicated by performance heuristics
        if (uvm_page_mask_test(&va_block->read_duplicated_pages, page_index))
            continue;

        prot_to_map = uvm_va_block_page_compute_highest_permission(va_block,
                                                                   va_block_context,
                                                                   processor_id,
                                                                   page_index);
        if (prot_to_map == UVM_PROT_NONE)
            continue;

        if (UVM_ID_IS_CPU(processor_id) && !uvm_va_block_is_hmm(va_block)) {
            while (uvm_va_block_cpu_page_index(va_block, iter.end) < page_index) {
                uvm_range_group_range_migratability_iter_next(va_space,
                                                              &iter,
                                                              uvm_va_block_region_end(va_block, region));
            }

            if (!iter.migratable)
                continue;
        }

        if (va_block_context->mask_by_prot[prot_to_map - 1].count++ == 0)
            uvm_page_mask_zero(&va_block_context->mask_by_prot[prot_to_map - 1].page_mask);

        uvm_page_mask_set(&va_block_context->mask_by_prot[prot_to_map - 1].page_mask, page_index);
    }

    for (prot_to_map = UVM_PROT_READ_ONLY; prot_to_map <= UVM_PROT_READ_WRITE_ATOMIC; ++prot_to_map) {
        if (va_block_context->mask_by_prot[prot_to_map - 1].count == 0)
            continue;

        status = uvm_va_block_map(va_block,
                                  va_block_context,
                                  processor_id,
                                  region,
                                  &va_block_context->mask_by_prot[prot_to_map - 1].page_mask,
                                  prot_to_map,
                                  cause,
                                  &va_block->tracker);
        if (status != NV_OK)
            break;
    }

    return status;
}

static bool can_read_duplicate(uvm_va_block_t *va_block,
                               uvm_page_index_t page_index,
                               const uvm_va_policy_t *policy,
                               const uvm_perf_thrashing_hint_t *thrashing_hint)
{
    if (uvm_va_policy_is_read_duplicate(policy, uvm_va_block_get_va_space(va_block)))
        return true;

    if (policy->read_duplication != UVM_READ_DUPLICATION_DISABLED &&
        uvm_page_mask_test(&va_block->read_duplicated_pages, page_index) &&
        thrashing_hint->type != UVM_PERF_THRASHING_HINT_TYPE_PIN)
        return true;

    return false;
}

// TODO: Bug 1827400: If the faulting processor has support for native
//       atomics to the current location and the faults on the page were
//       triggered by atomic accesses only, we keep the current residency.
//       This is a short-term solution to exercise remote atomics over
//       NVLINK when possible (not only when preferred location is set to
//       the remote GPU) as they are much faster than relying on page
//       faults and permission downgrades, which cause thrashing. In the
//       future, the thrashing detection/prevention heuristics should
//       detect and handle this case.
static bool map_remote_on_atomic_fault(uvm_va_space_t *va_space,
                                       NvU32 access_type_mask,
                                       uvm_processor_id_t processor_id,
                                       uvm_processor_id_t residency)
{
    // This policy can be enabled/disabled using a module parameter
    if (!uvm_perf_map_remote_on_native_atomics_fault)
        return false;

    // Only consider atomics faults
    if (uvm_fault_access_type_mask_lowest(access_type_mask) < UVM_FAULT_ACCESS_TYPE_ATOMIC_WEAK)
        return false;

    // We cannot differentiate CPU writes from atomics. We exclude CPU faults
    // from the logic explained above in order to avoid mapping CPU to vidmem
    // memory due to a write.
    if (UVM_ID_IS_CPU(processor_id))
        return false;

    // On P9 systems (which have native HW support for system-wide atomics), we
    // have determined experimentally that placing memory on a GPU yields the
    // best performance on most cases (since CPU can cache vidmem but not vice
    // versa). Therefore, don't map remotely if the current residency is
    // sysmem.
    if (UVM_ID_IS_CPU(residency))
        return false;

    return uvm_processor_mask_test(&va_space->has_native_atomics[uvm_id_value(residency)], processor_id);
}

// TODO: Bug 1766424: this function works on a single page at a time. This
//       could be changed in the future to optimize multiple faults or access
//       counter notifications on contiguous pages.
static uvm_processor_id_t block_select_processor_residency(uvm_va_block_t *va_block,
                                                           uvm_va_block_context_t *va_block_context,
                                                           uvm_page_index_t page_index,
                                                           uvm_processor_id_t processor_id,
                                                           NvU32 access_type_mask,
                                                           const uvm_va_policy_t *policy,
                                                           const uvm_perf_thrashing_hint_t *thrashing_hint,
                                                           uvm_service_operation_t operation,
                                                           const bool hmm_migratable,
                                                           bool *read_duplicate)
{
    uvm_processor_id_t closest_resident_processor;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    bool may_read_duplicate;
    uvm_processor_id_t preferred_location;

    // TODO: Bug 3660968: Remove uvm_hmm_force_sysmem_set() check as soon as
    // HMM migration is implemented VMAs other than anonymous memory.
    // TODO: Bug 4050579: Remove hmm_migratable check when swap cached pages
    // can be migrated.
    if (is_uvm_fault_force_sysmem_set() ||
        !hmm_migratable ||
        uvm_hmm_must_use_sysmem(va_block, va_block_context->hmm.vma)) {
        *read_duplicate = false;
        return UVM_ID_CPU;
    }

    may_read_duplicate = can_read_duplicate(va_block, page_index, policy, thrashing_hint);

    // Read/prefetch faults on a VA range with read duplication enabled
    // always create a copy of the page on the faulting processor's memory.
    // Note that access counters always use UVM_FAULT_ACCESS_TYPE_PREFETCH,
    // which will lead to read duplication if it is enabled.
    *read_duplicate = may_read_duplicate &&
                      (uvm_fault_access_type_mask_highest(access_type_mask) <= UVM_FAULT_ACCESS_TYPE_READ);

    if (*read_duplicate)
        return processor_id;

    *read_duplicate = false;

    // If read-duplication is active in the page but we are not
    // read-duplicating because the access type is not a read or a prefetch,
    // the faulting processor should get a local copy
    if (may_read_duplicate)
        return processor_id;

    // If the faulting processor is the preferred location always migrate
    preferred_location = policy->preferred_location;
    if (uvm_id_equal(processor_id, preferred_location)) {
        if (thrashing_hint->type != UVM_PERF_THRASHING_HINT_TYPE_NONE) {
            UVM_ASSERT(thrashing_hint->type == UVM_PERF_THRASHING_HINT_TYPE_PIN);
            if (uvm_va_space_processor_has_memory(va_space, processor_id))
                UVM_ASSERT(uvm_id_equal(thrashing_hint->pin.residency, processor_id));
        }

        return processor_id;
    }

    // If the faulting processor is the CPU, HMM has to migrate the block to
    // system memory.
    // TODO: Bug 3900021: [UVM-HMM] investigate thrashing improvements.
    if (UVM_ID_IS_CPU(processor_id) && uvm_va_block_is_hmm(va_block))
        return processor_id;

    if (thrashing_hint->type == UVM_PERF_THRASHING_HINT_TYPE_PIN) {
        UVM_ASSERT(uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(thrashing_hint->pin.residency)],
                                           processor_id));
        return thrashing_hint->pin.residency;
    }

    closest_resident_processor = uvm_va_block_page_get_closest_resident(va_block,
                                                                        va_block_context,
                                                                        page_index,
                                                                        processor_id);

    // If the page is not resident anywhere, select the preferred location as
    // long as the preferred location is accessible from the faulting processor.
    // Otherwise select the faulting processor.
    if (UVM_ID_IS_INVALID(closest_resident_processor)) {
        if (UVM_ID_IS_VALID(preferred_location) &&
            uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(preferred_location)],
                                    processor_id)) {
            return preferred_location;
        }

        return processor_id;
    }

    // AccessedBy mappings might have not been created for the CPU if the thread
    // which made the memory resident did not have the proper references on the
    // mm_struct (for example, the GPU fault handling path when
    // uvm_va_space_mm_enabled() is false).
    //
    // Also, in uvm_migrate_*, we implement a two-pass scheme in which
    // AccessedBy mappings may be delayed to the second pass. This can produce
    // faults even if the faulting processor is in the accessed_by mask.
    //
    // Here, we keep it on the current residency and we just add the missing
    // mapping.
    if (uvm_processor_mask_test(&policy->accessed_by, processor_id) &&
        uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(closest_resident_processor)], processor_id) &&
        operation != UVM_SERVICE_OPERATION_ACCESS_COUNTERS) {
        return closest_resident_processor;
    }

    // Check if we should map the closest resident processor remotely on atomic
    // fault
    if (map_remote_on_atomic_fault(va_space, access_type_mask, processor_id, closest_resident_processor))
        return closest_resident_processor;

    // If the processor has access to the preferred location, and the page is
    // not resident on the accessing processor, move it to the preferred
    // location.
    if (!uvm_id_equal(closest_resident_processor, processor_id) &&
        UVM_ID_IS_VALID(preferred_location) &&
        uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(preferred_location)], processor_id))
        return preferred_location;

    // Check if we should map the closest resident processor remotely on remote CPU fault
    //
    // When faulting on CPU, there's a linux process on behalf of it, which is associated
    // with a unique VM pointed by current->mm. A block of memory residing on GPU is also
    // associated with VM, pointed by va_block_context->mm. If they match, it's a regular
    // (local) fault, and we may want to migrate a page from GPU to CPU.
    // If it's a 'remote' fault, i.e. linux process differs from one associated with block
    // VM, we might preserve residence.
    //
    // Establishing a remote fault without access counters means the memory could stay in
    // the wrong spot for a long time, which is why we prefer to avoid creating remote
    // mappings. However when NIC accesses a memory residing on GPU, it's worth to keep it
    // in place for NIC accesses.
    //
    // The logic that's used to detect remote faulting also keeps memory in place for
    // ptrace accesses. We would prefer to control those policies separately, but the
    // NIC case takes priority.
    // If the accessing processor is CPU, we're either handling a fault
    // from other than owning process, or we're handling an MOMC
    // notification. Only prevent migration for the former.
    if (UVM_ID_IS_CPU(processor_id) &&
        operation != UVM_SERVICE_OPERATION_ACCESS_COUNTERS &&
        uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(closest_resident_processor)], processor_id) &&
        va_block_context->mm != current->mm) {
        UVM_ASSERT(va_block_context->mm != NULL);
        return closest_resident_processor;
    }

    // If the page is resident on a processor other than the preferred location,
    // or the faulting processor can't access the preferred location, we select
    // the faulting processor as the new residency.
    return processor_id;
}

static int block_select_node_residency(uvm_va_block_t *va_block,
                                       uvm_page_index_t page_index,
                                       uvm_processor_id_t new_residency,
                                       const uvm_va_policy_t *policy,
                                       const uvm_perf_thrashing_hint_t *thrashing_hint)
{
    // For CPU faults, the fault handler runs on the CPU that faulted.
    // For GPU faults, the bottom half is pinned to CPUs closest to their GPU.
    // Therefore, in both cases, we can use numa_mem_id() to get the NUMA node
    // ID of the faulting processor.
    // Note that numa_mem_id() returns the nearest node with memory. In most
    // cases, this will be the current NUMA node. However, in the case that the
    // current node does not have any memory, we probably want the nearest node
    // with memory, anyway.
    int current_nid = numa_mem_id();
    bool may_read_duplicate = can_read_duplicate(va_block, page_index, policy, thrashing_hint);

    // For HMM allocations UVM doesn't always control allocation of the
    // destination page as the kernel may have already allocated one. Therefore
    // we can't respect the preferred node ID for HMM pages.
    // TODO: Bug 4453874: [UVM-HMM] Respect the preferred CPU NUMA Node ID when making a HMM page resident
    if (uvm_va_block_is_hmm(va_block))
        return NUMA_NO_NODE;

    // If the new resident processor is not the CPU, return the preferred nid
    // since it could be used for CPU allocations of staging pages.
    if (!UVM_ID_IS_CPU(new_residency))
        return policy->preferred_nid;

    // If the preferred location is the CPU, the new resident nid is the
    // preferred nid.
    if (UVM_ID_IS_CPU(policy->preferred_location))
        return policy->preferred_nid;

    // If read duplication is enabled and the page is also resident on the CPU,
    // keep its current NUMA node residency.
    if (may_read_duplicate && uvm_va_block_cpu_is_page_resident_on(va_block, NUMA_NO_NODE, page_index))
        return NUMA_NO_NODE;

    // The new_residency processor is the CPU and the preferred location is not
    // the CPU. If the page is resident on the CPU, keep its current residency.
    if (uvm_va_block_cpu_is_page_resident_on(va_block, NUMA_NO_NODE, page_index))
        return NUMA_NO_NODE;

    return current_nid;
}

uvm_processor_id_t uvm_va_block_select_residency(uvm_va_block_t *va_block,
                                                 uvm_va_block_context_t *va_block_context,
                                                 uvm_page_index_t page_index,
                                                 uvm_processor_id_t processor_id,
                                                 NvU32 access_type_mask,
                                                 const uvm_va_policy_t *policy,
                                                 const uvm_perf_thrashing_hint_t *thrashing_hint,
                                                 uvm_service_operation_t operation,
                                                 const bool hmm_migratable,
                                                 bool *read_duplicate)
{
    uvm_processor_id_t id;
    int nid;

    UVM_ASSERT(uvm_hmm_check_context_vma_is_valid(va_block,
                                                  va_block_context->hmm.vma,
                                                  uvm_va_block_region_for_page(page_index)));

    // First, select the processor for the new residency.
    id = block_select_processor_residency(va_block,
                                          va_block_context,
                                          page_index,
                                          processor_id,
                                          access_type_mask,
                                          policy,
                                          thrashing_hint,
                                          operation,
                                          hmm_migratable,
                                          read_duplicate);

    // If the intended residency doesn't have memory, fall back to the CPU.
    if (!block_processor_has_memory(va_block, id)) {
        *read_duplicate = false;
        id = UVM_ID_CPU;
    }

    // Now, that we know the new residency processor, select the NUMA node ID
    // based on the new processor.
    nid = block_select_node_residency(va_block, page_index, id, policy, thrashing_hint);

    va_block_context->make_resident.dest_nid = nid;

    return id;
}

static bool check_access_counters_dont_revoke(uvm_va_block_t *block,
                                              uvm_va_block_context_t *block_context,
                                              uvm_va_block_region_t region,
                                              const uvm_processor_mask_t *revoke_processors,
                                              const uvm_page_mask_t *revoke_page_mask,
                                              uvm_prot_t revoke_prot)
{
    uvm_processor_id_t id;
    for_each_id_in_mask(id, revoke_processors) {
        const uvm_page_mask_t *mapped_with_prot = block_map_with_prot_mask_get(block, id, revoke_prot);

        uvm_page_mask_and(&block_context->caller_page_mask, revoke_page_mask, mapped_with_prot);

        UVM_ASSERT(uvm_page_mask_region_weight(&block_context->caller_page_mask, region) == 0);
    }

    return true;
}

// Update service_context->prefetch_hint, service_context->per_processor_masks,
// and service_context->region.
static void uvm_va_block_get_prefetch_hint(uvm_va_block_t *va_block,
                                           const uvm_va_policy_t *policy,
                                           uvm_service_block_context_t *service_context)
{
    uvm_processor_id_t new_residency;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

    // Performance heuristics policy: we only consider prefetching when there
    // are migrations to a single processor, only.
    if (uvm_processor_mask_get_count(&service_context->resident_processors) == 1) {
        uvm_page_index_t page_index;
        uvm_page_mask_t *new_residency_mask;

        new_residency = uvm_processor_mask_find_first_id(&service_context->resident_processors);
        new_residency_mask = &service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency;

        // Update prefetch tracking structure with the pages that will migrate
        // due to faults
        uvm_perf_prefetch_get_hint_va_block(va_block,
                                            service_context->block_context,
                                            new_residency,
                                            new_residency_mask,
                                            service_context->region,
                                            &service_context->prefetch_bitmap_tree,
                                            &service_context->prefetch_hint);

        // Obtain the prefetch hint and give a fake fault access type to the
        // prefetched pages
        if (UVM_ID_IS_VALID(service_context->prefetch_hint.residency)) {
            const uvm_page_mask_t *prefetch_pages_mask = &service_context->prefetch_hint.prefetch_pages_mask;

            for_each_va_block_page_in_mask(page_index, prefetch_pages_mask, va_block) {
                UVM_ASSERT(!uvm_page_mask_test(new_residency_mask, page_index));

                service_context->access_type[page_index] = UVM_FAULT_ACCESS_TYPE_PREFETCH;

                if (uvm_va_policy_is_read_duplicate(policy, va_space) ||
                    (policy->read_duplication != UVM_READ_DUPLICATION_DISABLED &&
                     uvm_page_mask_test(&va_block->read_duplicated_pages, page_index))) {
                    if (service_context->read_duplicate_count++ == 0)
                        uvm_page_mask_zero(&service_context->read_duplicate_mask);

                    uvm_page_mask_set(&service_context->read_duplicate_mask, page_index);
                }
            }

            uvm_page_mask_or(new_residency_mask, new_residency_mask, prefetch_pages_mask);
            service_context->region = uvm_va_block_region_from_mask(va_block, new_residency_mask);
        }
    }
    else {
        service_context->prefetch_hint.residency = UVM_ID_INVALID;
    }
}

NV_STATUS uvm_va_block_service_copy(uvm_processor_id_t processor_id,
                                    uvm_processor_id_t new_residency,
                                    uvm_va_block_t *va_block,
                                    uvm_va_block_retry_t *block_retry,
                                    uvm_service_block_context_t *service_context)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_processor_mask_t *all_involved_processors =
        &service_context->block_context->make_resident.all_involved_processors;
    uvm_page_mask_t *new_residency_mask =
        &service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency;
    uvm_page_mask_t *did_migrate_mask = &service_context->block_context->make_resident.pages_changed_residency;
    uvm_page_mask_t *caller_page_mask = &service_context->block_context->caller_page_mask;
    uvm_make_resident_cause_t cause;
    NV_STATUS status;

    // 1- Migrate pages
    switch (service_context->operation) {
        case UVM_SERVICE_OPERATION_REPLAYABLE_FAULTS:
            cause = UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT;
            break;
        case UVM_SERVICE_OPERATION_NON_REPLAYABLE_FAULTS:
            cause = UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT;
            break;
        case UVM_SERVICE_OPERATION_ACCESS_COUNTERS:
            cause = UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER;
            break;
        default:
            UVM_ASSERT_MSG(false, "Invalid operation value %d\n", service_context->operation);
            // Set cause to silence compiler warning that it may be unused.
            cause = UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER;
            break;
    }

    // Reset masks before all of the make_resident calls
    uvm_page_mask_zero(did_migrate_mask);
    uvm_processor_mask_zero(all_involved_processors);

    // Handle read duplication first so that the caller_page_mask will be free
    // to use below and still valid in uvm_va_block_service_finish().
    // TODO: Bug 3660922: need to implement HMM read duplication support.
    if (service_context->read_duplicate_count != 0 &&
        uvm_page_mask_and(caller_page_mask,
                          new_residency_mask,
                          &service_context->read_duplicate_mask)) {
        status = uvm_va_block_make_resident_read_duplicate(va_block,
                                                           block_retry,
                                                           service_context->block_context,
                                                           new_residency,
                                                           service_context->region,
                                                           caller_page_mask,
                                                           &service_context->prefetch_hint.prefetch_pages_mask,
                                                           cause);
        if (status != NV_OK)
            return status;
    }

    if (service_context->read_duplicate_count == 0 ||
        uvm_page_mask_andnot(caller_page_mask, new_residency_mask, &service_context->read_duplicate_mask)) {
        if (service_context->read_duplicate_count == 0)
            uvm_page_mask_copy(caller_page_mask, new_residency_mask);
        status = uvm_va_block_make_resident_copy(va_block,
                                                 block_retry,
                                                 service_context->block_context,
                                                 new_residency,
                                                 service_context->region,
                                                 caller_page_mask,
                                                 &service_context->prefetch_hint.prefetch_pages_mask,
                                                 cause);
        if (status != NV_OK)
            return status;
    }

    if (UVM_ID_IS_CPU(processor_id) && !uvm_processor_mask_empty(all_involved_processors))
        service_context->cpu_fault.did_migrate = true;

    // 2- Check for ECC errors on all GPUs involved in the migration if CPU is
    //    the destination. Migrations in response to CPU faults are special
    //    because they're on the only path (apart from tools) where CUDA is not
    //    involved and wouldn't have a chance to do its own ECC checking.
    if (service_context->operation == UVM_SERVICE_OPERATION_REPLAYABLE_FAULTS &&
        UVM_ID_IS_CPU(new_residency) &&
        !uvm_processor_mask_empty(all_involved_processors)) {
        uvm_gpu_t *gpu;

        // Before checking for ECC errors, make sure all of the GPU work
        // is finished. Creating mappings on the CPU would have to wait
        // for the tracker anyway so this shouldn't hurt performance.
        status = uvm_tracker_wait(&va_block->tracker);
        if (status != NV_OK)
            return status;

        for_each_va_space_gpu_in_mask(gpu, va_space, all_involved_processors) {
            // We cannot call into RM here so use the no RM ECC check.
            status = uvm_gpu_check_ecc_error_no_rm(gpu);
            if (status == NV_WARN_MORE_PROCESSING_REQUIRED) {
                // In case we need to call into RM to be sure whether
                // there is an ECC error or not, signal that to the
                // caller by adding the GPU to the mask.
                //
                // In that case the ECC error might be noticed only after
                // the CPU mappings have been already created below,
                // exposing different CPU threads to the possibly corrupt
                // data, but this thread will fault eventually and that's
                // considered to be an acceptable trade-off between
                // performance and ECC error containment.
                uvm_processor_mask_set(&service_context->cpu_fault.gpus_to_check_for_ecc, gpu->id);
                status = NV_OK;
            }
            if (status != NV_OK)
                return status;
        }
    }

    return NV_OK;
}

NV_STATUS uvm_va_block_service_finish(uvm_processor_id_t processor_id,
                                      uvm_va_block_t *va_block,
                                      uvm_service_block_context_t *service_context)
{
    uvm_processor_id_t new_residency = service_context->block_context->make_resident.dest_id;
    uvm_page_mask_t *new_residency_mask =
        &service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency;
    uvm_page_mask_t *did_migrate_mask = &service_context->block_context->make_resident.pages_changed_residency;
    uvm_page_mask_t *caller_page_mask = &service_context->block_context->caller_page_mask;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_prot_t new_prot;
    uvm_page_index_t page_index;
    uvm_processor_mask_t *revoke_processors;
    NV_STATUS status = NV_OK;

    // Update residency.
    if (service_context->read_duplicate_count == 0 || !uvm_page_mask_empty(caller_page_mask))
        uvm_va_block_make_resident_finish(va_block,
                                          service_context->block_context,
                                          service_context->region,
                                          caller_page_mask);

    uvm_page_mask_andnot(&service_context->did_not_migrate_mask, new_residency_mask, did_migrate_mask);

    // The loops below depend on the enums having the following values in order
    // to index into service_context->mappings_by_prot[].
    BUILD_BUG_ON(UVM_PROT_READ_ONLY != 1);
    BUILD_BUG_ON(UVM_PROT_READ_WRITE != 2);
    BUILD_BUG_ON(UVM_PROT_READ_WRITE_ATOMIC != 3);
    BUILD_BUG_ON(UVM_PROT_MAX != 4);

    revoke_processors = uvm_processor_mask_cache_alloc();
    if (!revoke_processors)
        return NV_ERR_NO_MEMORY;

    // 1- Compute mapping protections for the requesting processor on the new
    // residency.
    for (new_prot = UVM_PROT_READ_ONLY; new_prot < UVM_PROT_MAX; ++new_prot)
        service_context->mappings_by_prot[new_prot - 1].count = 0;

    for_each_va_block_page_in_region_mask(page_index, new_residency_mask, service_context->region) {
        new_prot = compute_new_permission(va_block,
                                          service_context->block_context,
                                          page_index,
                                          processor_id,
                                          new_residency,
                                          service_context->access_type[page_index]);

        if (service_context->mappings_by_prot[new_prot - 1].count++ == 0)
            uvm_page_mask_zero(&service_context->mappings_by_prot[new_prot - 1].page_mask);

        uvm_page_mask_set(&service_context->mappings_by_prot[new_prot - 1].page_mask, page_index);
    }

    // 2- Revoke permissions
    //
    // NOTE: uvm_va_block_make_resident_copy destroys mappings to old locations.
    //       Thus, we need to revoke only if residency did not change and we
    //       are mapping higher than READ ONLY.
    for (new_prot = UVM_PROT_READ_WRITE; new_prot <= UVM_PROT_READ_WRITE_ATOMIC; ++new_prot) {
        bool pages_need_revocation;
        uvm_prot_t revoke_prot;
        bool this_processor_has_enabled_atomics;

        if (service_context->mappings_by_prot[new_prot - 1].count == 0)
            continue;

        pages_need_revocation = uvm_page_mask_and(&service_context->revocation_mask,
                                                  &service_context->did_not_migrate_mask,
                                                  &service_context->mappings_by_prot[new_prot - 1].page_mask);
        if (!pages_need_revocation)
            continue;

        uvm_processor_mask_and(revoke_processors, &va_block->mapped, &va_space->faultable_processors);

        // Do not revoke the processor that took the fault
        uvm_processor_mask_clear(revoke_processors, processor_id);

        this_processor_has_enabled_atomics = uvm_processor_mask_test(&va_space->system_wide_atomics_enabled_processors,
                                                                     processor_id);

        // Atomic operations on processors with system-wide atomics
        // disabled or with native atomics access to new_residency
        // behave like writes.
        if (new_prot == UVM_PROT_READ_WRITE ||
            !this_processor_has_enabled_atomics ||
            uvm_processor_mask_test(&va_space->has_native_atomics[uvm_id_value(new_residency)], processor_id)) {

            // Exclude processors with native atomics on the resident copy
            uvm_processor_mask_andnot(revoke_processors,
                                      revoke_processors,
                                      &va_space->has_native_atomics[uvm_id_value(new_residency)]);

            // Exclude processors with disabled system-wide atomics
            uvm_processor_mask_and(revoke_processors,
                                   revoke_processors,
                                   &va_space->system_wide_atomics_enabled_processors);
        }

        if (UVM_ID_IS_CPU(processor_id)) {
            revoke_prot = UVM_PROT_READ_WRITE_ATOMIC;
        }
        else {
            revoke_prot = (new_prot == UVM_PROT_READ_WRITE_ATOMIC)? UVM_PROT_READ_WRITE:
                                                                    UVM_PROT_READ_WRITE_ATOMIC;
        }

        // UVM-Lite processors must always have RWA mappings
        if (uvm_processor_mask_andnot(revoke_processors, revoke_processors, block_get_uvm_lite_gpus(va_block))) {
            // Access counters should never trigger revocations apart from
            // read-duplication, which are performed in the calls to
            // uvm_va_block_make_resident_read_duplicate, above.
            if (service_context->operation == UVM_SERVICE_OPERATION_ACCESS_COUNTERS) {
                UVM_ASSERT(check_access_counters_dont_revoke(va_block,
                                                             service_context->block_context,
                                                             service_context->region,
                                                             revoke_processors,
                                                             &service_context->revocation_mask,
                                                             revoke_prot));
            }

            // Downgrade other processors' mappings
            status = uvm_va_block_revoke_prot_mask(va_block,
                                                   service_context->block_context,
                                                   revoke_processors,
                                                   service_context->region,
                                                   &service_context->revocation_mask,
                                                   revoke_prot);
            if (status != NV_OK)
                break;
        }
    }

    uvm_processor_mask_cache_free(revoke_processors);

    if (status != NV_OK)
        return status;

    // 3- Map requesting processor with the necessary privileges
    for (new_prot = UVM_PROT_READ_ONLY; new_prot <= UVM_PROT_READ_WRITE_ATOMIC; ++new_prot) {
        const uvm_page_mask_t *map_prot_mask = &service_context->mappings_by_prot[new_prot - 1].page_mask;

        if (service_context->mappings_by_prot[new_prot - 1].count == 0)
            continue;

        // 3.1 - Unmap CPU pages
        // HMM cpu mappings can be upgraded at any time without notification
        // so no need to downgrade first.
        if (service_context->operation != UVM_SERVICE_OPERATION_ACCESS_COUNTERS &&
            UVM_ID_IS_CPU(processor_id) &&
            !uvm_va_block_is_hmm(va_block)) {
            // The kernel can downgrade managed CPU mappings at any time without
            // notifying us, which means our PTE state could be stale. We
            // handle this by unmapping the CPU PTE and re-mapping it again.
            //
            // A CPU fault is unexpected if:
            // curr_prot == RW || (!is_write && curr_prot == RO)
            status = uvm_va_block_unmap(va_block,
                                        service_context->block_context,
                                        UVM_ID_CPU,
                                        service_context->region,
                                        map_prot_mask,
                                        NULL);
            if (status != NV_OK)
                return status;
        }

        // 3.2 - Add new mappings

        // The faulting processor can be mapped remotely due to user policy or
        // the thrashing mitigation heuristics. Therefore, we set the cause
        // accordingly in each case.

        // Map pages that are thrashing first
        if (service_context->thrashing_pin_count > 0 && va_space->tools.enabled) {
            uvm_page_mask_t *helper_page_mask = &service_context->block_context->caller_page_mask;
            bool pages_need_mapping = uvm_page_mask_and(helper_page_mask,
                                                        map_prot_mask,
                                                        &service_context->thrashing_pin_mask);
            if (pages_need_mapping) {
                status = uvm_va_block_map(va_block,
                                          service_context->block_context,
                                          processor_id,
                                          service_context->region,
                                          helper_page_mask,
                                          new_prot,
                                          UvmEventMapRemoteCauseThrashing,
                                          &va_block->tracker);
                if (status != NV_OK)
                    return status;

                // Remove thrashing pages from the map mask
                pages_need_mapping = uvm_page_mask_andnot(helper_page_mask,
                                                          map_prot_mask,
                                                          &service_context->thrashing_pin_mask);
                if (!pages_need_mapping)
                    continue;

                map_prot_mask = helper_page_mask;
            }
        }

        status = uvm_va_block_map(va_block,
                                  service_context->block_context,
                                  processor_id,
                                  service_context->region,
                                  map_prot_mask,
                                  new_prot,
                                  UvmEventMapRemoteCausePolicy,
                                  &va_block->tracker);
        if (status != NV_OK)
            return status;
    }

    // 4- If pages did migrate, map SetAccessedBy processors, except for
    // UVM-Lite
    for (new_prot = UVM_PROT_READ_ONLY; new_prot <= UVM_PROT_READ_WRITE_ATOMIC; ++new_prot) {
        bool pages_need_mapping;

        if (service_context->mappings_by_prot[new_prot - 1].count == 0)
            continue;

        pages_need_mapping = uvm_page_mask_and(caller_page_mask,
                                               new_residency_mask,
                                               &service_context->mappings_by_prot[new_prot - 1].page_mask);
        if (!pages_need_mapping)
            continue;

        // Map pages that are thrashing
        if (service_context->thrashing_pin_count > 0) {
            uvm_page_index_t page_index;

            for_each_va_block_page_in_region_mask(page_index,
                                                  &service_context->thrashing_pin_mask,
                                                  service_context->region) {
                uvm_processor_mask_t *map_thrashing_processors = NULL;
                NvU64 page_addr = uvm_va_block_cpu_page_address(va_block, page_index);

                // Check protection type
                if (!uvm_page_mask_test(caller_page_mask, page_index))
                    continue;

                map_thrashing_processors = uvm_perf_thrashing_get_thrashing_processors(va_block, page_addr);

                status = uvm_va_block_add_mappings_after_migration(va_block,
                                                                   service_context->block_context,
                                                                   new_residency,
                                                                   processor_id,
                                                                   uvm_va_block_region_for_page(page_index),
                                                                   caller_page_mask,
                                                                   new_prot,
                                                                   map_thrashing_processors);
                if (status != NV_OK)
                    return status;
            }

            pages_need_mapping = uvm_page_mask_andnot(caller_page_mask,
                                                      caller_page_mask,
                                                      &service_context->thrashing_pin_mask);
            if (!pages_need_mapping)
                continue;
        }

        // Map the rest of pages in a single shot
        status = uvm_va_block_add_mappings_after_migration(va_block,
                                                           service_context->block_context,
                                                           new_residency,
                                                           processor_id,
                                                           service_context->region,
                                                           caller_page_mask,
                                                           new_prot,
                                                           NULL);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

NV_STATUS uvm_va_block_service_locked(uvm_processor_id_t processor_id,
                                      uvm_va_block_t *va_block,
                                      uvm_va_block_retry_t *block_retry,
                                      uvm_service_block_context_t *service_context)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_processor_id_t new_residency;
    NV_STATUS status = NV_OK;

    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(uvm_hmm_check_context_vma_is_valid(va_block,
                                                  service_context->block_context->hmm.vma,
                                                  service_context->region));

    // GPU fault servicing must be done under the VA space read lock. GPU fault
    // servicing is required for RM to make forward progress, and we allow other
    // threads to call into RM while holding the VA space lock in read mode. If
    // we took the VA space lock in write mode on the GPU fault service path,
    // we could deadlock because the thread in RM which holds the VA space lock
    // for read wouldn't be able to complete until fault servicing completes.
    if (service_context->operation != UVM_SERVICE_OPERATION_REPLAYABLE_FAULTS || UVM_ID_IS_CPU(processor_id))
        uvm_assert_rwsem_locked(&va_space->lock);
    else
        uvm_assert_rwsem_locked_read(&va_space->lock);

    uvm_va_block_get_prefetch_hint(va_block,
                                   uvm_va_policy_get_region(va_block, service_context->region),
                                   service_context);

    for_each_id_in_mask(new_residency, &service_context->resident_processors) {
        if (uvm_va_block_is_hmm(va_block)) {
            status = uvm_hmm_va_block_service_locked(processor_id,
                                                     new_residency,
                                                     va_block,
                                                     block_retry,
                                                     service_context);
            if (status != NV_OK)
                break;

            continue;
        }

        status = uvm_va_block_service_copy(processor_id, new_residency, va_block, block_retry, service_context);
        if (status != NV_OK)
            break;

        status = uvm_va_block_service_finish(processor_id, va_block, service_context);
        if (status != NV_OK)
            break;
    }

    return status;
}

NV_STATUS uvm_va_block_check_logical_permissions(uvm_va_block_t *va_block,
                                                 uvm_va_block_context_t *va_block_context,
                                                 uvm_processor_id_t processor_id,
                                                 uvm_page_index_t page_index,
                                                 uvm_fault_access_type_t access_type,
                                                 bool allow_migration)
{
    uvm_va_range_t *va_range = va_block->va_range;
    uvm_prot_t access_prot = uvm_fault_access_type_to_prot(access_type);

    UVM_ASSERT(uvm_hmm_check_context_vma_is_valid(va_block,
                                                  va_block_context->hmm.vma,
                                                  uvm_va_block_region_for_page(page_index)));

    // CPU permissions are checked later by block_map_cpu_page.
    //
    // TODO: Bug 1766124: permissions are checked by block_map_cpu_page because
    //       it can also be called from change_pte. Make change_pte call this
    //       function and only check CPU permissions here.
    if (UVM_ID_IS_GPU(processor_id)) {
        if (va_range && uvm_va_range_is_managed_zombie(va_range))
            return NV_ERR_INVALID_ADDRESS;

        // GPU faults only check vma permissions if a mm is registered with the
        // VA space (ie. uvm_va_space_mm_retain_lock(va_space) != NULL) or if
        // uvm_enable_builtin_tests is set, because the Linux kernel can change
        // vm_flags at any moment (for example on mprotect) and here we are not
        // guaranteed to have vma->vm_mm->mmap_lock. During tests we ensure that
        // this scenario does not happen.
        if (((va_block->hmm.va_space && va_block->hmm.va_space->va_space_mm.mm) || uvm_enable_builtin_tests) &&
            (access_prot > compute_logical_prot(va_block, va_block_context->hmm.vma, page_index)))
            return NV_ERR_INVALID_ACCESS_TYPE;
    }

    // Non-migratable range:
    // - CPU accesses are always fatal, regardless of the VA range residency
    // - GPU accesses are fatal if the GPU can't map the preferred location
    if (!allow_migration) {
        UVM_ASSERT(!uvm_va_block_is_hmm(va_block));

        if (UVM_ID_IS_CPU(processor_id)) {
            return NV_ERR_INVALID_OPERATION;
        }
        else {
            uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);

            return uvm_processor_mask_test(
                    &va_space->accessible_from[uvm_id_value(uvm_va_range_get_policy(va_range)->preferred_location)],
                    processor_id)?
                NV_OK : NV_ERR_INVALID_ACCESS_TYPE;
        }
    }

    return NV_OK;
}

// Check if we are faulting on a page with valid permissions to check if we can
// skip fault handling. See uvm_va_block_t::cpu::fault_authorized for more
// details
static bool skip_cpu_fault_with_valid_permissions(uvm_va_block_t *va_block,
                                                  uvm_page_index_t page_index,
                                                  uvm_fault_access_type_t fault_access_type)
{
    // TODO: Bug 3900038: is skip_cpu_fault_with_valid_permissions() needed for
    // HMM?
    if (uvm_va_block_is_hmm(va_block))
        return false;

    if (block_page_is_processor_authorized(va_block,
                                           page_index,
                                           UVM_ID_CPU,
                                           uvm_fault_access_type_to_prot(fault_access_type))) {
        NvU64 now = NV_GETTIME();
        pid_t pid = current->pid;

        // Latch the pid/timestamp/page_index values for the first time
        if (!va_block->cpu.fault_authorized.first_fault_stamp) {
            va_block->cpu.fault_authorized.first_fault_stamp = now;
            va_block->cpu.fault_authorized.first_pid = pid;
            va_block->cpu.fault_authorized.page_index = page_index;

            return true;
        }

        // If the same thread shows up again, this means that the kernel
        // downgraded the page's PTEs. Service the fault to force a remap of
        // the page.
        if (va_block->cpu.fault_authorized.first_pid == pid &&
            va_block->cpu.fault_authorized.page_index == page_index) {
            va_block->cpu.fault_authorized.first_fault_stamp = 0;
        }
        else {
            // If the window has expired, clear the information and service the
            // fault. Otherwise, just return
            if (now - va_block->cpu.fault_authorized.first_fault_stamp > uvm_perf_authorized_cpu_fault_tracking_window_ns)
                va_block->cpu.fault_authorized.first_fault_stamp = 0;
            else
                return true;
        }
    }

    return false;
}

static NV_STATUS block_cpu_fault_locked(uvm_va_block_t *va_block,
                                        uvm_va_block_retry_t *va_block_retry,
                                        NvU64 fault_addr,
                                        uvm_fault_access_type_t fault_access_type,
                                        uvm_service_block_context_t *service_context)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    NV_STATUS status = NV_OK;
    uvm_page_index_t page_index;
    uvm_perf_thrashing_hint_t thrashing_hint;
    uvm_processor_id_t new_residency;
    bool read_duplicate;
    const uvm_va_policy_t *policy;
    const bool hmm_migratable = true;

    uvm_assert_rwsem_locked(&va_space->lock);

    UVM_ASSERT(fault_addr >= va_block->start);
    UVM_ASSERT(fault_addr <= va_block->end);

    uvm_assert_mmap_lock_locked(service_context->block_context->mm);

    policy = uvm_va_policy_get(va_block, fault_addr);

    if (service_context->num_retries == 0) {
        // notify event to tools/performance heuristics
        uvm_perf_event_notify_cpu_fault(&va_space->perf_events,
                                        va_block,
                                        policy->preferred_location,
                                        fault_addr,
                                        fault_access_type > UVM_FAULT_ACCESS_TYPE_READ,
                                        KSTK_EIP(current));
    }

    // Check logical permissions
    page_index = uvm_va_block_cpu_page_index(va_block, fault_addr);
    status = uvm_va_block_check_logical_permissions(va_block,
                                                    service_context->block_context,
                                                    UVM_ID_CPU,
                                                    page_index,
                                                    fault_access_type,
                                                    uvm_range_group_address_migratable(va_space, fault_addr));
    if (status != NV_OK)
        return status;

    uvm_processor_mask_zero(&service_context->cpu_fault.gpus_to_check_for_ecc);

    if (skip_cpu_fault_with_valid_permissions(va_block, page_index, fault_access_type))
        return NV_OK;

    thrashing_hint = uvm_perf_thrashing_get_hint(va_block, service_context->block_context, fault_addr, UVM_ID_CPU);
    // Throttling is implemented by sleeping in the fault handler on the CPU
    if (thrashing_hint.type == UVM_PERF_THRASHING_HINT_TYPE_THROTTLE) {
        service_context->cpu_fault.wakeup_time_stamp = thrashing_hint.throttle.end_time_stamp;
        return NV_WARN_MORE_PROCESSING_REQUIRED;
    }

    service_context->read_duplicate_count = 0;
    service_context->thrashing_pin_count = 0;
    service_context->operation = UVM_SERVICE_OPERATION_REPLAYABLE_FAULTS;

    if (thrashing_hint.type == UVM_PERF_THRASHING_HINT_TYPE_PIN) {
        uvm_page_mask_zero(&service_context->thrashing_pin_mask);
        uvm_page_mask_set(&service_context->thrashing_pin_mask, page_index);
        service_context->thrashing_pin_count = 1;
    }

    // Compute new residency and update the masks
    new_residency = uvm_va_block_select_residency(va_block,
                                                  service_context->block_context,
                                                  page_index,
                                                  UVM_ID_CPU,
                                                  uvm_fault_access_type_mask_bit(fault_access_type),
                                                  policy,
                                                  &thrashing_hint,
                                                  UVM_SERVICE_OPERATION_REPLAYABLE_FAULTS,
                                                  hmm_migratable,
                                                  &read_duplicate);

    // Initialize the minimum necessary state in the fault service context
    uvm_processor_mask_zero(&service_context->resident_processors);

    // Set new residency and update the masks
    uvm_processor_mask_set(&service_context->resident_processors, new_residency);

    // The masks need to be fully zeroed as the fault region may grow due to prefetching
    uvm_page_mask_zero(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency);
    uvm_page_mask_set(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency, page_index);

    if (read_duplicate) {
        uvm_page_mask_zero(&service_context->read_duplicate_mask);
        uvm_page_mask_set(&service_context->read_duplicate_mask, page_index);
        service_context->read_duplicate_count = 1;
    }

    service_context->access_type[page_index] = fault_access_type;

    service_context->region = uvm_va_block_region_for_page(page_index);

    status = uvm_va_block_service_locked(UVM_ID_CPU, va_block, va_block_retry, service_context);
    UVM_ASSERT(status != NV_WARN_MISMATCHED_TARGET);

    ++service_context->num_retries;

    return status;
}

NV_STATUS uvm_va_block_cpu_fault(uvm_va_block_t *va_block,
                                 NvU64 fault_addr,
                                 bool is_write,
                                 uvm_service_block_context_t *service_context)
{
    NV_STATUS status;
    uvm_va_block_retry_t va_block_retry;
    uvm_fault_access_type_t fault_access_type;

    if (is_write)
        fault_access_type = UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG;
    else
        fault_access_type = UVM_FAULT_ACCESS_TYPE_READ;

    service_context->num_retries = 0;
    service_context->cpu_fault.did_migrate = false;

    // We have to use vm_insert_page instead of handing the page to the kernel
    // and letting it insert the mapping, and we must do that while holding the
    // lock on this VA block. Otherwise there will be a window in which we think
    // we've mapped the page but the CPU mapping hasn't actually been created
    // yet. During that window a GPU fault event could arrive and claim
    // ownership of that VA, "unmapping" it. Then later the kernel would
    // eventually establish the mapping, and we'd end up with both CPU and GPU
    // thinking they each owned the page.
    //
    // This function must only be called when it's safe to call vm_insert_page.
    // That is, there must be a reference held on the vma's vm_mm, and
    // vm_mm->mmap_lock is held in at least read mode. Note that current->mm
    // might not be vma->vm_mm.
    status = UVM_VA_BLOCK_LOCK_RETRY(va_block,
                                     &va_block_retry,
                                     block_cpu_fault_locked(va_block,
                                                            &va_block_retry,
                                                            fault_addr,
                                                            fault_access_type,
                                                            service_context));
    return status;
}

NV_STATUS uvm_va_block_find(uvm_va_space_t *va_space, NvU64 addr, uvm_va_block_t **out_block)
{
    uvm_va_range_t *va_range;
    uvm_va_block_t *block;
    size_t index;

    va_range = uvm_va_range_find(va_space, addr);
    if (!va_range)
        return uvm_hmm_va_block_find(va_space, addr, out_block);

    UVM_ASSERT(uvm_hmm_va_block_find(va_space, addr, out_block) == NV_ERR_INVALID_ADDRESS ||
               uvm_hmm_va_block_find(va_space, addr, out_block) == NV_ERR_OBJECT_NOT_FOUND);

    if (va_range->type != UVM_VA_RANGE_TYPE_MANAGED)
        return NV_ERR_INVALID_ADDRESS;

    index = uvm_va_range_block_index(va_range, addr);
    block = uvm_va_range_block(va_range, index);
    if (!block)
        return NV_ERR_OBJECT_NOT_FOUND;

    *out_block = block;
    return NV_OK;
}

NV_STATUS uvm_va_block_find_create_in_range(uvm_va_space_t *va_space,
                                            uvm_va_range_t *va_range,
                                            NvU64 addr,
                                            uvm_va_block_t **out_block)
{
    size_t index;

    if (uvm_enable_builtin_tests && atomic_dec_if_positive(&va_space->test.va_block_allocation_fail_nth) == 0)
        return NV_ERR_NO_MEMORY;

    UVM_ASSERT(va_range);
    UVM_ASSERT(addr >= va_range->node.start);
    UVM_ASSERT(addr <= va_range->node.end);

    UVM_ASSERT(uvm_hmm_va_block_find(va_space, addr, out_block) == NV_ERR_INVALID_ADDRESS ||
               uvm_hmm_va_block_find(va_space, addr, out_block) == NV_ERR_OBJECT_NOT_FOUND);

    if (va_range->type != UVM_VA_RANGE_TYPE_MANAGED)
        return NV_ERR_INVALID_ADDRESS;

    index = uvm_va_range_block_index(va_range, addr);
    return uvm_va_range_block_create(va_range, index, out_block);
}

NV_STATUS uvm_va_block_find_create_managed(uvm_va_space_t *va_space,
                                   NvU64 addr,
                                   uvm_va_block_t **out_block)
{
    uvm_va_range_t *va_range = uvm_va_range_find(va_space, addr);

    if (va_range)
        return uvm_va_block_find_create_in_range(va_space, va_range, addr, out_block);
    else
        return NV_ERR_INVALID_ADDRESS;
}

NV_STATUS uvm_va_block_find_create(uvm_va_space_t *va_space,
                                   NvU64 addr,
                                   struct vm_area_struct **hmm_vma,
                                   uvm_va_block_t **out_block)
{
    uvm_va_range_t *va_range = uvm_va_range_find(va_space, addr);

    if (hmm_vma)
        *hmm_vma = NULL;

    if (va_range)
        return uvm_va_block_find_create_in_range(va_space, va_range, addr, out_block);
    else
        return uvm_hmm_va_block_find_create(va_space, addr, hmm_vma, out_block);
}

// Launch a synchronous, encrypted copy between GPU and CPU.
//
// The copy entails a GPU-side encryption (relying on the Copy Engine), and a
// CPU-side decryption step, such that the destination CPU buffer pointed by
// dst_plain will contain the unencrypted (plain text) contents. The destination
// buffer can be in protected or unprotected sysmem, while the source buffer
// must be in protected vidmem.
//
// The maximum copy size allowed is UVM_CONF_COMPUTING_DMA_BUFFER_SIZE.
//
// The input tracker, if not NULL, is internally acquired by the push
// responsible for the encrypted copy.
__attribute__ ((format(printf, 6, 7)))
static NV_STATUS encrypted_memcopy_gpu_to_cpu(uvm_gpu_t *gpu,
                                              void *dst_plain,
                                              uvm_gpu_address_t src_gpu_address,
                                              size_t size,
                                              uvm_tracker_t *tracker,
                                              const char *format,
                                              ...)
{
    NV_STATUS status;
    UvmCslIv decrypt_iv;
    uvm_push_t push;
    uvm_conf_computing_dma_buffer_t *dma_buffer;
    uvm_gpu_address_t dst_gpu_address, auth_tag_gpu_address;
    void *src_cipher, *auth_tag;
    va_list args;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(size <= UVM_CONF_COMPUTING_DMA_BUFFER_SIZE);

    status = uvm_conf_computing_dma_buffer_alloc(&gpu->conf_computing.dma_buffer_pool, &dma_buffer, NULL);
    if (status != NV_OK)
        return status;

    va_start(args, format);
    status = uvm_push_begin_acquire(gpu->channel_manager, UVM_CHANNEL_TYPE_GPU_TO_CPU, tracker, &push, format, args);
    va_end(args);

    if (status != NV_OK)
        goto out;

    uvm_conf_computing_log_gpu_encryption(push.channel, &decrypt_iv);

    dst_gpu_address = uvm_mem_gpu_address_virtual_kernel(dma_buffer->alloc, gpu);
    auth_tag_gpu_address = uvm_mem_gpu_address_virtual_kernel(dma_buffer->auth_tag, gpu);
    gpu->parent->ce_hal->encrypt(&push, dst_gpu_address, src_gpu_address, size, auth_tag_gpu_address);

    status = uvm_push_end_and_wait(&push);
    if (status != NV_OK)
        goto out;

    src_cipher = uvm_mem_get_cpu_addr_kernel(dma_buffer->alloc);
    auth_tag = uvm_mem_get_cpu_addr_kernel(dma_buffer->auth_tag);
    status = uvm_conf_computing_cpu_decrypt(push.channel, dst_plain, src_cipher, &decrypt_iv, size, auth_tag);

 out:
    uvm_conf_computing_dma_buffer_free(&gpu->conf_computing.dma_buffer_pool, dma_buffer, NULL);
    return status;
}

// Launch a synchronous, encrypted copy between CPU and GPU.
//
// The source CPU buffer pointed by src_plain contains the unencrypted (plain
// text) contents; the function internally performs a CPU-side encryption step
// before launching the GPU-side CE decryption. The source buffer can be in
// protected or unprotected sysmem, while the destination buffer must be in
// protected vidmem.
//
// The maximum copy size allowed is UVM_CONF_COMPUTING_DMA_BUFFER_SIZE.
//
// The input tracker, if not NULL, is internally acquired by the push
// responsible for the encrypted copy.
__attribute__ ((format(printf, 6, 7)))
static NV_STATUS encrypted_memcopy_cpu_to_gpu(uvm_gpu_t *gpu,
                                              uvm_gpu_address_t dst_gpu_address,
                                              void *src_plain,
                                              size_t size,
                                              uvm_tracker_t *tracker,
                                              const char *format,
                                              ...)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_conf_computing_dma_buffer_t *dma_buffer;
    uvm_gpu_address_t src_gpu_address, auth_tag_gpu_address;
    void *dst_cipher, *auth_tag;
    va_list args;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(size <= UVM_CONF_COMPUTING_DMA_BUFFER_SIZE);

    status = uvm_conf_computing_dma_buffer_alloc(&gpu->conf_computing.dma_buffer_pool, &dma_buffer, NULL);
    if (status != NV_OK)
        return status;

    va_start(args, format);
    status = uvm_push_begin_acquire(gpu->channel_manager, UVM_CHANNEL_TYPE_CPU_TO_GPU, tracker, &push, format, args);
    va_end(args);

    if (status != NV_OK)
        goto out;

    dst_cipher = uvm_mem_get_cpu_addr_kernel(dma_buffer->alloc);
    auth_tag = uvm_mem_get_cpu_addr_kernel(dma_buffer->auth_tag);
    uvm_conf_computing_cpu_encrypt(push.channel, dst_cipher, src_plain, NULL, size, auth_tag);

    src_gpu_address = uvm_mem_gpu_address_virtual_kernel(dma_buffer->alloc, gpu);
    auth_tag_gpu_address = uvm_mem_gpu_address_virtual_kernel(dma_buffer->auth_tag, gpu);
    gpu->parent->ce_hal->decrypt(&push, dst_gpu_address, src_gpu_address, size, auth_tag_gpu_address);

    status = uvm_push_end_and_wait(&push);

out:
    uvm_conf_computing_dma_buffer_free(&gpu->conf_computing.dma_buffer_pool, dma_buffer, NULL);
    return status;
}

static NV_STATUS va_block_write_cpu_to_gpu(uvm_va_block_t *va_block,
                                           uvm_gpu_t *gpu,
                                           uvm_gpu_address_t dst_gpu_address,
                                           NvU64 dst,
                                           uvm_mem_t *src_mem,
                                           size_t size)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_gpu_address_t src_gpu_address;

    if (g_uvm_global.conf_computing_enabled) {
        return encrypted_memcopy_cpu_to_gpu(gpu,
                                            dst_gpu_address,
                                            uvm_mem_get_cpu_addr_kernel(src_mem),
                                            size,
                                            &va_block->tracker,
                                            "Encrypted write to [0x%llx, 0x%llx)",
                                            dst,
                                            dst + size);
    }

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_CPU_TO_GPU,
                                    &va_block->tracker,
                                    &push,
                                    "Direct write to [0x%llx, 0x%llx)",
                                    dst,
                                    dst + size);
    if (status != NV_OK)
        return status;

    src_gpu_address = uvm_mem_gpu_address_virtual_kernel(src_mem, gpu);
    gpu->parent->ce_hal->memcopy(&push, dst_gpu_address, src_gpu_address, size);
    return uvm_push_end_and_wait(&push);
}

NV_STATUS uvm_va_block_write_from_cpu(uvm_va_block_t *va_block,
                                      uvm_va_block_context_t *block_context,
                                      NvU64 dst,
                                      uvm_mem_t *src_mem,
                                      size_t size)
{
    NV_STATUS status;
    uvm_page_index_t page_index = uvm_va_block_cpu_page_index(va_block, dst);
    NvU64 page_offset = dst & (PAGE_SIZE - 1);
    uvm_processor_id_t proc = uvm_va_block_page_get_closest_resident(va_block, block_context, page_index, UVM_ID_CPU);
    uvm_va_block_region_t region = uvm_va_block_region_for_page(page_index);

    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT_MSG(page_offset + size <= PAGE_SIZE, "Write spans multiple pages: dst 0x%llx, size 0x%zx\n", dst, size);

    if (UVM_ID_IS_INVALID(proc))
        proc = UVM_ID_CPU;

    // Use make_resident() in all cases to break read-duplication, but
    // block_retry can be NULL as if the page is not resident yet we will make
    // it resident on the CPU.
    // Notably we don't care about coherence with respect to atomics from other
    // processors.
    status = uvm_va_block_make_resident(va_block,
                                        NULL,
                                        block_context,
                                        proc,
                                        region,
                                        NULL,
                                        NULL,
                                        UVM_MAKE_RESIDENT_CAUSE_API_TOOLS);

    if (status != NV_OK)
        return status;

    if (UVM_ID_IS_CPU(proc)) {
        char *mapped_page;
        struct page *page = uvm_va_block_get_cpu_page(va_block, page_index);
        void *src = uvm_mem_get_cpu_addr_kernel(src_mem);

        status = uvm_tracker_wait(&va_block->tracker);
        if (status != NV_OK)
            return status;

        mapped_page = (char *)kmap(page);
        memcpy(mapped_page + page_offset, src, size);
        kunmap(page);

        return NV_OK;
    }
    else {
        uvm_gpu_t *dst_gpu;
        uvm_gpu_address_t dst_gpu_address;

        UVM_ASSERT(UVM_ID_IS_GPU(proc));

        dst_gpu = block_get_gpu(va_block, proc);

        dst_gpu_address = block_phys_page_copy_address(va_block,
                                                       block_phys_page(proc, NUMA_NO_NODE, page_index),
                                                       dst_gpu);
        dst_gpu_address.address += page_offset;

        return va_block_write_cpu_to_gpu(va_block, dst_gpu, dst_gpu_address, dst, src_mem, size);
    }
}

static NV_STATUS va_block_read_gpu_to_cpu(uvm_va_block_t *va_block,
                                          uvm_mem_t *dst_mem,
                                          uvm_gpu_t *gpu,
                                          uvm_gpu_address_t src_gpu_address,
                                          NvU64 src,
                                          size_t size)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_gpu_address_t dst_gpu_address;

    if (g_uvm_global.conf_computing_enabled) {
        return encrypted_memcopy_gpu_to_cpu(gpu,
                                            uvm_mem_get_cpu_addr_kernel(dst_mem),
                                            src_gpu_address,
                                            size,
                                            &va_block->tracker,
                                            "Encrypted read from [0x%llx, 0x%llx)",
                                            src,
                                            src + size);
    }

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_GPU_TO_CPU,
                                    &va_block->tracker,
                                    &push,
                                    "Direct read from [0x%llx, 0x%llx)",
                                    src,
                                    src + size);
    if (status != NV_OK)
        return status;

    dst_gpu_address = uvm_mem_gpu_address_virtual_kernel(dst_mem, gpu);
    gpu->parent->ce_hal->memcopy(&push, dst_gpu_address, src_gpu_address, size);
    return uvm_push_end_and_wait(&push);
}

NV_STATUS uvm_va_block_read_to_cpu(uvm_va_block_t *va_block,
                                   uvm_va_block_context_t *va_block_context,
                                   uvm_mem_t *dst_mem,
                                   NvU64 src,
                                   size_t size)
{
    uvm_page_index_t page_index = uvm_va_block_cpu_page_index(va_block, src);
    NvU64 page_offset = src & (PAGE_SIZE - 1);
    uvm_processor_id_t proc;
    void *dst = uvm_mem_get_cpu_addr_kernel(dst_mem);

    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT_MSG(page_offset + size <= PAGE_SIZE, "Read spans multiple pages: src 0x%llx, size 0x%zx\n", src, size);

    proc = uvm_va_block_page_get_closest_resident(va_block, va_block_context, page_index, UVM_ID_CPU);
    if (UVM_ID_IS_INVALID(proc)) {
        memset(dst, 0, size);
        return NV_OK;
    }
    else if (UVM_ID_IS_CPU(proc)) {
        NV_STATUS status;
        char *mapped_page;
        struct page *page = uvm_va_block_get_cpu_page(va_block, page_index);

        status = uvm_tracker_wait(&va_block->tracker);
        if (status != NV_OK)
            return status;

        mapped_page = (char *)kmap(page);
        memcpy(dst, mapped_page + page_offset, size);
        kunmap(page);

        return NV_OK;
    }
    else {
        uvm_gpu_address_t src_gpu_address;
        uvm_gpu_t *gpu = block_get_gpu(va_block, proc);

        src_gpu_address = block_phys_page_copy_address(va_block,
                                                       block_phys_page(proc, NUMA_NO_NODE, page_index),
                                                       gpu);
        src_gpu_address.address += page_offset;

        return va_block_read_gpu_to_cpu(va_block, dst_mem, gpu, src_gpu_address, src, size);
    }
}

// Deferred work item reestablishing accessed by mappings after eviction. On
// GPUs with access counters enabled, the evicted GPU will also get remote
// mappings.
static void block_add_eviction_mappings(void *args)
{
    uvm_va_block_t *va_block = (uvm_va_block_t*)args;
    uvm_va_space_t *va_space;
    uvm_processor_id_t id;
    uvm_va_block_context_t *block_context = NULL;
    struct mm_struct *mm = NULL;

    uvm_mutex_lock(&va_block->lock);
    va_space = uvm_va_block_get_va_space_maybe_dead(va_block);
    uvm_mutex_unlock(&va_block->lock);

    if (!va_space) {
        // Block has been killed in the meantime
        goto done;
    }

    mm = uvm_va_space_mm_retain_lock(va_space);

    block_context = uvm_va_block_context_alloc(mm);
    if (!block_context)
        goto done;

    // The block wasn't dead when we checked above and that's enough to
    // guarantee that the VA space is still around, because
    // uvm_va_space_destroy() flushes the associated nv_kthread_q, and that
    // flush waits for this function call to finish.
    uvm_va_space_down_read(va_space);

    // Now that we have the VA space lock held, we can check whether the block
    // is still alive since the VA space write lock is needed to kill blocks.
    if (uvm_va_block_is_dead(va_block))
        goto unlock;

    if (uvm_va_block_is_hmm(va_block)) {
        uvm_hmm_block_add_eviction_mappings(va_space, va_block, block_context);
    }
    else {
        uvm_va_range_t *va_range = va_block->va_range;
        NV_STATUS status = NV_OK;

        for_each_id_in_mask(id, &uvm_va_range_get_policy(va_range)->accessed_by) {
            status = uvm_va_block_set_accessed_by(va_block, block_context, id);
            if (status != NV_OK)
                break;
        }

        if (status == NV_OK && uvm_va_space_map_remote_on_eviction(va_space)) {
            uvm_processor_mask_t *map_processors = &block_context->map_processors_eviction;

            // Exclude the processors that have been already mapped due to
            // AccessedBy
            uvm_processor_mask_andnot(map_processors,
                                      &va_block->evicted_gpus,
                                      &uvm_va_range_get_policy(va_range)->accessed_by);

            for_each_gpu_id_in_mask(id, map_processors) {
                uvm_gpu_t *gpu = uvm_va_space_get_gpu(va_space, id);
                uvm_va_block_gpu_state_t *gpu_state;

                if (!gpu->parent->access_counters_supported)
                    continue;

                gpu_state = uvm_va_block_gpu_state_get(va_block, id);
                UVM_ASSERT(gpu_state);

                // TODO: Bug 2096389: uvm_va_block_add_mappings does not add
                // remote mappings to read-duplicated pages. Add support for it
                // or create a new function.
                status = UVM_VA_BLOCK_LOCK_RETRY(va_block, NULL,
                                                 uvm_va_block_add_mappings(va_block,
                                                                           block_context,
                                                                           id,
                                                                           uvm_va_block_region_from_block(va_block),
                                                                           &gpu_state->evicted,
                                                                           UvmEventMapRemoteCauseEviction));
                if (status != NV_OK)
                    break;
            }
        }

        if (status != NV_OK) {
            UVM_ERR_PRINT("Deferred mappings to evicted memory for block [0x%llx, 0x%llx] failed %s, processor %s\n",
                          va_block->start,
                          va_block->end,
                          nvstatusToString(status),
                          uvm_va_space_processor_name(va_space, id));
        }
    }

unlock:
    uvm_va_space_up_read(va_space);
    uvm_va_block_context_free(block_context);

done:
    uvm_va_space_mm_release_unlock(va_space, mm);
    uvm_va_block_release(va_block);
}

static void block_add_eviction_mappings_entry(void *args)
{
    UVM_ENTRY_VOID(block_add_eviction_mappings(args));
}

NV_STATUS uvm_va_block_evict_chunks(uvm_va_block_t *va_block,
                                    uvm_gpu_t *gpu,
                                    uvm_gpu_chunk_t *root_chunk,
                                    uvm_tracker_t *tracker)
{
    NV_STATUS status = NV_OK;
    NvU32 i;
    uvm_va_block_gpu_state_t *gpu_state;
    uvm_va_block_region_t chunk_region;
    size_t num_gpu_chunks = block_num_gpu_chunks(va_block, gpu);
    size_t chunks_to_evict = 0;
    uvm_service_block_context_t *service_context;
    uvm_va_block_context_t *block_context;
    uvm_page_mask_t *pages_to_evict;
    uvm_va_block_test_t *va_block_test = uvm_va_block_get_test(va_block);
    uvm_va_space_t *va_space = uvm_va_block_get_va_space_maybe_dead(va_block);
    struct mm_struct *mm;
    bool accessed_by_set = false;

    uvm_assert_mutex_locked(&va_block->lock);

    // The block might have been killed in the meantime
    if (!va_space)
        return NV_OK;

    gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    if (!gpu_state)
        return NV_OK;

    if (va_block_test && va_block_test->inject_eviction_error) {
        va_block_test->inject_eviction_error = false;
        return NV_ERR_NO_MEMORY;
    }

    // We cannot take this block's VA space or mmap_lock locks on the eviction
    // path, however, we retain mm in order to support accounting of CPU memory
    // allocations. If mappings need to be created,
    // block_add_eviction_mappings() will be scheduled below.
    mm = uvm_va_space_mm_retain(va_space);

    service_context = uvm_service_block_context_alloc(mm);
    if (!service_context) {
        if (mm)
            uvm_va_space_mm_release(va_space);

        return NV_ERR_NO_MEMORY;
    }

    block_context = service_context->block_context;

    pages_to_evict = &block_context->caller_page_mask;
    uvm_page_mask_zero(pages_to_evict);
    chunk_region.outer = 0;

    // Find all chunks that are subchunks of the root chunk
    for (i = 0; i < num_gpu_chunks; ++i) {
        uvm_chunk_size_t chunk_size;
        size_t chunk_index = block_gpu_chunk_index(va_block, gpu, chunk_region.outer, &chunk_size);
        UVM_ASSERT(chunk_index == i);
        chunk_region.first = chunk_region.outer;
        chunk_region.outer = chunk_region.first + chunk_size / PAGE_SIZE;

        if (!gpu_state->chunks[i])
            continue;
        if (!uvm_gpu_chunk_same_root(gpu_state->chunks[i], root_chunk))
            continue;

        if (uvm_va_block_is_hmm(va_block)) {
            status = uvm_hmm_va_block_evict_chunk_prep(va_block, block_context, gpu_state->chunks[i], chunk_region);
            if (status != NV_OK)
                break;
        }

        uvm_page_mask_region_fill(pages_to_evict, chunk_region);
        ++chunks_to_evict;
    }

    if (chunks_to_evict == 0)
        goto out;

    // Only move pages resident on the GPU
    uvm_page_mask_and(pages_to_evict, pages_to_evict, uvm_va_block_resident_mask_get(va_block, gpu->id, NUMA_NO_NODE));
    uvm_processor_mask_zero(&block_context->make_resident.all_involved_processors);

    if (uvm_va_block_is_hmm(va_block)) {
        status = uvm_hmm_va_block_evict_chunks(va_block,
                                               service_context,
                                               pages_to_evict,
                                               uvm_va_block_region_from_block(va_block),
                                               &accessed_by_set);
    }
    else {
        const uvm_va_policy_t *policy = uvm_va_range_get_policy(va_block->va_range);
        accessed_by_set = uvm_processor_mask_get_count(&policy->accessed_by) > 0;

        // TODO: Bug 1765193: make_resident() breaks read-duplication, but it's
        // not necessary to do so for eviction. Add a version that unmaps only
        // the processors that have mappings to the pages being evicted.
        status = uvm_va_block_make_resident(va_block,
                                            NULL,
                                            block_context,
                                            UVM_ID_CPU,
                                            uvm_va_block_region_from_block(va_block),
                                            pages_to_evict,
                                            NULL,
                                            UVM_MAKE_RESIDENT_CAUSE_EVICTION);
    }
    if (status != NV_OK)
        goto out;

    // VA space lock may not be held and hence we cannot reestablish any
    // mappings here and need to defer it to a work queue.
    //
    // Reading the accessed_by mask without the VA space lock is safe because
    // adding a new processor to the mask triggers going over all the VA blocks
    // in the range and locking them. And we hold one of the VA block's locks.
    //
    // If uvm_va_range_set_accessed_by() hasn't called
    // uvm_va_block_set_accessed_by() for this block yet then it will take care
    // of adding the mapping after we are done. If it already did then we are
    // guaranteed to see the new processor in the accessed_by mask because we
    // locked the block's lock that the thread calling
    // uvm_va_range_set_accessed_by() unlocked after updating the mask.
    //
    // If a processor gets removed from the mask then we might not notice and
    // schedule the work item anyway, but that's benign as
    // block_add_eviction_mappings() re-examines the mask.
    //
    // Checking if access counters migrations are enabled on a VA space is racy
    // without holding the VA space lock. However, this is fine as
    // block_add_eviction_mappings() reexamines the value with the VA space
    // lock being held.
    if (accessed_by_set || (gpu->parent->access_counters_supported && uvm_va_space_map_remote_on_eviction(va_space))) {
        // Always retain the VA block first so that it's safe for the deferred
        // callback to release it immediately after it runs.
        uvm_va_block_retain(va_block);

        if (!nv_kthread_q_schedule_q_item(&g_uvm_global.global_q,
                                          &va_block->eviction_mappings_q_item)) {
            // And release it if no new callback was scheduled
            uvm_va_block_release_no_destroy(va_block);
        }
    }

    status = uvm_tracker_add_tracker_safe(tracker, &va_block->tracker);
    if (status != NV_OK)
        goto out;

    for (i = 0; i < num_gpu_chunks; ++i) {
        uvm_gpu_chunk_t *chunk = gpu_state->chunks[i];

        if (!chunk)
            continue;
        if (!uvm_gpu_chunk_same_root(chunk, root_chunk))
            continue;

        uvm_mmu_chunk_unmap(chunk, tracker);

        uvm_pmm_gpu_mark_chunk_evicted(&gpu->pmm, gpu_state->chunks[i]);
        gpu_state->chunks[i] = NULL;
    }

out:
    uvm_service_block_context_free(service_context);

    if (mm)
        uvm_va_space_mm_release(va_space);

    return status;
}

static NV_STATUS block_gpu_force_4k_ptes(uvm_va_block_t *block, uvm_va_block_context_t *block_context, uvm_gpu_t *gpu)
{
    uvm_va_block_gpu_state_t *gpu_state = block_gpu_state_get_alloc(block, gpu);
    uvm_push_t push;
    NV_STATUS status;

    // See comment in uvm_va_block_set_cancel
    UVM_ASSERT(!gpu->parent->fault_cancel_va_supported);

    if (!gpu_state)
        return NV_ERR_NO_MEMORY;

    // Force all pages to be 4K and prevent future upgrades during cancel
    gpu_state->force_4k_ptes = true;

    // If we have no page tables we're done. For fault cancel we need to make
    // sure that fatal faults are on different 4k PTEs than non-fatal faults,
    // and we need to service all non-fatal faults before issuing the cancel. So
    // either all faults are fatal and we have no PTEs (we're PROT_NONE), or
    // we'll allocate PTEs later when we service the non-fatal faults. Those
    // PTEs will be 4k since force_4k_ptes is set.
    if (!block_gpu_has_page_tables(block, gpu))
        return NV_OK;

    // Are we 4k already?
    if (!gpu_state->pte_is_2m && bitmap_empty(gpu_state->big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK))
        return NV_OK;

    status = block_alloc_ptes_with_retry(block, gpu, UVM_PAGE_SIZE_4K, NULL);
    if (status != NV_OK)
        return status;

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_MEMOPS,
                                    &block->tracker,
                                    &push,
                                    "Forcing 4k PTEs on block [0x%llx, 0x%llx)",
                                    block->start,
                                    block->end + 1);
    if (status != NV_OK)
        return status;

    if (gpu_state->pte_is_2m)
        block_gpu_split_2m(block, block_context, gpu, NULL, &push);
    else
        block_gpu_split_big(block, block_context, gpu, gpu_state->big_ptes, &push);

    uvm_push_end(&push);

    UVM_ASSERT(block_check_mappings(block, block_context));

    return uvm_tracker_add_push_safe(&block->tracker, &push);
}

NV_STATUS uvm_va_block_set_cancel(uvm_va_block_t *va_block, uvm_va_block_context_t *block_context, uvm_gpu_t *gpu)
{
    uvm_assert_mutex_locked(&va_block->lock);

    // Volta+ devices support a global VA cancel method that does not require
    // 4k PTEs. Thus, skip doing this PTE splitting, particularly because it
    // could result in 4k PTEs on P9 systems which otherwise would never need
    // them.
    if (gpu->parent->fault_cancel_va_supported)
        return NV_OK;

    return block_gpu_force_4k_ptes(va_block, block_context, gpu);
}

NV_STATUS uvm_test_va_block_inject_error(UVM_TEST_VA_BLOCK_INJECT_ERROR_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    struct mm_struct *mm;
    uvm_va_block_t *va_block;
    uvm_va_block_test_t *va_block_test;
    NV_STATUS status = NV_OK;

    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_read(va_space);

    if (mm)
        status = uvm_va_block_find_create(va_space, params->lookup_address, NULL, &va_block);
    else
        status = uvm_va_block_find_create_managed(va_space, params->lookup_address, &va_block);

    if (status != NV_OK)
        goto out;

    va_block_test = uvm_va_block_get_test(va_block);
    UVM_ASSERT(va_block_test);

    uvm_mutex_lock(&va_block->lock);

    if (params->page_table_allocation_retry_force_count)
        va_block_test->page_table_allocation_retry_force_count = params->page_table_allocation_retry_force_count;

    if (params->user_pages_allocation_retry_force_count)
        va_block_test->user_pages_allocation_retry_force_count = params->user_pages_allocation_retry_force_count;

    if (params->cpu_chunk_allocation_size_mask) {
        if (params->cpu_chunk_allocation_size_mask & ~UVM_CPU_CHUNK_SIZES ||
            !(params->cpu_chunk_allocation_size_mask & PAGE_SIZE)) {
            status = NV_ERR_INVALID_ARGUMENT;
            goto block_unlock;
        }

        va_block_test->cpu_chunk_allocation_size_mask = params->cpu_chunk_allocation_size_mask & UVM_CPU_CHUNK_SIZES;
    }

    if (params->cpu_chunk_allocation_target_id != NUMA_NO_NODE)
        va_block_test->cpu_chunk_allocation_target_id = params->cpu_chunk_allocation_target_id;

    if (params->cpu_chunk_allocation_actual_id != NUMA_NO_NODE)
        va_block_test->cpu_chunk_allocation_actual_id = params->cpu_chunk_allocation_actual_id;

    if (params->eviction_error)
        va_block_test->inject_eviction_error = params->eviction_error;

    if (params->cpu_chunk_allocation_error_count)
        va_block_test->inject_cpu_chunk_allocation_error_count = params->cpu_chunk_allocation_error_count;

    if (params->populate_error)
        va_block_test->inject_populate_error = params->populate_error;

block_unlock:
    uvm_mutex_unlock(&va_block->lock);

out:
    uvm_va_space_up_read(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);
    return status;
}

static uvm_prot_t g_uvm_test_pte_mapping_to_prot[UVM_TEST_PTE_MAPPING_MAX] =
{
    [UVM_TEST_PTE_MAPPING_INVALID]           = UVM_PROT_NONE,
    [UVM_TEST_PTE_MAPPING_READ_ONLY]         = UVM_PROT_READ_ONLY,
    [UVM_TEST_PTE_MAPPING_READ_WRITE]        = UVM_PROT_READ_WRITE,
    [UVM_TEST_PTE_MAPPING_READ_WRITE_ATOMIC] = UVM_PROT_READ_WRITE_ATOMIC,
};

static UVM_TEST_PTE_MAPPING g_uvm_prot_to_test_pte_mapping[UVM_PROT_MAX] =
{
    [UVM_PROT_NONE]              = UVM_TEST_PTE_MAPPING_INVALID,
    [UVM_PROT_READ_ONLY]         = UVM_TEST_PTE_MAPPING_READ_ONLY,
    [UVM_PROT_READ_WRITE]        = UVM_TEST_PTE_MAPPING_READ_WRITE,
    [UVM_PROT_READ_WRITE_ATOMIC] = UVM_TEST_PTE_MAPPING_READ_WRITE_ATOMIC,
};

NV_STATUS uvm_test_change_pte_mapping(UVM_TEST_CHANGE_PTE_MAPPING_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_block_t *block;
    struct mm_struct *mm;
    NV_STATUS status = NV_OK;
    uvm_prot_t curr_prot, new_prot;
    uvm_gpu_t *gpu = NULL;
    uvm_processor_id_t id;
    uvm_tracker_t local_tracker;
    uvm_va_block_region_t region;
    uvm_va_block_context_t *block_context = NULL;

    if (!PAGE_ALIGNED(params->va))
        return NV_ERR_INVALID_ADDRESS;

    if (params->mapping >= UVM_TEST_PTE_MAPPING_MAX)
        return NV_ERR_INVALID_ARGUMENT;

    new_prot = g_uvm_test_pte_mapping_to_prot[params->mapping];

    // mmap_lock isn't needed for invalidating CPU mappings, but it will be
    // needed for inserting them.
    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_read(va_space);

    if (uvm_uuid_is_cpu(&params->uuid)) {
        id = UVM_ID_CPU;
    }
    else {
        gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->uuid);
        if (!gpu) {
            status = NV_ERR_INVALID_DEVICE;
            goto out;
        }

        // Check if the GPU can access the VA
        if (!uvm_gpu_can_address(gpu, params->va, PAGE_SIZE)) {
            status = NV_ERR_OUT_OF_RANGE;
            goto out;
        }

        id = gpu->id;
    }

    block_context = uvm_va_block_context_alloc(mm);
    if (!block_context) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    if (mm)
        status = uvm_va_block_find_create(va_space, params->va, &block_context->hmm.vma, &block);
    else
        status = uvm_va_block_find_create_managed(va_space, params->va, &block);

    if (status != NV_OK)
        goto out;

    // TODO: Bug 3912902: UvmTestChangePteMapping() doesn't work on CPU.
    if (UVM_ID_IS_CPU(id) && uvm_va_block_is_hmm(block))
        goto out;

    uvm_mutex_lock(&block->lock);

    region = uvm_va_block_region_from_start_size(block, params->va, PAGE_SIZE);
    curr_prot = block_page_prot(block, id, region.first);

    if (new_prot == curr_prot) {
        status = NV_OK;
        goto out_block;
    }

    // TODO: Bug 1766124: Upgrades might require revoking other processors'
    //       access privileges. We just fail for now. Only downgrades are
    //       supported. If we allowed upgrades, we would need to check the mm
    //       like we do for revocation below.
    if (new_prot > curr_prot) {
        status = NV_ERR_INVALID_OPERATION;
        goto out_block;
    }

    if (new_prot == UVM_PROT_NONE) {
        status = uvm_va_block_unmap(block, block_context, id, region, NULL, &block->tracker);
    }
    else {
        UVM_ASSERT(block_is_page_resident_anywhere(block, region.first));

        // Revoking CPU mappings performs a combination of unmap + map. The map
        // portion requires a valid mm.
        if (UVM_ID_IS_CPU(id) && !uvm_va_range_vma_check(block->va_range, mm)) {
            status = NV_ERR_INVALID_STATE;
        }
        else {
            status = uvm_va_block_revoke_prot(block,
                                              block_context,
                                              id,
                                              region,
                                              NULL,
                                              new_prot + 1,
                                              &block->tracker);
        }
    }

out_block:
    if (status == NV_OK)
        status = uvm_tracker_init_from(&local_tracker, &block->tracker);

    uvm_mutex_unlock(&block->lock);

    if (status == NV_OK)
        status = uvm_tracker_wait_deinit(&local_tracker);

out:
    uvm_va_space_up_read(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);

    uvm_va_block_context_free(block_context);

    return status;
}

NV_STATUS uvm_test_va_block_info(UVM_TEST_VA_BLOCK_INFO_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_block_t *va_block;
    uvm_va_range_t *va_range;
    struct mm_struct *mm;
    size_t index;
    NV_STATUS status = NV_OK;

    BUILD_BUG_ON(UVM_TEST_VA_BLOCK_SIZE != UVM_VA_BLOCK_SIZE);

    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_read(va_space);

    va_range = uvm_va_range_find(va_space, params->lookup_address);
    if (!va_range) {
        status = uvm_hmm_va_block_find(va_space, params->lookup_address, &va_block);
        if (status == NV_ERR_OBJECT_NOT_FOUND) {
            status = uvm_hmm_va_block_range_bounds(va_space,
                                                   mm,
                                                   params->lookup_address,
                                                   &params->va_block_start,
                                                   &params->va_block_end,
                                                   NULL);
            goto out;
        }
        else if (status != NV_OK) {
            goto out;
        }
    }
    else {
        index = uvm_va_range_block_index(va_range, params->lookup_address);
        va_block = uvm_va_range_block(va_range, index);
        if (!va_block) {
            status = NV_ERR_OBJECT_NOT_FOUND;
            goto out;
        }
    }

    params->va_block_start = va_block->start;
    params->va_block_end   = va_block->end;

out:
    uvm_va_space_up_read(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);
    return status;
}

NV_STATUS uvm_test_va_residency_info(UVM_TEST_VA_RESIDENCY_INFO_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_OK;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_t *va_range;
    uvm_va_block_t *block = NULL;
    uvm_va_block_context_t *block_context = NULL;
    struct mm_struct *mm;
    NvU32 count = 0;
    uvm_processor_mask_t *resident_on_mask;
    uvm_processor_id_t id;
    uvm_page_index_t page_index;
    unsigned release_block_count = 0;
    NvU64 addr = UVM_ALIGN_DOWN(params->lookup_address, PAGE_SIZE);
    size_t index;

    resident_on_mask = uvm_processor_mask_cache_alloc();
    if (!resident_on_mask)
        return NV_ERR_NO_MEMORY;

    mm = uvm_va_space_mm_or_current_retain_lock(va_space);

    block_context = uvm_va_block_context_alloc(mm);
    if (!block_context) {
        status = NV_ERR_NO_MEMORY;
        goto out_unlocked;
    }

    uvm_va_space_down_read(va_space);

    // Inline uvm_va_block_find() to get the va_range.
    va_range = uvm_va_range_find(va_space, addr);
    if (!va_range) {
        NvU64 start, end;

        status = uvm_hmm_va_block_find(va_space, addr, &block);
        if (status != NV_OK) {
            if (status != NV_ERR_OBJECT_NOT_FOUND)
                goto out;
            status = uvm_hmm_va_block_range_bounds(va_space, mm, addr, &start, &end, params);
            goto out;
        }
        // Update current CPU mapping information.
        status = uvm_hmm_va_block_update_residency_info(block, mm, addr, false);
        if (status != NV_OK) {
            block = NULL;
            goto out;
        }
    }
    else if (va_range->type != UVM_VA_RANGE_TYPE_MANAGED) {
        status = NV_ERR_INVALID_ADDRESS;
        goto out;
    }
    else {
        index = uvm_va_range_block_index(va_range, addr);
        block = uvm_va_range_block(va_range, index);
        if (!block) {
            params->resident_on_count = 0;
            params->populated_on_count = 0;
            params->mapped_on_count = 0;
            params->resident_nid = -1;

            status = NV_OK;

            goto out;
        }
    }

    uvm_mutex_lock(&block->lock);

    page_index = uvm_va_block_cpu_page_index(block, addr);
    uvm_va_block_page_resident_processors(block, page_index, resident_on_mask);

    params->resident_nid = -1;
    for_each_id_in_mask(id, resident_on_mask) {
        block_phys_page_t block_page;
        int nid = block_get_page_node_residency(block, page_index);

        block_page = block_phys_page(id, nid, page_index);
        uvm_va_space_processor_uuid(va_space, &params->resident_on[count], id);
        params->resident_physical_size[count] = block_phys_page_size(block, block_page);
        if (UVM_ID_IS_CPU(id)) {
            params->resident_physical_address[count] = page_to_phys(uvm_va_block_get_cpu_page(block, page_index));
            params->resident_nid = nid;

            // Check that the page is only resident on a single CPU NUMA node.
            for_each_possible_uvm_node(nid) {
                if (uvm_va_block_cpu_is_page_resident_on(block, nid, page_index) && nid != params->resident_nid) {
                    status = NV_ERR_INVALID_STATE;
                    goto out;
                }
            }
        }
        else {
            params->resident_physical_address[count] =
                block_phys_page_address(block, block_page, uvm_va_space_get_gpu(va_space, id)).address;
        }

        ++count;
    }

    params->resident_on_count = count;

    count = 0;
    for_each_id_in_mask(id, &block->mapped) {
        uvm_processor_id_t processor_to_map;
        block_phys_page_t block_page;
        NvU64 page_size = uvm_va_block_page_size_processor(block, id, page_index);
        int nid = NUMA_NO_NODE;

        if (page_size == 0)
            continue;

        uvm_va_space_processor_uuid(va_space, &params->mapped_on[count], id);

        params->mapping_type[count] = g_uvm_prot_to_test_pte_mapping[block_page_prot(block, id, page_index)];
        UVM_ASSERT(params->mapping_type[count] != UVM_TEST_PTE_MAPPING_INVALID);
        processor_to_map = block_get_processor_to_map(block, block_context, id, page_index);
        if (UVM_ID_IS_CPU(processor_to_map))
            nid = block_get_page_node_residency(block, page_index);

        block_page = block_phys_page(processor_to_map, nid, page_index);

        if (!UVM_ID_IS_CPU(id)) {
            uvm_gpu_phys_address_t gpu_phys_addr = block_phys_page_address(block,
                                                                           block_page,
                                                                           uvm_va_space_get_gpu(va_space, id));
            params->mapping_physical_address[count] = gpu_phys_addr.address;
        }
        else {
            struct page *page = block_page_get(block, block_page);

            params->mapping_physical_address[count] = page_to_phys(page);
        }

        params->page_size[count] = page_size;
        ++count;
    }

    if (params->resident_on_count == 1 && !uvm_processor_mask_test(resident_on_mask, UVM_ID_CPU)) {
        uvm_gpu_id_t id = uvm_processor_mask_find_first_id(resident_on_mask);
        uvm_reverse_map_t gpu_mapping;
        size_t num_pages;
        uvm_gpu_t *gpu = uvm_va_space_get_gpu(va_space, id);
        uvm_gpu_phys_address_t phys_addr;

        phys_addr = uvm_va_block_gpu_phys_page_address(block, page_index, gpu);
        num_pages = uvm_pmm_gpu_phys_to_virt(&gpu->pmm, phys_addr.address, PAGE_SIZE, &gpu_mapping);

        // Chunk may be in TEMP_PINNED state so it may not have a VA block
        // assigned. In that case, we don't get a valid translation.
        if (num_pages > 0) {
            UVM_ASSERT(num_pages == 1);
            UVM_ASSERT(gpu_mapping.va_block == block);
            UVM_ASSERT(uvm_reverse_map_start(&gpu_mapping) == addr);

            ++release_block_count;
        }
    }

    params->mapped_on_count = count;

    count = 0;
    for_each_id(id) {
        if (!block_processor_page_is_populated(block, id, page_index))
            continue;

        uvm_va_space_processor_uuid(va_space, &params->populated_on[count], id);
        ++count;
    }

    params->populated_on_count = count;

out:
    if (block) {
        if (!params->is_async && status == NV_OK)
            status = uvm_tracker_wait(&block->tracker);
        uvm_mutex_unlock(&block->lock);
        while (release_block_count--)
            uvm_va_block_release(block);
    }

    uvm_va_space_up_read(va_space);
    uvm_va_block_context_free(block_context);

out_unlocked:
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);
    uvm_processor_mask_cache_free(resident_on_mask);

    return status;
}

void uvm_va_block_mark_cpu_dirty(uvm_va_block_t *va_block)
{
    block_mark_region_cpu_dirty(va_block, uvm_va_block_region_from_block(va_block));
}
