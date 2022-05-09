/*******************************************************************************
    Copyright (c) 2015 NVIDIA Corporation

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

#include "uvm_test.h"
#include "uvm_lock.h"
#include "uvm_global.h"
#include "uvm_thread_context.h"

#define UVM_LOCK_ORDER_FIRST  (UVM_LOCK_ORDER_INVALID + 1)
#define UVM_LOCK_ORDER_SECOND (UVM_LOCK_ORDER_INVALID + 2)

static bool fake_lock(uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    // Just use the lock_order as the void * handle for the lock
    return __uvm_record_lock((void*)(long)lock_order, lock_order, flags);
}

static bool fake_unlock_common(uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    // Just use the lock_order as the void * handle for the lock
    return __uvm_record_unlock((void*)(long)lock_order, lock_order, flags);
}

static bool fake_unlock(uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    return fake_unlock_common(lock_order, flags);
}

static bool fake_unlock_out_of_order(uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    return fake_unlock_common(lock_order, flags | UVM_LOCK_FLAGS_OUT_OF_ORDER);
}

static bool fake_downgrade(uvm_lock_order_t lock_order)
{
    // Just use the lock_order as the void * handle for the lock
    return __uvm_record_downgrade((void*)(long)lock_order, lock_order);
}

static bool fake_check_locked(uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    return __uvm_check_locked((void*)(long)lock_order, lock_order, flags);
}

// TODO: Bug 1799173: The lock asserts verify that the RM GPU lock isn't taken
//       with the VA space lock in exclusive mode, and that the RM GPU lock
//       isn't taken with mmap_lock held in any mode. Hack around this in the
//       test to enable the checks until we figure out something better.
static bool skip_lock(uvm_lock_order_t lock_order, uvm_lock_flags_t flags)
{
    uvm_lock_flags_t mode_flags = (flags & UVM_LOCK_FLAGS_MODE_MASK);

    if (lock_order == UVM_LOCK_ORDER_RM_GPUS)
        return mode_flags == UVM_LOCK_FLAGS_MODE_EXCLUSIVE;

    return lock_order == UVM_LOCK_ORDER_MMAP_LOCK;
}

static NV_STATUS test_all_locks_from(uvm_lock_order_t from_lock_order)
{
    NvU32 exclusive;
    uvm_lock_flags_t flags;
    NvU32 out_of_order;
    NvU32 lock_order;

    TEST_CHECK_RET(from_lock_order != UVM_LOCK_ORDER_INVALID);

    for (out_of_order = 0; out_of_order < 2; ++out_of_order) {
        for (exclusive = 0; exclusive < 2; ++exclusive) {
            flags = exclusive ? UVM_LOCK_FLAGS_MODE_EXCLUSIVE : UVM_LOCK_FLAGS_MODE_SHARED;

            if (out_of_order)
                flags |= UVM_LOCK_FLAGS_OUT_OF_ORDER;

            for (lock_order = from_lock_order; lock_order < UVM_LOCK_ORDER_COUNT; ++lock_order) {
                TEST_CHECK_RET(__uvm_check_unlocked_order(lock_order));
                TEST_CHECK_RET(__uvm_check_lockable_order(lock_order, flags));
            }

            for (lock_order = from_lock_order; lock_order < UVM_LOCK_ORDER_COUNT; ++lock_order) {
                if (skip_lock(lock_order, flags))
                    continue;
                TEST_CHECK_RET(fake_lock(lock_order, flags));
            }

            if (!skip_lock(from_lock_order, flags)) {
                TEST_CHECK_RET(!__uvm_check_unlocked_order(from_lock_order));
                TEST_CHECK_RET(!__uvm_check_lockable_order(from_lock_order, flags));
            }

            for (lock_order = from_lock_order; lock_order < UVM_LOCK_ORDER_COUNT; ++lock_order) {
                if (skip_lock(lock_order, flags))
                    continue;
                TEST_CHECK_RET(fake_check_locked(lock_order, flags));
            }

            for (lock_order = from_lock_order; lock_order < UVM_LOCK_ORDER_COUNT; ++lock_order) {
                if (skip_lock(lock_order, flags))
                    continue;
                TEST_CHECK_RET(fake_check_locked(lock_order, UVM_LOCK_FLAGS_MODE_ANY));
            }

            if (out_of_order == 0) {
                for (lock_order = UVM_LOCK_ORDER_COUNT - 1; lock_order != from_lock_order - 1; --lock_order) {
                    if (skip_lock(lock_order, flags))
                        continue;
                    TEST_CHECK_RET(fake_unlock(lock_order, flags));
                }
            }
            else {
                for (lock_order = from_lock_order; lock_order < UVM_LOCK_ORDER_COUNT; ++lock_order) {
                    if (skip_lock(lock_order, flags))
                        continue;
                    TEST_CHECK_RET(fake_unlock_out_of_order(lock_order, flags));
                }
            }

            for (lock_order = from_lock_order; lock_order < UVM_LOCK_ORDER_COUNT; ++lock_order) {
                if (skip_lock(lock_order, flags))
                    continue;
                TEST_CHECK_RET(__uvm_check_unlocked_order(lock_order));
                TEST_CHECK_RET(__uvm_check_lockable_order(lock_order, flags));
            }
        }
    }

    return NV_OK;
}

static NV_STATUS test_all_locks(void)
{
    TEST_CHECK_RET(test_all_locks_from(UVM_LOCK_ORDER_FIRST) == NV_OK);

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_locking_first_as_shared_then_test_higher_order_locks(void)
{
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(test_all_locks_from(UVM_LOCK_ORDER_FIRST + 1) == NV_OK);
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_SHARED));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_locking_second_as_exclusive_then_test_higher_order_locks(void)
{
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(test_all_locks_from(UVM_LOCK_ORDER_SECOND + 1) == NV_OK);
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_unlocking_without_locking(void)
{
    // Unlocking a lock w/o locking any lock at all
    TEST_CHECK_RET(!fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_unlocking_different_lock_order_than_locked(void)
{
    // Unlocking a different lock than locked
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(!__uvm_thread_check_all_unlocked());

    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_unlocking_different_lock_instance_than_locked(void)
{
    // Unlocking a different instance of a lock than locked
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!__uvm_record_unlock(NULL, UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_unlocking_with_different_mode_than_locked(void)
{
    // Unlocking with different mode
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(!fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_unlocking_in_different_order_than_locked(void)
{
    // Unlocking in different order than locked
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock_out_of_order(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    // Unlocking in different order than locked (not necessarily incorrect, but
    // commonly pointing to issues)
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_locking_out_of_order(void)
{
    // Locking in wrong order
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_locking_same_order_twice(void)
{
    // Locking the same order twice (lock tracking doesn't support this case although
    // it's not necessarily incorrect)
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_checking_locked_when_no_locks_held(void)
{
    // Nothing locked
    TEST_CHECK_RET(!fake_check_locked(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(!fake_check_locked(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_check_locked(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_ANY));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_checking_exclusive_when_locked_as_shared(void)
{
    // Expecting exclusive while locked as shared
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(!fake_check_locked(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_SHARED));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_checking_shared_when_locked_as_exclusive(void)
{
    // Expecting shared while locked as exclusive
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_check_locked(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_checking_locked_when_different_instance_held(void)
{
    // Wrong instance of a lock held
    TEST_CHECK_RET(__uvm_record_lock(NULL, UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_check_locked(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(__uvm_record_unlock(NULL, UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_checking_all_unlocked_when_lock_held(void)
{
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(!__uvm_thread_check_all_unlocked());
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_SHARED));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_downgrading(void)
{
    // Lock downgrade
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_check_locked(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_check_locked(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_ANY));

    TEST_CHECK_RET(fake_downgrade(UVM_LOCK_ORDER_FIRST));
    TEST_CHECK_RET(fake_check_locked(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(fake_check_locked(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_ANY));

    // Can't downgrade twice
    TEST_CHECK_RET(!fake_downgrade(UVM_LOCK_ORDER_FIRST));
    TEST_CHECK_RET(fake_check_locked(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_ANY));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_SHARED));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_downgrading_without_locking(void)
{
    // Downgrading a lock w/o locking any lock at all
    TEST_CHECK_RET(!fake_downgrade(UVM_LOCK_ORDER_FIRST));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_downgrading_when_different_instance_held(void)
{
    // Wrong instance of lock to downgrade
    TEST_CHECK_RET(__uvm_record_lock(NULL, UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_downgrade(UVM_LOCK_ORDER_FIRST));
    TEST_CHECK_RET(__uvm_record_unlock(NULL, UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_downgrading_when_locked_as_shared(void)
{
    // Downgrading a lock that was acquired as shared
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_SHARED));
    TEST_CHECK_RET(!fake_downgrade(UVM_LOCK_ORDER_FIRST));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_SHARED));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS test_try_locking_out_of_order(void)
{
    // Try-locking in wrong order
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE | UVM_LOCK_FLAGS_TRYLOCK));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE | UVM_LOCK_FLAGS_TRYLOCK));
    TEST_CHECK_RET(fake_lock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(!fake_unlock(UVM_LOCK_ORDER_FIRST, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));
    TEST_CHECK_RET(fake_unlock(UVM_LOCK_ORDER_SECOND, UVM_LOCK_FLAGS_MODE_EXCLUSIVE));

    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    return NV_OK;
}

static NV_STATUS run_all_lock_tests(void)
{
    // The test needs all locks to be released initially
    TEST_CHECK_RET(__uvm_thread_check_all_unlocked());

    TEST_CHECK_RET(test_all_locks() == NV_OK);
    TEST_CHECK_RET(test_locking_first_as_shared_then_test_higher_order_locks() == NV_OK);
    TEST_CHECK_RET(test_locking_second_as_exclusive_then_test_higher_order_locks() == NV_OK);
    TEST_CHECK_RET(test_unlocking_without_locking() == NV_OK);
    TEST_CHECK_RET(test_unlocking_different_lock_order_than_locked() == NV_OK);
    TEST_CHECK_RET(test_unlocking_different_lock_instance_than_locked() == NV_OK);
    TEST_CHECK_RET(test_unlocking_with_different_mode_than_locked() == NV_OK);
    TEST_CHECK_RET(test_unlocking_in_different_order_than_locked() == NV_OK);
    TEST_CHECK_RET(test_locking_out_of_order() == NV_OK);
    TEST_CHECK_RET(test_locking_same_order_twice() == NV_OK);
    TEST_CHECK_RET(test_checking_locked_when_no_locks_held() == NV_OK);
    TEST_CHECK_RET(test_checking_exclusive_when_locked_as_shared() == NV_OK);
    TEST_CHECK_RET(test_checking_shared_when_locked_as_exclusive() == NV_OK);
    TEST_CHECK_RET(test_checking_locked_when_different_instance_held() == NV_OK);
    TEST_CHECK_RET(test_checking_all_unlocked_when_lock_held() == NV_OK);
    TEST_CHECK_RET(test_downgrading() == NV_OK);
    TEST_CHECK_RET(test_downgrading_without_locking() == NV_OK);
    TEST_CHECK_RET(test_downgrading_when_different_instance_held() == NV_OK);
    TEST_CHECK_RET(test_downgrading_when_locked_as_shared() == NV_OK);
    TEST_CHECK_RET(test_try_locking_out_of_order() == NV_OK);

    return NV_OK;
}

NV_STATUS uvm_test_lock_sanity(UVM_TEST_LOCK_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_thread_context_wrapper_t thread_context_wrapper_backup;

    // The global PM lock is acquired by the top-level UVM ioctl() entry point
    // and still held here, which confuses the (pre-existing) test logic that
    // assumes everything is unlocked at the beginning. Clearing the thread
    // context data resolves the issue, but the original state needs to be saved
    // and restored before exiting the test to avoid problems in the top-level
    // code.
    uvm_thread_context_save(&thread_context_wrapper_backup.context);

    status = run_all_lock_tests();

    uvm_thread_context_restore(&thread_context_wrapper_backup.context);

    return status;
}
