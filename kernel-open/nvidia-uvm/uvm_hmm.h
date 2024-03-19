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

#ifndef _UVM_HMM_H_
#define _UVM_HMM_H_

#include "nvtypes.h"
#include "uvm_forward_decl.h"
#include "uvm_va_block_types.h"
#include "uvm_va_policy.h"
#include "uvm_linux.h"
#include "uvm_range_tree.h"
#include "uvm_lock.h"

typedef struct
{
    // This stores pointers to uvm_va_block_t for HMM blocks.
    uvm_range_tree_t blocks;
    uvm_mutex_t blocks_lock;
} uvm_hmm_va_space_t;

#if UVM_IS_CONFIG_HMM()

    // Tells whether HMM is enabled for the given va_space.
    // If it is not enabled, all of the functions below are no-ops.
    bool uvm_hmm_is_enabled(uvm_va_space_t *va_space);

    // Self-explanatory name: reports if HMM is enabled system-wide.
    bool uvm_hmm_is_enabled_system_wide(void);

    // Initialize HMM for the given the va_space.
    void uvm_hmm_va_space_initialize(uvm_va_space_t *va_space);

    // Destroy any HMM state for the given the va_space.
    // Locking: va_space lock must be held in write mode.
    void uvm_hmm_va_space_destroy(uvm_va_space_t *va_space);

    // Unmap all page tables in this VA space which map memory owned by this
    // GPU. Any memory still resident on this GPU will be evicted to system
    // memory. Note that 'mm' can be NULL (e.g., when closing the UVM file)
    // in which case any GPU memory is simply freed.
    // Locking: if mm is not NULL, the caller must hold mm->mmap_lock in at
    // least read mode and the va_space lock must be held in write mode.
    void uvm_hmm_unregister_gpu(uvm_va_space_t *va_space, uvm_gpu_t *gpu, struct mm_struct *mm);

    // Destroy the VA space's mappings on the GPU, if it has any.
    // Locking: if mm is not NULL, the caller must hold mm->mmap_lock in at
    // least read mode and the va_space lock must be held in write mode.
    void uvm_hmm_remove_gpu_va_space(uvm_va_space_t *va_space,
                                     uvm_gpu_va_space_t *gpu_va_space,
                                     struct mm_struct *mm);

    // Find an existing HMM va_block.
    // This function can be called without having retained and locked the mm,
    // but in that case, the only allowed operations on the returned block are
    // locking the block, reading its state, and performing eviction. GPU fault
    // handling and user-initiated migrations are not allowed.
    // Return values are the same as uvm_va_block_find().
    // Locking: This must be called with va_space lock held in at least read
    // mode.
    NV_STATUS uvm_hmm_va_block_find(uvm_va_space_t *va_space,
                                    NvU64 addr,
                                    uvm_va_block_t **va_block_ptr);

    // Find or create a new HMM va_block.
    //
    // Return NV_ERR_INVALID_ADDRESS if there is no VMA associated with the
    // address 'addr' or the VMA does not have at least PROT_READ permission.
    // The caller is also responsible for checking that there is no UVM
    // va_range covering the given address before calling this function.
    // The VMA is returned in vma_out if it's not NULL.
    // Locking: This function must be called with mm retained and locked for
    // at least read and the va_space lock at least for read.
    NV_STATUS uvm_hmm_va_block_find_create(uvm_va_space_t *va_space,
                                           NvU64 addr,
                                           struct vm_area_struct **vma_out,
                                           uvm_va_block_t **va_block_ptr);

    // Find the VMA for the given address and return it in vma_out. Return
    // NV_ERR_INVALID_ADDRESS if mm is NULL or there is no VMA associated with
    // the address 'addr' or the VMA does not have at least PROT_READ
    // permission.
    // Locking: This function must be called with mm retained and locked for
    // at least read or mm equal to NULL.
    NV_STATUS uvm_hmm_find_vma(struct mm_struct *mm, struct vm_area_struct **vma_out, NvU64 addr);

    // If va_block is a HMM va_block, check that vma is not NULL and covers the
    // given region. This always returns true and is intended to only be used
    // with UVM_ASSERT().
    // Locking: This function must be called with the va_block lock held and if
    // va_block is a HMM block, va_space->va_space_mm.mm->mmap_lock must be
    // retained and locked for at least read.
    bool uvm_hmm_check_context_vma_is_valid(uvm_va_block_t *va_block,
                                            struct vm_area_struct *vma,
                                            uvm_va_block_region_t region);

    // Begin a migration critical section. When calling into the kernel it is
    // sometimes necessary to drop the va_block lock. This function returns
    // NV_OK when no other thread has started a migration critical section.
    // Otherwise, it returns NV_ERR_BUSY_RETRY and threads should then retry
    // this function to begin a critical section.
    // Locking: va_block lock must not be held.
    NV_STATUS uvm_hmm_migrate_begin(uvm_va_block_t *va_block);

    // Same as uvm_hmm_migrate_begin() but waits if required before beginning a
    // critical section.
    void uvm_hmm_migrate_begin_wait(uvm_va_block_t *va_block);

    // Finish a migration critical section.
    void uvm_hmm_migrate_finish(uvm_va_block_t *va_block);

    // Find or create a HMM va_block and mark it so the next va_block split
    // will fail for testing purposes.
    // Locking: This function must be called with mm retained and locked for
    // at least read and the va_space lock at least for read.
    NV_STATUS uvm_hmm_test_va_block_inject_split_error(uvm_va_space_t *va_space, NvU64 addr);

    // Reclaim any HMM va_blocks that overlap the given range.
    // Note that 'end' is inclusive. If mm is NULL, any HMM va_block in the
    // range will be reclaimed which assumes that the mm is being torn down
    // and was not retained.
    // Return values:
    // NV_ERR_NO_MEMORY: Reclaim required a block split, which failed.
    // NV_OK:            There were no HMM blocks in the range, or all HMM
    //                   blocks in the range were successfully reclaimed.
    // Locking: If mm is not NULL, it must equal va_space_mm.mm, the caller
    // must retain it with uvm_va_space_mm_or_current_retain() or be sure that
    // mm->mm_users is not zero, and it must be locked for at least read mode.
    // Also, the va_space lock must be held in write mode.
    // TODO: Bug 3372166: add asynchronous va_block reclaim.
    NV_STATUS uvm_hmm_va_block_reclaim(uvm_va_space_t *va_space,
                                       struct mm_struct *mm,
                                       NvU64 start,
                                       NvU64 end);

    // This is called to update the va_space tree of HMM va_blocks after an
    // existing va_block is split.
    // Locking: the va_space lock must be held in write mode.
    void uvm_hmm_va_block_split_tree(uvm_va_block_t *existing_va_block, uvm_va_block_t *new_block);

    // Find a HMM policy range that needs to be split. The callback function
    // 'split_needed_cb' returns true if the policy range needs to be split.
    // If a policy range is split, the existing range is truncated to
    // [existing_start, addr - 1] and a new policy node with the same policy
    // values as the existing node is created covering [addr, existing_end].
    // Before: [----------- existing ------------]
    // After:  [---- existing ----][---- new ----]
    //                             ^addr
    // Locking: the va_space must be write locked.
    NV_STATUS uvm_hmm_split_as_needed(uvm_va_space_t *va_space,
                                      NvU64 addr,
                                      uvm_va_policy_is_split_needed_t split_needed_cb,
                                      void *data);

    // Set the preferred location policy for the given range.
    // Note that 'last_address' is inclusive.
    // Locking: the va_space->va_space_mm.mm mmap_lock must be locked
    // and the va_space lock must be held in write mode.
    NV_STATUS uvm_hmm_set_preferred_location(uvm_va_space_t *va_space,
                                             uvm_processor_id_t preferred_location,
                                             int preferred_cpu_nid,
                                             NvU64 base,
                                             NvU64 last_address,
                                             uvm_tracker_t *out_tracker);

    // Set the accessed by policy for the given range. This also tries to
    // map the range. Note that 'last_address' is inclusive.
    // Locking: the va_space->va_space_mm.mm mmap_lock must be locked
    // and the va_space lock must be held in write mode.
    NV_STATUS uvm_hmm_set_accessed_by(uvm_va_space_t *va_space,
                                      uvm_processor_id_t processor_id,
                                      bool set_bit,
                                      NvU64 base,
                                      NvU64 last_address,
                                      uvm_tracker_t *out_tracker);

    // Deferred work item to reestablish accessed by mappings after eviction. On
    // GPUs with access counters enabled, the evicted GPU will also get remote
    // mappings.
    // Locking: the va_space->va_space_mm.mm mmap_lock must be locked
    // and the va_space lock must be held in at least read mode.
    void uvm_hmm_block_add_eviction_mappings(uvm_va_space_t *va_space,
                                             uvm_va_block_t *va_block,
                                             uvm_va_block_context_t *block_context);

    // Set the read duplication policy for the given range.
    // Note that 'last_address' is inclusive.
    // Locking: the va_space->va_space_mm.mm mmap_lock must be write locked
    // and the va_space lock must be held in write mode.
    // TODO: Bug 3660922: need to implement HMM read duplication support.
    static NV_STATUS uvm_hmm_set_read_duplication(uvm_va_space_t *va_space,
                                                  uvm_read_duplication_policy_t new_policy,
                                                  NvU64 base,
                                                  NvU64 last_address)
    {
        if (!uvm_hmm_is_enabled(va_space))
            return NV_ERR_INVALID_ADDRESS;
        return NV_OK;
    }

    // This function returns the policy covering the given address 'addr' and
    // assigns the ending address '*endp' to the minimum of va_block->end,
    // vma->vm_end - 1, and the ending address of the policy range. Locking:
    // This function must be called with vma->vm_mm retained and locked for at
    // least read and the va_block and va_space lock held.
    const uvm_va_policy_t *uvm_hmm_find_policy_end(uvm_va_block_t *va_block,
                                                   struct vm_area_struct *vma,
                                                   unsigned long addr,
                                                   NvU64 *endp);

    // This function finds the VMA for the page index 'page_index' and returns
    // it in vma_out which must not be NULL. Returns the policy covering the
    // given address, and sets the ending page range '*outerp' to the minimum of
    // *outerp, vma->vm_end - 1, the ending address of the policy range, and
    // va_block->end.
    // Return NV_ERR_INVALID_ADDRESS if no VMA is found; otherwise sets *vma
    // and returns NV_OK.
    // Locking: This function must be called with mm retained and locked for at
    // least read and the va_block and va_space lock held.
    NV_STATUS uvm_hmm_find_policy_vma_and_outer(uvm_va_block_t *va_block,
                                                struct vm_area_struct **vma,
                                                uvm_page_index_t page_index,
                                                const uvm_va_policy_t **policy,
                                                uvm_page_index_t *outerp);

    // Clear thrashing policy information from all HMM va_blocks.
    // Locking: va_space lock must be held in write mode.
    NV_STATUS uvm_hmm_clear_thrashing_policy(uvm_va_space_t *va_space);

    // Return the expanded region around 'address' limited to the intersection
    // of va_block start/end, vma start/end, and policy start/end.
    // Locking: the caller must hold va_space->va_space_mm.mm->mmap_lock in at
    // least read mode, the va_space lock must be held in at least read mode,
    // and the va_block lock held.
    uvm_va_block_region_t uvm_hmm_get_prefetch_region(uvm_va_block_t *va_block,
                                                      struct vm_area_struct *vma,
                                                      const uvm_va_policy_t *policy,
                                                      NvU64 address);

    // Return the logical protection allowed of a HMM va_block for the page at
    // the given address within the vma which must be valid. This is usually
    // obtained from uvm_hmm_va_block_find_create()).
    // Locking: the caller must hold va_space->va_space_mm.mm mmap_lock in at
    // least read mode.
    uvm_prot_t uvm_hmm_compute_logical_prot(uvm_va_block_t *va_block,
                                            struct vm_area_struct *vma,
                                            NvU64 addr);

    // This is called to service a GPU fault.
    // processor_id is the faulting processor.
    // new_residency is the processor where the data should be migrated to.
    // Special return values (besides things like NV_ERR_NO_MEMORY):
    // NV_WARN_MORE_PROCESSING_REQUIRED indicates that one or more pages could
    // not be migrated and that a retry might succeed after unlocking the
    // va_block lock, va_space lock, and mmap lock.
    // NV_WARN_MISMATCHED_TARGET is a special case of GPU fault handling when a
    // GPU is chosen as the destination and the source is a HMM CPU page that
    // can't be migrated (i.e., must remain in system memory). In that case,
    // uvm_va_block_select_residency() should be called with 'hmm_migratable'
    // set to false so that system memory will be selected. Then this call can
    // be retried to service the GPU fault by migrating to system memory.
    // Locking: the va_space->va_space_mm.mm mmap_lock must be locked,
    // the va_space read lock must be held, and the va_block lock held.
    NV_STATUS uvm_hmm_va_block_service_locked(uvm_processor_id_t processor_id,
                                              uvm_processor_id_t new_residency,
                                              uvm_va_block_t *va_block,
                                              uvm_va_block_retry_t *va_block_retry,
                                              uvm_service_block_context_t *service_context);

    // This is called to migrate a region within a HMM va_block. service_context
    // must not be NULL, service_context->va_block_context must not be NULL and
    // service_context->va_block_context->hmm.vma must be valid.
    //
    // Special return values (besides things like NV_ERR_NO_MEMORY):
    // NV_WARN_MORE_PROCESSING_REQUIRED indicates that one or more pages could
    // not be migrated and that a retry might succeed after unlocking the
    // va_block lock, va_space lock, and mmap lock.
    NV_STATUS uvm_hmm_va_block_migrate_locked(uvm_va_block_t *va_block,
                                              uvm_va_block_retry_t *va_block_retry,
                                              uvm_service_block_context_t *service_context,
                                              uvm_processor_id_t dest_id,
                                              uvm_va_block_region_t region,
                                              uvm_make_resident_cause_t cause);

    // This is called to migrate an address range of HMM allocations via
    // UvmMigrate().
    //
    // service_context and service_context->va_block_context must not be NULL.
    // The caller is not required to set
    // service_context->va_block_context->hmm.vma.
    //
    // Locking: the va_space->va_space_mm.mm mmap_lock must be locked and
    // the va_space read lock must be held.
    NV_STATUS uvm_hmm_migrate_ranges(uvm_va_space_t *va_space,
                                     uvm_service_block_context_t *service_context,
                                     NvU64 base,
                                     NvU64 length,
                                     uvm_processor_id_t dest_id,
                                     uvm_migrate_mode_t mode,
                                     uvm_tracker_t *out_tracker);

    // Handle a fault to a device-private page from a process other than the
    // process which created the va_space that originally allocated the
    // device-private page.
    NV_STATUS uvm_hmm_remote_cpu_fault(struct vm_fault *vmf);

    // This sets the va_block_context->hmm.src_pfns[] to the ZONE_DEVICE private
    // PFN for the GPU chunk memory.
    NV_STATUS uvm_hmm_va_block_evict_chunk_prep(uvm_va_block_t *va_block,
                                                uvm_va_block_context_t *va_block_context,
                                                uvm_gpu_chunk_t *gpu_chunk,
                                                uvm_va_block_region_t chunk_region);

    // Migrate pages to system memory for the given page mask. Note that the
    // mmap lock is not held and there is no MM retained. This must be called
    // after uvm_hmm_va_block_evict_chunk_prep() has initialized
    // service_context->va_block_context->hmm.src_pfns[] for the source GPU
    // physical PFNs being migrated. Note that the input mask 'pages_to_evict'
    // can be modified. If any of the evicted pages has the accessed by policy
    // set, then record that by setting out_accessed_by_set.
    // The caller is not required to set
    // service_context->va_block_context->hmm.vma, it will be cleared in
    // uvm_hmm_va_block_evict_chunks().
    // Locking: the va_block lock must be locked.
    NV_STATUS uvm_hmm_va_block_evict_chunks(uvm_va_block_t *va_block,
                                            uvm_service_block_context_t *service_context,
                                            const uvm_page_mask_t *pages_to_evict,
                                            uvm_va_block_region_t region,
                                            bool *out_accessed_by_set);

    // Migrate pages from the given GPU to system memory for the given page mask
    // and region. uvm_service_block_context_t and
    // uvm_service_block_context_t->va_block_context must not be NULL. Note that
    // the mmap lock is not held and there is no MM retained.
    // Locking: the va_block lock must be locked.
    NV_STATUS uvm_hmm_va_block_evict_pages_from_gpu(uvm_va_block_t *va_block,
                                                    uvm_gpu_t *gpu,
                                                    uvm_service_block_context_t *service_context,
                                                    const uvm_page_mask_t *pages_to_evict,
                                                    uvm_va_block_region_t region);

    // This returns what would be the intersection of va_block start/end and
    // VMA start/end-1 for the given 'lookup_address' if
    // uvm_hmm_va_block_find_create() was called.
    // Locking: the caller must hold mm->mmap_lock in at least read mode and
    // the va_space lock must be held in at least read mode.
    NV_STATUS uvm_hmm_va_block_range_bounds(uvm_va_space_t *va_space,
                                            struct mm_struct *mm,
                                            NvU64 lookup_address,
                                            NvU64 *startp,
                                            NvU64 *endp,
                                            UVM_TEST_VA_RESIDENCY_INFO_PARAMS *params);

    // This updates the HMM va_block CPU residency information for a single
    // page at 'lookup_address' by calling hmm_range_fault(). If 'populate' is
    // true, the CPU page will be faulted in read/write or read-only
    // (depending on the permission of the underlying VMA at lookup_address).
    // Locking: the caller must hold mm->mmap_lock in at least read mode and
    // the va_space lock must be held in at least read mode.
    NV_STATUS uvm_hmm_va_block_update_residency_info(uvm_va_block_t *va_block,
                                                     struct mm_struct *mm,
                                                     NvU64 lookup_address,
                                                     bool populate);

    NV_STATUS uvm_test_split_invalidate_delay(UVM_TEST_SPLIT_INVALIDATE_DELAY_PARAMS *params,
                                              struct file *filp);

    NV_STATUS uvm_hmm_va_range_info(uvm_va_space_t *va_space,
                                    struct mm_struct *mm,
                                    UVM_TEST_VA_RANGE_INFO_PARAMS *params);

    // Return true if GPU fault new residency location should be system memory.
    // va_block_context must not be NULL and va_block_context->hmm.vma must be
    // valid (this is usually set by uvm_hmm_va_block_find_create()).
    // TODO: Bug 3660968: Remove this hack as soon as HMM migration is
    // implemented for VMAs other than anonymous memory.
    // Locking: the va_block lock must be held. If the va_block is a HMM
    // va_block, the va_block_context->mm must be retained and locked for least
    // read.
    bool uvm_hmm_must_use_sysmem(uvm_va_block_t *va_block,
                                 struct vm_area_struct *vma);

#else // UVM_IS_CONFIG_HMM()

    static bool uvm_hmm_is_enabled(uvm_va_space_t *va_space)
    {
        return false;
    }

    static bool uvm_hmm_is_enabled_system_wide(void)
    {
        return false;
    }

    static void uvm_hmm_va_space_initialize(uvm_va_space_t *va_space)
    {
    }

    static void uvm_hmm_va_space_destroy(uvm_va_space_t *va_space)
    {
    }

    static void uvm_hmm_unregister_gpu(uvm_va_space_t *va_space, uvm_gpu_t *gpu, struct mm_struct *mm)
    {
    }

    static void uvm_hmm_remove_gpu_va_space(uvm_va_space_t *va_space,
                                            uvm_gpu_va_space_t *gpu_va_space,
                                            struct mm_struct *mm)
    {
    }

    static NV_STATUS uvm_hmm_va_block_find(uvm_va_space_t *va_space,
                                           NvU64 addr,
                                           uvm_va_block_t **va_block_ptr)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_va_block_find_create(uvm_va_space_t *va_space,
                                                  NvU64 addr,
                                                  struct vm_area_struct **vma,
                                                  uvm_va_block_t **va_block_ptr)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_find_vma(struct mm_struct *mm, struct vm_area_struct **vma, NvU64 addr)
    {
        return NV_OK;
    }

    static bool uvm_hmm_check_context_vma_is_valid(uvm_va_block_t *va_block,
                                                   struct vm_area_struct *vma,
                                                   uvm_va_block_region_t region)
    {
        return true;
    }

    static NV_STATUS uvm_hmm_migrate_begin(uvm_va_block_t *va_block)
    {
        return NV_OK;
    }

    static void uvm_hmm_migrate_begin_wait(uvm_va_block_t *va_block)
    {
    }

    static void uvm_hmm_migrate_finish(uvm_va_block_t *va_block)
    {
    }

    static NV_STATUS uvm_hmm_test_va_block_inject_split_error(uvm_va_space_t *va_space, NvU64 addr)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_va_block_reclaim(uvm_va_space_t *va_space,
                                              struct mm_struct *mm,
                                              NvU64 start,
                                              NvU64 end)
    {
        return NV_OK;
    }

    static void uvm_hmm_va_block_split_tree(uvm_va_block_t *existing_va_block, uvm_va_block_t *new_block)
    {
    }

    static NV_STATUS uvm_hmm_split_as_needed(uvm_va_space_t *va_space,
                                             NvU64 addr,
                                             uvm_va_policy_is_split_needed_t split_needed_cb,
                                             void *data)
    {
        return NV_OK;
    }

    static NV_STATUS uvm_hmm_set_preferred_location(uvm_va_space_t *va_space,
                                                    uvm_processor_id_t preferred_location,
                                                    int preferred_cpu_nid,
                                                    NvU64 base,
                                                    NvU64 last_address,
                                                    uvm_tracker_t *out_tracker)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_set_accessed_by(uvm_va_space_t *va_space,
                                             uvm_processor_id_t processor_id,
                                             bool set_bit,
                                             NvU64 base,
                                             NvU64 last_address,
                                             uvm_tracker_t *out_tracker)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static void uvm_hmm_block_add_eviction_mappings(uvm_va_space_t *va_space,
                                                    uvm_va_block_t *va_block,
                                                    uvm_va_block_context_t *block_context)
    {
    }

    static NV_STATUS uvm_hmm_set_read_duplication(uvm_va_space_t *va_space,
                                                  uvm_read_duplication_policy_t new_policy,
                                                  NvU64 base,
                                                  NvU64 last_address)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static const uvm_va_policy_t *uvm_hmm_find_policy_end(uvm_va_block_t *va_block,
                                                          struct vm_area_struct *vma,
                                                          unsigned long addr,
                                                          NvU64 *endp)
    {
        UVM_ASSERT(0);
        return NULL;
    }

    static NV_STATUS uvm_hmm_find_policy_vma_and_outer(uvm_va_block_t *va_block,
                                                       struct vm_area_struct **vma,
                                                       uvm_page_index_t page_index,
                                                       const uvm_va_policy_t **policy,
                                                       uvm_page_index_t *outerp)
    {
        return NV_OK;
    }

    static NV_STATUS uvm_hmm_clear_thrashing_policy(uvm_va_space_t *va_space)
    {
        return NV_OK;
    }

    static uvm_va_block_region_t uvm_hmm_get_prefetch_region(uvm_va_block_t *va_block,
                                                             struct vm_area_struct *vma,
                                                             const uvm_va_policy_t *policy,
                                                             NvU64 address)
    {
        return (uvm_va_block_region_t){};
    }

    static uvm_prot_t uvm_hmm_compute_logical_prot(uvm_va_block_t *va_block,
                                                   struct vm_area_struct *vma,
                                                   NvU64 addr)
    {
        return UVM_PROT_NONE;
    }

    static NV_STATUS uvm_hmm_va_block_service_locked(uvm_processor_id_t processor_id,
                                                     uvm_processor_id_t new_residency,
                                                     uvm_va_block_t *va_block,
                                                     uvm_va_block_retry_t *va_block_retry,
                                                     uvm_service_block_context_t *service_context)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_va_block_migrate_locked(uvm_va_block_t *va_block,
                                                     uvm_va_block_retry_t *va_block_retry,
                                                     uvm_service_block_context_t *service_context,
                                                     uvm_processor_id_t dest_id,
                                                     uvm_va_block_region_t region,
                                                     uvm_make_resident_cause_t cause)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_migrate_ranges(uvm_va_space_t *va_space,
                                            uvm_service_block_context_t *service_context,
                                            NvU64 base,
                                            NvU64 length,
                                            uvm_processor_id_t dest_id,
                                            uvm_migrate_mode_t mode,
                                            uvm_tracker_t *out_tracker)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_remote_cpu_fault(struct vm_fault *vmf)
    {
        UVM_ASSERT(0);
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_va_block_evict_chunk_prep(uvm_va_block_t *va_block,
                                                       uvm_va_block_context_t *va_block_context,
                                                       uvm_gpu_chunk_t *gpu_chunk,
                                                       uvm_va_block_region_t chunk_region)
    {
        return NV_OK;
    }

    static NV_STATUS uvm_hmm_va_block_evict_chunks(uvm_va_block_t *va_block,
                                                   uvm_service_block_context_t *service_context,
                                                   const uvm_page_mask_t *pages_to_evict,
                                                   uvm_va_block_region_t region,
                                                   bool *out_accessed_by_set)
    {
        return NV_OK;
    }

    static NV_STATUS uvm_hmm_va_block_evict_pages_from_gpu(uvm_va_block_t *va_block,
                                                           uvm_gpu_t *gpu,
                                                           uvm_service_block_context_t *service_context,
                                                           const uvm_page_mask_t *pages_to_evict,
                                                           uvm_va_block_region_t region)
    {
        return NV_OK;
    }

    static NV_STATUS uvm_hmm_va_block_range_bounds(uvm_va_space_t *va_space,
                                                   struct mm_struct *mm,
                                                   NvU64 lookup_address,
                                                   NvU64 *startp,
                                                   NvU64 *endp,
                                                   UVM_TEST_VA_RESIDENCY_INFO_PARAMS *params)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_va_block_update_residency_info(uvm_va_block_t *va_block,
                                                            struct mm_struct *mm,
                                                            NvU64 lookup_address,
                                                            bool populate)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_test_split_invalidate_delay(UVM_TEST_SPLIT_INVALIDATE_DELAY_PARAMS *params,
                                              struct file *filp)
    {
        return NV_ERR_INVALID_STATE;
    }

    static NV_STATUS uvm_hmm_va_range_info(uvm_va_space_t *va_space,
                                           struct mm_struct *mm,
                                           UVM_TEST_VA_RANGE_INFO_PARAMS *params)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static bool uvm_hmm_must_use_sysmem(uvm_va_block_t *va_block,
                                        struct vm_area_struct *vma)
    {
        return false;
    }

#endif // UVM_IS_CONFIG_HMM()

#endif  // _UVM_HMM_H_
