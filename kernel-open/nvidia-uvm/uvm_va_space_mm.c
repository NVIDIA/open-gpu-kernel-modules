/*******************************************************************************
    Copyright (c) 2018-2023 NVIDIA Corporation

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

#include "uvm_common.h"
#include "uvm_kvmalloc.h"
#include "uvm_va_space.h"
#include "uvm_va_space_mm.h"
#include "uvm_ats.h"
#include "uvm_api.h"
#include "uvm_test.h"
#include "uvm_test_ioctl.h"

//
// This comment block describes some implementation rationale. See the header
// for the API descriptions.
//
// ========================= Retain count vs mm_users ==========================
//
// To guarantee the mm is available and won't be destroyed we require
// userspace to open a second file descriptor (uvm_mm_fd) and
// initialize it with uvm_api_mm_initialize(). During initialization
// we take a mm_users reference to ensure the mm remains valid until
// the file descriptor is closed.
//
// To ensure userspace can't close the file descriptor and drop the
// mm_users refcount while it is in use threads must call either
// uvm_va_space_mm_retain() or uvm_va_space_mm_or_current_retain() to
// increment the retained count. This also checks that userspace has
// initialized the uvm_mm_fd and therefore holds a valid pagetable
// pin.
//
// Closing uvm_mm_fd will call uvm_va_space_mm_shutdown() prior to
// mmput() which ensures there are no active users of the mm. This
// indirection is required because not all threads can call mmput()
// directly. In particular the replayable GPU fault handling path
// can't call mmput() because it may result in exit_mmap() which could
// result in RM calls and VA space destroy and those need to wait for
// the GPU fault handler to finish.
//
// ============================ Handling mm teardown ===========================
//
// When the process is exiting we will get notified either via an
// explict close of uvm_mm_fd or implicitly as part of
// exit_files(). We are guaranteed to get this call because we don't
// allow mmap on uvm_mm_fd, and the userspace pagetables (mm_users)
// are guaranteed to exist because we hold a mm_users refcount
// which is released as part of file close.
//
// This allows any outstanding GPU faults to be processed. To prevent
// new faults occurring uvm_va_space_mm_shutdown() is called to stop
// all GPU memory accesses to the mm. Once all GPU memory has been
// stopped no new retainers of the va_space will be allowed and the
// mm_users reference will be dropped, potentially tearing down the mm
// and associated pagetables.
//
// This essentially shuts down the VA space for new work. The VA space
// object remains valid for most teardown ioctls until the file is
// closed, because it's legal for the associated process to die then
// for another process with a reference on the file to perform the
// unregisters or associated ioctls.  This is particularly true for
// tools users.
//
// An exception to the above is UvmUnregisterChannel. Since channels are
// completely removed from the VA space on mm teardown, later channel
// unregisters will fail to find the handles and will return an error.
//
// At a high level, the sequence of operations to perform prior to mm
// teardown is:
//
// 1) Stop all channels
//      - Prevents new faults and accesses on non-MPS
// 2) Detach all channels
//      - Prevents pending faults from being translated to this VA space
//      - Non-replayable faults will be dropped so no new ones can arrive
//      - Access counter notifications will be prevented from getting new
//        translations to this VA space. Pending entries may attempt to retain
//        the mm, but will drop the notification if they can't be serviced.
// 3) Flush the fault buffer
//      - The only reason to flush the fault buffer is to avoid spurious
//        cancels. If we didn't flush the fault buffer before marking the mm
//        as dead, then remaining faults which require the mm would be
//        cancelled. Since the faults might be stale, we would record cancel
//        events which didn't really happen (the access didn't happen after
//        the mm died). By flushing we clear out all stale faults, and in
//        the case of MPS, cancel real faults after.
// 4) UnsetPageDir
//      - Prevents new accesses on MPS
// 5) Mark the va_space_mm as released
//      - Prevents new retainers from using the mm. There won't be any more on
//        the fault handling paths, but there could be others in worker threads.
//
// Here are some tables of each step in the sequence, and what operations can
// still be performed after each step. This is all from the perspective of a
// single VA space. "Untranslated" means that the fault entry has not been
// translated to a uvm_va_space yet.
//
// Replayable non-MPS Behavior:
//
//                  Can              Pending         Pending         Can be
//                  access   Can     untranslated    translated      servicing
//                  memory   fault   faults          faults          faults
// -----------------------------------------------------------------------------
// Shutdown start   Yes      Yes     Service         Service         Yes
// Stop channels    No       No      Service [1]     Service [1]     Yes [1]
// Detach channels  No       No      Flush buffer    Service [1]     Yes [1], [2]
// Flush buffer     No       No      None possible   None possible   No
// UnsetPageDir     No       No      None possible   None possible   No
//
//
// Replayable MPS Behavior:
//
//                  Can              Pending         Pending         Can be
//                  access   Can     untranslated    translated      servicing
//                  memory   fault   faults          faults          faults
// -----------------------------------------------------------------------------
// Shutdown start   Yes      Yes     Service         Service         Yes
// Stop channels    Yes      Yes     Service         Service         Yes
// Detach channels  Yes      Yes     Cancel, flush   Service         Yes
// Flush buffer     Yes      Yes     Cancel, flush   None possible   No
// UnsetPageDir     No [3]   Yes     Cancel, flush   None possible   No
//
//
// [1]: All pending faults in this VA space are stale since channel stop
//      preempted the context.
// [2]: Faults in this VA space can't be serviced concurrently with detach since
//      detach holds the VA space lock in write mode. Faults in other VA spaces
//      can be serviced, and stale faults in this VA space can resume service
//      after detach is done.
// [3]: Due to the nature of MPS, remaining work which had started under the VA
//      space could still execute and attempt to make memory accesses. However,
//      since the PDB at that point is empty and ATS is disabled (if available),
//      all accesses will fault and be cancelled rather than successfully
//      translate to physical memory.
//
// =============================================================================

static void uvm_va_space_mm_shutdown(uvm_va_space_t *va_space);

static int uvm_enable_va_space_mm = 1;
module_param(uvm_enable_va_space_mm, int, S_IRUGO);
MODULE_PARM_DESC(uvm_enable_va_space_mm,
                 "Set to 0 to disable UVM from using mmu_notifiers to create "
                 "an association between a UVM VA space and a process. This "
                 "will also disable pageable memory access via either ATS or "
                 "HMM.");

bool uvm_va_space_mm_enabled_system(void)
{
    return UVM_CAN_USE_MMU_NOTIFIERS() && uvm_enable_va_space_mm;
}

bool uvm_va_space_mm_enabled(uvm_va_space_t *va_space)
{
    // A va_space doesn't have any association with an mm in multi-process
    // sharing mode.
    if (va_space->initialization_flags & UVM_INIT_FLAGS_MULTI_PROCESS_SHARING_MODE)
        return false;

    return uvm_va_space_mm_enabled_system();
}

#if UVM_CAN_USE_MMU_NOTIFIERS()
    static uvm_va_space_t *get_va_space(struct mmu_notifier *mn)
    {
        // This may be called without a thread context present, so be careful
        // what is used here.
        return container_of(mn, uvm_va_space_t, va_space_mm.mmu_notifier);
    }

    static void uvm_mmu_notifier_invalidate_range_ats(struct mmu_notifier *mn,
                                                      struct mm_struct *mm,
                                                      unsigned long start,
                                                      unsigned long end)
    {
        // In most cases ->invalidate_range() is called with exclusive end.
        // uvm_ats_invalidate() expects an inclusive end so we have to
        // convert it.
        //
        // There's a special case however. Kernel TLB gathering sometimes
        // identifies "fullmm" invalidates by setting both start and end to ~0.
        //
        // It's unclear if there are any other cases in which the kernel will
        // call us with start == end. Since we can't definitively say no, we
        // conservatively treat all such calls as full invalidates.
        if (start == end) {
            start = 0;
            end = ~0UL;
        }
        else {
            --end;
        }

        UVM_ENTRY_VOID(uvm_ats_invalidate(get_va_space(mn), start, end));
    }

    static struct mmu_notifier_ops uvm_mmu_notifier_ops_ats =
    {
#if defined(NV_MMU_NOTIFIER_OPS_HAS_INVALIDATE_RANGE)
        .invalidate_range = uvm_mmu_notifier_invalidate_range_ats,
#elif defined(NV_MMU_NOTIFIER_OPS_HAS_ARCH_INVALIDATE_SECONDARY_TLBS)
        .arch_invalidate_secondary_tlbs = uvm_mmu_notifier_invalidate_range_ats,
#else
        #error One of invalidate_range/arch_invalid_secondary must be present
#endif
    };

    static int uvm_mmu_notifier_register(uvm_va_space_mm_t *va_space_mm)
    {
        UVM_ASSERT(va_space_mm->mm);
        uvm_assert_mmap_lock_locked_write(va_space_mm->mm);

        va_space_mm->mmu_notifier.ops = &uvm_mmu_notifier_ops_ats;
        return __mmu_notifier_register(&va_space_mm->mmu_notifier, va_space_mm->mm);
    }

    static void uvm_mmu_notifier_unregister(uvm_va_space_mm_t *va_space_mm)
    {
        mmu_notifier_unregister(&va_space_mm->mmu_notifier, va_space_mm->mm);
    }
#else
    static int uvm_mmu_notifier_register(uvm_va_space_mm_t *va_space_mm)
    {
        UVM_ASSERT(0);
        return 0;
    }

    static void uvm_mmu_notifier_unregister(uvm_va_space_mm_t *va_space_mm)
    {
        UVM_ASSERT(0);
    }
#endif // UVM_CAN_USE_MMU_NOTIFIERS()

NV_STATUS uvm_va_space_mm_register(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    int ret;

    uvm_assert_mmap_lock_locked_write(current->mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    va_space_mm->state = UVM_VA_SPACE_MM_STATE_UNINITIALIZED;

    if (!uvm_va_space_mm_enabled(va_space))
        return NV_OK;

    UVM_ASSERT(!va_space_mm->mm);
    va_space_mm->mm = current->mm;
    uvm_mmgrab(va_space_mm->mm);

    // We must be prepared to handle callbacks as soon as we make this call,
    // except for ->release() which can't be called since the mm belongs to
    // current.
    if (UVM_ATS_IBM_SUPPORTED_IN_DRIVER() && g_uvm_global.ats.enabled) {
        ret = uvm_mmu_notifier_register(va_space_mm);
        if (ret) {
            // Inform uvm_va_space_mm_unregister() that it has nothing to do.
            uvm_mmdrop(va_space_mm->mm);
            va_space_mm->mm = NULL;
            return errno_to_nv_status(ret);
        }
    }

    if ((UVM_IS_CONFIG_HMM() || UVM_HMM_RANGE_FAULT_SUPPORTED()) &&
        uvm_va_space_pageable_mem_access_supported(va_space)) {

        #if UVM_CAN_USE_MMU_NOTIFIERS()
            // Initialize MMU interval notifiers for this process. This allows
            // mmu_interval_notifier_insert() to be called without holding the
            // mmap_lock for write.
            // Note: there is no __mmu_notifier_unregister(), this call just
            // allocates memory which is attached to the mm_struct and freed
            // when the mm_struct is freed.
            ret = __mmu_notifier_register(NULL, current->mm);
            if (ret)
                return errno_to_nv_status(ret);
        #else
            UVM_ASSERT(0);
        #endif
    }

    return NV_OK;
}

void uvm_va_space_mm_unregister(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;

    // We can't hold the VA space lock or mmap_lock because
    // uvm_va_space_mm_shutdown() waits for retainers which may take
    // these locks.
    uvm_assert_unlocked_order(UVM_LOCK_ORDER_MMAP_LOCK);
    uvm_assert_unlocked_order(UVM_LOCK_ORDER_VA_SPACE);

    uvm_va_space_mm_shutdown(va_space);
    UVM_ASSERT(va_space_mm->retained_count == 0);

    // Only happens if uvm_va_space_mm_register() fails
    if (!va_space_mm->mm)
        return;

    if (uvm_va_space_mm_enabled(va_space)) {
        if (UVM_ATS_IBM_SUPPORTED_IN_DRIVER() && g_uvm_global.ats.enabled)
            uvm_mmu_notifier_unregister(va_space_mm);
        uvm_mmdrop(va_space_mm->mm);
    }
}

struct mm_struct *uvm_va_space_mm_retain(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    struct mm_struct *mm = NULL;

    if (!uvm_va_space_mm_enabled(va_space))
        return NULL;

    uvm_spin_lock(&va_space_mm->lock);

    if (!uvm_va_space_mm_alive(va_space_mm))
        goto out;

    ++va_space_mm->retained_count;

    mm = va_space_mm->mm;
    UVM_ASSERT(mm);

out:

    // uvm_api_mm_init() holds a reference
    if (mm)
        UVM_ASSERT(atomic_read(&mm->mm_users) > 0);

    uvm_spin_unlock(&va_space_mm->lock);

    return mm;
}

struct mm_struct *uvm_va_space_mm_or_current_retain(uvm_va_space_t *va_space)
{
    // We should only attempt to use current->mm from a user thread
    UVM_ASSERT(!(current->flags & PF_KTHREAD));

    // current->mm is NULL when we're in process teardown. In that case it
    // doesn't make sense to use any mm.
    if (!current->mm)
        return NULL;

    // If !uvm_va_space_mm_enabled() we use current->mm on the ioctl
    // paths. In that case we don't need to mmget(current->mm) because
    // the current thread mm is always valid. On
    // uvm_va_space_mm_enabled() systems we skip trying to retain the
    // mm if it is current->mm because userspace may not have
    // initialised the mm fd but UVM callers on the ioctl path still
    // assume retaining current->mm will succeed.
    if (!uvm_va_space_mm_enabled(va_space))
        return current->mm;

    return uvm_va_space_mm_retain(va_space);
}

void uvm_va_space_mm_release(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;

    UVM_ASSERT(uvm_va_space_mm_enabled(va_space));

    // The mm must not have been torn down while we have it retained
    UVM_ASSERT(va_space_mm->mm);

    uvm_spin_lock(&va_space_mm->lock);

    UVM_ASSERT(va_space_mm->retained_count > 0);
    --va_space_mm->retained_count;

    // If we're the last retainer on a dead mm, signal any potential waiters
    if (va_space_mm->retained_count == 0 && !uvm_va_space_mm_alive(va_space_mm)) {
        uvm_spin_unlock(&va_space_mm->lock);

        // There could be a thread in uvm_va_space_mm_shutdown()
        // waiting on us, so wake it up.
        wake_up(&va_space_mm->last_retainer_wait_queue);
    }
    else {
        uvm_spin_unlock(&va_space_mm->lock);
    }
}

void uvm_va_space_mm_or_current_release(uvm_va_space_t *va_space, struct mm_struct *mm)
{
    if (!uvm_va_space_mm_enabled(va_space) || !mm)
        return;

    uvm_va_space_mm_release(va_space);
}

static void uvm_va_space_mm_shutdown(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_gpu_t *gpu;
    uvm_processor_mask_t *retained_gpus = &va_space_mm->scratch_processor_mask;
    uvm_parent_processor_mask_t flushed_parent_gpus;
    LIST_HEAD(deferred_free_list);

    uvm_va_space_down_write(va_space);

    // Prevent future registrations of any kind. We'll be iterating over all
    // GPUs and GPU VA spaces below but taking and dropping the VA space lock.
    // It's ok for other threads to unregister those objects, but not to
    // register new ones.
    //
    // We also need to prevent new channel work from arriving since we're trying
    // to stop memory accesses.
    va_space->disallow_new_registers = true;

    uvm_va_space_downgrade_write_rm(va_space);

    // Stop channels to prevent new accesses and new faults on non-MPS
    uvm_va_space_stop_all_user_channels(va_space);

    uvm_va_space_up_read_rm(va_space);

    // Detach all channels to prevent pending untranslated faults from getting
    // to this VA space. This also removes those channels from the VA space and
    // puts them on the deferred free list.
    uvm_va_space_down_write(va_space);
    uvm_va_space_detach_all_user_channels(va_space, &deferred_free_list);
    uvm_processor_mask_and(retained_gpus, &va_space->registered_gpus, &va_space->faultable_processors);
    uvm_global_gpu_retain(retained_gpus);
    uvm_va_space_up_write(va_space);

    // It's ok to use retained_gpus outside the lock since there can only be one
    // thread executing in uvm_va_space_mm_shutdown at a time.

    // Flush the fault buffer on all registered faultable GPUs.
    // This will avoid spurious cancels of stale pending translated
    // faults after we set UVM_VA_SPACE_MM_STATE_RELEASED later.
    uvm_parent_processor_mask_zero(&flushed_parent_gpus);
    for_each_gpu_in_mask(gpu, retained_gpus) {
        if (!uvm_parent_processor_mask_test_and_set(&flushed_parent_gpus, gpu->parent->id))
            uvm_gpu_fault_buffer_flush(gpu);
    }

    uvm_global_gpu_release(retained_gpus);

    // Call nvUvmInterfaceUnsetPageDirectory. This has no effect on non-MPS.
    // Under MPS this guarantees that no new GPU accesses will be made using
    // this mm.
    //
    // We need only one thread to make this call, but we could have one thread
    // in here and one in destroy_gpu_va_space. Serialize these by starting in
    // write mode then downgrading to read.
    uvm_va_space_down_write(va_space);
    uvm_va_space_downgrade_write_rm(va_space);
    for_each_gpu_va_space(gpu_va_space, va_space)
        uvm_gpu_va_space_unset_page_dir(gpu_va_space);
    uvm_va_space_up_read_rm(va_space);

    // The above call to uvm_gpu_va_space_unset_page_dir handles the GPU VA
    // spaces which are known to be registered. However, we could've raced with
    // a concurrent uvm_va_space_unregister_gpu_va_space, giving this sequence:
    //
    // unregister_gpu_va_space                  uvm_va_space_mm_shutdown
    //     uvm_va_space_down_write
    //     remove_gpu_va_space
    //     uvm_va_space_up_write
    //                                          uvm_va_space_down_write(va_space);
    //                                          // No GPU VA spaces
    //                                          Unlock, return
    //     uvm_deferred_free_object_list
    //         uvm_gpu_va_space_unset_page_dir
    //
    // We have to be sure that all accesses in this GPU VA space are done before
    // returning, so we have to wait for the other thread to finish its
    // uvm_gpu_va_space_unset_page_dir call.
    //
    // We can be sure that num_pending will eventually go to zero because we've
    // prevented new GPU VA spaces from being registered above.
    wait_event(va_space->gpu_va_space_deferred_free.wait_queue,
               atomic_read(&va_space->gpu_va_space_deferred_free.num_pending) == 0);

    // Now that there won't be any new GPU faults, prevent subsequent retainers
    // from accessing this mm.
    uvm_spin_lock(&va_space_mm->lock);
    va_space_mm->state = UVM_VA_SPACE_MM_STATE_RELEASED;
    uvm_spin_unlock(&va_space_mm->lock);

    // Finish channel destroy. This can be done at any point after detach as
    // long as we don't hold the VA space lock.
    uvm_deferred_free_object_list(&deferred_free_list);

    // Flush out all pending retainers
    wait_event(va_space_mm->last_retainer_wait_queue, va_space_mm->retained_count == 0);
}

static NV_STATUS mm_read64(struct mm_struct *mm, NvU64 addr, NvU64 *val)
{
    long ret;
    struct page *page;
    NvU64 *mapping;

    UVM_ASSERT(IS_ALIGNED(addr, sizeof(*val)));

    uvm_down_read_mmap_lock(mm);
    ret = NV_PIN_USER_PAGES_REMOTE(mm, (unsigned long)addr, 1, 0, &page, NULL);
    uvm_up_read_mmap_lock(mm);

    if (ret < 0)
        return errno_to_nv_status(ret);

    UVM_ASSERT(ret == 1);

    mapping = (NvU64 *)((char *)kmap(page) + (addr % PAGE_SIZE));
    *val = *mapping;
    kunmap(page);
    NV_UNPIN_USER_PAGE(page);

    return NV_OK;
}

NV_STATUS uvm_test_va_space_mm_retain(UVM_TEST_VA_SPACE_MM_RETAIN_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = NULL;
    struct mm_struct *mm = NULL;
    NV_STATUS status = NV_OK;

    if (!IS_ALIGNED(params->addr, sizeof(params->val_before)))
        return NV_ERR_INVALID_ARGUMENT;

    uvm_mutex_lock(&g_uvm_global.va_spaces.lock);

    list_for_each_entry(va_space, &g_uvm_global.va_spaces.list, list_node) {
        if ((uintptr_t)va_space == params->va_space_ptr) {
            mm = uvm_va_space_mm_retain(va_space);
            break;
        }
    }

    uvm_mutex_unlock(&g_uvm_global.va_spaces.lock);

    if ((uintptr_t)va_space != params->va_space_ptr)
        return NV_ERR_MISSING_TABLE_ENTRY;

    if (!mm)
        return NV_ERR_PAGE_TABLE_NOT_AVAIL;

    status = mm_read64(mm, params->addr, &params->val_before);

    if (status == NV_OK && params->sleep_us) {
        usleep_range(params->sleep_us, params->sleep_us + 1000);
        status = mm_read64(mm, params->addr, &params->val_after);
    }

    uvm_va_space_mm_release(va_space);
    return status;
}

NV_STATUS uvm_test_va_space_mm_or_current_retain(UVM_TEST_VA_SPACE_MM_OR_CURRENT_RETAIN_PARAMS *params,
                                                 struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    struct mm_struct *mm;
    NV_STATUS status = NV_OK;

    mm = uvm_va_space_mm_or_current_retain(va_space);
    if (!mm)
        return NV_ERR_PAGE_TABLE_NOT_AVAIL;

    if (params->retain_done_ptr) {
        NvU64 flag = true;

        if (nv_copy_to_user((void __user *)params->retain_done_ptr, &flag, sizeof(flag)))
            status = NV_ERR_INVALID_ARGUMENT;
    }

    if (status == NV_OK && params->sleep_us)
            usleep_range(params->sleep_us, params->sleep_us + 1000);

    uvm_va_space_mm_or_current_release(va_space, mm);

    return status;
}
