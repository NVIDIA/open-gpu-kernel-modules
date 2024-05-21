/*******************************************************************************
    Copyright (c) 2015-2022 NVIDIA Corporation

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

#ifndef __UVM_LOCK_H__
#define __UVM_LOCK_H__

#include "uvm_forward_decl.h"
#include "uvm_linux.h"
#include "uvm_common.h"

// --------------------------- UVM Locking Order ---------------------------- //
//
// Any locks described here should have their locking order added to
// uvm_lock_order_t below.
//
// - Global power management lock (g_uvm_global.pm.lock)
//      Order: UVM_LOCK_ORDER_GLOBAL_PM
//      Reader/write lock (rw_semaphore)
//
//      Synchronizes user threads with system power management.
//
//      Taken in read mode by most user-facing UVM driver entry points.  Taken
//      in write mode by uvm_suspend(), only, and held for the duration of
//      sleep cycles.
//
//      This lock is special: while it's taken by user-facing entry points,
//      and may be taken before or after mmap_lock, this apparent violation of
//      lock ordering is permissible because pm_lock may only be taken via
//      trylock in read mode by paths which already hold any lower-level
//      locks, as well as by paths subject to the kernel's freezer.  Paths
//      taking it must be prepared to back off in case of acquisition failures.
//
//      This, in turn, is acceptable because the lock is taken in write mode
//      infrequently, and only as part of to power management.  Starvation is
//      not a concern.
//
//      The mmap_lock deadlock potential aside, the trylock approch is also
//      motivated by the need to prevent user threads making UVM system calls
//      from blocking when UVM is suspended: when the kernel suspends the
//      system, the freezer employed to stop user tasks requires these tasks
//      to be interruptible.
//
// - Global driver state lock (g_uvm_global.global_lock)
//      Order: UVM_LOCK_ORDER_GLOBAL
//      Exclusive lock (mutex)
//
//      This protects state associated with GPUs, such as the P2P table
//      and instance pointer mappings.
//
//      This should be taken whenever global GPU state might need to be modified.
//
// - GPU ISR lock
//      Order: UVM_LOCK_ORDER_ISR
//      Exclusive lock (mutex) per gpu
//
//      Protects:
//      - gpu->parent->isr.replayable_faults.service_lock:
//        Changes to the state of a GPU as it transitions from top-half to
//        bottom-half interrupt handler for replayable faults. This lock is
//        acquired for that GPU, in the ISR top-half. Then a bottom-half is
//        scheduled (to run in a workqueue). Then the bottom-half releases the
//        lock when that GPU's processing appears to be done.
//
//      - gpu->parent->isr.non_replayable_faults.service_lock:
//        Changes to the state of a GPU in the bottom-half for non-replayable
//        faults. Non-replayable faults are handed-off from RM instead of
//        directly from the GPU hardware. This means that we do not keep
//        receiving interrupts after RM pops out the faults from the HW buffer.
//        In order not to miss fault notifications, we will always schedule a
//        bottom-half for non-replayable faults if there are faults ready to be
//        consumed in the buffer, even if there already is some bottom-half
//        running or scheduled. This lock serializes all scheduled bottom halves
//        per GPU which service non-replayable faults.
//
//      - gpu->parent->isr.access_counters.service_lock:
//        Changes to the state of a GPU as it transitions from top-half to
//        bottom-half interrupt handler for access counter notifications. This
//        lock is acquired for that GPU, in the ISR top-half. Then a bottom-half
//        is scheduled (to run in a workqueue). Then the bottom-half releases
//        the lock when that GPU's processing appears to be done.
//
// - mmap_lock (mmap_sem in kernels < 5.8)
//      Order: UVM_LOCK_ORDER_MMAP_LOCK
//      Reader/writer lock (rw_semaphore)
//
//      We're often called with the kernel already holding mmap_lock: mmap,
//      munmap, CPU fault, etc. These operations may have to take any number of
//      UVM locks, so mmap_lock requires special consideration in the lock
//      order, since it's sometimes out of our control.
//
//      We need to hold mmap_lock when calling vm_insert_page, which means that
//      any time an operation (such as an ioctl) might need to install a CPU
//      mapping, it must take mmap_lock in read mode very early on.
//
//      However, current->mm is not necessarily the owning mm of the UVM vma.
//      fork or fd passing via a UNIX doman socket can cause that. Notably, this
//      is also the case when handling GPU faults or doing other operations from
//      a kernel thread. In some cases we have an mm associated with a VA space,
//      and in those cases we lock that mm instead of current->mm. But since we
//      don't always have that luxury, each path specifies the mm to use (either
//      explicitly or via uvm_va_block_context_t::mm). That mm may be NULL.
//      Later on down the stack we look up the UVM vma and compare its mm before
//      operating on that vma.
//
//      With HMM and ATS, the GPU fault handler takes mmap_lock. GPU faults may
//      block forward progress of threads holding the RM GPUs lock until those
//      faults are serviced, which means that mmap_lock cannot be held when the
//      UVM driver calls into RM. In other words, mmap_lock and the RM GPUs lock
//      are mutually exclusive.
//
// - Global VA spaces list lock
//      Order: UVM_LOCK_ORDER_VA_SPACES_LIST
//      Mutex which protects g_uvm_global.va_spaces state.
//
// - VA space writer serialization lock (va_space->serialize_writers_lock)
//      Order: UVM_LOCK_ORDER_VA_SPACE_SERIALIZE_WRITERS
//      Exclusive lock (mutex) per uvm_va_space (UVM struct file)
//
//      This lock prevents a deadlock between RM and UVM by only allowing one
//      writer to queue up on the VA space lock at a time.
//
//      GPU faults are serviced by the UVM bottom half with the VA space lock
//      held in read mode. Until they're serviced, these faults may block
//      forward progress of RM threads.
//
//      This constraint means that the UVM driver cannot call into RM while
//      GPU fault servicing is blocked. We may block GPU fault servicing by:
//      - Taking the VA space lock in write mode
//      - Holding the VA space lock in read mode with a writer pending, since
//        Linux rw_semaphores are fair.
//
//      Example of the second condition:
//      Thread A        Thread B        UVM BH          Thread C
//      UVM API call    UVM API call    GPU fault       RM API call
//      ------------    ------------    ------------    ------------
//      down_read
//                      down_write
//                      // Blocked on A
//                                      down_read
//                                      // Blocked on B
//                                                      RM GPU lock
//                                                      // Blocked on GPU fault
//      RM GPU lock
//      // Deadlock
//
//      The writer serialization lock works around this by biasing the VA space
//      lock towards readers, without causing starvation of writers. Writers and
//      readers which will make RM calls take this lock, which prevents them
//      from queueing up on the VA space rw_semaphore and blocking the UVM
//      bottom half.
//
//      TODO: Bug 1799173: A better long-term approach might be to never allow
//            RM calls under the VA space lock at all, but that will take a
//            larger restructuring.
//
// - VA space serialization of down_read with up_write of the VA space lock
//   (va_space->read_acquire_write_release_lock)
//      Order: UVM_LOCK_ORDER_VA_SPACE_READ_ACQUIRE_WRITE_RELEASE_LOCK
//      Exclusive lock (mutex) per uvm_va_space (UVM struct file)
//
//      This lock prevents a deadlock between RM and UVM by preventing any
//      interleaving of down_reads on the VA space lock with concurrent
//      up_writes/downgrade_writes. The Linux rw_semaphore implementation does
//      not guarantee that two readers will always run concurrently, as shown by
//      the following interleaving:
//
//      Thread A                Thread B
//      UVM API call            UVM BH
//      ------------            ------------
//      down_write
//                              down_read
//                                  // Fails, calls handler
//      up_write
//      down_read
//          // Success
//                                  // Handler sees the lock still active
//                                  // Handler waits for lock to be released
//                                  // Blocked on A
//      RM GPU lock
//      // Blocked on GPU fault
//
//      Given the above interleaving, the kernel's implementation of the
//      down_read failure handler running in thread B does not distinguish
//      between a reader vs writer holding the lock. From the perspective of all
//      other threads, even those which attempt to take the lock for read while
//      thread A's reader holds it, a writer is active. Therefore no other
//      readers can take the lock, and we result in the same deadlock described
//      in the above comments on the VA space writer serialization lock.
//
//      This lock prevents any such interleaving:
//      - Writers take this lock for the duration of the write lock.
//
//      - Readers which do not call into RM only take this lock across the
//        down_read call. If a writer holds the lock, the reader would be
//        blocked on the VA space lock anyway. Concurrent readers will serialize
//        the taking of the VA space lock, but they will not be serialized
//        across their read sections.
//
//      - Readers which call into RM do not need to take this lock. Their
//        down_read is already serialized with a writer's up_write by the
//        serialize_writers_lock.
//
// - VA space lock (va_space->lock)
//      Order: UVM_LOCK_ORDER_VA_SPACE
//      Reader/writer lock (rw_semaphore) per uvm_va_space (UVM struct file)
//
//      This is the UVM equivalent of mmap_lock. It protects all state under
//      that va_space, such as the VA range tree.
//
//      Read mode: Faults (CPU and GPU), mapping creation, prefetches. These
//      will be serialized at the VA block level if necessary. RM calls are
//      allowed only if the VA space serialize_writers_lock is also taken.
//
//      Write mode: Modification of the range state such as mmap and changes to
//      logical permissions or location preferences. RM calls are never allowed.
//
// - External Allocation Tree lock
//      Order: UVM_LOCK_ORDER_EXT_RANGE_TREE
//      Exclusive lock (mutex) per external VA range, per GPU.
//
//      Protects the per-GPU sub-range tree mappings in each external VA range.
//
// - GPU semaphore pool lock (semaphore_pool->mutex)
//      Order: UVM_LOCK_ORDER_GPU_SEMAPHORE_POOL
//      Exclusive lock (mutex) per uvm_gpu_semaphore_pool
//
//      Protects the state of the semaphore pool.
//
// - RM API lock
//      Order: UVM_LOCK_ORDER_RM_API
//      Exclusive lock
//
//      This is an internal RM lock that's acquired by most if not all UVM-RM
//      APIs.
//      Notably this lock is also held on PMA eviction.
//
// - RM GPUs lock
//      Order: UVM_LOCK_ORDER_RM_GPUS
//      Exclusive lock
//
//      This is an internal RM lock that's acquired by most if not all UVM-RM
//      APIs and disables interrupts for the GPUs.
//      Notably this lock is *not* held on PMA eviction.
//
// - VA block lock (va_block->lock)
//      Order: UVM_LOCK_ORDER_VA_BLOCK
//      Exclusive lock (mutex)
//
//      Protects:
//      - CPU and GPU page table mappings for all VAs under the block
//      - Updates to the GPU work tracker for that block (migrations)
//
//      Operations allowed while holding the lock:
//      - CPU allocation (we don't evict CPU memory)
//      - GPU memory allocation which cannot evict
//      - CPU page table mapping/unmapping
//      - Pushing work (GPU page table mapping/unmapping)
//
//      Operations not allowed while holding the lock:
//      - GPU memory allocation which can evict memory (would require nesting
//        block locks)
//
// - GPU DMA Allocation pool lock (gpu->conf_computing.dma_buffer_pool.lock)
//      Order: UVM_LOCK_ORDER_CONF_COMPUTING_DMA_BUFFER_POOL
//      Condition: The Confidential Computing feature is enabled
//      Exclusive lock (mutex)
//
//      Protects:
//      - Protect the state of the uvm_conf_computing_dma_buffer_pool_t
//
// - Chunk mapping lock (gpu->root_chunk_mappings.bitlocks and
//   gpu->sysmem_mappings.bitlock)
//      Order: UVM_LOCK_ORDER_CHUNK_MAPPING
//      Exclusive bitlock (mutex) per each root chunk, or physical sysmem
//      segment.
//
//      A chunk mapping lock is used to enforce serialization when updating
//      kernel mappings of GPU root chunks (vidmem), or CPU chunks (sysmem).
//      The VA block lock is usually held during the mapping operation.
//
//      In the case of vidmem, each lock in the bitlock array serializes the
//      mapping and unmapping of a single GPU root chunk. If serialization
//      is required to update a root chunk, but no mappings are involved, use
//      the PMM root chunk lock (order UVM_LOCK_ORDER_PMM_ROOT_CHUNK) instead.
//
//      In the case of sysmem, each lock in the array serializes the mapping
//      of a large segment of system address space: the locking granularity is
//      significantly coarser than the CPU chunk size.
//
// - Page tree lock
//      Order: UVM_LOCK_ORDER_PAGE_TREE
//      Exclusive lock per GPU page tree
//
//      This protects a page tree.  All modifications to the device's page tree
//      and the host-side cache of that tree must be done under this lock.
//      The host-side cache and device state must be consistent when this lock
//      is released
//
//      Operations allowed while holding this lock
//      - Pushing work
//
//      Operations not allowed while holding this lock
//      - GPU memory allocation which can evict
//
// - CE channel CSL channel pool semaphore
//      Order: UVM_LOCK_ORDER_CSL_PUSH
//      Condition: The Confidential Computing feature is enabled
//      Semaphore per CE channel pool
//
//      The semaphore controls concurrent pushes to CE channels that are not WCL
//      channels. Secure work submission depends on channel availability in
//      GPFIFO entries (as in any other channel type) but also on channel
//      locking. Each channel has a lock to enforce ordering of pushes. The
//      channel's CSL lock is taken on channel reservation until uvm_push_end.
//      When the Confidential Computing feature is enabled, channels are
//      stateful, and the CSL lock protects their CSL state/context.
//
//      Operations allowed while holding this lock
//      - Pushing work to CE channels (except for WLC channels)
//
// - WLC CSL channel pool semaphore
//      Order: UVM_LOCK_ORDER_CSL_WLC_PUSH
//      Condition: The Confidential Computing feature is enabled
//      Semaphore per WLC channel pool
//
//      The semaphore controls concurrent pushes to WLC channels. WLC work
//      submission depends on channel availability in GPFIFO entries (as in any
//      other channel type) but also on channel locking. Each WLC channel has a
//      lock to enforce ordering of pushes. The channel's CSL lock is taken on
//      channel reservation until uvm_push_end. SEC2 channels are stateful
//      channels and the CSL lock protects their CSL state/context.
//
//      This lock ORDER is different and sits below the generic channel CSL
//      lock and above the SEC2 CSL lock. This reflects the dual nature of WLC
//      channels; they use SEC2 indirect work launch during initialization,
//      and after their schedule is initialized they provide indirect launch
//      functionality to other CE channels.
//
//      Operations allowed while holding this lock
//      - Pushing work to WLC channels
//
// - SEC2 CSL channel pool semaphore
//      Order: UVM_LOCK_ORDER_SEC2_CSL_PUSH
//      Condition: The Confidential Computing feature is enabled
//      Semaphore per SEC2 channel pool
//
//      The semaphore controls concurrent pushes to SEC2 channels. SEC2 work
//      submission depends on channel availability in GPFIFO entries (as in any
//      other channel type) but also on channel locking. Each SEC2 channel has a
//      lock to enforce ordering of pushes. The channel's CSL lock is taken on
//      channel reservation until uvm_push_end. SEC2 channels are stateful
//      channels and the CSL lock protects their CSL state/context.
//
//      This lock ORDER is different and lower than UVM_LOCK_ORDER_CSL_PUSH
//      to allow secure work submission to use a SEC2 channel to submit
//      work before releasing the CSL lock of the originating channel.
//
//      Operations allowed while holding this lock
//      - Pushing work to SEC2 channels
//
// - Concurrent push semaphore
//      Order: UVM_LOCK_ORDER_PUSH
//      Semaphore (uvm_semaphore_t)
//
//      This is a semaphore limiting the amount of concurrent pushes that is
//      held for the duration of a push (between uvm_push_begin*() and
//      uvm_push_end()).
//
// - PMM GPU lock (pmm->lock)
//      Order: UVM_LOCK_ORDER_PMM
//      Exclusive lock (mutex) per uvm_pmm_gpu_t
//
//      Protects the state of PMM - internal to PMM.
//
// - PMM GPU PMA lock (pmm->pma_lock)
//      Order: UVM_LOCK_ORDER_PMM_PMA
//      Reader/writer lock (rw_semaphore) per per uvm_pmm_gpu_t
//
//      Lock internal to PMM for synchronizing allocations from PMA with
//      PMA eviction.
//
// - PMM root chunk lock (pmm->root_chunks.bitlocks)
//      Order: UVM_LOCK_ORDER_PMM_ROOT_CHUNK
//      Exclusive bitlock (mutex) per each root chunk internal to PMM.
//
// - Channel lock
//      Order: UVM_LOCK_ORDER_CHANNEL
//      Spinlock (uvm_spinlock_t) or exclusive lock (mutex)
//
//      Lock protecting the state of all the channels in a channel pool. The
//      channel pool lock documentation contains the guidelines about which lock
//      type (mutex or spinlock) to use.
//
// - WLC Channel lock
//      Order: UVM_LOCK_ORDER_WLC_CHANNEL
//      Condition: The Confidential Computing feature is enabled
//      Spinlock (uvm_spinlock_t)
//
//      Lock protecting the state of WLC channels in a channel pool. This lock
//      is separate from the generic channel lock (UVM_LOCK_ORDER_CHANNEL)
//      to allow for indirect worklaunch pushes while holding the main channel
//      lock (WLC pushes don't need any of the pushbuffer locks described
//      above)
//
// - Tools global VA space list lock (g_tools_va_space_list_lock)
//      Order: UVM_LOCK_ORDER_TOOLS_VA_SPACE_LIST
//      Reader/writer lock (rw_semaphore)
//
//      This lock protects the list of VA spaces used when broadcasting
//      UVM profiling events.
//
// - VA space events
//      Order: UVM_LOCK_ORDER_VA_SPACE_EVENTS
//      Reader/writer lock (rw_semaphore) per uvm_perf_va_space_events_t.
//      serializes perf callbacks with event register/unregister. It's separate
//      from the VA space lock so it can be taken on the eviction path.
//
// - VA space tools
//      Order: UVM_LOCK_ORDER_VA_SPACE_TOOLS
//      Reader/writer lock (rw_semaphore) per uvm_va_space_t. Serializes tools
//      reporting with tools register/unregister. Since some of the tools
//      events come from perf events, both VA_SPACE_EVENTS and VA_SPACE_TOOLS
//      must be taken to register/report some tools events.
//
// - Tracking semaphores
//      Order: UVM_LOCK_ORDER_SECURE_SEMAPHORE
//      Condition: The Confidential Computing feature is enabled
//
//      CE semaphore payloads are encrypted, and require to take the CSL lock
//      (UVM_LOCK_ORDER_LEAF) to decrypt the payload.

// - CSL Context
//      Order: UVM_LOCK_ORDER_CSL_CTX
//      When the Confidential Computing feature is enabled, encrypt/decrypt
//      operations to communicate with GPU are handled by the CSL context.
//      This lock protects RM calls that use this context.
//
// - Leaf locks
//      Order: UVM_LOCK_ORDER_LEAF
//
//      All leaf locks.
//
// -------------------------------------------------------------------------- //

// Remember to add any new lock orders to uvm_lock_order_to_string() in
// uvm_lock.c
typedef enum
{
    UVM_LOCK_ORDER_INVALID = 0,
    UVM_LOCK_ORDER_GLOBAL_PM,
    UVM_LOCK_ORDER_GLOBAL,
    UVM_LOCK_ORDER_ISR,
    UVM_LOCK_ORDER_MMAP_LOCK,
    UVM_LOCK_ORDER_VA_SPACES_LIST,
    UVM_LOCK_ORDER_VA_SPACE_SERIALIZE_WRITERS,
    UVM_LOCK_ORDER_VA_SPACE_READ_ACQUIRE_WRITE_RELEASE_LOCK,
    UVM_LOCK_ORDER_VA_SPACE,
    UVM_LOCK_ORDER_EXT_RANGE_TREE,
    UVM_LOCK_ORDER_GPU_SEMAPHORE_POOL,
    UVM_LOCK_ORDER_RM_API,
    UVM_LOCK_ORDER_RM_GPUS,
    UVM_LOCK_ORDER_VA_BLOCK_MIGRATE,
    UVM_LOCK_ORDER_VA_BLOCK,
    UVM_LOCK_ORDER_CONF_COMPUTING_DMA_BUFFER_POOL,
    UVM_LOCK_ORDER_CHUNK_MAPPING,
    UVM_LOCK_ORDER_PAGE_TREE,
    UVM_LOCK_ORDER_CSL_PUSH,
    UVM_LOCK_ORDER_CSL_WLC_PUSH,
    UVM_LOCK_ORDER_CSL_SEC2_PUSH,
    UVM_LOCK_ORDER_PUSH,
    UVM_LOCK_ORDER_PMM,
    UVM_LOCK_ORDER_PMM_PMA,
    UVM_LOCK_ORDER_PMM_ROOT_CHUNK,
    UVM_LOCK_ORDER_CHANNEL,
    UVM_LOCK_ORDER_WLC_CHANNEL,
    UVM_LOCK_ORDER_TOOLS_VA_SPACE_LIST,
    UVM_LOCK_ORDER_VA_SPACE_EVENTS,
    UVM_LOCK_ORDER_VA_SPACE_TOOLS,
    UVM_LOCK_ORDER_SEMA_POOL_TRACKER,
    UVM_LOCK_ORDER_SECURE_SEMAPHORE,

    // TODO: Bug 4184836: [uvm][hcc] Remove UVM_LOCK_ORDER_CSL_CTX
    // This lock order can be removed after RM no longer relies on RPC event
    // notifications.
    UVM_LOCK_ORDER_CSL_CTX,
    UVM_LOCK_ORDER_LEAF,
    UVM_LOCK_ORDER_COUNT,
} uvm_lock_order_t;

const char *uvm_lock_order_to_string(uvm_lock_order_t lock_order);

typedef enum
{
    UVM_LOCK_FLAGS_INVALID          = 0,
    UVM_LOCK_FLAGS_MODE_EXCLUSIVE   = (1 << 0),
    UVM_LOCK_FLAGS_MODE_SHARED      = (1 << 1),
    UVM_LOCK_FLAGS_MODE_ANY         = (UVM_LOCK_FLAGS_MODE_EXCLUSIVE | UVM_LOCK_FLAGS_MODE_SHARED),
    UVM_LOCK_FLAGS_MODE_MASK        = (UVM_LOCK_FLAGS_MODE_EXCLUSIVE | UVM_LOCK_FLAGS_MODE_SHARED),
    UVM_LOCK_FLAGS_OUT_OF_ORDER     = (1 << 2),
    UVM_LOCK_FLAGS_TRYLOCK          = (1 << 3),
    UVM_LOCK_FLAGS_MASK             = (1 << 4) - 1
} uvm_lock_flags_t;

// Record locking a lock of given lock_order in exclusive or shared mode,
// distinguishing between trylock and normal acquisition attempts.
// Returns true if the recorded lock follows all the locking rules and false
// otherwise.
bool __uvm_record_lock(void *lock, uvm_lock_order_t lock_order, uvm_lock_flags_t flags);

// Record unlocking a lock of given lock_order in exclusive or shared mode and
// possibly out of order.
// Returns true if the unlock follows all the locking rules and false otherwise.
bool __uvm_record_unlock(void *lock, uvm_lock_order_t lock_order, uvm_lock_flags_t flags);

bool __uvm_record_downgrade(void *lock, uvm_lock_order_t lock_order);

// Check whether a lock of given lock_order is held in exclusive, shared, or
// either mode by the current thread.
bool __uvm_check_locked(void *lock, uvm_lock_order_t lock_order, uvm_lock_flags_t flags);

// Check that no locks are held with the given lock order
bool __uvm_check_unlocked_order(uvm_lock_order_t lock_order);

// Check that a lock of the given order can be locked, i.e. that no locks are
// held with the given or deeper lock order.  Allow for out-of-order locking
// when checking for a trylock.
bool __uvm_check_lockable_order(uvm_lock_order_t lock_order, uvm_lock_flags_t flags);

// Check that all locks have been released in a thread context lock
bool __uvm_check_all_unlocked(uvm_thread_context_lock_t *context_lock);

// Check that all locks have been released in the current thread context lock
bool __uvm_thread_check_all_unlocked(void);

// Check that the locking infrastructure has been initialized
bool __uvm_locking_initialized(void);

#if UVM_IS_DEBUG()
  // These macros are intended to be expanded on the call site directly and will
  // print the precise location of the violation while the __uvm_record*
  // functions will error print the details.
  #define uvm_record_lock_raw(lock, lock_order, flags) \
      UVM_ASSERT_MSG(__uvm_record_lock((lock), (lock_order), (flags)), "Locking violation\n")
  #define uvm_record_unlock_raw(lock, lock_order, flags) \
      UVM_ASSERT_MSG(__uvm_record_unlock((lock), (lock_order), (flags)), "Locking violation\n")
  #define uvm_record_downgrade_raw(lock, lock_order) \
      UVM_ASSERT_MSG(__uvm_record_downgrade((lock), (lock_order)), "Locking violation\n")

  // Record UVM lock (a lock that has a lock_order member) operation and assert
  // that it's correct
  #define uvm_record_lock(lock, flags) \
      uvm_record_lock_raw((lock), (lock)->lock_order, (flags))
  #define uvm_record_unlock(lock, flags) uvm_record_unlock_raw((lock), (lock)->lock_order, (flags))
  #define uvm_record_unlock_out_of_order(lock, flags) \
            uvm_record_unlock_raw((lock), (lock)->lock_order, (flags) | UVM_LOCK_FLAGS_OUT_OF_ORDER)
  #define uvm_record_downgrade(lock) uvm_record_downgrade_raw((lock), (lock)->lock_order)

  // Check whether a UVM lock (a lock that has a lock_order member) is held in
  // the given mode.
  #define uvm_check_locked(lock, flags) __uvm_check_locked((lock), (lock)->lock_order, (flags))

  // Helpers for recording and asserting mmap_lock
  // (mmap_sem in kernels < 5.8 ) state
  #define uvm_record_lock_mmap_lock_read(mm) \
          uvm_record_lock_raw(nv_mmap_get_lock(mm), UVM_LOCK_ORDER_MMAP_LOCK, UVM_LOCK_FLAGS_MODE_SHARED)

  #define uvm_record_unlock_mmap_lock_read(mm) \
          uvm_record_unlock_raw(nv_mmap_get_lock(mm), UVM_LOCK_ORDER_MMAP_LOCK, UVM_LOCK_FLAGS_MODE_SHARED)

  #define uvm_record_unlock_mmap_lock_read_out_of_order(mm) \
          uvm_record_unlock_raw(nv_mmap_get_lock(mm), UVM_LOCK_ORDER_MMAP_LOCK, \
                                UVM_LOCK_FLAGS_MODE_SHARED | UVM_LOCK_FLAGS_OUT_OF_ORDER)

  #define uvm_record_lock_mmap_lock_write(mm) \
          uvm_record_lock_raw(nv_mmap_get_lock(mm), UVM_LOCK_ORDER_MMAP_LOCK, UVM_LOCK_FLAGS_MODE_EXCLUSIVE)

  #define uvm_record_unlock_mmap_lock_write(mm) \
          uvm_record_unlock_raw(nv_mmap_get_lock(mm), UVM_LOCK_ORDER_MMAP_LOCK, UVM_LOCK_FLAGS_MODE_EXCLUSIVE)

  #define uvm_record_unlock_mmap_lock_write_out_of_order(mm) \
          uvm_record_unlock_raw(nv_mmap_get_lock(mm), UVM_LOCK_ORDER_MMAP_LOCK, \
                                UVM_LOCK_FLAGS_MODE_EXCLUSIVE | UVM_LOCK_FLAGS_OUT_OF_ORDER)

  #define uvm_check_locked_mmap_lock(mm, flags) \
           __uvm_check_locked(nv_mmap_get_lock(mm), UVM_LOCK_ORDER_MMAP_LOCK, (flags))

  // Helpers for recording RM API lock usage around UVM-RM interfaces
  #define uvm_record_lock_rm_api() \
          uvm_record_lock_raw((void*)UVM_LOCK_ORDER_RM_API, UVM_LOCK_ORDER_RM_API, \
                              UVM_LOCK_FLAGS_MODE_EXCLUSIVE)
  #define uvm_record_unlock_rm_api() \
          uvm_record_unlock_raw((void*)UVM_LOCK_ORDER_RM_API, UVM_LOCK_ORDER_RM_API, \
                                UVM_LOCK_FLAGS_MODE_EXCLUSIVE)

  // Helpers for recording RM GPUS lock usage around UVM-RM interfaces
  #define uvm_record_lock_rm_gpus() \
          uvm_record_lock_raw((void*)UVM_LOCK_ORDER_RM_GPUS, UVM_LOCK_ORDER_RM_GPUS, \
                              UVM_LOCK_FLAGS_MODE_EXCLUSIVE)
  #define uvm_record_unlock_rm_gpus() \
          uvm_record_unlock_raw((void*)UVM_LOCK_ORDER_RM_GPUS, UVM_LOCK_ORDER_RM_GPUS, \
                                UVM_LOCK_FLAGS_MODE_EXCLUSIVE)

  // Helpers for recording both RM locks usage around UVM-RM interfaces
  #define uvm_record_lock_rm_all() ({ uvm_record_lock_rm_api(); uvm_record_lock_rm_gpus(); })
  #define uvm_record_unlock_rm_all() ({ uvm_record_unlock_rm_gpus(); uvm_record_unlock_rm_api(); })

#else
  #define uvm_record_lock                               UVM_IGNORE_EXPR2
  #define uvm_record_unlock                             UVM_IGNORE_EXPR2
  #define uvm_record_unlock_out_of_order                UVM_IGNORE_EXPR2
  #define uvm_record_downgrade                          UVM_IGNORE_EXPR

  static bool uvm_check_locked(void *lock, uvm_lock_flags_t flags)
  {
      return false;
  }

  #define uvm_record_lock_mmap_lock_read                 UVM_IGNORE_EXPR
  #define uvm_record_unlock_mmap_lock_read               UVM_IGNORE_EXPR
  #define uvm_record_unlock_mmap_lock_read_out_of_order  UVM_IGNORE_EXPR
  #define uvm_record_lock_mmap_lock_write                UVM_IGNORE_EXPR
  #define uvm_record_unlock_mmap_lock_write              UVM_IGNORE_EXPR
  #define uvm_record_unlock_mmap_lock_write_out_of_order UVM_IGNORE_EXPR

  #define uvm_check_locked_mmap_lock                     uvm_check_locked

  #define uvm_record_lock_rm_api()
  #define uvm_record_unlock_rm_api()

  #define uvm_record_lock_rm_gpus()
  #define uvm_record_unlock_rm_gpus()

  #define uvm_record_lock_rm_all()
  #define uvm_record_unlock_rm_all()
#endif

#define uvm_locking_assert_initialized() UVM_ASSERT(__uvm_locking_initialized())
#define uvm_thread_assert_all_unlocked() UVM_ASSERT(__uvm_thread_check_all_unlocked())
#define uvm_assert_lockable_order(order) UVM_ASSERT(__uvm_check_lockable_order(order, UVM_LOCK_FLAGS_MODE_ANY))
#define uvm_assert_unlocked_order(order) UVM_ASSERT(__uvm_check_unlocked_order(order))

#if UVM_IS_DEBUG()
#define uvm_lock_debug_init(lock, order) ({        \
        uvm_locking_assert_initialized();          \
        (lock)->lock_order = (order);              \
    })
#else
#define uvm_lock_debug_init(lock, order) ((void) order)
#endif

// Helpers for locking mmap_lock (mmap_sem in kernels < 5.8)
// and recording its usage
#define uvm_assert_mmap_lock_locked_mode(mm, flags) ({                                      \
      typeof(mm) _mm = (mm);                                                                \
      UVM_ASSERT(nv_mm_rwsem_is_locked(_mm) && uvm_check_locked_mmap_lock((_mm), (flags))); \
  })

#define uvm_assert_mmap_lock_locked(mm) \
        uvm_assert_mmap_lock_locked_mode((mm), UVM_LOCK_FLAGS_MODE_ANY)
#define uvm_assert_mmap_lock_locked_read(mm) \
        uvm_assert_mmap_lock_locked_mode((mm), UVM_LOCK_FLAGS_MODE_SHARED)
#define uvm_assert_mmap_lock_locked_write(mm) \
        uvm_assert_mmap_lock_locked_mode((mm), UVM_LOCK_FLAGS_MODE_EXCLUSIVE)

#define uvm_down_read_mmap_lock(mm) ({                  \
        typeof(mm) _mm = (mm);                          \
        uvm_record_lock_mmap_lock_read(_mm);            \
        nv_mmap_read_lock(_mm);                         \
    })

#define uvm_up_read_mmap_lock(mm) ({                    \
        typeof(mm) _mm = (mm);                          \
        nv_mmap_read_unlock(_mm);                       \
        uvm_record_unlock_mmap_lock_read(_mm);          \
    })

#define uvm_up_read_mmap_lock_out_of_order(mm) ({           \
        typeof(mm) _mm = (mm);                              \
        nv_mmap_read_unlock(_mm);                           \
        uvm_record_unlock_mmap_lock_read_out_of_order(_mm); \
    })

#define uvm_down_write_mmap_lock(mm) ({                 \
        typeof(mm) _mm = (mm);                          \
        uvm_record_lock_mmap_lock_write(_mm);           \
        nv_mmap_write_lock(_mm);                        \
    })

#define uvm_up_write_mmap_lock(mm) ({                   \
        typeof(mm) _mm = (mm);                          \
        nv_mmap_write_unlock(_mm);                      \
        uvm_record_unlock_mmap_lock_write(_mm);         \
    })

// Helper for calling a UVM-RM interface function with lock recording
#define uvm_rm_locked_call(call) ({                     \
        typeof(call) ret;                               \
        uvm_record_lock_rm_all();                       \
        ret = call;                                     \
        uvm_record_unlock_rm_all();                     \
        ret;                                            \
    })

// Helper for calling a UVM-RM interface function that returns void with lock recording
#define uvm_rm_locked_call_void(call) ({                \
        uvm_record_lock_rm_all();                       \
        call;                                           \
        uvm_record_unlock_rm_all();                     \
    })

typedef struct
{
    struct rw_semaphore sem;
#if UVM_IS_DEBUG()
    uvm_lock_order_t lock_order;
#endif
} uvm_rw_semaphore_t;

//
// Note that this is a macro, not an inline or static function so the
// "uvm_sem" argument is subsituted as text. If this is invoked with
// uvm_assert_rwsem_locked_mode(_sem, flags) then we get code "_sem = _sem"
// and _sem is initialized to NULL. Avoid this by using a name unlikely to
// be the same as the string passed to "uvm_sem".
// See uvm_down_read() and uvm_up_read() below as examples.
//
#define uvm_assert_rwsem_locked_mode(uvm_sem, flags) ({                               \
        typeof(uvm_sem) _sem_ = (uvm_sem);                                            \
        UVM_ASSERT(rwsem_is_locked(&_sem_->sem) && uvm_check_locked(_sem_, (flags))); \
    })

#define uvm_assert_rwsem_locked(uvm_sem) \
        uvm_assert_rwsem_locked_mode(uvm_sem, UVM_LOCK_FLAGS_MODE_ANY)
#define uvm_assert_rwsem_locked_read(uvm_sem) \
        uvm_assert_rwsem_locked_mode(uvm_sem, UVM_LOCK_FLAGS_MODE_SHARED)
#define uvm_assert_rwsem_locked_write(uvm_sem) \
        uvm_assert_rwsem_locked_mode(uvm_sem, UVM_LOCK_FLAGS_MODE_EXCLUSIVE)

#define uvm_assert_rwsem_unlocked(uvm_sem) UVM_ASSERT(!rwsem_is_locked(&(uvm_sem)->sem))

#define uvm_init_rwsem(uvm_sem, order) ({                   \
        uvm_rw_semaphore_t *uvm_sem_ ## order = (uvm_sem);  \
        init_rwsem(&uvm_sem_ ## order->sem);                \
        uvm_lock_debug_init(uvm_sem, order);                \
        uvm_assert_rwsem_unlocked(uvm_sem);                 \
    })

#define uvm_down_read(uvm_sem) ({                          \
        typeof(uvm_sem) _sem = (uvm_sem);                  \
        uvm_record_lock(_sem, UVM_LOCK_FLAGS_MODE_SHARED); \
        down_read(&_sem->sem);                             \
        uvm_assert_rwsem_locked_read(_sem);                \
    })

#define uvm_up_read(uvm_sem) ({                              \
        typeof(uvm_sem) _sem = (uvm_sem);                    \
        uvm_assert_rwsem_locked_read(_sem);                  \
        up_read(&_sem->sem);                                 \
        uvm_record_unlock(_sem, UVM_LOCK_FLAGS_MODE_SHARED); \
    })

// Unlock w/o any tracking. This should be extremely rare and *_no_tracking
// helpers will be added only as needed.
//
// TODO: Bug 2594854:
// TODO: Bug 2583279: Remove macro when bugs are fixed
#define uvm_up_read_no_tracking(uvm_sem) ({                  \
        typeof(uvm_sem) _sem = (uvm_sem);                    \
        up_read(&_sem->sem);                                 \
    })

#define uvm_down_write(uvm_sem) ({                            \
        typeof (uvm_sem) _sem = (uvm_sem);                    \
        uvm_record_lock(_sem, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
        down_write(&_sem->sem);                               \
        uvm_assert_rwsem_locked_write(_sem);                  \
    })

// trylock for reading: returns 1 if successful, 0 if not.  Out-of-order lock
// acquisition via this function is legal, i.e. the lock order checker will
// allow it.  However, if an out-of-order lock acquisition attempt fails, it is
// the caller's responsibility to back off at least to the point where the
// next held lower-order lock is released.
#define uvm_down_read_trylock(uvm_sem) ({                                           \
        typeof(uvm_sem) _sem = (uvm_sem);                                           \
        int locked;                                                                 \
        uvm_record_lock(_sem, UVM_LOCK_FLAGS_MODE_SHARED | UVM_LOCK_FLAGS_TRYLOCK); \
        locked = down_read_trylock(&_sem->sem);                                     \
        if (locked == 0)                                                            \
            uvm_record_unlock(_sem, UVM_LOCK_FLAGS_MODE_SHARED);                    \
        else                                                                        \
            uvm_assert_rwsem_locked_read(_sem);                                     \
        locked;                                                                     \
    })

// Lock w/o any tracking. This should be extremely rare and *_no_tracking
// helpers will be added only as needed.
//
// TODO: Bug 2594854:
// TODO: Bug 2583279: Remove macro when bugs are fixed
#define uvm_down_read_trylock_no_tracking(uvm_sem) ({                               \
        typeof(uvm_sem) _sem = (uvm_sem);                                           \
        down_read_trylock(&_sem->sem);                                              \
    })

// trylock for writing: returns 1 if successful, 0 if not.  Out-of-order lock
// acquisition via this function is legal, i.e. the lock order checker will
// allow it.  However, if an out-of-order lock acquisition attempt fails, it is
// the caller's responsibility to back off at least to the point where the
// next held lower-order lock is released.
#define uvm_down_write_trylock(uvm_sem) ({                                             \
        typeof(uvm_sem) _sem = (uvm_sem);                                              \
        int locked;                                                                    \
        uvm_record_lock(_sem, UVM_LOCK_FLAGS_MODE_EXCLUSIVE | UVM_LOCK_FLAGS_TRYLOCK); \
        locked = down_write_trylock(&_sem->sem);                                       \
        if (locked == 0)                                                               \
            uvm_record_unlock(_sem, UVM_LOCK_FLAGS_MODE_EXCLUSIVE);                    \
        else                                                                           \
            uvm_assert_rwsem_locked_write(_sem);                                       \
        locked;                                                                        \
    })

#define uvm_up_write(uvm_sem) ({                                \
        typeof(uvm_sem) _sem = (uvm_sem);                       \
        uvm_assert_rwsem_locked_write(_sem);                    \
        up_write(&_sem->sem);                                   \
        uvm_record_unlock(_sem, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
    })

#define uvm_downgrade_write(uvm_sem) ({                 \
        typeof(uvm_sem) _sem = (uvm_sem);               \
        uvm_assert_rwsem_locked_write(_sem);            \
        downgrade_write(&_sem->sem);                    \
        uvm_record_downgrade(_sem);                     \
    })

typedef struct
{
    struct mutex m;
#if UVM_IS_DEBUG()
    uvm_lock_order_t lock_order;
#endif
} uvm_mutex_t;

// Note that this is a macro, not an inline or static function so the
// "uvm_macro" argument is subsituted as text. If this is invoked with
// uvm__mutex_is_locked(_mutex) then we get code "_mutex = _mutex" and _mutex is
// initialized to NULL. Avoid this by using a name unlikely to be the same as
// the string passed to "uvm_mutex".
// See uvm_mutex_lock() and uvm_mutex_unlock() below as examples.
//
#define uvm_mutex_is_locked(uvm_mutex) ({                                                           \
        typeof(uvm_mutex) _mutex_ = (uvm_mutex);                                                    \
        (mutex_is_locked(&_mutex_->m) && uvm_check_locked(_mutex_, UVM_LOCK_FLAGS_MODE_EXCLUSIVE)); \
    })

#define uvm_assert_mutex_locked(uvm_mutex) UVM_ASSERT(uvm_mutex_is_locked(uvm_mutex))
#define uvm_assert_mutex_unlocked(uvm_mutex) UVM_ASSERT(!mutex_is_locked(&(uvm_mutex)->m))

//
// Linux kernel mutexes cannot be used with interrupts disabled. Doing so
// can lead to deadlocks.
// To warn about mutex usages with interrupts disabled, the following
// macros and inline functions wrap around the raw kernel mutex operations
// in order to check if the interrupts have been disabled and assert if so.
//
// TODO: Bug 2690258: evaluate whether !irqs_disabled() && !in_interrupt() is
//       enough.
//
#define uvm_assert_mutex_interrupts() ({                                                                        \
        UVM_ASSERT_MSG(!irqs_disabled() && !in_interrupt(), "Mutexes cannot be used with interrupts disabled"); \
    })

#define uvm_mutex_init(mutex, order) ({                \
        uvm_mutex_t *mutex_ ## order = (mutex);        \
        mutex_init(&mutex_ ## order->m);               \
        uvm_lock_debug_init(mutex, order);             \
        uvm_assert_mutex_unlocked(mutex);              \
    })

#define uvm_mutex_lock(mutex) ({                                \
        typeof(mutex) _mutex = (mutex);                         \
        uvm_assert_mutex_interrupts();                          \
        uvm_record_lock(_mutex, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
        mutex_lock(&_mutex->m);                                 \
        uvm_assert_mutex_locked(_mutex);                        \
    })

// Lock while already holding a lock of the same order taken with
// uvm_mutex_lock() variant. Note this shouldn't be used if the held lock was
// taken with uvm_mutex_lock_nested() because we only support a single level of
// nesting. This should be extremely rare and *_nested helpers will only be
// added as needed.
#define uvm_mutex_lock_nested(mutex) ({         \
        uvm_assert_mutex_interrupts();          \
        mutex_lock_nested(&(mutex)->m, 1);      \
    })

#define uvm_mutex_trylock(mutex) ({                                                      \
        typeof(mutex) _mutex = (mutex);                                                  \
        int locked;                                                                      \
        uvm_record_lock(_mutex, UVM_LOCK_FLAGS_MODE_EXCLUSIVE | UVM_LOCK_FLAGS_TRYLOCK); \
        locked = mutex_trylock(&_mutex->m);                                              \
        if (locked == 0)                                                                 \
            uvm_record_unlock(_mutex, UVM_LOCK_FLAGS_MODE_EXCLUSIVE);                    \
        else                                                                             \
            uvm_assert_mutex_locked(_mutex);                                             \
        locked;                                                                          \
    })

#define uvm_mutex_unlock(mutex) ({                                \
        typeof(mutex) _mutex = (mutex);                           \
        uvm_assert_mutex_interrupts();                            \
        uvm_assert_mutex_locked(_mutex);                          \
        mutex_unlock(&_mutex->m);                                 \
        uvm_record_unlock(_mutex, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
    })
#define uvm_mutex_unlock_out_of_order(mutex) ({                                \
        typeof(mutex) _mutex = (mutex);                                        \
        uvm_assert_mutex_interrupts();                                         \
        uvm_assert_mutex_locked(_mutex);                                       \
        mutex_unlock(&_mutex->m);                                              \
        uvm_record_unlock_out_of_order(_mutex, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
    })

// Unlock w/o any tracking.
#define uvm_mutex_unlock_nested(mutex) ({       \
        uvm_assert_mutex_interrupts();          \
        mutex_unlock(&(mutex)->m);              \
    })

typedef struct
{
    struct semaphore sem;
#if UVM_IS_DEBUG()
    uvm_lock_order_t lock_order;
#endif
} uvm_semaphore_t;

#define uvm_sema_init(semaphore, val, order) ({         \
        uvm_semaphore_t *sem_ ## order = (semaphore);   \
        sema_init(&sem_ ## order->sem, (val));          \
        uvm_lock_debug_init(semaphore, order);          \
    })

#define uvm_sem_is_locked(uvm_sem) uvm_check_locked(uvm_sem, UVM_LOCK_FLAGS_MODE_SHARED)

#define uvm_down(uvm_sem) ({                               \
        typeof(uvm_sem) _sem = (uvm_sem);                  \
        uvm_record_lock(_sem, UVM_LOCK_FLAGS_MODE_SHARED); \
        down(&_sem->sem);                                  \
    })

#define uvm_up(uvm_sem) ({                                   \
        typeof(uvm_sem) _sem = (uvm_sem);                    \
        UVM_ASSERT(uvm_sem_is_locked(_sem));                 \
        up(&_sem->sem);                                      \
        uvm_record_unlock(_sem, UVM_LOCK_FLAGS_MODE_SHARED); \
    })
#define uvm_up_out_of_order(uvm_sem) ({                                   \
        typeof(uvm_sem) _sem = (uvm_sem);                                 \
        UVM_ASSERT(uvm_sem_is_locked(_sem));                              \
        up(&_sem->sem);                                                   \
        uvm_record_unlock_out_of_order(_sem, UVM_LOCK_FLAGS_MODE_SHARED); \
    })


// A regular spinlock
// Locked/unlocked with uvm_spin_lock()/uvm_spin_unlock()
typedef struct
{
    spinlock_t lock;
#if UVM_IS_DEBUG()
    uvm_lock_order_t lock_order;
#endif
} uvm_spinlock_t;

// A separate spinlock type for spinlocks that need to disable interrupts. For
// guaranteed correctness and convenience embed the saved and restored irq state
// in the lock itself.
// Locked/unlocked with uvm_spin_lock_irqsave()/uvm_spin_unlock_irqrestore()
typedef struct
{
    spinlock_t lock;
    unsigned long irq_flags;
#if UVM_IS_DEBUG()
    uvm_lock_order_t lock_order;
#endif
} uvm_spinlock_irqsave_t;

// Asserts that the spinlock is held. Notably the macros below support both
// types of spinlocks.
// Note that this is a macro, not an inline or static function so the
// "spinlock" argument is subsituted as text. If this is invoked with
// uvm_assert_spinlock_locked(_lock) then we get code "_lock = _lock"
// and _lock is initialized to NULL. Avoid this by using a name unlikely to
// be the same as the string passed to "spinlock".
// See uvm_spin_lock() and uvm_spin_unlock() below as examples.
//
#define uvm_assert_spinlock_locked(spinlock) ({                              \
        typeof(spinlock) _lock_ = (spinlock);                                \
        UVM_ASSERT(spin_is_locked(&_lock_->lock));                           \
        UVM_ASSERT(uvm_check_locked(_lock_, UVM_LOCK_FLAGS_MODE_EXCLUSIVE)); \
    })

#define uvm_assert_spinlock_unlocked(spinlock) UVM_ASSERT(!spin_is_locked(&(spinlock)->lock))

#define uvm_spin_lock_init(spinlock, order) ({                  \
            uvm_spinlock_t *spinlock_ ## order = (spinlock);    \
            spin_lock_init(&spinlock_ ## order->lock);          \
            uvm_lock_debug_init(spinlock, order);               \
            uvm_assert_spinlock_unlocked(spinlock);             \
    })

#define uvm_spin_lock(uvm_lock) ({                             \
        typeof(uvm_lock) _lock = (uvm_lock);                   \
        uvm_record_lock(_lock, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
        spin_lock(&_lock->lock);                               \
        uvm_assert_spinlock_locked(_lock);                     \
    })

#define uvm_spin_unlock(uvm_lock) ({                             \
        typeof(uvm_lock) _lock = (uvm_lock);                     \
        uvm_assert_spinlock_locked(_lock);                       \
        spin_unlock(&_lock->lock);                               \
        uvm_record_unlock(_lock, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
    })

#define uvm_spin_lock_irqsave_init(spinlock, order) ({                  \
            uvm_spinlock_irqsave_t *spinlock_ ## order = (spinlock);    \
            spin_lock_init(&spinlock_ ## order->lock);                  \
            uvm_lock_debug_init(spinlock, order);                       \
            uvm_assert_spinlock_unlocked(spinlock);                     \
    })

// Use a temp to not rely on flags being written after acquiring the lock.
#define uvm_spin_lock_irqsave(uvm_lock) ({                     \
        typeof(uvm_lock) _lock = (uvm_lock);                   \
        unsigned long irq_flags;                               \
        uvm_record_lock(_lock, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
        spin_lock_irqsave(&_lock->lock, irq_flags);            \
        _lock->irq_flags = irq_flags;                          \
        uvm_assert_spinlock_locked(_lock);                     \
    })

// Use a temp to not rely on flags being read before releasing the lock.
#define uvm_spin_unlock_irqrestore(uvm_lock) ({                  \
        typeof(uvm_lock) _lock = (uvm_lock);                     \
        unsigned long irq_flags = _lock->irq_flags;              \
        uvm_assert_spinlock_locked(_lock);                       \
        spin_unlock_irqrestore(&_lock->lock, irq_flags);         \
        uvm_record_unlock(_lock, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
    })

// Wrapper for a reader-writer spinlock that disables and enables interrupts
typedef struct
{
    rwlock_t lock;

    // This flags variable is only used by writers, since concurrent readers may
    // have different values.
    unsigned long irq_flags;

#if UVM_IS_DEBUG()
    uvm_lock_order_t lock_order;

    // The kernel doesn't provide a function to tell if an rwlock_t is locked,
    // so we create our own.
    atomic_t lock_count;
#endif
} uvm_rwlock_irqsave_t;

static bool uvm_rwlock_irqsave_is_locked(uvm_rwlock_irqsave_t *rwlock)
{
#if UVM_IS_DEBUG()
    return atomic_read(&rwlock->lock_count) > 0;
#else
    return false;
#endif
}

static void uvm_rwlock_irqsave_inc(uvm_rwlock_irqsave_t *rwlock)
{
#if UVM_IS_DEBUG()
    atomic_inc(&rwlock->lock_count);
#endif
}

static void uvm_rwlock_irqsave_dec(uvm_rwlock_irqsave_t *rwlock)
{
#if UVM_IS_DEBUG()
    atomic_dec(&rwlock->lock_count);
#endif
}

#define uvm_assert_rwlock_locked(uvm_rwlock) \
    UVM_ASSERT(uvm_rwlock_irqsave_is_locked(uvm_rwlock) && uvm_check_locked(uvm_rwlock, UVM_LOCK_FLAGS_MODE_ANY))
#define uvm_assert_rwlock_locked_read(uvm_rwlock) \
    UVM_ASSERT(uvm_rwlock_irqsave_is_locked(uvm_rwlock) && uvm_check_locked(uvm_rwlock, UVM_LOCK_FLAGS_MODE_SHARED))
#define uvm_assert_rwlock_locked_write(uvm_rwlock) \
    UVM_ASSERT(uvm_rwlock_irqsave_is_locked(uvm_rwlock) && uvm_check_locked(uvm_rwlock, UVM_LOCK_FLAGS_MODE_EXCLUSIVE))

#if UVM_IS_DEBUG()
    #define uvm_assert_rwlock_unlocked(uvm_rwlock) UVM_ASSERT(!uvm_rwlock_irqsave_is_locked(uvm_rwlock))
#else
    #define uvm_assert_rwlock_unlocked(uvm_rwlock)
#endif

#define uvm_rwlock_irqsave_init(rwlock, order) ({               \
            uvm_rwlock_irqsave_t *rwlock_ ## order = rwlock;    \
            rwlock_init(&rwlock_ ## order->lock);               \
            uvm_lock_debug_init(rwlock, order);                 \
            uvm_assert_rwlock_unlocked(rwlock);                 \
        })

// We can't store the irq_flags within the lock itself for readers, so they must
// pass in their flags.
#define uvm_read_lock_irqsave(uvm_rwlock, irq_flags) ({     \
        typeof(uvm_rwlock) _lock = (uvm_rwlock);            \
        uvm_record_lock(_lock, UVM_LOCK_FLAGS_MODE_SHARED); \
        read_lock_irqsave(&_lock->lock, irq_flags);         \
        uvm_rwlock_irqsave_inc(uvm_rwlock);                 \
        uvm_assert_rwlock_locked_read(_lock);               \
    })

#define uvm_read_unlock_irqrestore(uvm_rwlock, irq_flags) ({    \
        typeof(uvm_rwlock) _lock = (uvm_rwlock);                \
        uvm_assert_rwlock_locked_read(_lock);                   \
        uvm_rwlock_irqsave_dec(uvm_rwlock);                     \
        read_unlock_irqrestore(&_lock->lock, irq_flags);        \
        uvm_record_unlock(_lock, UVM_LOCK_FLAGS_MODE_SHARED);   \
    })

// Use a temp to not rely on flags being written after acquiring the lock.
#define uvm_write_lock_irqsave(uvm_rwlock) ({                   \
        typeof(uvm_rwlock) _lock = (uvm_rwlock);                \
        unsigned long irq_flags;                                \
        uvm_record_lock(_lock, UVM_LOCK_FLAGS_MODE_EXCLUSIVE);  \
        write_lock_irqsave(&_lock->lock, irq_flags);            \
        uvm_rwlock_irqsave_inc(uvm_rwlock);                     \
        _lock->irq_flags = irq_flags;                           \
        uvm_assert_rwlock_locked_write(_lock);                  \
    })

// Use a temp to not rely on flags being written after acquiring the lock.
#define uvm_write_unlock_irqrestore(uvm_rwlock) ({                  \
        typeof(uvm_rwlock) _lock = (uvm_rwlock);                    \
        unsigned long irq_flags = _lock->irq_flags;                 \
        uvm_assert_rwlock_locked_write(_lock);                      \
        uvm_rwlock_irqsave_dec(uvm_rwlock);                         \
        write_unlock_irqrestore(&_lock->lock, irq_flags);           \
        uvm_record_unlock(_lock, UVM_LOCK_FLAGS_MODE_EXCLUSIVE);    \
    })

// Bit locks are 'compressed' mutexes which take only 1 bit per lock by virtue
// of using shared waitqueues.
typedef struct
{
    unsigned long *bits;

#if UVM_IS_DEBUG()
    uvm_lock_order_t lock_order;
#endif
} uvm_bit_locks_t;

NV_STATUS uvm_bit_locks_init(uvm_bit_locks_t *bit_locks, size_t count, uvm_lock_order_t lock_order);
void uvm_bit_locks_deinit(uvm_bit_locks_t *bit_locks);

// Asserts that the bit lock is held.
//
// TODO: Bug 1766601:
//  - assert for the right ownership (defining the owner might be tricky in
//    the kernel).
#define uvm_assert_bit_locked(bit_locks, bit) ({                             \
    typeof(bit_locks) _bit_locks = (bit_locks);                              \
    typeof(bit) _bit = (bit);                                                \
    UVM_ASSERT(test_bit(_bit, _bit_locks->bits));                            \
    UVM_ASSERT(uvm_check_locked(_bit_locks, UVM_LOCK_FLAGS_MODE_EXCLUSIVE)); \
})

#define uvm_assert_bit_unlocked(bit_locks, bit) ({                      \
    typeof(bit_locks) _bit_locks = (bit_locks);                         \
    typeof(bit) _bit = (bit);                                           \
    UVM_ASSERT(!test_bit(_bit, _bit_locks->bits));                      \
})

static void __uvm_bit_lock(uvm_bit_locks_t *bit_locks, unsigned long bit)
{
    int res;

    res = UVM_WAIT_ON_BIT_LOCK(bit_locks->bits, bit, TASK_UNINTERRUPTIBLE);
    UVM_ASSERT_MSG(res == 0, "Uninterruptible task interrupted: %d\n", res);
    uvm_assert_bit_locked(bit_locks, bit);
}
#define uvm_bit_lock(bit_locks, bit) ({                         \
    typeof(bit_locks) _bit_locks = (bit_locks);                 \
    typeof(bit) _bit = (bit);                                   \
    uvm_record_lock(_bit_locks, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
    __uvm_bit_lock(_bit_locks, _bit);                           \
})

static void __uvm_bit_unlock(uvm_bit_locks_t *bit_locks, unsigned long bit)
{
    uvm_assert_bit_locked(bit_locks, bit);

    clear_bit_unlock(bit, bit_locks->bits);
    // Make sure we don't reorder release with wakeup as it would cause
    // deadlocks (other thread checking lock and adding itself to queue
    // in reversed order). clear_bit_unlock has only release semantics.
    smp_mb__after_atomic();
    wake_up_bit(bit_locks->bits, bit);
}
#define uvm_bit_unlock(bit_locks, bit) ({                         \
    typeof(bit_locks) _bit_locks = (bit_locks);                   \
    typeof(bit) _bit = (bit);                                     \
    __uvm_bit_unlock(_bit_locks, _bit);                           \
    uvm_record_unlock(_bit_locks, UVM_LOCK_FLAGS_MODE_EXCLUSIVE); \
})

#endif // __UVM_LOCK_H__
