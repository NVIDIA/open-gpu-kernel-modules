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

#ifndef __UVM_VA_BLOCK_TYPES_H__
#define __UVM_VA_BLOCK_TYPES_H__

#include "uvm_common.h"
#include "uvm_pte_batch.h"
#include "uvm_tlb_batch.h"
#include "uvm_forward_decl.h"

#include <linux/migrate.h>
#include <linux/nodemask.h>

// UVM_VA_BLOCK_BITS is 21, meaning the maximum block size is 2MB. Rationale:
// - 2MB matches the largest Pascal GPU page size so it's a natural fit
// - 2MB won't span more than one PDE on any chip, so the VA blocks never need
//   to track more than a single GPU PDE.
// - 2MB is a decent tradeoff between memory overhead and serialization
//   contention.
//
#define UVM_VA_BLOCK_BITS               21

// Max size of a block in bytes
#define UVM_VA_BLOCK_SIZE               (1ULL << UVM_VA_BLOCK_BITS)

#define UVM_VA_BLOCK_ALIGN_DOWN(addr)   UVM_ALIGN_DOWN(addr, UVM_VA_BLOCK_SIZE)
#define UVM_VA_BLOCK_ALIGN_UP(addr)     UVM_ALIGN_UP(addr, UVM_VA_BLOCK_SIZE)

#define PAGES_PER_UVM_VA_BLOCK          (UVM_VA_BLOCK_SIZE / PAGE_SIZE)

#define UVM_MIN_BIG_PAGE_SIZE           UVM_PAGE_SIZE_64K
#define MAX_BIG_PAGES_PER_UVM_VA_BLOCK  (UVM_VA_BLOCK_SIZE / UVM_MIN_BIG_PAGE_SIZE)

// Prefetch heuristics shift the VA Block page mask so that it is always
// aligned to big page granularity. Big page is guaranteed not to exceed
// UVM_VA_BLOCK_SIZE, so it will use 2 * PAGES_PER_UVM_VA_BLOCK pages at
// most. Note that uvm_page_index_t needs to be able to hold outer page
// indices (one beyond the last one), for example in uvm_va_block_region_t.
#if (2 * PAGES_PER_UVM_VA_BLOCK) <= NV_U8_MAX
    typedef NvU8 uvm_page_index_t;
#elif (2 * PAGES_PER_UVM_VA_BLOCK) <= NV_U16_MAX
    typedef NvU16 uvm_page_index_t;
#else
    #warning "Suspicious value for PAGES_PER_UVM_VA_BLOCK"
    typedef NvU32 uvm_page_index_t;
#endif

// Encapsulates a [first, outer) region of pages within a va block
typedef struct
{
    // Page indices within the va block
    uvm_page_index_t first;
    uvm_page_index_t outer;
} uvm_va_block_region_t;

typedef struct
{
    DECLARE_BITMAP(bitmap, PAGES_PER_UVM_VA_BLOCK);
} uvm_page_mask_t;

// When updating GPU PTEs, this struct describes the new arrangement of PTE
// sizes. It is calculated before the operation is applied so we know which PTE
// sizes to allocate.
//
// This only decribes the new layout. The operation page mask describes the new
// permissions of each of these PTEs.
typedef struct
{
    // Whether the new PTE should remain 2m (if already 2m) or merged to 2m.
    // The meaning is the same as uvm_va_block_gpu_state_t::pte_is_2m. If this
    // is set, the other fields can be ignored.
    bool pte_is_2m;

    // Whether the operation requires writing 4k PTEs and thus needs them
    // allocated. Mutually exclusive to pte_is_2m, but not to big_ptes.
    bool needs_4k;

    // These are the PTEs which will be big after the operation is done. This
    // field will become the new value of uvm_va_block_gpu_state_t::big_ptes, so
    // it contains both those big PTEs which are being modified by the
    // operation, and any pre-existing big PTEs which remain unchanged. The
    // latter will not have the corresponding bit set in big_ptes_covered.
    DECLARE_BITMAP(big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    // These are the big PTE regions which the operation is touching. These may
    // or may not be big PTEs: use the big_ptes bitmap to determine that. For
    // example, a bit set here but not in big_ptes means that the PTE size for
    // that region should be 4k, and that some of those 4k PTEs will be written
    // by the operation.
    DECLARE_BITMAP(big_ptes_covered, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);
} uvm_va_block_new_pte_state_t;

// Event that triggered the call to uvm_va_block_make_resident/
// uvm_va_block_make_resident_read_duplicate
typedef enum
{
    UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT,
    UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT,
    UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER,
    UVM_MAKE_RESIDENT_CAUSE_PREFETCH,
    UVM_MAKE_RESIDENT_CAUSE_EVICTION,
    UVM_MAKE_RESIDENT_CAUSE_API_TOOLS,
    UVM_MAKE_RESIDENT_CAUSE_API_MIGRATE,
    UVM_MAKE_RESIDENT_CAUSE_API_SET_RANGE_GROUP,
    UVM_MAKE_RESIDENT_CAUSE_API_HINT,

    UVM_MAKE_RESIDENT_CAUSE_MAX
} uvm_make_resident_cause_t;

// Page masks are printed using hex digits printing last to first from left to
// right. For readability, a colon is added to separate each group of pages
// stored in the same word of the bitmap.
#define UVM_PAGE_MASK_WORDS                 (PAGES_PER_UVM_VA_BLOCK / BITS_PER_LONG)
#define UVM_PAGE_MASK_PRINT_NUM_COLONS      (UVM_PAGE_MASK_WORDS > 0? UVM_PAGE_MASK_WORDS - 1 : 0)
#define UVM_PAGE_MASK_PRINT_MIN_BUFFER_SIZE (PAGES_PER_UVM_VA_BLOCK / 4 + UVM_PAGE_MASK_PRINT_NUM_COLONS + 1)

typedef struct
{
    // Pages that need to be mapped with the corresponding protection
    uvm_page_mask_t page_mask;

    // Number of pages that need to be mapped with the corresponding
    // protections. This is the same value as the result of
    // bitmap_weight(page_mask)
    unsigned count;
} uvm_prot_page_mask_array_t[UVM_PROT_MAX - 1];

typedef struct
{
    // A per-NUMA-node array of page masks (size num_possible_nodes()) that hold
    // the set of CPU pages used by the migration operation.
    uvm_page_mask_t **node_masks;

    // Node mask used to iterate over the page masks above.
    // If a node's bit is set, it means that the page mask given by
    // node_to_index() in node_masks has set pages.
    nodemask_t nodes;
} uvm_make_resident_page_tracking_t;

// In the worst case some VA block operations require more state than we should
// reasonably store on the stack. Instead, we dynamically allocate VA block
// contexts. These are used for almost all operations on VA blocks.
typedef struct
{
    // Available as scratch space for the caller. Not used by any of the VA
    // block APIs.
    uvm_page_mask_t caller_page_mask;

    // Available as scratch space for the caller. Not used by any of the VA
    // block APIs.
    uvm_processor_mask_t caller_processor_mask;

    // Available as scratch space for the internal APIs. This is like a caller-
    // save register: it shouldn't be used across function calls which also take
    // this block_context.
    uvm_page_mask_t scratch_page_mask;

    // Scratch node mask. This follows the same rules as scratch_page_mask;
    nodemask_t scratch_node_mask;

    // Available as scratch space for the internal APIs. This is like a caller-
    // save register: it shouldn't be used across function calls which also take
    // this va_block_context.
    uvm_processor_mask_t scratch_processor_mask;

    // Temporary mask used in block_add_eviction_mappings().
    uvm_processor_mask_t map_processors_eviction;

    // Temporary mask used in uvm_perf_thrashing_unmap_remote_pinned_pages_all.
    uvm_processor_mask_t unmap_processors_mask;

    // Temporary mask used in thrashing_processors_have_fast_access().
    uvm_processor_mask_t fast_access_mask;

    // State used by uvm_va_block_make_resident
    struct uvm_make_resident_context_struct
    {
        // Masks used internally
        uvm_page_mask_t page_mask;
        uvm_page_mask_t copy_resident_pages_mask;
        uvm_page_mask_t pages_staged;

        // This is used to store which pages were successfully copied to the
        // destination processor and used by uvm_va_block_make_resident_finish()
        // to update the va_block state.
        uvm_page_mask_t pages_migrated;

        // Out mask filled in by uvm_va_block_make_resident to indicate which
        // pages actually changed residency.
        uvm_page_mask_t pages_changed_residency;

        // Out mask of all processors involved in the migration either as
        // source, destination or the processor performing the copy.
        // Used to perform ECC checks after the migration is done.
        uvm_processor_mask_t all_involved_processors;

        // Page mask used to compute the set of CPU pages for each CPU node.
        uvm_page_mask_t node_pages_mask;

        // Final residency for the data. This is useful for callees to know if
        // a migration is part of a staging copy
        uvm_processor_id_t dest_id;

        // Final residency NUMA node if the migration destination is the CPU.
        int dest_nid;

        // This structure is used to track CPU pages used for migrations on
        // a per-NUMA node basis.
        //
        // The pages could be used for either migrations to the CPU (used to
        // track the destination CPU pages) or staging copies (used to track
        // the CPU pages used for the staging).
        uvm_make_resident_page_tracking_t cpu_pages_used;

        // Event that triggered the call
        uvm_make_resident_cause_t cause;
    } make_resident;

    // State used by the mapping APIs (unmap, map, revoke). This could be used
    // at the same time as the state in make_resident.
    struct
    {
        // Master mask used by uvm_va_block_map and uvm_va_block_unmap, but
        // they are never called concurrently. Bits are removed as the operation
        // progresses.
        uvm_page_mask_t map_running_page_mask;

        // Master mask used by uvm_va_block_revoke. It can be used concurrently
        // with map_running_page_mask since revoke calls unmap and map. Bits
        // are removed as the operation progresses.
        uvm_page_mask_t revoke_running_page_mask;

        // Mask used by block_gpu_split_2m and block_gpu_split_big to track
        // splitting of big PTEs but they are never called concurrently. This
        // mask can be used concurrently with other page masks.
        uvm_page_mask_t big_split_page_mask;

        // Mask used by block_unmap_gpu to track non_uvm_lite_gpus which have
        // this block mapped. This mask can be used concurrently with other page
        // masks.
        uvm_processor_mask_t non_uvm_lite_gpus;

        uvm_page_mask_t page_mask;
        uvm_page_mask_t filtered_page_mask;
        uvm_page_mask_t migratable_mask;

        uvm_va_block_new_pte_state_t new_pte_state;

        uvm_pte_batch_t pte_batch;
        uvm_tlb_batch_t tlb_batch;

        // Event that triggered the call to the mapping function
        UvmEventMapRemoteCause cause;
    } mapping;

    // Used when adding page mappings with using different protections
    uvm_prot_page_mask_array_t mask_by_prot;

    struct
    {
        uvm_page_mask_t running_page_mask;
    } update_read_duplicated_pages;

    // mm to use for the operation. If this is non-NULL, the caller guarantees
    // that the mm will be valid (reference held) for the duration of the
    // block operation.
    //
    // If this is NULL, the block operation skips anything which would require
    // the mm, such as creating CPU mappings.
    struct mm_struct *mm;

    struct
    {
        // These are used for migrate_vma_*(), hmm_range_fault(), and
        // make_device_exclusive_range() handling.
        unsigned long src_pfns[PAGES_PER_UVM_VA_BLOCK];
        union {
            unsigned long dst_pfns[PAGES_PER_UVM_VA_BLOCK];
            struct page *pages[PAGES_PER_UVM_VA_BLOCK];
        };

        // Cached VMA pointer. This is only valid while holding the mmap_lock.
        struct vm_area_struct *vma;

#if UVM_IS_CONFIG_HMM()

        // Temporary mask used in uvm_hmm_block_add_eviction_mappings().
        uvm_processor_mask_t map_processors_eviction;

        // Used for migrate_vma_*() to migrate pages to/from GPU/CPU.
        struct migrate_vma migrate_vma_args;
#endif
    } hmm;

    // Convenience buffer for page mask prints
    char page_mask_string_buffer[UVM_PAGE_MASK_PRINT_MIN_BUFFER_SIZE];
} uvm_va_block_context_t;

typedef enum
{
    UVM_VA_BLOCK_TRANSFER_MODE_MOVE = 1,
    UVM_VA_BLOCK_TRANSFER_MODE_COPY = 2
} uvm_va_block_transfer_mode_t;

struct uvm_reverse_map_struct
{
    // VA block where the VA region of this Phys/DMA -> Virt translation
    // belongs to
    uvm_va_block_t             *va_block;

    // VA block region covered by this translation
    uvm_va_block_region_t         region;

    // Processor the physical memory range belongs to
    uvm_processor_id_t             owner;
};

typedef enum
{
    UVM_SERVICE_OPERATION_REPLAYABLE_FAULTS,
    UVM_SERVICE_OPERATION_NON_REPLAYABLE_FAULTS,
    UVM_SERVICE_OPERATION_ACCESS_COUNTERS,
} uvm_service_operation_t;

typedef enum
{
    UVM_MIGRATE_MODE_MAKE_RESIDENT,
    UVM_MIGRATE_MODE_MAKE_RESIDENT_AND_MAP,
} uvm_migrate_mode_t;

#endif
