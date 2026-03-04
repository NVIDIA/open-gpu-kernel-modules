/*******************************************************************************
    Copyright (c) 2015-2023 NVIDIA Corporation

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

#ifndef __UVM_THREAD_CONTEXT_H__
#define __UVM_THREAD_CONTEXT_H__

#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_common.h"
#include "uvm_linux.h"

#define UVM_THREAD_CONTEXT_TABLE_SIZE 64

// Used to track lock correctness and store information about locks held by each
// thread.
struct uvm_thread_context_lock_struct
{
    // Opt-out of lock tracking if >0
    NvU32 skip_lock_tracking;

    // Bitmap of acquired lock orders
    DECLARE_BITMAP(acquired_lock_orders, UVM_LOCK_ORDER_COUNT);

    // Bitmap of exclusively acquired lock orders
    DECLARE_BITMAP(exclusive_acquired_lock_orders, UVM_LOCK_ORDER_COUNT);

    // Bitmap of lock orders acquired out of order (via trylock)
    DECLARE_BITMAP(out_of_order_acquired_lock_orders, UVM_LOCK_ORDER_COUNT);

    // Array of pointers to acquired locks. Indexed by lock order, so the
    // array size is UVM_LOCK_ORDER_COUNT
    //
    // The value at a given index is undefined if the corresponding bit is not
    // set in acquired_locked_orders.
    void **acquired;
};

// UVM thread contexts provide thread local storage for all logical threads
// executing in the UVM driver. Both user and interrupt contexts are supported.
struct uvm_thread_context_struct
{
    // Pointer to the thread (task) associated with the context
    //
    // This field is ignored in interrupt paths
    struct task_struct *task;

    // This context is present at the given array index if array_index is less
    // than UVM_THREAD_CONTEXT_ARRAY_SIZE; otherwise is in the tree.
    //
    // This field is ignored in interrupt paths
    NvU32 array_index;

    // Set if uvm_hmm_invalidate() callbacks should be ignored on this va_block.
    // This needs to be set whenever the va_block lock is held and
    // migrate_vma_setup() needs to be called since the "slow path" which
    // calls try_to_migrate() doesn't pass the pgmap_owner.
    uvm_va_block_t *ignore_hmm_invalidate_va_block;

    // Used to filter out invalidations we don't care about.
    unsigned long hmm_invalidate_seqnum;

    // Pointer to enclosing node (if any) in red-black tree
    //
    // This field is ignored in interrupt paths
    struct rb_node node;
};

bool uvm_thread_context_wrapper_is_used(void);

// A thread context wrapper augments a thread context with additional
// information useful for debugging, profiling, etc. Since the extra information
// can take up considerable space, it is only allocated if
// uvm_thread_context_wrapper_is_used() returns true.
struct uvm_thread_context_wrapper_struct
{
    uvm_thread_context_t context;

    uvm_thread_context_lock_t context_lock;
};

bool uvm_thread_context_global_initialized(void);
void uvm_thread_context_global_init(void);
void uvm_thread_context_global_exit(void);

// Retrieve the current thread context. Asserts if it does not exist.
uvm_thread_context_t *uvm_thread_context(void);

// Returns true if the current thread context is present: it has been added but
// not removed, or we are in a interrupt path.
bool uvm_thread_context_present(void);

// Set the current thread context to be thread_context. Returns false if there
// is a different thread context already associated with the current task i.e.
// the insertion did not happen. This scenario arises when re-entering the UVM
// module, because the first entry point already associated the thread with a
// different thread context.
//
// Do not invoke this function in a interrupt path.
bool uvm_thread_context_add(uvm_thread_context_t *thread_context);

// Reset the current thread context, which should be thread_context.
//
// The current thread context is passed, even if it is guaranteed to match
// uvm_thread_context(), to avoid the lookup overhead.
//
// Do not invoke this function in a interrupt path.
void uvm_thread_context_remove(uvm_thread_context_t *thread_context);

// Add or remove thread contexts at the given global thread context table
// index. Used only in testing.
//
// Thread contexts inserted using uvm_thread_context_add_at cannot be retrieved
// using uvm_thread_context()
bool uvm_thread_context_add_at(uvm_thread_context_t *op_context, size_t table_index);
void uvm_thread_context_remove_at(uvm_thread_context_t *op_context, size_t table_index);

// Save the state of the current thread context to the given thread context, and
// then clear the state of the current thread context.
//
// Used only in testing: for example, we may want to ensure that a test starts
// with no locks held in the current thread context, so we save its state at the
// beginning of the test, and restore the state at the end.
//
// This function breaks any expectations about what the current thread context
// should contain. For example, any IOCTL entry point into the UVM module
// results on a power management lock being acquired. Clearing the lock state
// in the current thread context will confuse code that assumes the lock is
// acquired at all times.
void uvm_thread_context_save(uvm_thread_context_t *dst);

// Restore the state of the current thread context out of the given (backup)
// thread context.
void uvm_thread_context_restore(uvm_thread_context_t *src);

// Get the current thread lock context. Returns NULL if there is no thread lock
// context (we are in release mode, or an internal allocation failed).
uvm_thread_context_lock_t *uvm_thread_context_lock_get(void);

// Disable lock tracking in the current thread lock context
// Lock tracking is enabled by default, but can be disabled by using this
// function.
// The disable lock tracking calls are refcounted so to enable tracking back all
// of the disable calls have to be paired with an enable call.
//
// This is needed in some tests that need to violate lock ordering, e.g. one of
// the push tests acquires the push sema multiple times.
void uvm_thread_context_lock_disable_tracking(void);

// Enable back lock tracking in the current thread lock context. Should be
// paired with a previous uvm_thread_lock_context_disable_lock_tracking() call.
// The lock tracking is enabled back only when all previous disable calls have
// been paired with an enable call.
void uvm_thread_context_lock_enable_tracking(void);

#endif // __UVM_THREAD_CONTEXT_H__
