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

#include "uvm_hmm.h"

static bool uvm_disable_hmm = false;
module_param(uvm_disable_hmm, bool, 0444);
MODULE_PARM_DESC(uvm_disable_hmm,
                 "Force-disable HMM functionality in the UVM driver. "
                 "Default: false (i.e, HMM is potentially enabled). Ignored if "
                 "HMM is not supported in the driver, or if ATS settings "
                 "conflict with HMM.");

#if UVM_IS_CONFIG_HMM()

#include <linux/hmm.h>
#include <linux/userfaultfd_k.h>

#include "uvm_common.h"
#include "uvm_gpu.h"
#include "uvm_va_block_types.h"
#include "uvm_va_space_mm.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_range_tree.h"
#include "uvm_lock.h"
#include "uvm_api.h"
#include "uvm_va_policy.h"
#include "uvm_tools.h"

bool uvm_hmm_is_enabled_system_wide(void)
{
    return !uvm_disable_hmm && !g_uvm_global.ats.enabled && uvm_va_space_mm_enabled_system();
}

bool uvm_hmm_is_enabled(uvm_va_space_t *va_space)
{
    // TODO: Bug 3351822: [UVM-HMM] Remove temporary testing changes.
    return uvm_hmm_is_enabled_system_wide() &&
           uvm_va_space_mm_enabled(va_space) &&
           !(va_space->initialization_flags & UVM_INIT_FLAGS_DISABLE_HMM) &&
           !va_space->hmm.disable;
}

static uvm_va_block_t *hmm_va_block_from_node(uvm_range_tree_node_t *node)
{
    if (!node)
        return NULL;
    return container_of(node, uvm_va_block_t, hmm.node);
}

NV_STATUS uvm_hmm_va_space_initialize(uvm_va_space_t *va_space)
{
    struct mm_struct *mm = va_space->va_space_mm.mm;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_OK;

    uvm_assert_mmap_lock_locked_write(mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    // TODO: Bug 3351822: [UVM-HMM] Remove temporary testing changes.
    // Disable HMM by default for each va_space until enough functionality is
    // implemented that this can be enabled by default.
    // Note that it can be enabled for testing under controlled circumstances.
    va_space->hmm.disable = true;

    return NV_OK;
}

NV_STATUS uvm_hmm_va_space_initialize_test(uvm_va_space_t *va_space)
{
    uvm_hmm_va_space_t *hmm_va_space = &va_space->hmm;
    struct mm_struct *mm = va_space->va_space_mm.mm;
    int ret;

    if (!uvm_hmm_is_enabled_system_wide() || !mm)
        return NV_WARN_NOTHING_TO_DO;

    if (va_space->initialization_flags & UVM_INIT_FLAGS_DISABLE_HMM)
        return NV_ERR_INVALID_STATE;

    uvm_assert_mmap_lock_locked_write(mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    // Temporarily enable HMM for testing.
    va_space->hmm.disable = false;

    // Initialize MMU interval notifiers for this process.
    // This allows mmu_interval_notifier_insert() to be called without holding
    // the mmap_lock for write.
    // Note: there is no __mmu_notifier_unregister(), this call just allocates
    // memory which is attached to the mm_struct and freed when the mm_struct
    // is freed.
    ret = __mmu_notifier_register(NULL, mm);
    if (ret)
        return errno_to_nv_status(ret);

    uvm_range_tree_init(&hmm_va_space->blocks);
    uvm_mutex_init(&hmm_va_space->blocks_lock, UVM_LOCK_ORDER_LEAF);

    return NV_OK;
}

void uvm_hmm_va_space_destroy(uvm_va_space_t *va_space)
{
    uvm_hmm_va_space_t *hmm_va_space = &va_space->hmm;
    uvm_range_tree_node_t *node, *next;
    uvm_va_block_t *va_block;

    if (!uvm_hmm_is_enabled(va_space) || uvm_va_space_initialized(va_space) != NV_OK)
        return;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    // The blocks_lock is not needed when the va_space lock is held for write.
    uvm_range_tree_for_each_safe(node, next, &hmm_va_space->blocks) {
        va_block = hmm_va_block_from_node(node);
        uvm_range_tree_remove(&hmm_va_space->blocks, node);
        mmu_interval_notifier_remove(&va_block->hmm.notifier);
        uvm_va_block_kill(va_block);
    }

    // TODO: Bug 3351822: [UVM-HMM] Remove temporary testing changes.
    va_space->hmm.disable = true;
}

static bool hmm_invalidate(uvm_va_block_t *va_block,
                           const struct mmu_notifier_range *range,
                           unsigned long cur_seq)
{
    struct mmu_interval_notifier *mni = &va_block->hmm.notifier;
    NvU64 start, end;

    // The MMU_NOTIFY_RELEASE event isn't really needed since mn_itree_release()
    // doesn't remove the interval notifiers from the struct_mm so there will
    // be a full range MMU_NOTIFY_UNMAP event after the release from
    // unmap_vmas() during exit_mmap().
    if (range->event == MMU_NOTIFY_SOFT_DIRTY || range->event == MMU_NOTIFY_RELEASE)
        return true;

    // Blockable is only set false by
    // mmu_notifier_invalidate_range_start_nonblock() which is only called in
    // __oom_reap_task_mm().
    if (!mmu_notifier_range_blockable(range))
        return false;

    // Ignore invalidation callbacks for device private pages since the
    // invalidation is handled as part of the migration process.
    // Note that the va_space pointer won't be NULL if the callback is for
    // MMU_NOTIFY_MIGRATE/MMU_NOTIFY_EXCLUSIVE because the va_block lock
    // is already held and we have to prevent recursively getting the lock.
    if ((range->event == MMU_NOTIFY_MIGRATE || range->event == MMU_NOTIFY_EXCLUSIVE) &&
            range->owner == va_block->hmm.va_space)
        return true;

    uvm_mutex_lock(&va_block->lock);

    // Ignore this invalidation callback if the block is dead.
    if (uvm_va_block_is_dead(va_block))
        goto unlock;

    mmu_interval_set_seq(mni, cur_seq);

    // Note: unmap_vmas() does MMU_NOTIFY_UNMAP [0, 0xffffffffffffffff]
    // Also note that hmm_invalidate() can be called when a new va_block is not
    // yet inserted into the va_space->hmm.blocks table while the original
    // va_block is being split. The original va_block may have its end address
    // updated before the mmu interval notifier is updated so this invalidate
    // may be for a range past the va_block end address.
    start = range->start;
    end = (range->end == ULONG_MAX) ? range->end : range->end - 1;
    if (start < va_block->start)
        start = va_block->start;
    if (end > va_block->end)
        end = va_block->end;
    if (start > end)
        goto unlock;

    if (range->event == MMU_NOTIFY_UNMAP)
        uvm_va_policy_clear(va_block, start, end);

unlock:
    uvm_mutex_unlock(&va_block->lock);

    return true;
}

static bool uvm_hmm_invalidate_entry(struct mmu_interval_notifier *mni,
                                     const struct mmu_notifier_range *range,
                                     unsigned long cur_seq)
{
    uvm_va_block_t *va_block = container_of(mni, uvm_va_block_t, hmm.notifier);

    UVM_ENTRY_RET(hmm_invalidate(va_block, range, cur_seq));
}

static const struct mmu_interval_notifier_ops uvm_hmm_notifier_ops =
{
    .invalidate = uvm_hmm_invalidate_entry,
};

NV_STATUS uvm_hmm_va_block_find(uvm_va_space_t *va_space,
                                NvU64 addr,
                                uvm_va_block_t **va_block_ptr)
{
    uvm_range_tree_node_t *node;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    UVM_ASSERT(uvm_va_space_initialized(va_space) == NV_OK);
    uvm_assert_rwsem_locked(&va_space->lock);

    uvm_mutex_lock(&va_space->hmm.blocks_lock);
    node = uvm_range_tree_find(&va_space->hmm.blocks, addr);
    uvm_mutex_unlock(&va_space->hmm.blocks_lock);

    if (!node)
        return NV_ERR_OBJECT_NOT_FOUND;

    *va_block_ptr = hmm_va_block_from_node(node);

    return NV_OK;
}

static bool uvm_hmm_vma_is_valid(struct vm_area_struct *vma,
                                 unsigned long addr,
                                 bool allow_unreadable_vma)
{
    // UVM doesn't support userfaultfd. hmm_range_fault() doesn't support
    // VM_IO, VM_PFNMAP, or VM_MIXEDMAP VMAs. It also doesn't support
    // VMAs without VM_READ but we allow those VMAs to have policy set on
    // them.
    return vma &&
           addr >= vma->vm_start &&
           !userfaultfd_armed(vma) &&
           !(vma->vm_flags & (VM_IO | VM_PFNMAP | VM_MIXEDMAP)) &&
           (allow_unreadable_vma || (vma->vm_flags & VM_READ));
}

static NV_STATUS hmm_va_block_find_create(uvm_va_space_t *va_space,
                                          NvU64 addr,
                                          bool allow_unreadable_vma,
                                          uvm_va_block_context_t *va_block_context,
                                          uvm_va_block_t **va_block_ptr)
{
    struct mm_struct *mm = va_space->va_space_mm.mm;
    struct vm_area_struct *vma;
    uvm_va_block_t *va_block;
    NvU64 start, end;
    NV_STATUS status;
    int ret;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    UVM_ASSERT(uvm_va_space_initialized(va_space) == NV_OK);
    UVM_ASSERT(mm);
    UVM_ASSERT(!va_block_context || va_block_context->mm == mm);
    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);
    UVM_ASSERT(PAGE_ALIGNED(addr));

    // Note that we have to allow PROT_NONE VMAs so that policies can be set.
    vma = find_vma(mm, addr);
    if (!uvm_hmm_vma_is_valid(vma, addr, allow_unreadable_vma))
        return NV_ERR_INVALID_ADDRESS;

    // Since we only hold the va_space read lock, there can be multiple
    // parallel va_block insertions.
    uvm_mutex_lock(&va_space->hmm.blocks_lock);

    va_block = hmm_va_block_from_node(uvm_range_tree_find(&va_space->hmm.blocks, addr));
    if (va_block)
        goto done;

    // The va_block is always created to cover the whole aligned
    // UVM_VA_BLOCK_SIZE interval unless there are existing UVM va_ranges or
    // HMM va_blocks. In that case, the new HMM va_block size is adjusted so it
    // doesn't overlap.
    start = UVM_VA_BLOCK_ALIGN_DOWN(addr);
    end = start + UVM_VA_BLOCK_SIZE - 1;

    // Search for existing UVM va_ranges in the start/end interval and create
    // a maximum interval that doesn't overlap any existing UVM va_ranges.
    // We know that 'addr' is not within a va_range or
    // hmm_va_block_find_create() wouldn't be called.
    status = uvm_range_tree_find_hole_in(&va_space->va_range_tree, addr, &start, &end);
    UVM_ASSERT(status == NV_OK);

    // Search for existing HMM va_blocks in the start/end interval and create
    // a maximum interval that doesn't overlap any existing HMM va_blocks.
    status = uvm_range_tree_find_hole_in(&va_space->hmm.blocks, addr, &start, &end);
    UVM_ASSERT(status == NV_OK);

    // Create a HMM va_block with a NULL va_range pointer.
    status = uvm_va_block_create(NULL, start, end, &va_block);
    if (status != NV_OK)
        goto err_unlock;

    va_block->hmm.node.start = start;
    va_block->hmm.node.end = end;
    va_block->hmm.va_space = va_space;
    uvm_range_tree_init(&va_block->hmm.va_policy_tree);

    ret = mmu_interval_notifier_insert(&va_block->hmm.notifier,
                                       mm,
                                       start,
                                       end - start + 1,
                                       &uvm_hmm_notifier_ops);
    if (ret) {
        status = errno_to_nv_status(ret);
        goto err_release;
    }

    status = uvm_range_tree_add(&va_space->hmm.blocks, &va_block->hmm.node);
    UVM_ASSERT(status == NV_OK);

done:
    uvm_mutex_unlock(&va_space->hmm.blocks_lock);
    if (va_block_context)
        va_block_context->hmm.vma = vma;
    *va_block_ptr = va_block;
    return NV_OK;

err_release:
    uvm_va_block_release(va_block);

err_unlock:
    uvm_mutex_unlock(&va_space->hmm.blocks_lock);
    return status;
}

NV_STATUS uvm_hmm_va_block_find_create(uvm_va_space_t *va_space,
                                       NvU64 addr,
                                       uvm_va_block_context_t *va_block_context,
                                       uvm_va_block_t **va_block_ptr)
{
    return hmm_va_block_find_create(va_space, addr, false, va_block_context, va_block_ptr);
}

NV_STATUS uvm_hmm_find_vma(uvm_va_block_context_t *va_block_context, NvU64 addr)
{
    struct mm_struct *mm = va_block_context->mm;
    struct vm_area_struct *vma;

    if (!mm)
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(mm);

    vma = find_vma(mm, addr);
    if (!uvm_hmm_vma_is_valid(vma, addr, false))
        return NV_ERR_INVALID_ADDRESS;

    va_block_context->hmm.vma = vma;

    return NV_OK;
}

bool uvm_hmm_va_block_context_vma_is_valid(uvm_va_block_t *va_block,
                                           uvm_va_block_context_t *va_block_context,
                                           uvm_va_block_region_t region)
{
    uvm_assert_mutex_locked(&va_block->lock);

    if (uvm_va_block_is_hmm(va_block)) {
        struct vm_area_struct *vma = va_block_context->hmm.vma;

        UVM_ASSERT(vma);
        UVM_ASSERT(va_block_context->mm == vma->vm_mm);
        uvm_assert_mmap_lock_locked(va_block_context->mm);
        UVM_ASSERT(vma->vm_start <= uvm_va_block_region_start(va_block, region));
        UVM_ASSERT(vma->vm_end > uvm_va_block_region_end(va_block, region));
    }

    return true;
}

NV_STATUS uvm_hmm_test_va_block_inject_split_error(uvm_va_space_t *va_space, NvU64 addr)
{
    uvm_va_block_test_t *block_test;
    uvm_va_block_t *va_block;
    NV_STATUS status;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    status = hmm_va_block_find_create(va_space, addr, false, NULL, &va_block);
    if (status != NV_OK)
        return status;

    block_test = uvm_va_block_get_test(va_block);
    if (block_test)
        block_test->inject_split_error = true;

    return NV_OK;
}

typedef struct {
    struct mmu_interval_notifier notifier;
    uvm_va_block_t *existing_block;
} hmm_split_invalidate_data_t;

static bool hmm_split_invalidate(struct mmu_interval_notifier *mni,
                                 const struct mmu_notifier_range *range,
                                 unsigned long cur_seq)
{
    hmm_split_invalidate_data_t *split_data = container_of(mni, hmm_split_invalidate_data_t, notifier);

    uvm_tools_test_hmm_split_invalidate(split_data->existing_block->hmm.va_space);
    hmm_invalidate(split_data->existing_block, range, cur_seq);

    return true;
}

static bool hmm_split_invalidate_entry(struct mmu_interval_notifier *mni,
                                       const struct mmu_notifier_range *range,
                                       unsigned long cur_seq)
{
    UVM_ENTRY_RET(hmm_split_invalidate(mni, range, cur_seq));
}

static const struct mmu_interval_notifier_ops hmm_notifier_split_ops =
{
    .invalidate = hmm_split_invalidate_entry,
};

// Splits existing va_block into two pieces, with new_va_block always after
// va_block. va_block is updated to have new_end. new_end+1 must be page-
// aligned.
//
// Before: [----------- existing ------------]
// After:  [---- existing ----][---- new ----]
//                            ^new_end
//
// On error, va_block is still accessible and is left in its original
// functional state.
static NV_STATUS hmm_split_block(uvm_va_block_t *va_block,
                                 NvU64 new_end,
                                 uvm_va_block_t **new_block_ptr)
{
    uvm_va_space_t *va_space = va_block->hmm.va_space;
    struct mm_struct *mm = va_space->va_space_mm.mm;
    hmm_split_invalidate_data_t split_data;
    NvU64 delay_us;
    uvm_va_block_t *new_va_block;
    NV_STATUS status;
    int ret;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    UVM_ASSERT(new_end > va_block->start);
    UVM_ASSERT(new_end < va_block->end);
    UVM_ASSERT(PAGE_ALIGNED(new_end + 1));

    status = uvm_va_block_create(NULL, new_end + 1, va_block->end, &new_va_block);
    if (status != NV_OK)
        return status;

    // Initialize the newly created HMM va_block.
    new_va_block->hmm.node.start = new_va_block->start;
    new_va_block->hmm.node.end = new_va_block->end;
    new_va_block->hmm.va_space = va_space;
    uvm_range_tree_init(&new_va_block->hmm.va_policy_tree);

    ret = mmu_interval_notifier_insert(&new_va_block->hmm.notifier,
                                       mm,
                                       new_va_block->start,
                                       uvm_va_block_size(new_va_block),
                                       &uvm_hmm_notifier_ops);

    // Since __mmu_notifier_register() was called when the va_space was
    // initially created, we know that mm->notifier_subscriptions is valid
    // and mmu_interval_notifier_insert() can't return ENOMEM.
    // The only error return is for start + length overflowing but we already
    // registered the same address range before so there should be no error.
    UVM_ASSERT(!ret);

    uvm_mutex_lock(&va_block->lock);

    status = uvm_va_block_split_locked(va_block, new_end, new_va_block, NULL);
    if (status != NV_OK)
        goto err;

    uvm_mutex_unlock(&va_block->lock);

    // The MMU interval notifier has to be removed in order to resize it.
    // That means there would be a window of time when invalidation callbacks
    // could be missed. To handle this case, we register a temporary notifier
    // to cover the address range while resizing the old notifier (it is
    // OK to have multiple notifiers for the same range, we may simply try to
    // invalidate twice).
    split_data.existing_block = va_block;
    ret = mmu_interval_notifier_insert(&split_data.notifier,
                                       mm,
                                       va_block->start,
                                       new_end - va_block->start + 1,
                                       &hmm_notifier_split_ops);
    UVM_ASSERT(!ret);

    // Delay to allow hmm_sanity test to trigger an mmu_notifier during the
    // critical window where the split invalidate callback is active.
    delay_us = atomic64_read(&va_space->test.split_invalidate_delay_us);
    if (delay_us)
        udelay(delay_us);

    mmu_interval_notifier_remove(&va_block->hmm.notifier);

    // Enable notifications on the old block with the smaller size.
    ret = mmu_interval_notifier_insert(&va_block->hmm.notifier,
                                       mm,
                                       va_block->start,
                                       uvm_va_block_size(va_block),
                                       &uvm_hmm_notifier_ops);
    UVM_ASSERT(!ret);

    mmu_interval_notifier_remove(&split_data.notifier);

    if (new_block_ptr)
        *new_block_ptr = new_va_block;

    return status;

err:
    uvm_mutex_unlock(&va_block->lock);
    mmu_interval_notifier_remove(&new_va_block->hmm.notifier);
    uvm_va_block_release(new_va_block);
    return status;
}

// Check to see if the HMM va_block would overlap the range start/end and
// split it so it can be removed. That breaks down to the following cases:
// start/end could cover all of the HMM va_block ->
//     remove the va_block
// start/end could cover the left part of the HMM va_block ->
//     remove the left part
// start/end could cover the right part of the HMM va_block ->
//     remove the right part
// or start/end could "punch a hole" in the middle and leave the ends intact.
// In each case, only one HMM va_block is removed so return it in out_va_block.
static NV_STATUS split_block_if_needed(uvm_va_block_t *va_block,
                                       NvU64 start,
                                       NvU64 end,
                                       uvm_va_block_t **out_va_block)
{
    uvm_va_block_t *new;
    NV_STATUS status;

    if (va_block->start < start) {
        status = hmm_split_block(va_block, start - 1, &new);
        if (status != NV_OK)
            return status;

        // Keep the left part, the right part will be deleted.
        va_block = new;
    }

    if (va_block->end > end) {
        status = hmm_split_block(va_block, end, NULL);
        if (status != NV_OK)
            return status;

        // Keep the right part, the left part will be deleted.
    }

    *out_va_block = va_block;

    return NV_OK;
}

// Normally, the HMM va_block is destroyed when the va_space is destroyed
// (i.e., when the /dev/nvidia-uvm device is closed). A munmap() call triggers
// a uvm_hmm_invalidate() callback which unmaps the VMA's range from the GPU's
// page tables. However, it doesn't destroy the va_block because that would
// require calling mmu_interval_notifier_remove() which can't be called from
// the invalidate callback due to Linux locking constraints. If a process
// calls mmap()/munmap() for SAM and then creates a managed allocation,
// the same VMA range can be picked and there would be a UVM/HMM va_block
// conflict. Creating a managed allocation (or other va_range) calls this
// function to remove stale HMM va_blocks or split the HMM va_block so there
// is no overlap.
NV_STATUS uvm_hmm_va_block_reclaim(uvm_va_space_t *va_space,
                                   struct mm_struct *mm,
                                   NvU64 start,
                                   NvU64 end)
{
    uvm_range_tree_node_t *node, *next;
    uvm_va_block_t *va_block;
    NV_STATUS status;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_OK;

    if (mm) {
        UVM_ASSERT(mm == va_space->va_space_mm.mm);
        uvm_assert_mmap_lock_locked(mm);
    }
    uvm_assert_rwsem_locked_write(&va_space->lock);

    // Process each HMM va_block that overlaps the interval [start, end].
    // Note that end is inclusive.
    // The blocks_lock is not needed when the va_space lock is held for write.
    uvm_range_tree_for_each_in_safe(node, next, &va_space->hmm.blocks, start, end) {
        va_block = hmm_va_block_from_node(node);

        if (mm) {
            status = split_block_if_needed(va_block, start, end, &va_block);
            if (status != NV_OK)
                return status;
        }

        // Note that this waits for any invalidations callbacks to complete
        // so uvm_hmm_invalidate() won't see a block disapear.
        // The va_space write lock should prevent uvm_hmm_va_block_find_create()
        // from adding it back.
        mmu_interval_notifier_remove(&va_block->hmm.notifier);
        uvm_range_tree_remove(&va_space->hmm.blocks, &va_block->hmm.node);
        uvm_va_block_kill(va_block);
    }

    UVM_ASSERT(!uvm_range_tree_iter_first(&va_space->hmm.blocks, start, end));

    return NV_OK;
}

void uvm_hmm_va_block_split_tree(uvm_va_block_t *existing_va_block, uvm_va_block_t *new_block)
{
    uvm_va_space_t *va_space = existing_va_block->hmm.va_space;

    UVM_ASSERT(uvm_va_block_is_hmm(existing_va_block));
    uvm_assert_rwsem_locked_write(&va_space->lock);

    uvm_range_tree_split(&existing_va_block->hmm.va_space->hmm.blocks,
                         &existing_va_block->hmm.node,
                         &new_block->hmm.node);
}

NV_STATUS uvm_hmm_split_as_needed(uvm_va_space_t *va_space,
                                  NvU64 addr,
                                  uvm_va_policy_is_split_needed_t split_needed_cb,
                                  void *data)
{
    uvm_va_block_t *va_block;
    uvm_va_policy_node_t *node;
    NV_STATUS status;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    // If there is no HMM va_block or the va_block doesn't span the policy
    // addr, there is no need to split.
    status = uvm_hmm_va_block_find(va_space, addr, &va_block);
    if (status != NV_OK || va_block->start == addr)
        return NV_OK;

    uvm_mutex_lock(&va_block->lock);

    node = uvm_va_policy_node_find(va_block, addr);
    if (!node)
        goto done;

    // If the policy range doesn't span addr, we're done.
    if (addr == node->node.start)
        goto done;

    if (split_needed_cb(&node->policy, data))
        status = uvm_va_policy_node_split(va_block, node, addr - 1, NULL);

done:
    uvm_mutex_unlock(&va_block->lock);
    return status;
}

NV_STATUS uvm_hmm_set_preferred_location(uvm_va_space_t *va_space,
                                         uvm_processor_id_t preferred_location,
                                         NvU64 base,
                                         NvU64 last_address)
{
    bool is_default = UVM_ID_IS_INVALID(preferred_location);
    uvm_va_block_t *va_block;
    NvU64 addr;
    NV_STATUS status = NV_OK;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(va_space->va_space_mm.mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);
    UVM_ASSERT(PAGE_ALIGNED(base));
    UVM_ASSERT(PAGE_ALIGNED(last_address + 1));
    UVM_ASSERT(base < last_address);

    // Update HMM preferred location policy.

    for (addr = base; addr < last_address; addr = va_block->end + 1) {
        NvU64 end;

        status = hmm_va_block_find_create(va_space, addr, true, NULL, &va_block);
        if (status != NV_OK)
            break;

        end = min(last_address, va_block->end);

        uvm_mutex_lock(&va_block->lock);

        status = uvm_va_policy_set_range(va_block,
                                         addr,
                                         end,
                                         UVM_VA_POLICY_PREFERRED_LOCATION,
                                         is_default,
                                         preferred_location,
                                         UVM_READ_DUPLICATION_MAX);

        // TODO: Bug 1750144: unset requires re-evaluating accessed-by mappings
        // (see uvm_va_range_set_preferred_location's call of
        // uvm_va_block_set_accessed_by), and set requires unmapping remote
        // mappings (uvm_va_block_set_preferred_location_locked).

        uvm_mutex_unlock(&va_block->lock);

        if (status != NV_OK)
            break;
    }

    return status;
}

NV_STATUS uvm_hmm_set_accessed_by(uvm_va_space_t *va_space,
                                  uvm_processor_id_t processor_id,
                                  bool set_bit,
                                  NvU64 base,
                                  NvU64 last_address)
{
    uvm_va_block_t *va_block;
    NvU64 addr;
    NV_STATUS status = NV_OK;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(va_space->va_space_mm.mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);
    UVM_ASSERT(PAGE_ALIGNED(base));
    UVM_ASSERT(PAGE_ALIGNED(last_address + 1));
    UVM_ASSERT(base < last_address);

    // Update HMM accessed by policy.

    for (addr = base; addr < last_address; addr = va_block->end + 1) {
        NvU64 end;

        status = hmm_va_block_find_create(va_space, addr, true, NULL, &va_block);
        if (status != NV_OK)
            break;

        end = min(last_address, va_block->end);

        uvm_mutex_lock(&va_block->lock);

        status = uvm_va_policy_set_range(va_block,
                                         addr,
                                         end,
                                         UVM_VA_POLICY_ACCESSED_BY,
                                         !set_bit,
                                         processor_id,
                                         UVM_READ_DUPLICATION_MAX);

        // TODO: Bug 1750144: need to call va_block_set_accessed_by_locked()
        // if read duplication isn't enabled.

        uvm_mutex_unlock(&va_block->lock);

        if (status != NV_OK)
            break;
    }

    return status;
}

void uvm_hmm_find_policy_end(uvm_va_block_t *va_block,
                             uvm_va_block_context_t *va_block_context,
                             unsigned long addr,
                             NvU64 *endp)
{
    struct vm_area_struct *vma = va_block_context->hmm.vma;
    uvm_va_policy_node_t *node;
    NvU64 end = va_block->end;

    uvm_assert_mmap_lock_locked(vma->vm_mm);
    uvm_assert_mutex_locked(&va_block->lock);

    if (end > vma->vm_end - 1)
        end = vma->vm_end - 1;

    node = uvm_va_policy_node_find(va_block, addr);
    if (node) {
        va_block_context->policy = &node->policy;
        if (end > node->node.end)
            end = node->node.end;
    }
    else {
        va_block_context->policy = &uvm_va_policy_default;
    }

    *endp = end;
}

NV_STATUS uvm_hmm_find_policy_vma_and_outer(uvm_va_block_t *va_block,
                                            uvm_va_block_context_t *va_block_context,
                                            uvm_page_index_t page_index,
                                            uvm_page_index_t *outerp)
{
    struct vm_area_struct *vma;
    unsigned long addr;
    NvU64 end;
    uvm_page_index_t outer;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mmap_lock_locked(va_block_context->mm);
    uvm_assert_mutex_locked(&va_block->lock);

    addr = uvm_va_block_cpu_page_address(va_block, page_index);

    vma = vma_lookup(va_block_context->mm, addr);
    if (!vma || !(vma->vm_flags & VM_READ))
        return NV_ERR_INVALID_ADDRESS;

    va_block_context->hmm.vma = vma;

    uvm_hmm_find_policy_end(va_block, va_block_context, addr, &end);

    outer = uvm_va_block_cpu_page_index(va_block, end) + 1;
    if (*outerp > outer)
        *outerp = outer;

    return NV_OK;
}

static NV_STATUS hmm_clear_thrashing_policy(uvm_va_block_t *va_block,
                                            uvm_va_block_context_t *block_context)
{
    uvm_va_policy_t *policy;
    uvm_va_policy_node_t *node;
    uvm_va_block_region_t region;
    NV_STATUS status = NV_OK;

    uvm_mutex_lock(&va_block->lock);

    uvm_for_each_va_policy_in(policy, va_block, va_block->start, va_block->end, node, region) {
        block_context->policy = policy;

        // Unmap may split PTEs and require a retry. Needs to be called
        // before the pinned pages information is destroyed.
        status = UVM_VA_BLOCK_RETRY_LOCKED(va_block,
                                           NULL,
                                           uvm_perf_thrashing_unmap_remote_pinned_pages_all(va_block,
                                                                                            block_context,
                                                                                            region));

        uvm_perf_thrashing_info_destroy(va_block);

        if (status != NV_OK)
            break;
    }

    uvm_mutex_unlock(&va_block->lock);

    return status;
}

NV_STATUS uvm_hmm_clear_thrashing_policy(uvm_va_space_t *va_space)
{
    uvm_va_block_context_t *block_context = uvm_va_space_block_context(va_space, NULL);
    uvm_range_tree_node_t *node, *next;
    uvm_va_block_t *va_block;
    NV_STATUS status = NV_OK;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_OK;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    uvm_range_tree_for_each_safe(node, next, &va_space->hmm.blocks) {
        va_block = hmm_va_block_from_node(node);

        status = hmm_clear_thrashing_policy(va_block, block_context);
        if (status != NV_OK)
            break;
    }

    return status;
}

uvm_va_block_region_t uvm_hmm_get_prefetch_region(uvm_va_block_t *va_block,
                                                  uvm_va_block_context_t *va_block_context,
                                                  NvU64 address)
{
    struct vm_area_struct *vma = va_block_context->hmm.vma;
    uvm_va_policy_t *policy = va_block_context->policy;
    NvU64 start, end;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));

    // We need to limit the prefetch region to the VMA.
    start = max(va_block->start, (NvU64)vma->vm_start);
    end = min(va_block->end, (NvU64)vma->vm_end - 1);

    // Also, we need to limit the prefetch region to the policy range.
    if (policy == &uvm_va_policy_default) {
        NV_STATUS status = uvm_range_tree_find_hole_in(&va_block->hmm.va_policy_tree,
                                                       address,
                                                       &start,
                                                       &end);
        // We already know the hole exists and covers the fault region.
        UVM_ASSERT(status == NV_OK);
    }
    else {
        uvm_va_policy_node_t *node = uvm_va_policy_node_from_policy(policy);

        start = max(start, node->node.start);
        end = min(end, node->node.end);
    }

    return uvm_va_block_region_from_start_end(va_block, start, end);
}

uvm_prot_t uvm_hmm_compute_logical_prot(uvm_va_block_t *va_block,
                                        uvm_va_block_context_t *va_block_context,
                                        NvU64 addr)
{
    struct vm_area_struct *vma = va_block_context->hmm.vma;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mmap_lock_locked(va_block_context->mm);
    UVM_ASSERT(vma && addr >= vma->vm_start && addr < vma->vm_end);

    if (!(vma->vm_flags & VM_READ))
        return UVM_PROT_NONE;
    else if (!(vma->vm_flags & VM_WRITE))
        return UVM_PROT_READ_ONLY;
    else
        return UVM_PROT_READ_WRITE_ATOMIC;
}

NV_STATUS uvm_test_split_invalidate_delay(UVM_TEST_SPLIT_INVALIDATE_DELAY_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    atomic64_set(&va_space->test.split_invalidate_delay_us, params->delay_us);

    return NV_OK;
}

NV_STATUS uvm_test_hmm_init(UVM_TEST_HMM_INIT_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    struct mm_struct *mm;
    NV_STATUS status;

    mm = uvm_va_space_mm_or_current_retain(va_space);
    if (!mm)
        return NV_WARN_NOTHING_TO_DO;

    uvm_down_write_mmap_lock(mm);
    uvm_va_space_down_write(va_space);
    if (va_space->hmm.disable)
        status = uvm_hmm_va_space_initialize_test(va_space);
    else
        status = NV_OK;
    uvm_va_space_up_write(va_space);
    uvm_up_write_mmap_lock(mm);
    uvm_va_space_mm_or_current_release(va_space, mm);

    return status;
}

NV_STATUS uvm_hmm_va_range_info(uvm_va_space_t *va_space,
                                struct mm_struct *mm,
                                UVM_TEST_VA_RANGE_INFO_PARAMS *params)
{
    uvm_range_tree_node_t *tree_node;
    uvm_va_policy_node_t *node;
    struct vm_area_struct *vma;
    uvm_va_block_t *va_block;

    if (!mm || !uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);

    params->type = UVM_TEST_VA_RANGE_TYPE_MANAGED;
    params->managed.subtype = UVM_TEST_RANGE_SUBTYPE_HMM;
    params->va_range_start = 0;
    params->va_range_end = ULONG_MAX;
    params->read_duplication = UVM_TEST_READ_DUPLICATION_UNSET;
    memset(&params->preferred_location, 0, sizeof(params->preferred_location));
    params->accessed_by_count = 0;
    params->managed.vma_start = 0;
    params->managed.vma_end = 0;
    params->managed.is_zombie = NV_FALSE;
    params->managed.owned_by_calling_process = (mm == current->mm ? NV_TRUE : NV_FALSE);

    vma = find_vma(mm, params->lookup_address);
    if (!uvm_hmm_vma_is_valid(vma, params->lookup_address, false))
        return NV_ERR_INVALID_ADDRESS;

    params->va_range_start = vma->vm_start;
    params->va_range_end   = vma->vm_end - 1;
    params->managed.vma_start = vma->vm_start;
    params->managed.vma_end   = vma->vm_end - 1;

    uvm_mutex_lock(&va_space->hmm.blocks_lock);
    tree_node = uvm_range_tree_find(&va_space->hmm.blocks, params->lookup_address);
    if (!tree_node) {
        UVM_ASSERT(uvm_range_tree_find_hole_in(&va_space->hmm.blocks, params->lookup_address,
                                               &params->va_range_start, &params->va_range_end) == NV_OK);
        uvm_mutex_unlock(&va_space->hmm.blocks_lock);
        return NV_OK;
    }

    uvm_mutex_unlock(&va_space->hmm.blocks_lock);
    va_block = hmm_va_block_from_node(tree_node);
    uvm_mutex_lock(&va_block->lock);

    params->va_range_start = va_block->start;
    params->va_range_end   = va_block->end;

    node = uvm_va_policy_node_find(va_block, params->lookup_address);
    if (node) {
        uvm_processor_id_t processor_id;

        if (params->va_range_start < node->node.start)
            params->va_range_start = node->node.start;
        if (params->va_range_end > node->node.end)
            params->va_range_end = node->node.end;

        params->read_duplication = node->policy.read_duplication;

        if (!UVM_ID_IS_INVALID(node->policy.preferred_location))
            uvm_va_space_processor_uuid(va_space, &params->preferred_location, node->policy.preferred_location);

        for_each_id_in_mask(processor_id, &node->policy.accessed_by)
            uvm_va_space_processor_uuid(va_space, &params->accessed_by[params->accessed_by_count++], processor_id);
    }
    else {
        uvm_range_tree_find_hole_in(&va_block->hmm.va_policy_tree, params->lookup_address,
                                    &params->va_range_start, &params->va_range_end);
    }

    uvm_mutex_unlock(&va_block->lock);

    return NV_OK;
}

// TODO: Bug 3660968: Remove this hack as soon as HMM migration is implemented
// for VMAs other than anonymous private memory.
bool uvm_hmm_must_use_sysmem(uvm_va_block_t *va_block,
                             uvm_va_block_context_t *va_block_context)
{
    struct vm_area_struct *vma = va_block_context->hmm.vma;

    uvm_assert_mutex_locked(&va_block->lock);

    if (!uvm_va_block_is_hmm(va_block))
        return false;

    UVM_ASSERT(vma);
    UVM_ASSERT(va_block_context->mm == vma->vm_mm);
    uvm_assert_mmap_lock_locked(va_block_context->mm);

    return !vma_is_anonymous(vma);
}

#endif // UVM_IS_CONFIG_HMM()

