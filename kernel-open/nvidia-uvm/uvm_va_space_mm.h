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

#ifndef __UVM_VA_SPACE_MM_H__
#define __UVM_VA_SPACE_MM_H__

#include "uvm_linux.h"
#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_test_ioctl.h"
#include "nv-kref.h"

#if defined(NV_LINUX_SCHED_MM_H_PRESENT)
#include <linux/sched/mm.h>
#elif defined(NV_LINUX_SCHED_H_PRESENT)
#include <linux/sched.h>
#endif

typedef enum
{
    // The va_space_mm has not yet been initialized by
    // uvm_api_mm_initialize(). Only current->mm can be retained for
    // !uvm_va_space_mm_enabled() VA spaces.
    UVM_VA_SPACE_MM_STATE_UNINITIALIZED,

    // The va_space_mm has been initialized and if this is a
    // uvm_va_space_mm_enabled() space CPU user space page tables are
    // valid due to holding a mm_struct reference. However the GPU VA
    // space may be in the process of being torn down.
    UVM_VA_SPACE_MM_STATE_ALIVE,

    // The va_space_mm has been released. There is no longer any
    // mm_struct associated with this va_space_mm and CPU page tables
    // may have been released. GPU VA state has been destroyed.
    UVM_VA_SPACE_MM_STATE_RELEASED,
} uvm_va_space_mm_state_t;

struct uvm_va_space_mm_struct
{
  // The mm currently associated with this VA space. Most callers shouldn't
  // access this directly, but should instead use uvm_va_space_mm_retain()/
  // uvm_va_space_mm_release().
  //
  // The pointer itself is valid when the va_space_mm state is
  // UVM_VA_SPACE_MM_STATE_ALIVE, but should only be considered usable
  // when retained or current.
  struct mm_struct *mm;

    // Lock protecting the state and retained_count fields.
    uvm_spinlock_t lock;

    uvm_va_space_mm_state_t state;

    // Refcount for uvm_va_space_mm_retain()/uvm_va_space_mm_release()
    NvU32 retained_count;

    // Wait queue for threads waiting for retainers to finish (retained_count
    // going to 0 when not alive).
    wait_queue_head_t last_retainer_wait_queue;

    // Available as scratch space for the internal APIs. This is like a caller-
    // save register: it shouldn't be used across function calls which also take
    // this va_space_mm.
    uvm_processor_mask_t scratch_processor_mask;
};

static bool uvm_va_space_mm_alive(struct uvm_va_space_mm_struct *va_space_mm)
{
    return va_space_mm->state == UVM_VA_SPACE_MM_STATE_ALIVE;
}

// Whether the system can support creating an association between a VA space and
// an mm.
bool uvm_va_space_mm_enabled_system(void);

// Whether this VA space is associated with an mm. This must not be called
// before uvm_va_space_initialize().
bool uvm_va_space_mm_enabled(uvm_va_space_t *va_space);

// Registers current->mm with the va_space. A reference is taken on the mm,
// meaning that until uvm_va_space_mm_unregister() is called the mm will remain
// a valid object in memory (mm_count), but is not guaranteed to remain alive
// (mm_users).
//
// Use uvm_va_space_mm_retain() to retrieve the mm.
//
// Locking: mmap_lock and the VA space lock must both be held for write.
NV_STATUS uvm_va_space_mm_register(uvm_va_space_t *va_space);

// Handles the va_space's mm being torn down while the VA space still exists.
// This function won't return until all in-flight retainers have called
// uvm_va_space_mm_release(). Subsequent calls to uvm_va_space_mm_retain() will
// return NULL.
//
// After this call returns the VA space is essentially dead. GPUs cannot make
// any new memory accesses in registered GPU VA spaces, and no more GPU faults
// which are attributed to this VA space will arrive. Additionally, no more
// registration within the VA space is allowed (GPU, GPU VA space, or channel).
//
// The requirements for this call are that, once we return, the GPU and
// driver are completely done using the associated mm_struct. This includes:
//
// 1) GPUs will not issue any more memory accesses under this mm
// 2) [ATS only] GPUs will not issue any more ATRs under this mm
// 3) The driver will not ask the kernel to service faults on this mm
//
// Locking: This function will take both mmap_lock and the VA space lock.
void uvm_va_space_mm_unregister(uvm_va_space_t *va_space);

// Retains the current mm registered with this VA space. If no mm is currently
// registered, or if the registered mm is in the process of tearing down, NULL
// is returned. Otherwise, the returned mm will remain valid for normal use
// (locking mmap_lock, find_vma, get_user_pages, cgroup-accounted allocations,
// etc) until uvm_va_space_mm_release() is called.
//
// A retained mm is guaranteed to have mm->users >= 1.
//
// It is NOT necessary to hold the VA space lock when calling this function.
struct mm_struct *uvm_va_space_mm_retain(uvm_va_space_t *va_space);

// Similar to uvm_va_space_mm_retain(), but falls back to returning
// current->mm when there is no mm registered with the VA space (that
// is, uvm_va_space_mm_enabled() would return false).
// uvm_va_space_mm_or_current_release() must be called to release the
// mm, and it must be called from the same thread which called
// uvm_va_space_mm_or_current_retain().
//
// If a non-NULL mm is returned, the guarantees described by
// uvm_va_space_mm_retain() apply. If uvm_va_space_mm_enabled() is
// false, the caller is responsible for validating that the returned
// mm matches the desired mm before performing an operation such as
// vm_insert_page(). See uvm_va_range_vma_check().
//
// This should not be called from a kernel thread.
struct mm_struct *uvm_va_space_mm_or_current_retain(uvm_va_space_t *va_space);

// Convenience wrapper around uvm_va_space_mm_retain() which also locks
// mmap_lock for read if valid.
static struct mm_struct *uvm_va_space_mm_retain_lock(uvm_va_space_t *va_space)
{
    struct mm_struct *mm = uvm_va_space_mm_retain(va_space);
    if (mm)
        uvm_down_read_mmap_lock(mm);
    return mm;
}

// Convenience wrapper around uvm_va_space_mm_or_current_retain() which also
// locks mmap_lock for read if valid.
static struct mm_struct *uvm_va_space_mm_or_current_retain_lock(uvm_va_space_t *va_space)
{
    struct mm_struct *mm = uvm_va_space_mm_or_current_retain(va_space);
    if (mm)
        uvm_down_read_mmap_lock(mm);
    return mm;
}

// Counterpart to uvm_va_space_mm_retain(). After this call, the mm must not be
// used again without another call to uvm_va_space_mm_retain().
void uvm_va_space_mm_release(uvm_va_space_t *va_space);

// Counterpart to uvm_va_space_mm_or_current_retain(). Must be called from the
// same thread which called uvm_va_space_mm_or_current_retain(). mm may be NULL,
// in which case this is a no-op. This function may invoke
// uvm_va_space_mm_shutdown() so the caller must not hold either mmap_lock or
// the VA space lock.
void uvm_va_space_mm_or_current_release(uvm_va_space_t *va_space, struct mm_struct *mm);

static void uvm_va_space_mm_release_unlock(uvm_va_space_t *va_space, struct mm_struct *mm)
{
    if (mm) {
        uvm_up_read_mmap_lock(mm);
        uvm_va_space_mm_release(va_space);
    }
}

static void uvm_va_space_mm_or_current_release_unlock(uvm_va_space_t *va_space, struct mm_struct *mm)
{
    if (mm) {
        uvm_up_read_mmap_lock(mm);
        uvm_va_space_mm_or_current_release(va_space, mm);
    }
}

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

static void uvm_mmgrab(struct mm_struct *mm)
{
#if defined(NV_MMGRAB_PRESENT)
    mmgrab(mm);
#else
    atomic_inc(&mm->mm_count);
#endif
}

static void uvm_mmdrop(struct mm_struct *mm)
{
    mmdrop(mm);
}
#else
static void uvm_mmput(struct mm_struct *mm)
{
    UVM_ASSERT(0);
}

static void uvm_mmgrab(struct mm_struct *mm)
{
    UVM_ASSERT(0);
}

static void uvm_mmdrop(struct mm_struct *mm)
{
    UVM_ASSERT(0);
}
#endif

NV_STATUS uvm_test_va_space_mm_retain(UVM_TEST_VA_SPACE_MM_RETAIN_PARAMS *params, struct file *filp);
NV_STATUS uvm_test_va_space_mm_or_current_retain(UVM_TEST_VA_SPACE_MM_OR_CURRENT_RETAIN_PARAMS *params,
                                                 struct file *filp);

#endif // __UVM_VA_SPACE_MM_H__
