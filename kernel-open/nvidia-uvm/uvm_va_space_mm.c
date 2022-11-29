/*******************************************************************************
    Copyright (c) 2018-2022 NVIDIA Corporation

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

#if defined(NV_LINUX_SCHED_MM_H_PRESENT)
#include <linux/sched/mm.h>
#elif defined(NV_LINUX_SCHED_H_PRESENT)
#include <linux/sched.h>
#endif

//
// This comment block describes some implementation rationale. See the header
// for the API descriptions.
//
// ========================= Retain count vs mm_users ==========================
//
// We use two methods to guarantee the mm is available and won't be destroyed.
//
// On the call paths where mmput() can be called, we call
// uvm_va_space_mm_or_current_retain() which calls mmget_not_zero(). This
// prevents mm teardown and avoids races with uvm_va_space_mm_shutdown() since
// it prevents mmput() -> __mmput() -> exit_mmap() -> mmu_notifier_release() ->
// uvm_va_space_mm_shutdown() until uvm_va_space_mm_or_current_release(), and
// we guarantee that we can't call uvm_va_space_mm_unregister() ->
// mmu_notifier_unregister() -> uvm_va_space_mm_shutdown() path when someone is
// about to call uvm_va_space_mm_or_current_retain().
// Kernel calls like mmu_interval_notifier_insert() require mm_users to be
// greater than 0. In general, these are the ioctl paths.
//
// On the replayable GPU fault handling path, we need the mm to be able to
// service faults in the window when mm_users == 0 but mmu_notifier_release()
// hasn't yet been called. We can't call mmput() because it may result in
// exit_mmap(), which could result in RM calls and VA space destroy. Those need
// to wait for the GPU fault handler to finish, so on that path we use an
// internal retained reference count and wait queue. When the mm is disabled
// via mmu_notifier_release(), we use the wait queue to wait for the reference
// count to go to 0.
// We also use this path for older Linux kernels where mm_users > 0 isn't
// required.
//
// ============================ Handling mm teardown ===========================
//
// mmu_notifiers call the mm release callback both when the mm is really getting
// shut down, and whenever mmu_notifier_unregister is called. This has several
// consequences, including that these two paths can race. If they do race, they
// wait for each other to finish (real teardown of the mm won't start until the
// mmu_notifier_unregister's callback has returned, and mmu_notifier_unregister
// won't return until the mm release callback has returned).
//
// When the mm is really getting torn down, uvm_va_space_mm_shutdown is expected
// to stop all GPU memory accesses to that mm and stop servicing faults in that
// mm. This essentially shuts down the VA space for new work. The VA space
// object remains valid for most teardown ioctls until the file is closed,
// because it's legal for the associated process to die then for another process
// with a reference on the file to perform the unregisters or associated ioctls.
// This is particularly true for tools users.
//
// An exception to the above is UvmUnregisterChannel. Since channels are
// completely removed from the VA space on mm teardown, later channel
// unregisters will fail to find the handles and will return an error.
//
// The UVM driver will only call mmu_notifier_unregister during VA space destroy
// (file close).
//
// Here is a table of the various teardown scenarios:
//
//                                                Can race with
// Scenario                                       mm teardown
// -----------------------------------------------------------------------------
// 1) Process exit (mm teardown, file open)            -
// 2) Explicit file close in original mm               No
// 3) Explicit file close in different mm              Yes
// 4) Implicit file close (exit) in original mm        No
// 5) Implicit file close (exit) in different mm       Yes
//
// At a high level, the sequence of operations to perform during mm teardown is:
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
// 5) Mark the va_space_mm as dead
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

#define UVM_VA_SPACE_MM_SHUTDOWN_DELAY_MAX_MS 100

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

static void uvm_va_space_mm_shutdown(uvm_va_space_t *va_space);

#if !defined(NV_MMGET_NOT_ZERO_PRESENT)
static bool mmget_not_zero(struct mm_struct *mm)
{
    return atomic_inc_not_zero(&mm->mm_users);
}
#endif

#if UVM_CAN_USE_MMU_NOTIFIERS()

    static void uvm_mmput(struct mm_struct *mm)
    {
        mmput(mm);
    }

    static uvm_va_space_t *get_va_space(struct mmu_notifier *mn)
    {
        // This may be called without a thread context present, so be careful
        // what is used here.
        return container_of(mn, uvm_va_space_t, va_space_mm.mmu_notifier);
    }

    static void uvm_mmu_notifier_release(struct mmu_notifier *mn, struct mm_struct *mm)
    {
        UVM_ENTRY_VOID(uvm_va_space_mm_shutdown(get_va_space(mn)));
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

    static struct mmu_notifier_ops uvm_mmu_notifier_ops_release =
    {
        .release = uvm_mmu_notifier_release,
    };

    static struct mmu_notifier_ops uvm_mmu_notifier_ops_ats =
    {
        .release          = uvm_mmu_notifier_release,
        .invalidate_range = uvm_mmu_notifier_invalidate_range_ats,
    };

    static int uvm_mmu_notifier_register(uvm_va_space_mm_t *va_space_mm)
    {
        UVM_ASSERT(va_space_mm->mm);
        uvm_assert_mmap_lock_locked_write(va_space_mm->mm);

        if (UVM_ATS_IBM_SUPPORTED_IN_DRIVER() && g_uvm_global.ats.enabled)
            va_space_mm->mmu_notifier.ops = &uvm_mmu_notifier_ops_ats;
        else
            va_space_mm->mmu_notifier.ops = &uvm_mmu_notifier_ops_release;

        return __mmu_notifier_register(&va_space_mm->mmu_notifier, va_space_mm->mm);
    }

    static void uvm_mmu_notifier_unregister(uvm_va_space_mm_t *va_space_mm)
    {
        mmu_notifier_unregister(&va_space_mm->mmu_notifier, va_space_mm->mm);
    }
#else
    static void uvm_mmput(struct mm_struct *mm)
    {
        UVM_ASSERT(0);
    }

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

    UVM_ASSERT(uvm_va_space_initialized(va_space) != NV_OK);
    if (!uvm_va_space_mm_enabled(va_space))
        return NV_OK;

    UVM_ASSERT(!va_space_mm->mm);
    va_space_mm->mm = current->mm;

    // We must be prepared to handle callbacks as soon as we make this call,
    // except for ->release() which can't be called since the mm belongs to
    // current.
    ret = uvm_mmu_notifier_register(va_space_mm);
    if (ret) {
        // Inform uvm_va_space_mm_unregister() that it has nothing to do.
        va_space_mm->mm = NULL;
        return errno_to_nv_status(ret);
    }

    uvm_spin_lock(&va_space_mm->lock);
    va_space_mm->alive = true;
    uvm_spin_unlock(&va_space_mm->lock);

    return NV_OK;
}

void uvm_va_space_mm_unregister(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;

    // We can't hold the VA space lock or mmap_lock across this function since
    // mmu_notifier_unregister() may trigger uvm_va_space_mm_shutdown(), which
    // takes those locks and also waits for other threads which may take those
    // locks.
    uvm_assert_unlocked_order(UVM_LOCK_ORDER_MMAP_LOCK);
    uvm_assert_unlocked_order(UVM_LOCK_ORDER_VA_SPACE);

    if (!va_space_mm->mm)
        return;

    UVM_ASSERT(uvm_va_space_mm_enabled(va_space));
    uvm_mmu_notifier_unregister(va_space_mm);

    // We're guaranteed that upon return from mmu_notifier_unregister(),
    // uvm_va_space_mm_shutdown() will have been called (though perhaps not by
    // this thread). Therefore all retainers have been flushed.
    UVM_ASSERT(!va_space_mm->alive);
    UVM_ASSERT(va_space_mm->retained_count == 0);
    va_space_mm->mm = NULL;
}

struct mm_struct *uvm_va_space_mm_retain(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    struct mm_struct *mm = NULL;

    UVM_ASSERT(uvm_va_space_initialized(va_space) == NV_OK);

    if (!uvm_va_space_mm_enabled(va_space))
        return NULL;

    uvm_spin_lock(&va_space_mm->lock);

    if (va_space_mm->alive) {
        ++va_space_mm->retained_count;
        mm = va_space_mm->mm;
        UVM_ASSERT(mm);
    }

    uvm_spin_unlock(&va_space_mm->lock);

    return mm;
}

struct mm_struct *uvm_va_space_mm_or_current_retain(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;

    // We should only attempt to use current->mm from a user thread
    UVM_ASSERT(!(current->flags & PF_KTHREAD));

    UVM_ASSERT(uvm_va_space_initialized(va_space) == NV_OK);

    // current->mm is NULL when we're in process teardown. In that case it
    // doesn't make sense to use any mm.
    if (!current->mm)
        return NULL;

    // If the va_space_mm matches current->mm then it would be safe but sub-
    // optimal to call mmget_not_zero(). current->mm is always valid to
    // use when non-NULL so there is no need to retain it.
    if (!uvm_va_space_mm_enabled(va_space) || va_space_mm->mm == current->mm)
        return current->mm;

    return mmget_not_zero(va_space_mm->mm) ? va_space_mm->mm : NULL;
}

void uvm_va_space_mm_release(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    bool do_wake = false;

    UVM_ASSERT(uvm_va_space_mm_enabled(va_space));

    // The mm must not have been torn down while we have it retained
    UVM_ASSERT(va_space_mm->mm);

    uvm_spin_lock(&va_space_mm->lock);

    UVM_ASSERT(va_space_mm->retained_count > 0);
    --va_space_mm->retained_count;

    // If we're the last retainer on a dead mm, signal any potential waiters
    if (va_space_mm->retained_count == 0 && !va_space_mm->alive)
        do_wake = true;

    uvm_spin_unlock(&va_space_mm->lock);

    // There could be multiple threads in uvm_va_space_mm_shutdown() waiting on
    // us, so we have to wake up all waiters.
    if (do_wake)
        wake_up_all(&va_space_mm->last_retainer_wait_queue);
}

void uvm_va_space_mm_or_current_release(uvm_va_space_t *va_space, struct mm_struct *mm)
{
    // We can't hold the VA space lock or mmap_lock across this function since
    // mmput() may trigger uvm_va_space_mm_shutdown(), which takes those locks
    // and also waits for other threads which may take those locks.
    uvm_assert_unlocked_order(UVM_LOCK_ORDER_MMAP_LOCK);
    uvm_assert_unlocked_order(UVM_LOCK_ORDER_VA_SPACE);

    if (mm && mm != current->mm)
        uvm_mmput(mm);
}

static void uvm_va_space_mm_shutdown_delay(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    NvU64 start_time;
    int num_threads;
    bool timed_out = false;

    if (!va_space_mm->test.delay_shutdown)
        return;

    start_time = NV_GETTIME();

    num_threads = atomic_inc_return(&va_space_mm->test.num_mm_shutdown_threads);
    UVM_ASSERT(num_threads > 0);

    if (num_threads == 1) {
        // Wait for another thread to arrive unless we time out
        while (atomic_read(&va_space_mm->test.num_mm_shutdown_threads) == 1) {
            if (NV_GETTIME() - start_time >= 1000*1000*UVM_VA_SPACE_MM_SHUTDOWN_DELAY_MAX_MS) {
                timed_out = true;
                break;
            }
        }

        if (va_space_mm->test.verbose)
            UVM_TEST_PRINT("Multiple threads: %d\n", !timed_out);
    }

    // No need to decrement num_mm_shutdown_threads since this va_space_mm is
    // being shut down.
}

// Handles the va_space's mm being torn down while the VA space still exists.
// This function won't return until all in-flight retainers have called
// uvm_va_space_mm_release(). Subsequent calls to uvm_va_space_mm_retain() will
// return NULL.
//
// uvm_va_space_mm_unregister() must still be called. It is guaranteed that
// uvm_va_space_mm_shutdown() will not be called after
// uvm_va_space_mm_unregister() returns, though they may execute concurrently.
// If so, uvm_va_space_mm_unregister() will not return until
// uvm_va_space_mm_shutdown() is done.
//
// After this call returns the VA space is essentially dead. GPUs cannot make
// any new memory accesses in registered GPU VA spaces, and no more GPU faults
// which are attributed to this VA space will arrive. Additionally, no more
// registration within the VA space is allowed (GPU, GPU VA space, or channel).
//
// The requirements for this callback are that, once we return, the GPU and
// driver are completely done using the associated mm_struct. This includes:
//
// 1) GPUs will not issue any more memory accesses under this mm
// 2) [ATS only] GPUs will not issue any more ATRs under this mm
// 3) The driver will not ask the kernel to service faults on this mm
//
static void uvm_va_space_mm_shutdown(uvm_va_space_t *va_space)
{
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_gpu_t *gpu;
    uvm_global_processor_mask_t gpus_to_flush;
    LIST_HEAD(deferred_free_list);

    // The mm must not have been torn down completely yet, but it may have been
    // marked as dead by a concurrent thread.
    UVM_ASSERT(uvm_va_space_mm_enabled(va_space));
    UVM_ASSERT(va_space_mm->mm);

    // Inject a delay for testing if requested
    uvm_va_space_mm_shutdown_delay(va_space);

    // There can be at most two threads here concurrently:
    //
    // 1) Thread A in process teardown of the original process
    //
    // 2) Thread B must be in the file close path of another process (either
    //    implicit or explicit), having already stopped all GPU accesses and
    //    having called uvm_va_space_mm_unregister.
    //
    // This corresponds to scenario #5 in the mm teardown block comment at the
    // top of the file. We serialize between these threads with the VA space
    // lock, but otherwise don't have any special handling: both threads will
    // execute the full teardown sequence below. Also, remember that the threads
    // won't return to their callers until both threads have returned from this
    // function (following the rules for mmu_notifier_unregister).

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
    // puts them on the deferred free list, so only one thread will do this.
    uvm_va_space_down_write(va_space);
    uvm_va_space_detach_all_user_channels(va_space, &deferred_free_list);
    uvm_va_space_global_gpus_in_mask(va_space, &gpus_to_flush, &va_space->faultable_processors);
    uvm_global_mask_retain(&gpus_to_flush);
    uvm_va_space_up_write(va_space);

    // Flush the fault buffer on all GPUs. This will avoid spurious cancels
    // of stale pending translated faults after we clear va_space_mm->alive
    // later.
    for_each_global_gpu_in_mask(gpu, &gpus_to_flush)
        uvm_gpu_fault_buffer_flush(gpu);

    uvm_global_mask_release(&gpus_to_flush);

    // Call nvUvmInterfaceUnsetPageDirectory. This has no effect on non-MPS.
    // Under MPS this guarantees that no new GPU accesses will be made using
    // this mm.
    //
    // We need only one thread to make this call, but two threads in here could
    // race for it, or we could have one thread in here and one in
    // destroy_gpu_va_space. Serialize these by starting in write mode then
    // downgrading to read.
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
    va_space_mm->alive = false;
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
    ret = NV_PIN_USER_PAGES_REMOTE(mm, (unsigned long)addr, 1, 0, &page, NULL, NULL);
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
        if ((uintptr_t)va_space == params->va_space_ptr && uvm_va_space_initialized(va_space) == NV_OK) {
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

NV_STATUS uvm_test_va_space_mm_delay_shutdown(UVM_TEST_VA_SPACE_MM_DELAY_SHUTDOWN_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_space_mm_t *va_space_mm = &va_space->va_space_mm;
    NV_STATUS status = NV_ERR_PAGE_TABLE_NOT_AVAIL;

    uvm_va_space_down_write(va_space);

    if (uvm_va_space_mm_retain(va_space)) {
        va_space_mm->test.delay_shutdown = true;
        va_space_mm->test.verbose = params->verbose;
        uvm_va_space_mm_release(va_space);
        status = NV_OK;
    }

    uvm_va_space_up_write(va_space);

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

    if (status == NV_OK) {
        if (params->sleep_us)
            usleep_range(params->sleep_us, params->sleep_us + 1000);

        params->mm_users = atomic_read(&mm->mm_users);
    }

    uvm_va_space_mm_or_current_release(va_space, mm);

    return status;
}
