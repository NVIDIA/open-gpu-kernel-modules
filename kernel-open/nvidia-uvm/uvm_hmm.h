/*******************************************************************************
    Copyright (c) 2016-2022 NVIDIA Corporation

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

    // TODO: Bug 3351822: [UVM-HMM] Remove temporary testing changes.
    // This flag is set true by default for each va_space so most processes
    // don't see partially implemented UVM-HMM behavior but can be enabled by
    // test code for a given va_space so the test process can do some interim
    // testing. It needs to be a separate flag instead of modifying
    // uvm_disable_hmm or va_space->flags since those are user inputs and are
    // visible/checked by test code.
    // Remove this when UVM-HMM is fully integrated into chips_a.
    bool disable;
} uvm_hmm_va_space_t;

#if UVM_IS_CONFIG_HMM()
    // Tells whether HMM is enabled for the given va_space.
    // If it is not enabled, all of the functions below are no-ops.
    bool uvm_hmm_is_enabled(uvm_va_space_t *va_space);

    // Self-explanatory name: reports if HMM is enabled system-wide.
    bool uvm_hmm_is_enabled_system_wide(void);

    // Initialize HMM for the given the va_space.
    // Locking: the va_space->va_space_mm.mm mmap_lock must be write locked
    // and the va_space lock must be held in write mode.
    NV_STATUS uvm_hmm_va_space_initialize(uvm_va_space_t *va_space);

    // Initialize HMM for the given the va_space for testing.
    // Bug 1750144: UVM: Add HMM (Heterogeneous Memory Management) support to
    // the UVM driver. Remove this when enough HMM functionality is implemented.
    // Locking: the va_space->va_space_mm.mm mmap_lock must be write locked
    // and the va_space lock must be held in write mode.
    NV_STATUS uvm_hmm_va_space_initialize_test(uvm_va_space_t *va_space);

    // Destroy any HMM state for the given the va_space.
    // Locking: va_space lock must be held in write mode.
    void uvm_hmm_va_space_destroy(uvm_va_space_t *va_space);

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
    // If va_block_context is not NULL, the VMA is cached in
    // va_block_context->hmm.vma.
    // Locking: This function must be called with mm retained and locked for
    // at least read and the va_space lock at least for read.
    NV_STATUS uvm_hmm_va_block_find_create(uvm_va_space_t *va_space,
                                           NvU64 addr,
                                           uvm_va_block_context_t *va_block_context,
                                           uvm_va_block_t **va_block_ptr);

    // Find the VMA for the given address and set va_block_context->hmm.vma.
    // Return NV_ERR_INVALID_ADDRESS if va_block_context->mm is NULL or there
    // is no VMA associated with the address 'addr' or the VMA does not have at
    // least PROT_READ permission.
    // Locking: This function must be called with mm retained and locked for
    // at least read or mm equal to NULL.
    NV_STATUS uvm_hmm_find_vma(uvm_va_block_context_t *va_block_context, NvU64 addr);

    // If va_block is a HMM va_block, check that va_block_context->hmm.vma is
    // not NULL and covers the given region. This always returns true and is
    // intended to only be used with UVM_ASSERT().
    // Locking: This function must be called with the va_block lock held and if
    // va_block is a HMM block, va_block_context->mm must be retained and
    // locked for at least read.
    bool uvm_hmm_va_block_context_vma_is_valid(uvm_va_block_t *va_block,
                                               uvm_va_block_context_t *va_block_context,
                                               uvm_va_block_region_t region);

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
                                             NvU64 base,
                                             NvU64 last_address);

    // Set the accessed by policy for the given range. This also tries to
    // map the range. Note that 'last_address' is inclusive.
    // Locking: the va_space->va_space_mm.mm mmap_lock must be locked
    // and the va_space lock must be held in write mode.
    NV_STATUS uvm_hmm_set_accessed_by(uvm_va_space_t *va_space,
                                      uvm_processor_id_t processor_id,
                                      bool set_bit,
                                      NvU64 base,
                                      NvU64 last_address);

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

    // This function assigns va_block_context->policy to the policy covering
    // the given address 'addr' and assigns the ending address '*endp' to the
    // minimum of va_block->end, va_block_context->hmm.vma->vm_end - 1, and the
    // ending address of the policy range. Note that va_block_context->hmm.vma
    // is expected to be initialized before calling this function.
    // Locking: This function must be called with
    // va_block_context->hmm.vma->vm_mm retained and locked for least read and
    // the va_block lock held.
    void uvm_hmm_find_policy_end(uvm_va_block_t *va_block,
                                 uvm_va_block_context_t *va_block_context,
                                 unsigned long addr,
                                 NvU64 *endp);

    // This function finds the VMA for the page index 'page_index' and assigns
    // it to va_block_context->vma, sets va_block_context->policy to the policy
    // covering the given address, and sets the ending page range '*outerp'
    // to the minimum of *outerp, va_block_context->hmm.vma->vm_end - 1, the
    // ending address of the policy range, and va_block->end.
    // Return NV_ERR_INVALID_ADDRESS if no VMA is found; otherwise, NV_OK.
    // Locking: This function must be called with
    // va_block_context->hmm.vma->vm_mm retained and locked for least read and
    // the va_block lock held.
    NV_STATUS uvm_hmm_find_policy_vma_and_outer(uvm_va_block_t *va_block,
                                                uvm_va_block_context_t *va_block_context,
                                                uvm_page_index_t page_index,
                                                uvm_page_index_t *outerp);

    // Clear thrashing policy information from all HMM va_blocks.
    // Locking: va_space lock must be held in write mode.
    NV_STATUS uvm_hmm_clear_thrashing_policy(uvm_va_space_t *va_space);

    // Return the expanded region around 'address' limited to the intersection
    // of va_block start/end, vma start/end, and policy start/end.
    // va_block_context must not be NULL, va_block_context->hmm.vma must be
    // valid (this is usually set by uvm_hmm_va_block_find_create()), and
    // va_block_context->policy must be valid.
    // Locking: the caller must hold mm->mmap_lock in at least read mode, the
    // va_space lock must be held in at least read mode, and the va_block lock
    // held.
    uvm_va_block_region_t uvm_hmm_get_prefetch_region(uvm_va_block_t *va_block,
                                                      uvm_va_block_context_t *va_block_context,
                                                      NvU64 address);

    // Return the logical protection allowed of a HMM va_block for the page at
    // the given address.
    // va_block_context must not be NULL and va_block_context->hmm.vma must be
    // valid (this is usually set by uvm_hmm_va_block_find_create()).
    // Locking: the caller must hold va_block_context->mm mmap_lock in at least
    // read mode.
    uvm_prot_t uvm_hmm_compute_logical_prot(uvm_va_block_t *va_block,
                                            uvm_va_block_context_t *va_block_context,
                                            NvU64 addr);

    NV_STATUS uvm_test_hmm_init(UVM_TEST_HMM_INIT_PARAMS *params, struct file *filp);

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
                                 uvm_va_block_context_t *va_block_context);

#else // UVM_IS_CONFIG_HMM()

    static bool uvm_hmm_is_enabled(uvm_va_space_t *va_space)
    {
        return false;
    }

    static bool uvm_hmm_is_enabled_system_wide(void)
    {
        return false;
    }

    static NV_STATUS uvm_hmm_va_space_initialize(uvm_va_space_t *va_space)
    {
        return NV_OK;
    }

    static NV_STATUS uvm_hmm_va_space_initialize_test(uvm_va_space_t *va_space)
    {
        return NV_WARN_NOTHING_TO_DO;
    }

    static void uvm_hmm_va_space_destroy(uvm_va_space_t *va_space)
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
                                                  uvm_va_block_context_t *va_block_context,
                                                  uvm_va_block_t **va_block_ptr)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_find_vma(uvm_va_block_context_t *va_block_context, NvU64 addr)
    {
        return NV_OK;
    }

    static bool uvm_hmm_va_block_context_vma_is_valid(uvm_va_block_t *va_block,
                                                      uvm_va_block_context_t *va_block_context,
                                                      uvm_va_block_region_t region)
    {
        return true;
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
                                                    NvU64 base,
                                                    NvU64 last_address)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_set_accessed_by(uvm_va_space_t *va_space,
                                             uvm_processor_id_t processor_id,
                                             bool set_bit,
                                             NvU64 base,
                                             NvU64 last_address)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static NV_STATUS uvm_hmm_set_read_duplication(uvm_va_space_t *va_space,
                                                  uvm_read_duplication_policy_t new_policy,
                                                  NvU64 base,
                                                  NvU64 last_address)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    static void uvm_hmm_find_policy_end(uvm_va_block_t *va_block,
                                        uvm_va_block_context_t *va_block_context,
                                        unsigned long addr,
                                        NvU64 *endp)
    {
    }

    static NV_STATUS uvm_hmm_find_policy_vma_and_outer(uvm_va_block_t *va_block,
                                                       uvm_va_block_context_t *va_block_context,
                                                       uvm_page_index_t page_index,
                                                       uvm_page_index_t *outerp)
    {
        return NV_OK;
    }

    static NV_STATUS uvm_hmm_clear_thrashing_policy(uvm_va_space_t *va_space)
    {
        return NV_OK;
    }

    static uvm_va_block_region_t uvm_hmm_get_prefetch_region(uvm_va_block_t *va_block,
                                                             uvm_va_block_context_t *va_block_context,
                                                             NvU64 address)
    {
        return (uvm_va_block_region_t){};
    }

    static uvm_prot_t uvm_hmm_compute_logical_prot(uvm_va_block_t *va_block,
                                                   uvm_va_block_context_t *va_block_context,
                                                   NvU64 addr)
    {
        return UVM_PROT_NONE;
    }

    static NV_STATUS uvm_test_hmm_init(UVM_TEST_HMM_INIT_PARAMS *params, struct file *filp)
    {
        return NV_WARN_NOTHING_TO_DO;
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
                                        uvm_va_block_context_t *va_block_context)
    {
        return false;
    }

#endif // UVM_IS_CONFIG_HMM()

#endif  // _UVM_HMM_H_
