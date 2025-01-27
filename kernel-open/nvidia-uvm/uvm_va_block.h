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

#ifndef __UVM_VA_BLOCK_H__
#define __UVM_VA_BLOCK_H__

#include "uvm_forward_decl.h"
#include "uvm_types.h"
#include "uvm_linux.h"
#include "nv-kref.h"
#include "uvm_common.h"
#include "uvm_perf_module.h"
#include "uvm_processors.h"
#include "uvm_lock.h"
#include "uvm_test_ioctl.h"
#include "uvm_tracker.h"
#include "uvm_pmm_gpu.h"
#include "uvm_perf_thrashing.h"
#include "uvm_va_block_types.h"
#include "uvm_range_tree.h"
#include "uvm_mmu.h"
#include "nv-kthread-q.h"

#include <linux/mmu_notifier.h>
#include <linux/wait.h>
#include <linux/nodemask.h>

// VA blocks are the leaf nodes in the uvm_va_space tree for managed allocations
// (VA ranges of type uvm_va_range_managed_t):
//
//  UVM: uvm_va_space -> uvm_va_range -> uvm_va_block
//  HMM: uvm_va_space -> uvm_va_block
//
// Each VA block is contained within a single VA range, and contains state on
// VAs covered by that block. Most importantly, the block tracks the current
// state of the virtual-to-physical mappings for all VAs within that block
// across all processors, along with the physical residency location for each
// VA.
//
// The block serializes both CPU and GPU operations on all VAs under that block.
// The CPU work is serialized with the block lock, and the GPU work is
// serialized by the block work tracker which itself is protected by the block
// lock.
//
// The size of each block varies from the size of the smallest VA range
// (PAGE_SIZE) to the max block size specified by UVM_VA_BLOCK_BITS. No block
// will span a 2^UVM_VA_BLOCK_BITS boundary in VA space. The size of the block
// is determined by the alignment of the parent VA range and the block's
// placement within the range.
//
// Note that this means user space will get best allocation efficiency if it
// allocates memory in 2^UVM_VA_BLOCK_BITS naturally-aligned chunks.

// enums used for indexing into the array of pte_bits bitmaps in the VA block
// which hold the current state of each PTE. For a given {processor, PTE}, the
// bits represented here must be enough to re-create the non-address portion of
// the PTE for that processor.

// If _READ is not set, the PTE mapping is not valid.
// If _WRITE is set, _READ is also set (_WRITE implies _READ).
typedef enum
{
    UVM_PTE_BITS_CPU_READ,
    UVM_PTE_BITS_CPU_WRITE,
    UVM_PTE_BITS_CPU_MAX
} uvm_pte_bits_cpu_t;

// If _READ is not set, the PTE mapping is not valid.
// If _WRITE is set, _READ is also set (_WRITE implies _READ).
// If _ATOMIC is set, _WRITE is also set (_ATOMIC implies _WRITE and _READ).
//
// TODO: Bug 1764925: Track volatile here too if we add GPU L2 caching
typedef enum
{
    UVM_PTE_BITS_GPU_READ,
    UVM_PTE_BITS_GPU_WRITE,
    UVM_PTE_BITS_GPU_ATOMIC,
    UVM_PTE_BITS_GPU_MAX
} uvm_pte_bits_gpu_t;

typedef struct
{
    // Per-page residency bit vector, used for fast traversal
    // of resident pages.
    //
    // This follows the same semantics as the CPU residency bit vector and
    // notably each bit still represents a PAGE_SIZE amount of data, but the
    // physical GPU memory is tracked by an array of GPU chunks below.
    uvm_page_mask_t resident;

    // Pages that have been evicted to sysmem
    uvm_page_mask_t evicted;

    // Array of naturally-aligned chunks. Each chunk has the largest possible
    // size which can fit within the block, so they are not uniform size.
    //
    // The number of chunks in the array is calculated using
    // block_num_gpu_chunks. The size of each chunk is calculated using
    // block_gpu_chunk_index.
    uvm_gpu_chunk_t **chunks;

    // These page table ranges are not necessarily all used at the same time.
    // The block might also be too small or not aligned properly to use the
    // larger ranges, in which case they're never allocated.
    //
    // Once a range is allocated we keep it around to avoid constant allocation
    // overhead when doing PTE splitting and merging.
    //
    // Check range.table to see if a given range has been allocated yet.
    //
    // page_table_range_big's range covers the big PTEs which fit within the
    // interior of this block. See the big_ptes field.
    uvm_page_table_range_t page_table_range_2m;
    uvm_page_table_range_t page_table_range_big;
    uvm_page_table_range_t page_table_range_4k;

    // These flags are ignored unless the {block, gpu} pair supports a 2M page
    // size. In that case it's the responsibility of the block code to make the
    // lower page tables active by calling uvm_page_tree_write_pde.
    //
    // They can be allocated and activated separately, so we have to track them
    // separately.
    //
    // Activated only means that uvm_page_tree_write_pde has been called at some
    // point in the past with the appropriate range allocated. It does not imply
    // that the 2M entry is a PDE (see pte_is_2m).
    bool activated_big;
    bool activated_4k;

    // For {block, gpu} pairs which support the 2M page size, the page table
    // ranges are uninitialized on allocation. This flag tracks whether the big
    // PTEs have been initialized.
    //
    // We don't need an equivalent flag for the 4k range because we always write
    // just the 4k PTEs not covered by higher-level PTEs. Big PTEs however can
    // be allocated and activated late while the 4k PTEs are already active, in
    // which case we need to initialize the entire big range.
    bool initialized_big;

    // Sticky state to split PTEs to 4k and keep them there. Used when a fatal
    // fault has been detected on this GPU to avoid false dependencies within
    // the uTLB for fatal and non-fatal faults on the same larger PTE, which
    // could lead to wrong fault attribution.
    bool force_4k_ptes;

    // This table shows the HW PTE states given all permutations of pte_is_2m,
    // big_ptes, and pte_bits. Note that the first row assumes that the 4k page
    // tables have been allocated (if not, then no PDEs are allocated either).
    //
    // |-------------- SW state --------------|------------------- HW state --------------------|
    //  pte_is_2m  pte_is_big  pte_bits[READ] | Page size  PDE0(2M only)  Big PTE       4k PTE
    //  ----------------------------------------------------------------------------------------
    //  0          0           0              | 4k         Valid PDE      Invalid [1]   Invalid
    //  0          0           1              | 4k         Valid PDE      Invalid [1]   Valid
    //  0          1           0              | Big        Valid PDE      Unmapped [2]  x
    //  0          1           1              | Big        Valid PDE      Valid         x
    //  1          must be 0   0              | 2M         Invalid        x             x
    //  1          must be 0   1              | 2M         Valid PTE      x             x
    //
    // [1]: The big PTE may be unallocated, in which case its pointer won't be
    //      valid in the parent PDE. If the big PTE is allocated, it will be
    //      invalid so the 4k PTEs are active.
    //
    // [2]: The unmapped big PTE pattern differs from the invalid pattern, and
    //      it prevents HW from reading the 4k entries. See the unmapped_pte()
    //      MMU HAL function.

    // If pte_is_2m is true, there is a 2M PTE covering this VA block (valid or
    // invalid). If false then we're in one of the following scenarios:
    // 1) This {block, gpu} does not support 2M pages.
    // 2) 2M pages are supported but the page_table_range_2m has not been
    //    allocated (implying that the other page table ranges have not been
    //    allocated either).
    // 3) page_table_range_2m has been allocated, but the big_ptes bitmap should
    //    be used to determine the mix of big and 4k PTEs.
    bool pte_is_2m;

    // When pte_is_2m is false, this block consists of any possible mix of big
    // and 4k PTEs. This bitmap describes that mix. A set bit indicates that the
    // corresponding big-page-sized region of the block is covered by a big PTE.
    // A cleared bit indicates that it is covered by 4k PTEs.
    //
    // Neither setting implies that the PTE currently has a valid mapping, it
    // just indicates which PTE is read by the GPU (see the table above).
    //
    // The indices represent the corresponding big PTEs in the block's interior.
    // For example, a block with alignment and size of one 4k page on either
    // side of a big page will only use bit 0. Use uvm_va_block_big_page_index to look
    // the big_ptes index of a page.
    //
    // The block might not be able to fit any big PTEs, in which case this
    // bitmap is always zero. Use uvm_va_block_gpu_num_big_pages to find the number of
    // valid bits in this mask.
    DECLARE_BITMAP(big_ptes, MAX_BIG_PAGES_PER_UVM_VA_BLOCK);

    // See the comments for uvm_va_block_mmap_t::cpu.pte_bits.
    //
    // The major difference is that these bits are always accurate since, unlike
    // the CPU PTEs, the UVM driver is in full control of these mappings.
    //
    // Note that the granularity is always PAGE_SIZE, not whatever GPU PTE size
    // happens to currently map these regions. PAGE_SIZE is the minimum
    // granularity of operations on the VA blocks. As a future optimization we
    // could consider sub-PAGE_SIZE operations if PAGE_SIZE > 4K and the CPU
    // isn't involved, for example false sharing among peer GPUs.
    uvm_page_mask_t pte_bits[UVM_PTE_BITS_GPU_MAX];

    // Set of pages using EGM mappings.
    uvm_page_mask_t egm_pages;
} uvm_va_block_gpu_state_t;

typedef struct
{
    // Per-page residency bit vector, used for fast traversal of resident
    // pages.
    //
    // A set bit means the CPU has a coherent copy of the physical page
    // resident in the NUMA node's memory, and that a CPU chunk for the
    // corresponding page index has been allocated. This does not mean that
    // the coherent copy is currently mapped anywhere, however. A page may be
    // resident on multiple processors (but not multiple CPU NUMA nodes) when in
    // read-duplicate mode.
    //
    // A cleared bit means the CPU NUMA node does not have a coherent copy of
    // that page resident. A CPU chunk for the corresponding page index may or
    // may not have been allocated. If the chunk is present, it's a cached chunk
    // which can be reused in the future.
    //
    // Allocating PAGES_PER_UVM_VA_BLOCK is overkill when the block is
    // smaller than UVM_VA_BLOCK_SIZE, but it's not much extra memory
    // overhead on the whole.
    uvm_page_mask_t resident;

    // Per-page allocation bit vector.
    //
    // A set bit means that a CPU chunk has been allocated for the
    // corresponding page index on this NUMA node.
    uvm_page_mask_t allocated;

    // CPU memory chunks represent physically contiguous CPU memory
    // allocations. See uvm_pmm_sysmem.h for more details on CPU chunks.
    // This member is meant to hold an opaque value indicating the CPU
    // chunk storage method. For more details on CPU chunk storage,
    // see uvm_cpu_chunk_storage_type_t in uvm_va_block.c.
    unsigned long chunks;
} uvm_va_block_cpu_node_state_t;

// TODO: Bug 1766180: Worst-case we could have one of these per system page.
//       Options:
//       1) Rely on the OOM killer to prevent the user from trying to do that
//       2) Be much more space-conscious in this struct (difficult)
//       3) Cap the per-process range and/or block count, like vm.max_map_count
//          does for vmas
struct uvm_va_block_struct
{
    // Reference count for this block. References are held by:
    // - The parent managed range for managed blocks or VA space for HMM blocks
    // - The reverse map
    // - The eviction path temporarily when attempting to evict a GPU page under
    //   this block
    //
    // This isn't protected by the lock on the eviction path, so it must be
    // atomic. nv_kref provides that.
    nv_kref_t kref;

    // Lock protecting the block. See the comment at the top of uvm.c.
    uvm_mutex_t lock;

    // Parent managed range. Managed blocks have this set. HMM blocks will have
    // managed_range set to NULL and hmm.va_space set instead. Dead blocks that
    // are waiting for the last ref count to be removed have managed_range and
    // hmm.va_space set to NULL (could be either type of block).
    //
    // This field can be read while holding either the block lock or just the VA
    // space lock in read mode, since it can only change when the VA space lock
    // is held in write mode.
    uvm_va_range_managed_t *managed_range;

    // Virtual address [start, end] covered by this block. These fields can be
    // read while holding either the block lock or just the VA space lock in
    // read mode, since they can only change when the VA space lock is held in
    // write mode.
    NvU64 start;
    NvU64 end;

    // Per-processor residency bit vector, used for fast lookup of which
    // processors are active in this block.
    //
    // A set bit means the corresponding processor has a coherent physical copy
    // of memory somewhere in the block. The per-processor state must then be
    // inspected to find out which pages. The processor may or may not have a
    // mapping to that physical memory, however.
    //
    // A cleared bit means the corresponding processor does not have a coherent
    // physical copy of any pages under this block. The processor may still have
    // cached pages allocated for future use, however. It also may have mappings
    // to pages resident on other processors.
    uvm_processor_mask_t resident;

    // Per-processor mapping bit vector, used for fast lookup of which
    // processors are active in this block.
    //
    // A set bit means the corresponding processor has an active, valid page
    // table mapping to some VA in this block. The per-processor pte_bits state
    // must then be inspected to find out the mapping address and permissions.
    //
    // A cleared bit means the corresponding processor has no virtual mappings
    // within this block (all pte_bits entries are 0).
    uvm_processor_mask_t mapped;

    // Per-processor evicted bit vector, used for fast lookup of which GPUs
    // have evicted pages in this block.
    //
    // A set bit means the corresponding processor was the residency of some of
    // the pages in the block when they were evicted due to memory capacity
    // limitations. The per-processor state must then be inspected to find out
    // which pages.
    //
    // A cleared bit means the corresponding processor has no evicted pages
    // within this block (all evicted entries are 0).
    uvm_processor_mask_t evicted_gpus;

    struct
    {
        // Per-NUMA node tracking of CPU allocations.
        // This is a dense array with one entry per possible NUMA node.
        uvm_va_block_cpu_node_state_t **node_state;

        // Per-page allocation bit vector.
        //
        // A set bit means that a CPU page has been allocated for the
        // corresponding page index on at least one CPU NUMA node.
        uvm_page_mask_t allocated;

        // Per-page residency bit vector. See
        // uvm_va_block_cpu_numa_state_t::resident for a detailed description.
        // This mask is a cumulative mask (logical OR) of all
        // uvm_va_block_cpu_node_state_t::resident masks. It is meant to be used
        // only for fast testing of page residency when it matters only if the
        // page is resident on the CPU.
        //
        // Note that this mask cannot be set directly as this will cause
        // inconsistencies between this mask and the per-NUMA residency masks.
        // In order to properly maintain consistency between the per-NUMA masks
        // and this one, uvm_va_block_cpu_[set|clear]_residency_*() helpers
        // should be used.
        uvm_page_mask_t resident;

        // Per-page mapping bit vectors, one per bit we need to track. These are
        // used for fast traversal of valid mappings in the block. These contain
        // all non-address bits needed to establish a virtual mapping on this
        // processor (permissions, cacheability, etc).
        //
        // A cleared bit in UVM_PTE_BITS_CPU_READ means the CPU has no valid
        // virtual mapping to that address (the access will fault). Further,
        // UVM_PTE_BITS_CPU_WRITE is guaranteed to also be clear.
        //
        // A set bit in UVM_PTE_BITS_CPU_READ means the CPU has a valid mapping
        // at that address with at least read permissions. The physical page for
        // that mapping is contained in the pages array. If
        // UVM_PTE_BITS_CPU_WRITE is not set, the mapping is read-only.
        // Otherwise, the mapping is read-write.
        //
        // For managed allocations, this is the maximum permissions a PTE
        // could have, but not necessarily the actual current permissions of the
        // CPU PTEs. The UVM driver will never change the PTEs without updating
        // this state, but the kernel can downgrade our CPU mappings at any time
        // without notifying the UVM driver (for example in response to user
        // space calling madvise with MADV_DONTNEED).
        //
        // For HMM allocations, this is the minimum permission the CPU has since
        // Linux can upgrade a read-only PTE to read-write without notifying
        // the UVM driver. This is why read duplication isn't currently
        // supported.
        // TODO: Bug 3660922: Need to handle read duplication at some point.
        uvm_page_mask_t pte_bits[UVM_PTE_BITS_CPU_MAX];

        // Whether the CPU has ever mapped a page on this VA block. This is
        // used to force GMMU PDE1 pre-population on ATS systems. See
        // pre_populate_gpu_pde1 in uvm_va_block.c for more information.
        NvU8 ever_mapped        : 1;

        // We can get "unexpected" faults if multiple CPU threads fault on the
        // same address simultaneously and race to create the mapping. Since
        // our CPU fault handler always unmaps to handle the case where the
        // kernel downgrades our CPU mappings, we can introduce an infinite
        // stream of CPU faults in multi-threaded workloads.
        //
        // In order to handle this scenario, we keep track of the first thread
        // that faulted on a page with valid permissions and the timestamp.
        // Then, we keep track of the subsequent faults on that page during a
        // window of time. If the first thread faults again on the page, that
        // will indicate that the mapping has been downgraded by the kernel and
        // we need to remap it. Faults from the rest of threads are just
        // ignored. The information is also cleared on the following events:
        // - The tracking window finishes
        // - The page is unmapped
        struct
        {
            // Timestamp when the first fault was detected. This also is used
            // as a flag that the contents of this struct are valid
            NvU64             first_fault_stamp;

            // First thread that faulted while having valid permissions. we
            // don't take a reference on the pid so we shouldn't ever use it
            // for task-lookup in the kernel. We only use it as a heuristic so
            // it's OK if the pid gets destroyed or reused.
            pid_t             first_pid;

            // Index of the page whose faults are being tracked
            uvm_page_index_t  page_index;
        } fault_authorized;
    } cpu;

    // Per-GPU residency and mapping state
    //
    // TODO: Bug 1766180: Even though these are pointers, making this a static
    //       array will use up a non-trivial amount of storage for small blocks.
    //       In most cases we won't have anywhere near this many GPUs active
    //       anyway. Consider using a dense array of just the GPUs registered in
    //       this VA space, depending on the perf of accessing that array and on
    //       how noticeable this memory overhead actually is.
    uvm_va_block_gpu_state_t *gpus[UVM_ID_MAX_GPUS];

    // Mask to keep track of the pages that are read-duplicate
    uvm_page_mask_t read_duplicated_pages;

    // Mask to keep track of the pages that are not mapped on any non-UVM-Lite
    // processor. This mask is not used for HMM because the CPU can map pages
    // at any time without notifying the driver.
    //     0: Page is definitely not mapped by any processors
    //     1: Page may or may not be mapped by a processor
    //
    // This mask sets the bit when the page is mapped on any non-UVM-Lite
    // processor but it is not always unset on unmap (to avoid a performance
    // impact). Therefore, it can contain false negatives. It should be only
    // used for opportunistic optimizations that have a fast path for pages
    // that are not mapped anywhere (see uvm_va_block_migrate_locked, for
    // example), but not the other way around.
    uvm_page_mask_t maybe_mapped_pages;

    // Tracks all outstanding GPU work related to this block: GPU copies, PTE
    // updates, TLB invalidates, etc. The residency and mapping state is only
    // valid once this tracker is done.
    //
    // CPU operations need to wait for this tracker to be done. GPU operations
    // need to acquire it before pushing their work, then that work must be
    // added to this tracker before the block's lock is dropped.
    uvm_tracker_t tracker;

    // A queue item for establishing eviction mappings in a deferred way
    nv_kthread_q_item_t eviction_mappings_q_item;

    uvm_perf_module_data_desc_t perf_modules_data[UVM_PERF_MODULE_TYPE_COUNT];

    // Prefetch infomation that is updated while holding the va_block lock but
    // records state while the lock is not held.
    struct
    {
        uvm_processor_id_t last_migration_proc_id;

        NvU16 fault_migrations_to_last_proc;
    } prefetch_info;

    struct
    {
#if UVM_IS_CONFIG_HMM()
        // The MMU notifier is registered per va_block.
        struct mmu_interval_notifier notifier;
#endif

        // This is used to serialize migrations between CPU and GPU while
        // allowing the va_block lock to be dropped.
        // This must be acquired before locking the va_block lock if the
        // critical section can change the residency state.
        // Do not access directly, use the uvm_hmm_migrate_*() routines.
        uvm_mutex_t migrate_lock;

        // Sequence number to tell if any changes were made to the va_block
        // while not holding the block lock and calling hmm_range_fault().
        unsigned long changed;

        // Parent VA space pointer. It is NULL for managed blocks or if
        // the HMM block is dead. This field can be read while holding the
        // block lock and is only modified while holding the va_space write
        // lock and va_block lock (same as the managed_range pointer).
        uvm_va_space_t *va_space;

        // Tree of uvm_va_policy_node_t. The policy node ranges always cover
        // all or part of a VMA range or a contiguous range of VMAs within the
        // va_block. Policy nodes are resized or deleted when the underlying
        // VMA range is changed by Linux via the invalidate() callback.
        // Otherwise, policies could be stale after munmap().
        // Locking: The va_block lock is needed to access or modify the tree.
        uvm_range_tree_t va_policy_tree;

        // Storage node for range tree of va_blocks.
        uvm_range_tree_node_t node;
    } hmm;
};

// We define additional per-VA Block fields for testing. When
// uvm_enable_builtin_tests is defined, all VA Blocks will have
// uvm_va_block_wrapper_t size. Otherwise, the test fields are not available.
// Use the uvm_va_block_get_test function defined below to obtain a safe
// pointer to uvm_va_block_test_t from a uvm_va_block_t pointer.
struct uvm_va_block_wrapper_struct
{
    uvm_va_block_t block;

    struct uvm_va_block_test_struct
    {
        // Count of how many page table allocations should be forced to retry
        // with eviction enabled. Used for testing only.
        NvU32 page_table_allocation_retry_force_count;

        // Count of how many user pages allocations should be forced to retry
        // with eviction enabled. Used for testing only.
        NvU32 user_pages_allocation_retry_force_count;

        // Mask of chunk sizes to be used for CPU chunk allocations.
        // The actual set of chunk sizes to be used will be the set resulting
        // from AND'ing this value with the value of
        // uvm_cpu_chunk_allocation_sizes module parameter.
        NvU32 cpu_chunk_allocation_size_mask;

        // Subsequent operations that need to allocate CPU chunks will fail. As
        // opposed to other error injection settings, this one fails N times
        // and then succeeds instead of failing on the Nth try. A value of ~0u
        // means fail indefinitely.
        // This is because this error is supposed to be fatal and tests verify
        // the state of the VA blocks after the failure. However, some tests
        // use kernels to trigger migrations and a fault replay could trigger
        // a successful migration if this error flag is cleared.
        NvU32 inject_cpu_chunk_allocation_error_count;

        // A NUMA node ID on which any CPU chunks will be allocated from.
        // This will override any other setting and/or policy.
        // Note that the kernel is still free to allocate from any of the
        // nodes in the thread's policy.
        int cpu_chunk_allocation_target_id;
        int cpu_chunk_allocation_actual_id;

        // Force the next eviction attempt on this block to fail. Used for
        // testing only.
        bool inject_eviction_error;

        // Force the next successful chunk allocation to then fail. Used for testing
        // only to simulate driver metadata allocation failure.
        bool inject_populate_error;

        // Force the next split on this block to fail.
        // Set by error injection ioctl for testing purposes only.
        bool inject_split_error;
    } test;
};

// Tracking needed for supporting allocation-retry of user GPU memory
struct uvm_va_block_retry_struct
{
    // A tracker used for all allocations from PMM.
    uvm_tracker_t tracker;

    // List of allocated chunks (uvm_gpu_chunk_t). Currently all chunks are of
    // the same size. However it can contain chunks from multiple GPUs. All
    // remaining free chunks are freed when the operation is finished with
    // uvm_va_block_retry_deinit().
    struct list_head free_chunks;

    // List of chunks allocated and used during the block operation. This list
    // can contain chunks from multiple GPUs. All the used chunks are unpinned
    // when the operation is finished with uvm_va_block_retry_deinit().
    struct list_head used_chunks;
};

// Module load/exit
NV_STATUS uvm_va_block_init(void);
void uvm_va_block_exit(void);

// Allocates and initializes the block. The block's ref count is initialized to
// 1. The caller is responsible for inserting the block into its parent
// managed range.
//
// The caller must be holding the VA space lock in at least read mode.
NV_STATUS uvm_va_block_create(uvm_va_range_managed_t *managed_range,
                              NvU64 start,
                              NvU64 end,
                              uvm_va_block_t **out_block);

// Internal function called only when uvm_va_block_release drops the ref count
// to 0. Do not call directly.
void uvm_va_block_destroy(nv_kref_t *kref);

static inline void uvm_va_block_retain(uvm_va_block_t *va_block)
{
    nv_kref_get(&va_block->kref);
}

// Locking: The va_block lock must not be held.
// The va_space lock must be held in write mode unless it is the special case
// that the block has no GPU state; for example, right after calling
// uvm_va_block_create(). In that case, the va_space lock can be held in read
// mode.
static inline void uvm_va_block_release(uvm_va_block_t *va_block)
{
    if (va_block) {
        // The calling thread shouldn't be holding the block's mutex when
        // releasing the block as it might get destroyed.
        uvm_assert_unlocked_order(UVM_LOCK_ORDER_VA_BLOCK);
        nv_kref_put(&va_block->kref, uvm_va_block_destroy);
    }
}

// Same as uvm_va_block_release but the caller may be holding the VA block lock.
// The caller must ensure that the refcount will not get to zero in this call.
static inline void uvm_va_block_release_no_destroy(uvm_va_block_t *va_block)
{
    int destroyed = nv_kref_put(&va_block->kref, uvm_va_block_destroy);
    UVM_ASSERT(!destroyed);
}

// Returns true if the block is managed by HMM.
// Locking: This can be called while holding either the block lock or just the
// VA space lock in read mode, since it can only change when the VA space lock
// is held in write mode.
static inline bool uvm_va_block_is_hmm(uvm_va_block_t *va_block)
{
#if UVM_IS_CONFIG_HMM()
    return va_block->hmm.va_space;
#else
    return false;
#endif
}

// Return true if the block is dead.
// Locking: This can be called while holding either the block lock or just the
// VA space lock in read mode, since it can only change when the VA space lock
// is held in write mode.
static inline bool uvm_va_block_is_dead(uvm_va_block_t *va_block)
{
    if (va_block->managed_range)
        return false;

#if UVM_IS_CONFIG_HMM()
    if (va_block->hmm.va_space)
        return false;
#endif

    return true;
}

static inline uvm_va_block_gpu_state_t *uvm_va_block_gpu_state_get(uvm_va_block_t *va_block, uvm_gpu_id_t gpu_id)
{
    return va_block->gpus[uvm_id_gpu_index(gpu_id)];
}

// Return the va_space pointer of the given block or NULL if the block is dead.
// Locking: This can be called while holding either the block lock or just the
// VA space lock in read mode, since it can only change when the VA space lock
// is held in write mode.
uvm_va_space_t *uvm_va_block_get_va_space_maybe_dead(uvm_va_block_t *va_block);

// Return the va_space pointer of the given block assuming the block is not dead
// (asserts that it is not dead and asserts va_space is not NULL).
// Locking: This can be called while holding either the block lock or just the
// VA space lock in read mode, since it can only change when the VA space lock
// is held in write mode.
uvm_va_space_t *uvm_va_block_get_va_space(uvm_va_block_t *va_block);

// Return true if the VA space has access counter migrations enabled and should
// remote map pages evicted to system memory. This is OK since access counters
// can pull the data back to vidmem if sufficient accesses trigger a migration.
// The caller must ensure that the VA space cannot go away.
bool uvm_va_space_map_remote_on_eviction(uvm_va_space_t *va_space);

// Dynamic cache-based allocation for uvm_va_block_context_t.
//
// See uvm_va_block_context_init() for a description of the mm parameter.
uvm_va_block_context_t *uvm_va_block_context_alloc(struct mm_struct *mm);
void uvm_va_block_context_free(uvm_va_block_context_t *va_block_context);

// Initialization of an already-allocated uvm_va_block_context_t.
//
// mm is used to initialize the value of va_block_context->mm. NULL is allowed.
void uvm_va_block_context_init(uvm_va_block_context_t *va_block_context, struct mm_struct *mm);

// TODO: Bug 1766480: Using only page masks instead of a combination of regions
//       and page masks could simplify the below APIs and their implementations
//       at the cost of having to scan the whole mask for small regions.
//       Investigate the performance effects of doing that.

// Moves the physical pages of the given region onto the destination processor.
// If page_mask is non-NULL, the movement is further restricted to only those
// pages in the region which are present in the mask.
//
// prefetch_page_mask may be passed as a subset of page_mask when cause is
// UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT,
// UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT, or
// UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER to indicate pages that have been
// pulled due to automatic page prefetching heuristics. For pages in this mask,
// UVM_MAKE_RESIDENT_CAUSE_PREFETCH will be reported in migration events,
// instead.
//
// This function breaks read duplication for all given pages even if they
// don't migrate. Pages which are not resident on the destination processor
// will also be unmapped from all existing processors, be populated in the
// destination processor's memory, and copied to the new physical location.
// Any new memory will be zeroed if it is the first allocation for that page
// in the system.
//
// This function does not create any new virtual mappings.
//
// This function acquires/waits for the va_block tracker and updates that
// tracker with any new work pushed.
//
// Allocation-retry: this operation may need to perform eviction to be able to
// allocate GPU memory successfully and if that happens,
// NV_ERR_MORE_PROCESSING_REQUIRED will be returned. That also means that the
// block's lock has been unlocked and relocked as part of the call and that the
// whole sequence of operations performed under the block's lock needs to be
// attempted again. To facilitate that, the caller needs to provide the same
// va_block_retry struct for each attempt that has been initialized before the
// first attempt and needs to be deinitialized after the last one. Most callers
// can just use UVM_VA_BLOCK_LOCK_RETRY() that takes care of that for the
// caller.
//
// If dest_id is the CPU then va_block_retry can be NULL and allocation-retry of
// user memory is guaranteed not to happen. Allocation-retry of GPU page tables
// can still occur though.
//
// va_block_context must not be NULL and policy for the region must
// match. This function will set a bit in
// va_block_context->make_resident.pages_changed_residency for each
// page that changed residency (due to a migration or first
// population) as a result of the operation and
// va_block_context->make_resident.all_involved_processors for each
// processor involved in the copy. This function only sets bits in
// those masks. It is the caller's responsiblity to zero the masks or
// not first.
//
// va_block_context->make_resident.dest_nid is used to guide the NUMA node for
// CPU allocations.
//
// Notably any status other than NV_OK indicates that the block's lock might
// have been unlocked and relocked.
//
// LOCKING: The caller must hold the va_block lock.
// If va_block_context->mm != NULL, va_block_context->mm->mmap_lock must be
// held in at least read mode.
NV_STATUS uvm_va_block_make_resident(uvm_va_block_t *va_block,
                                     uvm_va_block_retry_t *va_block_retry,
                                     uvm_va_block_context_t *va_block_context,
                                     uvm_processor_id_t dest_id,
                                     uvm_va_block_region_t region,
                                     const uvm_page_mask_t *page_mask,
                                     const uvm_page_mask_t *prefetch_page_mask,
                                     uvm_make_resident_cause_t cause);

// Similar to uvm_va_block_make_resident (read documentation there). The main
// differences are:
// - Pages are copied not moved (i.e. other copies of the page are not
//   unmapped)
// - Processors with a resident copy of pages that migrated have write and
//   atomic access permission revoked, unlike in uvm_va_block_make_resident
//   where they are unmapped
// - All remote mappings (due to either SetAccessedBy or performance heuristics)
//   are broken
// - Only managed va_blocks are supported.
//   TODO: Bug 3660922: need to implement HMM read duplication support.
NV_STATUS uvm_va_block_make_resident_read_duplicate(uvm_va_block_t *va_block,
                                                    uvm_va_block_retry_t *va_block_retry,
                                                    uvm_va_block_context_t *va_block_context,
                                                    uvm_processor_id_t dest_id,
                                                    uvm_va_block_region_t region,
                                                    const uvm_page_mask_t *page_mask,
                                                    const uvm_page_mask_t *prefetch_page_mask,
                                                    uvm_make_resident_cause_t cause);

// Similar to uvm_va_block_make_resident() (read documentation there). The
// difference is that source pages are only copied to the destination and the
// residency is not updated until uvm_va_block_make_resident_finish() is called.
// Otherwise, the combination of uvm_va_block_make_resident_copy() and
// uvm_va_block_make_resident_finish() is the same as just calling
// uvm_va_block_make_resident(). Note, however, that the va_block lock must be
// held across the two calls for the operation to be complete. The va_block
// lock can be dropped after calling uvm_va_block_make_resident_copy() but
// uvm_va_block_make_resident_copy() must be called again after relocking the
// va_block lock and before calling uvm_va_block_make_resident_finish().
// This split is needed when using migrate_vma_setup() and migrate_vma_pages()
// so that when migrate_vma_pages() indicates a page is not migrating, the
// va_block state is not updated.
// LOCKING: The caller must hold the va_block lock.
NV_STATUS uvm_va_block_make_resident_copy(uvm_va_block_t *va_block,
                                          uvm_va_block_retry_t *va_block_retry,
                                          uvm_va_block_context_t *va_block_context,
                                          uvm_processor_id_t dest_id,
                                          uvm_va_block_region_t region,
                                          const uvm_page_mask_t *page_mask,
                                          const uvm_page_mask_t *prefetch_page_mask,
                                          uvm_make_resident_cause_t cause);

// The page_mask must be the same or a subset of the page_mask passed to
// uvm_va_block_make_resident_copy(). This step updates the residency and breaks
// read duplication.
// LOCKING: The caller must hold the va_block lock.
void uvm_va_block_make_resident_finish(uvm_va_block_t *va_block,
                                       uvm_va_block_context_t *va_block_context,
                                       uvm_va_block_region_t region,
                                       const uvm_page_mask_t *page_mask);

// Creates or upgrades a mapping from the input processor to the given virtual
// address region. Pages which already have new_prot permissions or higher are
// skipped, so this call ensures that the range is mapped with at least new_prot
// permissions. new_prot must not be UVM_PROT_NONE. uvm_va_block_unmap or
// uvm_va_block_revoke_prot should be used to downgrade permissions instead.
//
// The mapped pages are described by the region parameter and the map page mask
// that allows the caller to restrict the map operation to specific pages within
// the region. If the page mask is NULL then the whole region is mapped.
//
// If the input processor is a GPU with no GPU VA space registered, or if the
// input processor is the CPU and this thread is not allowed to create CPU
// mappings, this function does nothing. CPU mappings are only allowed if
// uvm_va_range_vma_check(va_block_context->mm) is valid, so the caller must
// set va_block_context->mm before calling this function.
//
// cause specifies the cause to be reported in events in case a remote mapping
// is created.
//
// Any CPU mappings will wait for the va_block tracker. If this function pushes
// GPU work it will first acquire the va_block tracker, then add the pushed work
// to out_tracker. It is the caller's responsibility to add this work to
// va_block's tracker. Note that while it is generally safe to run map
// operations on different GPUs concurrently, two PTE operations (map, unmap,
// revoke) on the same GPU must be serialized even if they target different
// pages because the earlier operation can cause a PTE split or merge which is
// assumed by the later operation.
//
// va_block_context must not be NULL and policy for the region must match.
// See the comments for uvm_va_block_check_policy_is_valid().
//
// If allocation-retry was required as part of the operation and was successful,
// NV_ERR_MORE_PROCESSING_REQUIRED is returned. In this case, the entries in the
// out_tracker were added to the block's tracker and then the block's lock was
// unlocked and relocked.
//
// In general, any status other than NV_OK indicates that the block's lock might
// have been unlocked and relocked.
//
// LOCKING: The caller must hold the va block lock. If va_block_context->mm !=
//          NULL, va_block_context->mm->mmap_lock must be held in at least read
//          mode.
NV_STATUS uvm_va_block_map(uvm_va_block_t *va_block,
                           uvm_va_block_context_t *va_block_context,
                           uvm_processor_id_t id,
                           uvm_va_block_region_t region,
                           const uvm_page_mask_t *map_page_mask,
                           uvm_prot_t new_prot,
                           UvmEventMapRemoteCause cause,
                           uvm_tracker_t *out_tracker);

// Like uvm_va_block_map, except it maps all processors in the input mask. The
// VA block tracker contains all map operations on return.
//
// Note that this can return NV_ERR_MORE_PROCESSING_REQUIRED just like
// uvm_va_block_map() indicating that the operation needs to be retried.
NV_STATUS uvm_va_block_map_mask(uvm_va_block_t *va_block,
                                uvm_va_block_context_t *va_block_context,
                                const uvm_processor_mask_t *map_processor_mask,
                                uvm_va_block_region_t region,
                                const uvm_page_mask_t *map_page_mask,
                                uvm_prot_t new_prot,
                                UvmEventMapRemoteCause cause);

// Unmaps virtual regions from a single processor. This does not free page
// tables or physical memory. This is safe to call on the eviction path, but the
// caller must ensure that the block hasn't been killed.
//
// The unmapped pages are described by the region parameter and the unmap page
// mask that allows the caller to restrict the unmap operation to specific pages
// within the region. If the page mask is NULL then the whole region is
// unmapped.
//
// If id is UVM_ID_CPU, this is guaranteed to return NV_OK, and this is safe to
// call without holding a reference on the mm which owns the associated vma.
//
// Any CPU unmappings will wait for the va_block tracker. If this function
// pushes GPU work it will first acquire the va_block tracker, then add the
// pushed work to out_tracker. It is the caller's responsibility to add this
// work to va_block's tracker. Note that while it is generally safe to run unmap
// operations on different GPUs concurrently, two PTE operations (map, unmap,
// revoke) on the same GPU must be serialized even if they target different
// pages because the earlier operation can cause a PTE split or merge which is
// assumed by the later operation.
//
// va_block_context must not be NULL.
//
// If allocation-retry was required as part of the operation and was successful,
// NV_ERR_MORE_PROCESSING_REQUIRED is returned. In this case, the entries in the
// out_tracker were added to the block's tracker and then the block's lock was
// unlocked and relocked. It is guaranteed that retry will not be required if
// the unmap does not cause a PTE split. Examples of operations which will not
// cause a PTE split include unmapping the entire block, unmapping all PTEs with
// matching attributes, and unmapping all PTEs which point to the same physical
// chunk.
//
// LOCKING: The caller must hold the va_block lock.
NV_STATUS uvm_va_block_unmap(uvm_va_block_t *va_block,
                             uvm_va_block_context_t *va_block_context,
                             uvm_processor_id_t id,
                             uvm_va_block_region_t region,
                             const uvm_page_mask_t *unmap_page_mask,
                             uvm_tracker_t *out_tracker);

// Like uvm_va_block_unmap, except it unmaps all processors in the input mask.
// The VA block tracker contains all map operations on return.
NV_STATUS uvm_va_block_unmap_mask(uvm_va_block_t *va_block,
                                  uvm_va_block_context_t *va_block_context,
                                  const uvm_processor_mask_t *unmap_processor_mask,
                                  uvm_va_block_region_t region,
                                  const uvm_page_mask_t *unmap_page_mask);

// Function called when the preferred location changes. Notably:
// - Mark all CPU pages as dirty because the new processor may not have
//   up-to-date data.
// - Unmap the preferred location's processor from any pages in this region
//   which are not resident on the preferred location.
//
// va_block_context must not be NULL and policy for the region must match.
// See the comments for uvm_va_block_check_policy_is_valid().
//
// LOCKING: The caller must hold the VA block lock.
NV_STATUS uvm_va_block_set_preferred_location_locked(uvm_va_block_t *va_block,
                                                     uvm_va_block_context_t *va_block_context,
                                                     uvm_va_block_region_t region);

// Maps the given processor to all resident pages in this block, as allowed by
// location and policy. Waits for the operation to complete before returning.
// This function should only be called with managed va_blocks.
//
// va_block_context must not be NULL and policy for the region must match.
// See the comments for uvm_va_block_check_policy_is_valid().
//
// LOCKING: This takes and releases the VA block lock. If va_block_context->mm
//          != NULL, va_block_context->mm->mmap_lock must be held in at least
//          read mode.
NV_STATUS uvm_va_block_set_accessed_by(uvm_va_block_t *va_block,
                                       uvm_va_block_context_t *va_block_context,
                                       uvm_processor_id_t processor_id);

// Maps given processor to all resident pages in this block and region, as
// allowed by location and policy. The caller is responsible for waiting for
// the tracker after all mappings have been started.
// This function can be called with HMM and managed va_blocks.
//
// va_block_context must not be NULL and policy for the region must match.
// See the comments for uvm_va_block_check_policy_is_valid().
//
// LOCKING: The caller must hold the va_block lock and
//          va_block_context->mm->mmap_lock must be held in at least read mode.
NV_STATUS uvm_va_block_set_accessed_by_locked(uvm_va_block_t *va_block,
                                              uvm_va_block_context_t *va_block_context,
                                              uvm_processor_id_t processor_id,
                                              uvm_va_block_region_t region,
                                              uvm_tracker_t *out_tracker);

// Breaks SetAccessedBy and remote mappings
// This function should only be called with managed va_blocks.
//
// va_block_context must not be NULL and policy for the region must match.
// See the comments for uvm_va_block_check_policy_is_valid().
//
// LOCKING: This takes and releases the VA block lock. If va_block_context->mm
//          != NULL, va_block_context->mm->mmap_lock must be held in at least
//          read mode.
NV_STATUS uvm_va_block_set_read_duplication(uvm_va_block_t *va_block,
                                            uvm_va_block_context_t *va_block_context);

// Restores SetAccessedBy mappings
// This function should only be called with managed va_blocks.
//
// va_block_context must not be NULL and policy for the region must match.
// See the comments for uvm_va_block_check_policy_is_valid().
//
// LOCKING: This takes and releases the VA block lock. If va_block_context->mm
//          != NULL, va_block_context->mm->mmap_lock must be held in at least
//          read mode.
NV_STATUS uvm_va_block_unset_read_duplication(uvm_va_block_t *va_block,
                                              uvm_va_block_context_t *va_block_context);

// Check if processor_id is allowed to access the va_block with access_type
// permissions. Return values:
//
// NV_ERR_INVALID_ADDRESS       The VA block is logically dead (zombie)
// NV_ERR_INVALID_ACCESS_TYPE   The vma corresponding to the VA range does not
//                              allow access_type permissions, or migration is
//                              disallowed and processor_id cannot access the
//                              range remotely (UVM-Lite).
// NV_ERR_INVALID_OPERATION     The access would violate the policies specified
//                              by UvmPreventMigrationRangeGroups.
//
// va_block_context must not be NULL, policy must match, and if the va_block is
// a HMM block, va_block_context->hmm.vma must be valid which also means the
// va_block_context->mm is not NULL, retained, and locked for at least read.
// Locking: the va_block lock must be held.
NV_STATUS uvm_va_block_check_logical_permissions(uvm_va_block_t *va_block,
                                                 uvm_va_block_context_t *va_block_context,
                                                 uvm_processor_id_t processor_id,
                                                 uvm_page_index_t page_index,
                                                 uvm_fault_access_type_t access_type,
                                                 bool allow_migration);

// API for access privilege revocation
//
// Revoke prot_to_revoke access permissions for the given processor.
//
// The revoked pages are described by the region parameter and the revoke page
// mask that allows the caller to restrict the revoke operation to specific
// pages within the region.
//
// prot_to_revoke must be greater than UVM_PROT_READ_ONLY. Caller should call
// unmap explicitly if it wants to revoke all access privileges.
//
// If id is UVM_ID_CPU, and prot_to_revoke is UVM_PROT_READ_WRITE_ATOMIC, no
// action is performed. If the processor id corresponds to the CPU and the
// caller cannot establish CPU mappings because it does not have a reference on
// vma->vm_mm (va_block_context->mm != vma->vm_mm), the page will be simply
// unmapped. Caller should call unmap explicitly if it wants to revoke all
// access privileges.
//
// Any CPU revocation will wait for the va_block tracker. If this function
// pushes GPU work it will first acquire the va_block tracker, then add the
// pushed work to out_tracker. It is the caller's responsibility to add this
// work to va_block's tracker. Note that while it is generally safe to run
// revocation operations on different GPUs concurrently, two PTE operations
// (map, unmap, revoke) on the same GPU must be serialized even if they target
// different pages because the earlier operation can cause a PTE split or merge
// which is assumed by the later operation.
//
// va_block_context must not be NULL.
//
// If allocation-retry was required as part of the operation and was successful,
// NV_ERR_MORE_PROCESSING_REQUIRED is returned. In this case, the entries in the
// out_tracker were added to the block's tracker and then the block's lock was
// unlocked and relocked.
//
// In general, any status other than NV_OK indicates that the block's lock might
// have been unlocked and relocked.
//
// LOCKING: The caller must hold the va block lock. If va_block_context->mm !=
//          NULL, va_block_context->mm->mmap_lock must be held in at least read
//          mode.
NV_STATUS uvm_va_block_revoke_prot(uvm_va_block_t *va_block,
                                   uvm_va_block_context_t *va_block_context,
                                   uvm_processor_id_t id,
                                   uvm_va_block_region_t region,
                                   const uvm_page_mask_t *revoke_page_mask,
                                   uvm_prot_t prot_to_revoke,
                                   uvm_tracker_t *out_tracker);

// Like uvm_va_block_revoke_prot(), except it revokes all processors in the
// input mask. The VA block tracker contains all revocation operations on
// return.
//
// Note that this can return NV_ERR_MORE_PROCESSING_REQUIRED just like
// uvm_va_block_revoke_prot() indicating that the operation needs to be retried.
NV_STATUS uvm_va_block_revoke_prot_mask(uvm_va_block_t *va_block,
                                        uvm_va_block_context_t *va_block_context,
                                        const uvm_processor_mask_t *revoke_processor_mask,
                                        uvm_va_block_region_t region,
                                        const uvm_page_mask_t *revoke_page_mask,
                                        uvm_prot_t prot_to_revoke);

// Tries to map all pages in the given region and map_page_mask with at most
// max_prot privileges for appropriate processors as determined by the
// accessed_by mask, heuristics and the given processor mask (excluding
// processor_id, which triggered the migration and should have already been
// mapped).
//
// va_block_context must not be NULL and policy for the region must match.
// See the comments for uvm_va_block_check_policy_is_valid().
//
// This function acquires/waits for the va_block tracker and updates that
// tracker with any new work pushed.
//
// Note that this can return NV_ERR_MORE_PROCESSING_REQUIRED just like
// uvm_va_block_map() indicating that the operation needs to be retried.
//
// LOCKING: The caller must hold the va block lock. If va_block_context->mm !=
//          NULL, va_block_context->mm->mmap_lock must be held in at least read
//          mode.
NV_STATUS uvm_va_block_add_mappings_after_migration(uvm_va_block_t *va_block,
                                                    uvm_va_block_context_t *va_block_context,
                                                    uvm_processor_id_t new_residency,
                                                    uvm_processor_id_t processor_id,
                                                    uvm_va_block_region_t region,
                                                    const uvm_page_mask_t *map_page_mask,
                                                    uvm_prot_t max_prot,
                                                    const uvm_processor_mask_t *processor_mask);

// Maps processors using SetAccessedBy to all resident pages in the region
// parameter. On Volta+ it is also used to map evicted pages that can be later
// pulled back by using access counters.
//
// This function acquires/waits for the va_block tracker and updates that
// tracker with any new work pushed.
//
// Note that this can return NV_ERR_MORE_PROCESSING_REQUIRED just like
// uvm_va_block_map() indicating that the operation needs to be retried.
//
// va_block_context must not be NULL and policy must for the region must match.
// See the comments for uvm_va_block_check_policy_is_valid().
//
// LOCKING: The caller must hold the va block lock. If va_block_context->mm !=
//          NULL, va_block_context->mm->mmap_lock must be held in at least read
//          mode.
NV_STATUS uvm_va_block_add_mappings(uvm_va_block_t *va_block,
                                    uvm_va_block_context_t *va_block_context,
                                    uvm_processor_id_t processor_id,
                                    uvm_va_block_region_t region,
                                    const uvm_page_mask_t *page_mask,
                                    UvmEventMapRemoteCause cause);

// Notifies the VA block that a new GPU VA space has been created.
// LOCKING: The caller must hold the va_block lock
NV_STATUS uvm_va_block_add_gpu_va_space(uvm_va_block_t *va_block, uvm_gpu_va_space_t *gpu_va_space);

// Destroys the VA block's mappings and page tables on the GPU, if it has any.
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// va_block_context must not be NULL.
//
// LOCKING: The caller must hold the va_block lock. If block_context->mm is not
// NULL, the caller must hold mm->mmap_lock in at least read mode.
void uvm_va_block_remove_gpu_va_space(uvm_va_block_t *va_block,
                                      uvm_gpu_va_space_t *gpu_va_space,
                                      uvm_va_block_context_t *block_context);

// Unmaps all page tables in this VA block which have peer mappings between
// the two GPUs, in either direction.
// LOCKING: The caller must hold the va_block lock
void uvm_va_block_disable_peer(uvm_va_block_t *va_block, uvm_gpu_t *gpu0, uvm_gpu_t *gpu1);

// Unmap any mappings from GPU to the preferred location.
//
// The GPU has to be in UVM-Lite mode.
//
// LOCKING: The caller must hold the va_block lock
void uvm_va_block_unmap_preferred_location_uvm_lite(uvm_va_block_t *va_block, uvm_gpu_t *gpu);

// Frees all memory under this block associated with this GPU. Any portion of
// the block which is resident on the GPU is evicted to sysmem before being
// freed.
//
// If mm != NULL, that mm is used for any CPU mappings which may be created as
// a result of this call. See uvm_va_block_context_t::mm for details.
//
// LOCKING: This takes and releases the VA block lock. If mm != NULL, the caller
//          must hold mm->mmap_lock in at least read mode.
void uvm_va_block_unregister_gpu(uvm_va_block_t *va_block, uvm_gpu_t *gpu, struct mm_struct *mm);

// Same as uvm_va_block_unregister_gpu() but the VA block lock must be held.
// Note that this handles allocation-retry internally and hence might unlock
// and relock block's lock.
void uvm_va_block_unregister_gpu_locked(uvm_va_block_t *va_block, uvm_gpu_t *gpu, struct mm_struct *mm);

// Unmaps all memory associated with the block and drops the ref count of the
// block. This allows the caller to free resources associated with this block
// regardless of the block's current ref count. Most importantly it allows the
// VA covered by this block to be immediately available for other page table
// mappings upon return.
//
// This clears block->managed_range, so only the managed range destroy path
// should call it. Other paths with references on this block, specifically the
// eviction path which temporarily takes a reference to the block, must always
// check the block state after taking the block lock to see if their mapping is
// still in place.
//
// All of the unmap and state destruction steps are also performed when the ref
// count goes to 0, so this function only needs to be called if the block's
// resources need to be reclaimed immediately.
//
// The caller should not lock the block before calling this function.
//
// This performs a uvm_va_block_release.
void uvm_va_block_kill(uvm_va_block_t *va_block);

// Exactly the same split semantics as uvm_va_range_split, including error
// handling. See that function's comments for details.
//
// new_va_block's managed range is set to new_managed_range before any reverse
// mapping is established to the new block, but the caller is responsible for
// inserting the new block into the range.
NV_STATUS uvm_va_block_split(uvm_va_block_t *existing_va_block,
                             NvU64 new_end,
                             uvm_va_block_t **new_va_block,
                             uvm_va_range_managed_t *new_managed_range);

// Exactly the same split semantics as uvm_va_block_split, including error
// handling except the existing_va_block block lock needs to be held and
// the new_va_block has to be preallocated.
// Also note that the existing_va_block lock may be dropped and re-acquired.
NV_STATUS uvm_va_block_split_locked(uvm_va_block_t *existing_va_block,
                                    NvU64 new_end,
                                    uvm_va_block_t *new_va_block);

// Handles a CPU fault in the given VA block, performing any operations
// necessary to establish a coherent CPU mapping (migrations, cache invalidates,
// etc.).
//
// Locking:
//  - vma->vm_mm->mmap_lock must be held in at least read mode. Note, that
//    might not be the same as current->mm->mmap_lock.
//  - va_space lock must be held in at least read mode
//
// service_context->block_context.mm is ignored and vma->vm_mm is used instead.
//
// Returns NV_ERR_INVALID_ACCESS_TYPE if a CPU mapping to fault_addr cannot be
// accessed, for example because it's within a range group which is non-
// migratable.
NV_STATUS uvm_va_block_cpu_fault(uvm_va_block_t *va_block,
                                 NvU64 fault_addr,
                                 bool is_write,
                                 uvm_service_block_context_t *service_context);

// Performs any operations necessary to establish a coherent mapping
// (migrations, cache invalidates, etc.) in response to the given service block
// context.
//
// service_context must not be NULL and policy for service_context->region must
// match. See the comments for uvm_va_block_check_policy_is_valid().  If
// va_block is a HMM block, va_block_context->hmm.vma must be valid.  See the
// comments for uvm_hmm_check_context_vma_is_valid() in uvm_hmm.h.
// service_context->prefetch_hint is set by this function.
//
// Locking:
//  - service_context->block_context.mm->mmap_lock must be held in at least
//    read mode, if valid.
//  - va_space lock must be held in at least read mode
//  - va_block lock must be held
//
// If allocation-retry was required as part of the operation and was successful,
// NV_ERR_MORE_PROCESSING_REQUIRED is returned. In this case, the block's lock
// was unlocked and relocked.
//
// NV_WARN_MORE_PROCESSING_REQUIRED indicates that thrashing has been detected
// and the performance heuristics logic decided to throttle execution.
// Any other error code different than NV_OK indicates OOM or a global fatal
// error.
// NV_WARN_MISMATCHED_TARGET is a special case of GPU fault handling when a
// GPU is chosen as the destination and the source is a HMM CPU page that can't
// be migrated. In that case, uvm_va_block_select_residency() should be called
// with 'hmm_migratable' set to true so that system memory will be selected.
NV_STATUS uvm_va_block_service_locked(uvm_processor_id_t processor_id,
                                      uvm_va_block_t *va_block,
                                      uvm_va_block_retry_t *block_retry,
                                      uvm_service_block_context_t *service_context);

// Performs population of the destination pages, unmapping and copying source
// pages to new_residency.
//
// service_context must not be NULL and policy for service_context->region must
// match.  See the comments for uvm_va_block_check_policy_is_valid().  If
// va_block is a HMM block, va_block_context->hmm.vma must be valid.  See the
// comments for uvm_hmm_check_context_vma_is_valid() in uvm_hmm.h.
// service_context->prefetch_hint should be set before calling this function.
//
// Locking:
//  - service_context->block_context.mm->mmap_lock must be held in at least
//    read mode, if valid.
//  - va_space lock must be held in at least read mode
//  - va_block lock must be held
//
// If allocation-retry was required as part of the operation and was successful,
// NV_ERR_MORE_PROCESSING_REQUIRED is returned. In this case, the block's lock
// was unlocked and relocked.
//
// NV_WARN_MORE_PROCESSING_REQUIRED indicates that thrashing has been detected
// and the performance heuristics logic decided to throttle execution.
// Any other error code different than NV_OK indicates OOM or a global fatal
// error.
NV_STATUS uvm_va_block_service_copy(uvm_processor_id_t processor_id,
                                    uvm_processor_id_t new_residency,
                                    uvm_va_block_t *va_block,
                                    uvm_va_block_retry_t *block_retry,
                                    uvm_service_block_context_t *service_context);

// This updates the va_block residency state and maps the faulting processor_id
// to the new residency (which may be remote).
//
// service_context must not be NULL and policy for service_context->region must
// match. See the comments for uvm_va_block_check_policy_is_valid().  If
// va_block is a HMM block, va_block_context->hmm.vma must be valid.  See the
// comments for uvm_hmm_check_context_vma_is_valid() in uvm_hmm.h.
// service_context must be initialized by calling uvm_va_block_service_copy()
// before calling this function.
//
// Locking:
//  - service_context->block_context.mm->mmap_lock must be held in at least
//    read mode, if valid.
//  - va_space lock must be held in at least read mode
//  - va_block lock must be held
//  - the mmap lock and va_space lock must be held across the calls to
//    uvm_va_block_service_copy() and this function. If the va_block lock is
//    dropped inbetween, special care is needed to check for eviction and
//    invalidation callbacks.
//
// If allocation-retry was required as part of the operation and was successful,
// NV_ERR_MORE_PROCESSING_REQUIRED is returned. In this case, the block's lock
// was unlocked and relocked.
//
// NV_WARN_MORE_PROCESSING_REQUIRED indicates that thrashing has been detected
// and the performance heuristics logic decided to throttle execution.
// Any other error code different than NV_OK indicates OOM or a global fatal
// error.
NV_STATUS uvm_va_block_service_finish(uvm_processor_id_t processor_id,
                                      uvm_va_block_t *va_block,
                                      uvm_service_block_context_t *service_context);

// Allocate GPU state for the given va_block and registered GPUs.
// Locking: The block lock must be held.
NV_STATUS uvm_va_block_gpu_state_alloc(uvm_va_block_t *va_block);

// Release any GPU or policy data associated with the given region in response
// to munmap().
// Locking: The va_block lock must be held.
void uvm_va_block_munmap_region(uvm_va_block_t *va_block,
                                uvm_va_block_region_t region);

// Size of the block in bytes. Guaranteed to be a page-aligned value between
// PAGE_SIZE and UVM_VA_BLOCK_SIZE.
static inline NvU64 uvm_va_block_size(uvm_va_block_t *block)
{
    NvU64 size = block->end - block->start + 1;
    UVM_ASSERT(PAGE_ALIGNED(size));
    UVM_ASSERT(size >= PAGE_SIZE);
    UVM_ASSERT(size <= UVM_VA_BLOCK_SIZE);
    return size;
}

// Number of pages with PAGE_SIZE in the block
static inline size_t uvm_va_block_num_cpu_pages(uvm_va_block_t *block)
{
    return uvm_va_block_size(block) / PAGE_SIZE;
}

// VA of the given page using CPU page size. page_index must be valid
static inline NvU64 uvm_va_block_cpu_page_address(uvm_va_block_t *block, uvm_page_index_t page_index)
{
    UVM_ASSERT(page_index < uvm_va_block_num_cpu_pages(block));
    return block->start + PAGE_SIZE * page_index;
}

// Get the physical address on the given GPU for given residency
uvm_gpu_phys_address_t uvm_va_block_res_phys_page_address(uvm_va_block_t *va_block,
                                                          uvm_page_index_t page_index,
                                                          uvm_processor_id_t residency,
                                                          uvm_gpu_t *gpu);

// Get the page physical address on the given GPU
//
// This will assert that GPU state is indeed present.
uvm_gpu_phys_address_t uvm_va_block_gpu_phys_page_address(uvm_va_block_t *va_block,
                                                          uvm_page_index_t page_index,
                                                          uvm_gpu_t *gpu);

static bool uvm_va_block_contains_address(uvm_va_block_t *block, NvU64 address)
{
    return address >= block->start && address <= block->end;
}

// Obtain a pointer to the uvm_va_block_test_t structure for the given VA
// block. If uvm_enable_builtin_tests is unset, NULL will be returned.
static uvm_va_block_test_t *uvm_va_block_get_test(uvm_va_block_t *va_block)
{
    if (uvm_enable_builtin_tests)
        return &container_of(va_block, uvm_va_block_wrapper_t, block)->test;

    return NULL;
}

// Get the page residency mask for a processor if it's known to be there.
//
// If the processor is the CPU, the residency mask for the NUMA node ID
// specified by nid will be returned (see
// uvm_va_block_cpu_node_state_t::resident). If nid is NUMA_NO_NODE,
// the cumulative CPU residency mask will be returned (see
// uvm_va_block_t::cpu::resident).
//
// If the processor is a GPU, this will assert that GPU state is indeed present.
uvm_page_mask_t *uvm_va_block_resident_mask_get(uvm_va_block_t *block, uvm_processor_id_t processor, int nid);

// Get the page mapped mask for a processor. The returned mask cannot be
// directly modified by the caller
//
// If the processor is a GPU, this will assert that GPU state is indeed present.
const uvm_page_mask_t *uvm_va_block_map_mask_get(uvm_va_block_t *block, uvm_processor_id_t processor);

// Return a mask of non-UVM-Lite pages that are unmapped within the given
// region.
// Locking: The block lock must be held.
void uvm_va_block_unmapped_pages_get(uvm_va_block_t *va_block,
                                     uvm_va_block_region_t region,
                                     uvm_page_mask_t *out_mask);

// VA block lookup functions. There are a number of permutations which might be
// useful, such as looking up the block from {va_space, va_range} x {addr,
// block index}. The ones implemented here and in uvm_va_range.h support the
// primary three use cases, which are:
// 1) Iterating over all VA blocks in a VA range. This uses block indices on the
//    VA range:
//      uvm_va_range_num_blocks
//      uvm_va_range_block_index
//      uvm_va_range_block
//      uvm_va_range_block_create
// 2) Operating on a single VA block (fault). This looks up the block using the
//    VA space and address:
//      uvm_va_block_find
//      uvm_va_block_find_create
// 3) Operating on a single VA block (fault). This looks up the block using the
//    supplied VA range and address:
//      uvm_va_block_find_create_in_range

// Finds the UVM or HMM VA block containing addr, if any. The va_space->lock
// must be held in at least read mode. Return values:
// NV_ERR_INVALID_ADDRESS   addr is not a UVM_VA_RANGE_TYPE_MANAGED va_range nor
//                          a HMM enabled VMA.
//
// NV_ERR_OBJECT_NOT_FOUND  addr is valid but no block has been allocated to
//                          cover it yet
//
// NV_OK                    The block was returned successfully
NV_STATUS uvm_va_block_find(uvm_va_space_t *va_space, NvU64 addr, uvm_va_block_t **out_block);

// Same as uvm_va_block_find except that the block is created if not found.
// If addr is covered by a UVM_VA_RANGE_TYPE_MANAGED va_range a managed block
// will be created. If addr is not covered by any va_range and HMM is
// enabled in the va_space then a HMM block will be created and hmm_vma is
// set to the VMA covering 'addr'. The va_space_mm must be retained and locked.
// Otherwise hmm_vma is set to NULL.
// Return values:
// NV_ERR_INVALID_ADDRESS   addr is not a UVM_VA_RANGE_TYPE_MANAGED va_range nor
//                          a HMM enabled VMA.
// NV_ERR_NO_MEMORY         memory could not be allocated.
NV_STATUS uvm_va_block_find_create(uvm_va_space_t *va_space,
                                   NvU64 addr,
                                   struct vm_area_struct **hmm_vma,
                                   uvm_va_block_t **out_block);

// Same as uvm_va_block_find_create except that only managed va_blocks are
// created if not already present in the VA range. Does not require va_space_mm
// to be locked or retained.
NV_STATUS uvm_va_block_find_create_managed(uvm_va_space_t *va_space,
                                           NvU64 addr,
                                           uvm_va_block_t **out_block);

// Same as uvm_va_block_find_create_managed except that va_range lookup was
// already done by the caller. The supplied va_range must not be NULL.
NV_STATUS uvm_va_block_find_create_in_range(uvm_va_space_t *va_space,
                                            uvm_va_range_t *va_range,
                                            NvU64 addr,
                                            uvm_va_block_t **out_block);

// Look up a chunk backing a specific address within the VA block.
// Returns NULL if none.
uvm_gpu_chunk_t *uvm_va_block_lookup_gpu_chunk(uvm_va_block_t *va_block, uvm_gpu_t *gpu, NvU64 address);

// Implementation of the UvmMigrate() API at the VA block scope.
//
// The out_tracker can be NULL.
//
// If do_mappings is false, mappings are not added after pages have been
// migrated.
//
// The caller needs to handle allocation-retry. va_block_retry can be NULL if
// the destination is the CPU.
//
// service_context and service_context->block_context must not be NULL and
// policy for the region must match. See the comments for
// uvm_va_block_check_policy_is_valid().  If va_block is a HMM block,
// service->block_context->hmm.vma must be valid.  See the comments for
// uvm_hmm_check_context_vma_is_valid() in uvm_hmm.h.
//
// LOCKING: The caller must hold the va_block lock. If
//          service_context->va_block_context->mm != NULL,
//          service_context->va_block_context->mm->mmap_lock must be held in at
//          least read mode.
NV_STATUS uvm_va_block_migrate_locked(uvm_va_block_t *va_block,
                                      uvm_va_block_retry_t *va_block_retry,
                                      uvm_service_block_context_t *service_context,
                                      uvm_va_block_region_t region,
                                      uvm_processor_id_t dest_id,
                                      uvm_migrate_mode_t mode,
                                      uvm_tracker_t *out_tracker);

// Write block's data from a CPU buffer
//
// The [dst, dst + size) range has to fit within a single PAGE_SIZE page.
//
// va_block_context must not be NULL. The caller is not required to set
// va_block_context->hmm.vma.
//
// The caller needs to support allocation-retry of page tables.
//
// LOCKING: The caller must hold the va_block lock
NV_STATUS uvm_va_block_write_from_cpu(uvm_va_block_t *va_block,
                                      uvm_va_block_context_t *block_context,
                                      NvU64 dst,
                                      uvm_mem_t *src,
                                      size_t size);

// Read block's data into a CPU buffer
//
// The [src, src + size) range has to fit within a single PAGE_SIZE page.
//
// LOCKING: The caller must hold the va_block lock
NV_STATUS uvm_va_block_read_to_cpu(uvm_va_block_t *va_block,
                                   uvm_va_block_context_t *va_block_context,
                                   uvm_mem_t *dst,
                                   NvU64 src,
                                   size_t size);

// Initialize va block retry tracking
void uvm_va_block_retry_init(uvm_va_block_retry_t *uvm_va_block_retry);

// Deinitialize va block retry tracking after a block operation
//
// Frees all the remaining free chunks and unpins all the used chunks.
void uvm_va_block_retry_deinit(uvm_va_block_retry_t *uvm_va_block_retry, uvm_va_block_t *va_block);

// Evict all chunks from the block that are subchunks of the passed in root_chunk.
//
// Add all the work tracking the eviction to the tracker.
//
// Returns NV_OK if the block is dead or doesn't have any subchunks of the
// root_chunk.
//
// LOCKING: The caller must hold the va_block lock
NV_STATUS uvm_va_block_evict_chunks(uvm_va_block_t *va_block,
                                    uvm_gpu_t *gpu,
                                    uvm_gpu_chunk_t *root_chunk,
                                    uvm_tracker_t *tracker);

NV_STATUS uvm_test_va_block_inject_error(UVM_TEST_VA_BLOCK_INJECT_ERROR_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_change_pte_mapping(UVM_TEST_CHANGE_PTE_MAPPING_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_va_block_info(UVM_TEST_VA_BLOCK_INFO_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_va_residency_info(UVM_TEST_VA_RESIDENCY_INFO_PARAMS *params, struct file *filp);

// Compute the offset in system pages of addr from the start of va_block.
static uvm_page_index_t uvm_va_block_cpu_page_index(uvm_va_block_t *va_block, NvU64 addr)
{
    UVM_ASSERT(addr >= va_block->start);
    UVM_ASSERT(addr <= va_block->end);
    return (addr - va_block->start) / PAGE_SIZE;
}

// Computes the size and index in the gpu_state chunks array of the GPU chunk
// which corresponds to the given page_index of the VA region.
// Note this is only used for testing and does not work on HMM va_blocks as it
// returns incorrect results for those.
size_t uvm_va_block_gpu_chunk_index_range(NvU64 start,
                                          NvU64 size,
                                          uvm_gpu_t *gpu,
                                          uvm_page_index_t page_index,
                                          uvm_chunk_size_t *out_chunk_size);

// If there are any resident CPU pages in the block, mark them as dirty
void uvm_va_block_mark_cpu_dirty(uvm_va_block_t *va_block);

// Sets the internal state required to handle fault cancellation
//
// This function may require allocating page tables to split big pages into 4K
// pages. If allocation-retry was required as part of the operation and was
// successful, NV_ERR_MORE_PROCESSING_REQUIRED is returned. In this case the
// block's lock was unlocked and relocked.
//
// va_block_context must not be NULL.
//
// LOCKING: The caller must hold the va_block lock.
NV_STATUS uvm_va_block_set_cancel(uvm_va_block_t *va_block, uvm_va_block_context_t *block_context, uvm_gpu_t *gpu);

//
// uvm_va_block_region_t helpers
//

static uvm_va_block_region_t uvm_va_block_region(uvm_page_index_t first, uvm_page_index_t outer)
{
    BUILD_BUG_ON(PAGES_PER_UVM_VA_BLOCK >= (1 << (sizeof(first) * 8)));

    UVM_ASSERT(first <= outer);

    return (uvm_va_block_region_t){ .first = first, .outer = outer };
}

static uvm_va_block_region_t uvm_va_block_region_for_page(uvm_page_index_t page_index)
{
    return uvm_va_block_region(page_index, page_index + 1);
}

static size_t uvm_va_block_region_num_pages(uvm_va_block_region_t region)
{
    return region.outer - region.first;
}

static NvU64 uvm_va_block_region_size(uvm_va_block_region_t region)
{
    return uvm_va_block_region_num_pages(region) * PAGE_SIZE;
}

static NvU64 uvm_va_block_region_start(uvm_va_block_t *va_block, uvm_va_block_region_t region)
{
    return va_block->start + region.first * PAGE_SIZE;
}

static NvU64 uvm_va_block_region_end(uvm_va_block_t *va_block, uvm_va_block_region_t region)
{
    return va_block->start + region.outer * PAGE_SIZE - 1;
}

static bool uvm_va_block_region_contains_region(uvm_va_block_region_t region, uvm_va_block_region_t subregion)
{
    return subregion.first >= region.first && subregion.outer <= region.outer;
}

static bool uvm_va_block_region_contains_page(uvm_va_block_region_t region, uvm_page_index_t page_index)
{
    return uvm_va_block_region_contains_region(region, uvm_va_block_region_for_page(page_index));
}

// Create a block range from a va block and start and end virtual addresses
// within the block.
static uvm_va_block_region_t uvm_va_block_region_from_start_end(uvm_va_block_t *va_block, NvU64 start, NvU64 end)
{
    uvm_va_block_region_t region;

    UVM_ASSERT(start < end);
    UVM_ASSERT(start >= va_block->start);
    UVM_ASSERT(end <= va_block->end);
    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(end + 1));

    region.first = uvm_va_block_cpu_page_index(va_block, start);
    region.outer = uvm_va_block_cpu_page_index(va_block, end) + 1;

    return region;
}

static uvm_va_block_region_t uvm_va_block_region_from_start_size(uvm_va_block_t *va_block, NvU64 start, NvU64 size)
{
    return uvm_va_block_region_from_start_end(va_block, start, start + size - 1);
}

static uvm_va_block_region_t uvm_va_block_region_from_block(uvm_va_block_t *va_block)
{
    return uvm_va_block_region(0, uvm_va_block_num_cpu_pages(va_block));
}

// Create a block region from a va block and page mask. If va_block is NULL, the
// region is assumed to cover the maximum va_block size. Note that the region
// covers the first through the last set bit and may have unset bits in between.
static uvm_va_block_region_t uvm_va_block_region_from_mask(uvm_va_block_t *va_block, const uvm_page_mask_t *page_mask)
{
    uvm_va_block_region_t region;
    uvm_page_index_t outer;

    if (va_block)
        outer = uvm_va_block_num_cpu_pages(va_block);
    else
        outer = PAGES_PER_UVM_VA_BLOCK;

    region.first = find_first_bit(page_mask->bitmap, outer);
    if (region.first >= outer) {
        region = uvm_va_block_region(0, 0);
    }
    else {
        // At least one bit is set so find_last_bit() should not return 'outer'.
        region.outer = find_last_bit(page_mask->bitmap, outer) + 1;
        UVM_ASSERT(region.outer <= outer);
    }

    return region;
}

static bool uvm_page_mask_test(const uvm_page_mask_t *mask, uvm_page_index_t page_index)
{
    UVM_ASSERT(page_index < PAGES_PER_UVM_VA_BLOCK);

    return test_bit(page_index, mask->bitmap);
}

static bool uvm_page_mask_test_and_set(uvm_page_mask_t *mask, uvm_page_index_t page_index)
{
    UVM_ASSERT(page_index < PAGES_PER_UVM_VA_BLOCK);

    return __test_and_set_bit(page_index, mask->bitmap);
}

static bool uvm_page_mask_test_and_clear(uvm_page_mask_t *mask, uvm_page_index_t page_index)
{
    UVM_ASSERT(page_index < PAGES_PER_UVM_VA_BLOCK);

    return __test_and_clear_bit(page_index, mask->bitmap);
}

static void uvm_page_mask_set(uvm_page_mask_t *mask, uvm_page_index_t page_index)
{
    UVM_ASSERT(page_index < PAGES_PER_UVM_VA_BLOCK);

    __set_bit(page_index, mask->bitmap);
}

static void uvm_page_mask_clear(uvm_page_mask_t *mask, uvm_page_index_t page_index)
{
    UVM_ASSERT(page_index < PAGES_PER_UVM_VA_BLOCK);

    __clear_bit(page_index, mask->bitmap);
}

static bool uvm_page_mask_region_test(const uvm_page_mask_t *mask,
                                      uvm_va_block_region_t region,
                                      uvm_page_index_t page_index)
{
    if (!uvm_va_block_region_contains_page(region, page_index))
        return false;

    return !mask || uvm_page_mask_test(mask, page_index);
}

static NvU32 uvm_page_mask_region_weight(const uvm_page_mask_t *mask, uvm_va_block_region_t region)
{
    NvU32 weight_before = 0;

    if (region.first > 0)
        weight_before = bitmap_weight(mask->bitmap, region.first);

    return bitmap_weight(mask->bitmap, region.outer) - weight_before;
}

static bool uvm_page_mask_region_empty(const uvm_page_mask_t *mask, uvm_va_block_region_t region)
{
    return find_next_bit(mask->bitmap, region.outer, region.first) == region.outer;
}

static bool uvm_page_mask_region_full(const uvm_page_mask_t *mask, uvm_va_block_region_t region)
{
    return find_next_zero_bit(mask->bitmap, region.outer, region.first) == region.outer;
}

static void uvm_page_mask_region_fill(uvm_page_mask_t *mask, uvm_va_block_region_t region)
{
    bitmap_set(mask->bitmap, region.first, region.outer - region.first);
}

static void uvm_page_mask_region_clear(uvm_page_mask_t *mask, uvm_va_block_region_t region)
{
    bitmap_clear(mask->bitmap, region.first, region.outer - region.first);
}

static void uvm_page_mask_region_clear_outside(uvm_page_mask_t *mask, uvm_va_block_region_t region)
{
    if (region.first > 0)
        bitmap_clear(mask->bitmap, 0, region.first);
    if (region.outer < PAGES_PER_UVM_VA_BLOCK)
        bitmap_clear(mask->bitmap, region.outer, PAGES_PER_UVM_VA_BLOCK - region.outer);
}

static void uvm_page_mask_zero(uvm_page_mask_t *mask)
{
    bitmap_zero(mask->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static bool uvm_page_mask_empty(const uvm_page_mask_t *mask)
{
    return bitmap_empty(mask->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static bool uvm_page_mask_full(const uvm_page_mask_t *mask)
{
    return bitmap_full(mask->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static void uvm_page_mask_fill(uvm_page_mask_t *mask)
{
    bitmap_fill(mask->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static bool uvm_page_mask_and(uvm_page_mask_t *mask_out,
                              const uvm_page_mask_t *mask_in1,
                              const uvm_page_mask_t *mask_in2)
{
    return bitmap_and(mask_out->bitmap, mask_in1->bitmap, mask_in2->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static bool uvm_page_mask_andnot(uvm_page_mask_t *mask_out,
                                 const uvm_page_mask_t *mask_in1,
                                 const uvm_page_mask_t *mask_in2)
{
    return bitmap_andnot(mask_out->bitmap, mask_in1->bitmap, mask_in2->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static void uvm_page_mask_or(uvm_page_mask_t *mask_out,
                             const uvm_page_mask_t *mask_in1,
                             const uvm_page_mask_t *mask_in2)
{
    bitmap_or(mask_out->bitmap, mask_in1->bitmap, mask_in2->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static void uvm_page_mask_complement(uvm_page_mask_t *mask_out, const uvm_page_mask_t *mask_in)
{
    bitmap_complement(mask_out->bitmap, mask_in->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static void uvm_page_mask_copy(uvm_page_mask_t *mask_out, const uvm_page_mask_t *mask_in)
{
    bitmap_copy(mask_out->bitmap, mask_in->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static NvU32 uvm_page_mask_weight(const uvm_page_mask_t *mask)
{
    return bitmap_weight(mask->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static bool uvm_page_mask_subset(const uvm_page_mask_t *subset, const uvm_page_mask_t *mask)
{
    return bitmap_subset(subset->bitmap, mask->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static bool uvm_page_mask_equal(const uvm_page_mask_t *mask_in1, const uvm_page_mask_t *mask_in2)
{
    return bitmap_equal(mask_in1->bitmap, mask_in2->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

static bool uvm_page_mask_init_from_region(uvm_page_mask_t *mask_out,
                                           uvm_va_block_region_t region,
                                           const uvm_page_mask_t *mask_in)
{
    uvm_page_mask_zero(mask_out);
    uvm_page_mask_region_fill(mask_out, region);

    if (mask_in)
        return uvm_page_mask_and(mask_out, mask_out, mask_in);

    return true;
}

static void uvm_page_mask_shift_right(uvm_page_mask_t *mask_out, const uvm_page_mask_t *mask_in, unsigned shift)
{
    bitmap_shift_right(mask_out->bitmap, mask_in->bitmap, shift, PAGES_PER_UVM_VA_BLOCK);
}

static void uvm_page_mask_shift_left(uvm_page_mask_t *mask_out, const uvm_page_mask_t *mask_in, unsigned shift)
{
    bitmap_shift_left(mask_out->bitmap, mask_in->bitmap, shift, PAGES_PER_UVM_VA_BLOCK);
}

static bool uvm_page_mask_intersects(const uvm_page_mask_t *mask1, const uvm_page_mask_t *mask2)
{
    return bitmap_intersects(mask1->bitmap, mask2->bitmap, PAGES_PER_UVM_VA_BLOCK);
}

// Print the given page mask on the given buffer using hex symbols. The
// minimum required size of the buffer is UVM_PAGE_MASK_PRINT_MIN_BUFFER_SIZE.
static void uvm_page_mask_print(const uvm_page_mask_t *mask, char *buffer)
{
    // There are two cases, which depend on PAGE_SIZE
    if (PAGES_PER_UVM_VA_BLOCK > 32) {
        NvLength current_long_idx = UVM_PAGE_MASK_WORDS - 1;
        const char *buffer_end = buffer + UVM_PAGE_MASK_PRINT_MIN_BUFFER_SIZE;

        UVM_ASSERT(sizeof(*mask->bitmap) == 8);

        // For 4KB pages, we need to iterate over multiple words
        do {
            NvU64 current_long = mask->bitmap[current_long_idx];

            buffer += sprintf(buffer, "%016llx", current_long);
            if (current_long_idx != 0)
                buffer += sprintf(buffer, ":");
        } while (current_long_idx-- != 0);

        UVM_ASSERT(buffer <= buffer_end);
    }
    else {
        NvU32 value = (unsigned)*mask->bitmap;

        UVM_ASSERT(PAGES_PER_UVM_VA_BLOCK == 32);

        // For 64KB pages, a single print suffices
        sprintf(buffer, "%08x", value);
    }
}

static uvm_va_block_region_t uvm_va_block_first_subregion_in_mask(uvm_va_block_region_t region,
                                                                  const uvm_page_mask_t *page_mask)
{
    uvm_va_block_region_t subregion;

    if (!page_mask)
        return region;

    subregion.first = find_next_bit(page_mask->bitmap, region.outer, region.first);
    subregion.outer = find_next_zero_bit(page_mask->bitmap, region.outer, subregion.first + 1);
    return subregion;
}

static uvm_va_block_region_t uvm_va_block_next_subregion_in_mask(uvm_va_block_region_t region,
                                                                 const uvm_page_mask_t *page_mask,
                                                                 uvm_va_block_region_t previous_subregion)
{
    uvm_va_block_region_t subregion;

    if (!page_mask) {
        subregion.first = region.outer;
        subregion.outer = region.outer;
        return subregion;
    }

    subregion.first = find_next_bit(page_mask->bitmap, region.outer, previous_subregion.outer + 1);
    subregion.outer = find_next_zero_bit(page_mask->bitmap, region.outer, subregion.first + 1);
    return subregion;
}

// Iterate over contiguous subregions of the region given by the page mask.
// If the page mask is NULL then it behaves as if it was a fully set mask and
// the only subregion iterated over will be the region itself.
#define for_each_va_block_subregion_in_mask(subregion, page_mask, region)                       \
    for ((subregion) = uvm_va_block_first_subregion_in_mask((region), (page_mask));             \
         (subregion).first != (region).outer;                                                   \
         (subregion) = uvm_va_block_next_subregion_in_mask((region), (page_mask), (subregion)))

static uvm_page_index_t uvm_va_block_first_page_in_mask(uvm_va_block_region_t region,
                                                        const uvm_page_mask_t *page_mask)
{
    if (page_mask)
        return find_next_bit(page_mask->bitmap, region.outer, region.first);
    else
        return region.first;
}

static uvm_page_index_t uvm_va_block_next_page_in_mask(uvm_va_block_region_t region,
                                                       const uvm_page_mask_t *page_mask,
                                                       uvm_page_index_t previous_page)
{
    if (page_mask) {
        return find_next_bit(page_mask->bitmap, region.outer, previous_page + 1);
    }
    else {
        UVM_ASSERT(previous_page < region.outer);
        return previous_page + 1;
    }
}

static uvm_page_index_t uvm_va_block_first_unset_page_in_mask(uvm_va_block_region_t region,
                                                              const uvm_page_mask_t *page_mask)
{
    if (page_mask)
        return find_next_zero_bit(page_mask->bitmap, region.outer, region.first);
    else
        return region.first;
}

static uvm_page_index_t uvm_va_block_next_unset_page_in_mask(uvm_va_block_region_t region,
                                                             const uvm_page_mask_t *page_mask,
                                                             uvm_page_index_t previous_page)
{
    if (page_mask) {
        return find_next_zero_bit(page_mask->bitmap, region.outer, previous_page + 1);
    }
    else {
        UVM_ASSERT(previous_page < region.outer);
        return previous_page + 1;
    }
}

static NvU64 uvm_reverse_map_start(const uvm_reverse_map_t *reverse_map)
{
    return uvm_va_block_cpu_page_address(reverse_map->va_block, reverse_map->region.first);
}

static NvU64 uvm_reverse_map_end(const uvm_reverse_map_t *reverse_map)
{
    return uvm_va_block_cpu_page_address(reverse_map->va_block, reverse_map->region.first) +
           uvm_va_block_region_size(reverse_map->region) - 1;
}

// Iterate over contiguous pages of the region given by the page mask.
// If the page mask is NULL then it behaves as if it was a fully set mask and
// it will iterate over all pages within the region.
#define for_each_va_block_page_in_region_mask(page_index, page_mask, region)                 \
    for ((page_index) = uvm_va_block_first_page_in_mask((region), (page_mask));              \
         (page_index) != (region).outer;                                                     \
         (page_index) = uvm_va_block_next_page_in_mask((region), (page_mask), (page_index)))

// Same as for_each_va_block_page_in_region_mask, but the region spans the
// whole given VA block
#define for_each_va_block_page_in_mask(page_index, page_mask, va_block)                      \
    for_each_va_block_page_in_region_mask(page_index, page_mask, uvm_va_block_region_from_block(va_block))

// Similar to for_each_va_block_page_in_region_mask, but iterating over pages
// whose bit is unset.
#define for_each_va_block_unset_page_in_region_mask(page_index, page_mask, region)           \
    for ((page_index) = uvm_va_block_first_unset_page_in_mask((region), (page_mask));        \
         (page_index) != (region).outer;                                                     \
         (page_index) = uvm_va_block_next_unset_page_in_mask((region), (page_mask), (page_index)))

// Similar to for_each_va_block_page_in_mask, but iterating over pages whose
// bit is unset.
#define for_each_va_block_unset_page_in_mask(page_index, page_mask, va_block)                \
    for_each_va_block_unset_page_in_region_mask(page_index, page_mask, uvm_va_block_region_from_block(va_block))

// Iterate over all pages within the given region
#define for_each_va_block_page_in_region(page_index, region)                                 \
    for_each_va_block_page_in_region_mask((page_index), NULL, (region))

// Iterate over all pages within the given VA block
#define for_each_va_block_page(page_index, va_block)                                         \
    for_each_va_block_page_in_region((page_index), uvm_va_block_region_from_block(va_block))

// Return the first vma intersecting the region [start, va_block->end]
// or NULL if no such vma exists. Also returns the region covered by
// the vma within the va_block.
struct vm_area_struct *uvm_va_block_find_vma_region(uvm_va_block_t *va_block,
                                                    struct mm_struct *mm,
                                                    NvU64 start,
                                                    uvm_va_block_region_t *region);

// Iterate over all vma regions covered by a va_block
#define for_each_va_block_vma_region(va_block, mm, vma, region)                                 \
    for (vma = uvm_va_block_find_vma_region((va_block), (mm), (va_block)->start, (region));     \
         (vma);                                                                                 \
         vma = uvm_va_block_find_vma_region((va_block),                                         \
                                            (mm),                                               \
                                            uvm_va_block_region_end((va_block), *(region)) + 1, \
                                            (region)))

// Return the block region covered by the given chunk size. page_index must be
// any page within the block known to be covered by the chunk.
static uvm_va_block_region_t uvm_va_block_chunk_region(uvm_va_block_t *block,
                                                       uvm_chunk_size_t chunk_size,
                                                       uvm_page_index_t page_index)
{
    NvU64 page_addr = uvm_va_block_cpu_page_address(block, page_index);
    NvU64 chunk_start_addr = UVM_ALIGN_DOWN(page_addr, chunk_size);
    uvm_page_index_t first = (uvm_page_index_t)((chunk_start_addr - block->start) / PAGE_SIZE);
    return uvm_va_block_region(first, first + (chunk_size / PAGE_SIZE));
}

//
// Helpers for page state (permissions, size, residency)
//

bool uvm_va_block_page_is_gpu_authorized(uvm_va_block_t *va_block,
                                         uvm_page_index_t page_index,
                                         uvm_gpu_id_t gpu_id,
                                         uvm_prot_t required_prot);

// Compute the processors that have a copy of the given page resident in their
// memory.
void uvm_va_block_page_resident_processors(uvm_va_block_t *va_block,
                                           uvm_page_index_t page_index,
                                           uvm_processor_mask_t *resident_processors);

// Count how many processors have a copy of the given page resident in their
// memory.
NvU32 uvm_va_block_page_resident_processors_count(uvm_va_block_t *va_block,
                                                  uvm_va_block_context_t *va_block_context,
                                                  uvm_page_index_t page_index);

// Get the processor with a resident copy of a page closest to the given
// processor.
uvm_processor_id_t uvm_va_block_page_get_closest_resident(uvm_va_block_t *va_block,
                                                          uvm_va_block_context_t *va_block_context,
                                                          uvm_page_index_t page_index,
                                                          uvm_processor_id_t processor);

// Mark CPU page page_index as resident on NUMA node specified by nid.
// nid cannot be NUMA_NO_NODE.
void uvm_va_block_cpu_set_resident_page(uvm_va_block_t *va_block, int nid, uvm_page_index_t page_index);

// Test if a CPU page is resident on NUMA node nid. If nid is NUMA_NO_NODE,
// the function will return True if the page is resident on any CPU NUMA node.
bool uvm_va_block_cpu_is_page_resident_on(uvm_va_block_t *va_block, int nid, uvm_page_index_t page_index);

// Test if all pages in region are resident on NUMA node nid. If nid is
// NUMA_NO_NODE, the function will test if the pages in the region are
// resident on any CPU NUMA node.
bool uvm_va_block_cpu_is_region_resident_on(uvm_va_block_t *va_block, int nid, uvm_va_block_region_t region);

// Insert a CPU chunk at the given page_index into the va_block.
// Locking: The va_block lock must be held.
NV_STATUS uvm_cpu_chunk_insert_in_block(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Remove a CPU chunk at the given page_index from the va_block.
// nid cannot be NUMA_NO_NODE.
// Locking: The va_block lock must be held.
void uvm_cpu_chunk_remove_from_block(uvm_va_block_t *va_block, int nid, uvm_page_index_t page_index);

// Return the CPU chunk at the given page_index on the given NUMA node from the
// va_block. nid cannot be NUMA_NO_NODE.
// Locking: The va_block lock must be held.
uvm_cpu_chunk_t *uvm_cpu_chunk_get_chunk_for_page(uvm_va_block_t *va_block,
                                                  int nid,
                                                  uvm_page_index_t page_index);

// Return the CPU chunk for the given page_index from the first available NUMA
// node from the va_block. Should only be called for HMM va_blocks.
// Locking: The va_block lock must be held.
uvm_cpu_chunk_t *uvm_cpu_chunk_get_any_chunk_for_page(uvm_va_block_t *va_block, uvm_page_index_t page_index);

// Return the struct page * from the chunk corresponding to the given page_index
// Locking: The va_block lock must be held.
struct page *uvm_cpu_chunk_get_cpu_page(uvm_va_block_t *va_block, uvm_cpu_chunk_t *chunk, uvm_page_index_t page_index);

// Return the struct page * of the resident chunk at the given page_index from
// the va_block. The given page_index must be resident on the CPU.
// Locking: The va_block lock must be held.
struct page *uvm_va_block_get_cpu_page(uvm_va_block_t *va_block, uvm_page_index_t page_index);

// Physically map a CPU chunk so it is DMA'able from all registered GPUs.
// nid cannot be NUMA_NO_NODE.
// Locking: The va_block lock must be held.
NV_STATUS uvm_va_block_map_cpu_chunk_on_gpus(uvm_va_block_t *va_block,
                                             uvm_cpu_chunk_t *chunk,
                                             uvm_page_index_t page_index);

// Physically unmap a CPU chunk from all registered GPUs.
// Locking: The va_block lock must be held.
void uvm_va_block_unmap_cpu_chunk_on_gpus(uvm_va_block_t *va_block,
                                          uvm_cpu_chunk_t *chunk);

// Remove any CPU chunks in the given region.
// Locking: The va_block lock must be held.
void uvm_va_block_remove_cpu_chunks(uvm_va_block_t *va_block, uvm_va_block_region_t region);

// Get the size of the physical allocation backing the page at page_index on the
// specified processor in the block. Returns 0 if the address is not resident on
// the specified processor.
// Locking: The va_block lock must be held.
NvU64 uvm_va_block_get_physical_size(uvm_va_block_t *block,
                                     uvm_processor_id_t processor,
                                     uvm_page_index_t page_index);

// Get CPU page size or 0 if it is not mapped
NvU64 uvm_va_block_page_size_cpu(uvm_va_block_t *va_block,
                                 uvm_page_index_t page_index);

// Get GPU page size or 0 if it is not mapped on the given GPU
NvU64 uvm_va_block_page_size_gpu(uvm_va_block_t *va_block, uvm_gpu_id_t gpu_id, uvm_page_index_t page_index);

// Get page size or 0 if it is not mapped on the given processor
static NvU64 uvm_va_block_page_size_processor(uvm_va_block_t *va_block,
                                              uvm_processor_id_t processor_id,
                                              uvm_page_index_t page_index)
{
    if (UVM_ID_IS_CPU(processor_id))
        return uvm_va_block_page_size_cpu(va_block, page_index);
    else
        return uvm_va_block_page_size_gpu(va_block, processor_id, page_index);
}

// Returns the big page size for the GPU VA space of the block
NvU64 uvm_va_block_gpu_big_page_size(uvm_va_block_t *va_block, uvm_gpu_t *gpu);

// Returns the number of big pages in the VA block for the given size
size_t uvm_va_block_num_big_pages(uvm_va_block_t *va_block, NvU64 big_page_size);

// Returns the number of big pages in the VA block for the big page size on the
// given GPU
static size_t uvm_va_block_gpu_num_big_pages(uvm_va_block_t *va_block, uvm_gpu_t *gpu)
{
    return uvm_va_block_num_big_pages(va_block, uvm_va_block_gpu_big_page_size(va_block, gpu));
}

// Returns the start address of the given big page index and big page size
NvU64 uvm_va_block_big_page_addr(uvm_va_block_t *va_block, size_t big_page_index, NvU64 big_page_size);

// Returns the region [start, end] of the given big page index and big page size
uvm_va_block_region_t uvm_va_block_big_page_region(uvm_va_block_t *va_block,
                                                   size_t big_page_index,
                                                   NvU64 big_page_size);

// Returns the largest sub-region region of [start, end] which can fit big
// pages. If the region cannot fit any big pages, an invalid region (0, 0) is
// returned.
uvm_va_block_region_t uvm_va_block_big_page_region_all(uvm_va_block_t *va_block, NvU64 big_page_size);

// Returns the largest sub-region region of 'region' which can fit big pages.
// If the region cannot fit any big pages, an invalid region (0, 0) is returned.
uvm_va_block_region_t uvm_va_block_big_page_region_subset(uvm_va_block_t *va_block,
                                                          uvm_va_block_region_t region,
                                                          NvU64 big_page_size);

// Returns the big page index (the bit index within
// uvm_va_block_gpu_state_t::big_ptes) corresponding to page_index. If
// page_index cannot be covered by a big PTE due to alignment or block size,
// MAX_BIG_PAGES_PER_UVM_VA_BLOCK is returned.
size_t uvm_va_block_big_page_index(uvm_va_block_t *va_block, uvm_page_index_t page_index, NvU64 big_page_size);

// Returns the new residency for a page that faulted or triggered access counter
// notifications. The read_duplicate output parameter indicates if the page
// meets the requirements to be read-duplicated. va_block_context must not be
// NULL, and if the va_block is a HMM block, va_block_context->hmm.vma must be
// valid which also means the va_block_context->mm is not NULL, retained, and
// locked for at least read. See the comments for
// uvm_va_block_check_policy_is_valid() and uvm_hmm_check_context_vma_is_valid()
// in uvm_hmm.h. hmm_migratable should be true if the residency should be forced
// to be system memory.
// Locking: the va_block lock must be held.
uvm_processor_id_t uvm_va_block_select_residency(uvm_va_block_t *va_block,
                                                 uvm_va_block_context_t *va_block_context,
                                                 uvm_page_index_t page_index,
                                                 uvm_processor_id_t processor_id,
                                                 NvU32 access_type_mask,
                                                 const uvm_va_policy_t *policy,
                                                 const uvm_perf_thrashing_hint_t *thrashing_hint,
                                                 uvm_service_operation_t operation,
                                                 const bool hmm_migratable,
                                                 bool *read_duplicate);

// Return the maximum mapping protection for processor_id that will not require
// any permision revocation on the rest of processors.
uvm_prot_t uvm_va_block_page_compute_highest_permission(uvm_va_block_t *va_block,
                                                        uvm_va_block_context_t *va_block_context,
                                                        uvm_processor_id_t processor_id,
                                                        uvm_page_index_t page_index);

// Allocates a page for the given page_index in the va_block and maps
// it to the GPU.
// Locking: the va_block lock must be held.
NV_STATUS uvm_va_block_populate_page_cpu(uvm_va_block_t *va_block,
                                         uvm_page_index_t page_index,
                                         uvm_va_block_context_t *block_context);

// A helper macro for handling allocation-retry
//
// The macro takes a VA block, uvm_va_block_retry_t struct and a function call
// to retry as long as it returns NV_ERR_MORE_PROCESSING_REQUIRED.
//
// block_retry can be NULL if it's not necessary for the function call,
// otherwise it will be initialized and deinitialized by the macro.
//
// The macro also locks and unlocks the block's lock internally as it's expected
// that the block's lock has been unlocked and relocked whenever the function call
// returns NV_ERR_MORE_PROCESSING_REQUIRED and this makes it clear that the
// block's state is not locked across these calls.
#define UVM_VA_BLOCK_LOCK_RETRY(va_block, block_retry, call) ({     \
    NV_STATUS __status;                                             \
    uvm_va_block_t *__block = (va_block);                           \
    uvm_va_block_retry_t *__retry = (block_retry);                  \
                                                                    \
    uvm_va_block_retry_init(__retry);                               \
                                                                    \
    uvm_mutex_lock(&__block->lock);                                 \
                                                                    \
    do {                                                            \
        __status = (call);                                          \
    } while (__status == NV_ERR_MORE_PROCESSING_REQUIRED);          \
                                                                    \
    uvm_mutex_unlock(&__block->lock);                               \
                                                                    \
    uvm_va_block_retry_deinit(__retry, __block);                    \
                                                                    \
    __status;                                                       \
})

// A helper macro for handling allocation-retry
//
// The macro takes a VA block, uvm_va_block_retry_t struct and a function call
// to retry as long as it returns NV_ERR_MORE_PROCESSING_REQUIRED.
//
// block_retry can be NULL if it's not necessary for the function call,
// otherwise it will be initialized and deinitialized by the macro.
//
// This macro, as opposed to UVM_VA_BLOCK_LOCK_RETRY(), expects the block lock
// to be already taken. Notably the block's lock might be unlocked and relocked
// as part of the call.
#define UVM_VA_BLOCK_RETRY_LOCKED(va_block, block_retry, call) ({   \
    NV_STATUS __status;                                             \
    uvm_va_block_t *__block = (va_block);                           \
    uvm_va_block_retry_t *__retry = (block_retry);                  \
                                                                    \
    uvm_va_block_retry_init(__retry);                               \
                                                                    \
    uvm_assert_mutex_locked(&__block->lock);                        \
                                                                    \
    do {                                                            \
        __status = (call);                                          \
    } while (__status == NV_ERR_MORE_PROCESSING_REQUIRED);          \
                                                                    \
    uvm_va_block_retry_deinit(__retry, __block);                    \
                                                                    \
    __status;                                                       \
})

#endif // __UVM_VA_BLOCK_H__
