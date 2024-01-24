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

#include "uvm_lock.h"
#include "uvm_thread_context.h"
#include "uvm_kvmalloc.h"

const char *uvm_lock_order_to_string(uvm_lock_order_t lock_order)
{
    BUILD_BUG_ON(UVM_LOCK_ORDER_COUNT != 34);

    switch (lock_order) {
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_INVALID);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_GLOBAL_PM);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_GLOBAL);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_ISR);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_MMAP_LOCK);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_VA_SPACES_LIST);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_VA_SPACE_SERIALIZE_WRITERS);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_VA_SPACE_READ_ACQUIRE_WRITE_RELEASE_LOCK);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_VA_SPACE);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_EXT_RANGE_TREE);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_GPU_SEMAPHORE_POOL);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_RM_API);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_RM_GPUS);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_VA_BLOCK_MIGRATE);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_VA_BLOCK);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_CONF_COMPUTING_DMA_BUFFER_POOL);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_CHUNK_MAPPING);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_PAGE_TREE);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_CSL_PUSH);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_CSL_WLC_PUSH);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_CSL_SEC2_PUSH);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_PUSH);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_PMM);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_PMM_PMA);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_PMM_ROOT_CHUNK);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_CHANNEL);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_WLC_CHANNEL);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_TOOLS_VA_SPACE_LIST);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_VA_SPACE_EVENTS);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_VA_SPACE_TOOLS);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_SEMA_POOL_TRACKER);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_SECURE_SEMAPHORE);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_CSL_CTX);
        UVM_ENUM_STRING_CASE(UVM_LOCK_ORDER_LEAF);
        UVM_ENUM_STRING_DEFAULT();
    }
}

bool __uvm_record_lock(void *lock, uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    bool correct = true;
    uvm_lock_order_t conflicting_order;
    uvm_thread_context_lock_t *uvm_context = uvm_thread_context_lock_get();
    uvm_lock_flags_t mode_flags = (flags & UVM_LOCK_FLAGS_MODE_MASK);
    bool trylock = (flags & UVM_LOCK_FLAGS_TRYLOCK);

    UVM_ASSERT(mode_flags == UVM_LOCK_FLAGS_MODE_EXCLUSIVE || mode_flags == UVM_LOCK_FLAGS_MODE_SHARED);

    if (!uvm_context) {
        UVM_ERR_PRINT("Failed to acquire the thread context when recording lock of %s\n",
                      uvm_lock_order_to_string(lock_order));
        return false;
    }

    if (uvm_context->skip_lock_tracking > 0)
        return true;

    if (lock_order == UVM_LOCK_ORDER_INVALID) {
        UVM_ERR_PRINT("Acquiring a lock (0x%llx) with an invalid lock order\n", (NvU64)lock);
        return false;
    }

    // TODO: Bug 1799173: Hack in special rules for the RM locks so we don't add
    //       any new invalid uses while we figure out a better way to handle
    //       these dependencies.
    if (lock_order == UVM_LOCK_ORDER_RM_GPUS) {
        if (test_bit(UVM_LOCK_ORDER_MMAP_LOCK, uvm_context->acquired_lock_orders)) {
            UVM_ERR_PRINT("Acquiring RM GPU lock with mmap_lock held\n");
            correct = false;
        }

        if (test_bit(UVM_LOCK_ORDER_VA_SPACE, uvm_context->exclusive_acquired_lock_orders)) {
            UVM_ERR_PRINT("Acquiring RM GPU lock with VA space lock held in write mode\n");
            correct = false;
        }
        else if (test_bit(UVM_LOCK_ORDER_VA_SPACE, uvm_context->acquired_lock_orders) &&
                 !test_bit(UVM_LOCK_ORDER_VA_SPACE_SERIALIZE_WRITERS, uvm_context->acquired_lock_orders)) {
            UVM_ERR_PRINT("Acquiring RM GPU lock with the VA space lock held in read mode, but without the VA space writer serialization lock held\n");
            correct = false;
        }
    }

    conflicting_order = find_next_bit(uvm_context->acquired_lock_orders, UVM_LOCK_ORDER_COUNT, lock_order);
    if (conflicting_order != UVM_LOCK_ORDER_COUNT) {
        if (trylock) {
            // If the lock attempt is a trylock, i.e. non-blocking, then
            // out-of-order lock acquisition is acceptable.  Record it
            // to enable __uvm_record_unlock() to skip enforcing in-order
            // lock release for this lock order.
            __set_bit(lock_order, uvm_context->out_of_order_acquired_lock_orders);
        } else {
            correct = false;
            // Equivalent order is not necessarily incorrect. However, it is not yet supported,
            // and is therefore treated as an error case.
            UVM_ERR_PRINT("Already acquired equivalent or deeper lock %s when trying to acquire %s\n",
                          uvm_lock_order_to_string(conflicting_order),
                          uvm_lock_order_to_string(lock_order));
        }
    }

    __set_bit(lock_order, uvm_context->acquired_lock_orders);

    if (mode_flags == UVM_LOCK_FLAGS_MODE_EXCLUSIVE)
        __set_bit(lock_order, uvm_context->exclusive_acquired_lock_orders);

    uvm_context->acquired[lock_order] = lock;

    return correct;
}

bool __uvm_record_unlock(void *lock, uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    bool correct = true;
    uvm_thread_context_lock_t *uvm_context = uvm_thread_context_lock_get();
    uvm_lock_flags_t mode_flags = (flags & UVM_LOCK_FLAGS_MODE_MASK);
    bool exclusive = (mode_flags == UVM_LOCK_FLAGS_MODE_EXCLUSIVE);
    bool out_of_order = (flags & UVM_LOCK_FLAGS_OUT_OF_ORDER);

    UVM_ASSERT(mode_flags == UVM_LOCK_FLAGS_MODE_EXCLUSIVE || mode_flags == UVM_LOCK_FLAGS_MODE_SHARED);

    if (!uvm_context) {
        UVM_ERR_PRINT("Failed to acquire the thread context when recording unlock of %s\n",
                      uvm_lock_order_to_string(lock_order));
        return false;
    }

    if (uvm_context->skip_lock_tracking > 0)
        return true;

    if (lock_order == UVM_LOCK_ORDER_INVALID) {
        UVM_ERR_PRINT("Releasing a lock (0x%llx) with an invalid lock order\n", (NvU64)lock);
        return false;
    }

    // Releasing a lock out of order is not incorrect, but often points to
    // issues.  Consider it an error by default, unless the lock was
    // legally acquired out-of-order via trylock, in which case out-of-order
    // lock release is expected.  But also give an option to opt out of
    // enforcing in-order lock release, if needed.
    if (!__test_and_clear_bit(lock_order, uvm_context->out_of_order_acquired_lock_orders) && !out_of_order) {
        uvm_lock_order_t deeper_order = find_next_bit(uvm_context->acquired_lock_orders,
                                                      UVM_LOCK_ORDER_COUNT, lock_order + 1);
        if (deeper_order != UVM_LOCK_ORDER_COUNT) {
            correct = false;
            UVM_ERR_PRINT("Releasing lock %s while still holding %s\n",
                          uvm_lock_order_to_string(lock_order),
                          uvm_lock_order_to_string(deeper_order));
        }
    }

    if (!__test_and_clear_bit(lock_order, uvm_context->acquired_lock_orders)) {
        correct = false;
        UVM_ERR_PRINT("Releasing lock %s that's not held\n", uvm_lock_order_to_string(lock_order));
    }
    else if (uvm_context->acquired[lock_order] != lock) {
        correct = false;
        UVM_ERR_PRINT("Releasing a different instance of lock %s than held, held 0x%llx releasing 0x%llx\n",
                      uvm_lock_order_to_string(lock_order),
                      (NvU64)uvm_context->acquired[lock_order],
                      (NvU64)lock);
    }
    else if (!!__test_and_clear_bit(lock_order, uvm_context->exclusive_acquired_lock_orders) != exclusive) {
        correct = false;
        UVM_ERR_PRINT("Releasing lock %s as %s while it was acquired as %s\n",
                      uvm_lock_order_to_string(lock_order),
                      exclusive ? "exclusive" : "shared", exclusive ? "shared" : "exclusive");
    }
    uvm_context->acquired[lock_order] = NULL;

    return correct;
}

bool __uvm_record_downgrade(void *lock, uvm_lock_order_t lock_order)
{
    uvm_thread_context_lock_t *uvm_context = uvm_thread_context_lock_get();

    if (!uvm_context) {
        UVM_ERR_PRINT("Failed to acquire the thread context when recording downgrade of %s\n",
                      uvm_lock_order_to_string(lock_order));
        return false;
    }

    if (uvm_context->skip_lock_tracking > 0)
        return true;

    if (!__uvm_check_locked(lock, lock_order, UVM_LOCK_FLAGS_MODE_EXCLUSIVE)) {
        UVM_ERR_PRINT("Lock %s is not held in exclusive mode: downgrading failed\n",
                      uvm_lock_order_to_string(lock_order));
        return false;
    }

    clear_bit(lock_order, uvm_context->exclusive_acquired_lock_orders);
    return true;
}

bool __uvm_check_locked(void *lock, uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    uvm_thread_context_lock_t *uvm_context = uvm_thread_context_lock_get();
    uvm_lock_flags_t mode_flags = (flags & UVM_LOCK_FLAGS_MODE_MASK);
    bool exclusive = (mode_flags == UVM_LOCK_FLAGS_MODE_EXCLUSIVE);

    if (!uvm_context) {
        UVM_ERR_PRINT("Failed to acquire the thread context when checking that lock %s is locked\n",
                      uvm_lock_order_to_string(lock_order));
        return false;
    }

    if (uvm_context->skip_lock_tracking > 0)
        return true;

    if (!test_bit(lock_order, uvm_context->acquired_lock_orders)) {
        UVM_ERR_PRINT("No lock with order %s acquired at all\n", uvm_lock_order_to_string(lock_order));
        return false;
    }
    if (uvm_context->acquired[lock_order] != lock) {
        UVM_ERR_PRINT("Different instance of lock %s acquired, 0x%llx != 0x%llx\n",
                      uvm_lock_order_to_string(lock_order),
                      (NvU64)lock,
                      (NvU64)uvm_context->acquired[lock_order]);
        return false;
    }

    if (mode_flags != UVM_LOCK_FLAGS_MODE_ANY &&
        !!test_bit(lock_order, uvm_context->exclusive_acquired_lock_orders) != exclusive) {
        UVM_ERR_PRINT("Lock %s acquired in %s mode instead of %s mode\n",
                      uvm_lock_order_to_string(lock_order),
                      exclusive ? "shared" : "exclusive", exclusive ? "exclusive" : "shared");
        return false;
    }

    return true;
}

bool __uvm_locking_initialized(void)
{
    return uvm_thread_context_global_initialized();
}

bool __uvm_check_lockable_order(uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    uvm_lock_order_t conflicting_order;
    uvm_thread_context_lock_t *uvm_context = uvm_thread_context_lock_get();
    bool trylock = (flags & UVM_LOCK_FLAGS_TRYLOCK);

    if (!uvm_context)
        return true;

    if (uvm_context->skip_lock_tracking > 0)
        return true;

    if (lock_order == UVM_LOCK_ORDER_INVALID) {
        UVM_ERR_PRINT("Checking for an invalid lock order\n");
        return false;
    }

    if (!trylock) {
        conflicting_order = find_next_bit(uvm_context->acquired_lock_orders, UVM_LOCK_ORDER_COUNT, lock_order);
        if (conflicting_order != UVM_LOCK_ORDER_COUNT) {
            UVM_ERR_PRINT("Acquired equivalent or deeper lock %s when checking that %s is lockable\n",
                          uvm_lock_order_to_string(conflicting_order),
                          uvm_lock_order_to_string(lock_order));
            return false;
        }
    }

    return true;
}

bool __uvm_check_unlocked_order(uvm_lock_order_t lock_order)
{
    uvm_thread_context_lock_t *uvm_context = uvm_thread_context_lock_get();
    if (!uvm_context)
        return true;

    if (uvm_context->skip_lock_tracking > 0)
        return true;

    if (lock_order == UVM_LOCK_ORDER_INVALID) {
        UVM_ERR_PRINT("Checking for an invalid lock order\n");
        return false;
    }

    if (test_bit(lock_order, uvm_context->acquired_lock_orders)) {
        UVM_ERR_PRINT("Lock order %s acquired\n", uvm_lock_order_to_string(lock_order));
        return false;
    }
    return true;
}

bool __uvm_check_all_unlocked(uvm_thread_context_lock_t *uvm_context)
{
    uvm_lock_order_t lock_order;
    NvU32 still_locked_count;

    if (!uvm_context)
        return true;

    still_locked_count = bitmap_weight(uvm_context->acquired_lock_orders, UVM_LOCK_ORDER_COUNT);
    if (still_locked_count == 0)
        return true;

    UVM_ERR_PRINT("Still %u acquired lock(s):\n", still_locked_count);

    for_each_set_bit(lock_order, uvm_context->acquired_lock_orders, UVM_LOCK_ORDER_COUNT) {
        UVM_ERR_PRINT(" Lock %s, instance 0x%llx\n",
                      uvm_lock_order_to_string(lock_order),
                      (NvU64)uvm_context->acquired[lock_order]);
    }

    return false;
}

bool __uvm_thread_check_all_unlocked(void)
{
    return __uvm_check_all_unlocked(uvm_thread_context_lock_get());
}

NV_STATUS uvm_bit_locks_init(uvm_bit_locks_t *bit_locks, size_t count, uvm_lock_order_t lock_order)
{
    // TODO: Bug 1772140: Notably bit locks currently do not work on memory
    // allocated through vmalloc() (including big allocations created with
    // uvm_kvmalloc()). The problem is the bit_waitqueue() helper used by the
    // kernel internally that uses virt_to_page().
    // To prevent us from using kmalloc() for a huge allocation, warn if the
    // allocation size gets bigger than what we are comfortable with for
    // kmalloc() in uvm_kvmalloc().
    size_t size = sizeof(unsigned long) * BITS_TO_LONGS(count);
    WARN_ON_ONCE(size > UVM_KMALLOC_THRESHOLD);

    bit_locks->bits = kzalloc(size, NV_UVM_GFP_FLAGS);
    if (!bit_locks->bits)
        return NV_ERR_NO_MEMORY;

    uvm_lock_debug_init(bit_locks, lock_order);

    return NV_OK;
}

void uvm_bit_locks_deinit(uvm_bit_locks_t *bit_locks)
{
    kfree(bit_locks->bits);
    memset(bit_locks, 0, sizeof(*bit_locks));
}
