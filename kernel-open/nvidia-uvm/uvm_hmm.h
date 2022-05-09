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
    // Locking: This function must be called with mm retained and locked for
    // at least read and the va_space lock at least for read.
    NV_STATUS uvm_hmm_va_block_find_create(uvm_va_space_t *va_space,
                                           NvU64 addr,
                                           uvm_va_block_context_t *va_block_context,
                                           uvm_va_block_t **va_block_ptr);

    // Reclaim any HMM va_blocks that overlap the given range.
    // Note that 'end' is inclusive.
    // A HMM va_block can be reclaimed if it doesn't contain any "valid" VMAs.
    // See uvm_hmm_vma_is_valid() for details.
    // Return values:
    // NV_ERR_NO_MEMORY: Reclaim required a block split, which failed.
    // NV_OK:            There were no HMM blocks in the range, or all HMM
    //                   blocks in the range were successfully reclaimed.
    // Locking: If mm is not NULL, it must equal va_space_mm.mm, the caller
    // must hold a reference on it, and it must be locked for at least read
    // mode. Also, the va_space lock must be held in write mode.
    // TODO: Bug 3372166: add asynchronous va_block reclaim.
    NV_STATUS uvm_hmm_va_block_reclaim(uvm_va_space_t *va_space,
                                       struct mm_struct *mm,
                                       NvU64 start,
                                       NvU64 end);

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
    // TODO: Bug 2046423: need to implement read duplication support in Linux.
    static NV_STATUS uvm_hmm_set_read_duplication(uvm_va_space_t *va_space,
                                                  uvm_read_duplication_policy_t new_policy,
                                                  NvU64 base,
                                                  NvU64 last_address)
    {
        if (!uvm_hmm_is_enabled(va_space))
            return NV_ERR_INVALID_ADDRESS;
        return NV_OK;
    }

    // Set va_block_context->policy to the policy covering the given address
    // 'addr' and update the ending address '*endp' to the minimum of *endp,
    // va_block_context->hmm.vma->vm_end - 1, and the ending address of the
    // policy range.
    // Locking: This function must be called with
    // va_block_context->hmm.vma->vm_mm retained and locked for least read and
    // the va_block lock held.
    void uvm_hmm_find_policy_end(uvm_va_block_t *va_block,
                                 uvm_va_block_context_t *va_block_context,
                                 unsigned long addr,
                                 NvU64 *endp);

    // Find the VMA for the page index 'page_index',
    // set va_block_context->policy to the policy covering the given address,
    // and update the ending page range '*outerp' to the minimum of *outerp,
    // va_block_context->hmm.vma->vm_end - 1, and the ending address of the
    // policy range.
    // Return NV_ERR_INVALID_ADDRESS if no VMA is found; otherwise, NV_OK.
    // Locking: This function must be called with
    // va_block_context->hmm.vma->vm_mm retained and locked for least read and
    // the va_block lock held.
    NV_STATUS uvm_hmm_find_policy_vma_and_outer(uvm_va_block_t *va_block,
                                                uvm_va_block_context_t *va_block_context,
                                                uvm_page_index_t page_index,
                                                uvm_page_index_t *outerp);

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

    static NV_STATUS uvm_hmm_va_block_reclaim(uvm_va_space_t *va_space,
                                              struct mm_struct *mm,
                                              NvU64 start,
                                              NvU64 end)
    {
        return NV_OK;
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

#endif // UVM_IS_CONFIG_HMM()

#endif  // _UVM_HMM_H_
