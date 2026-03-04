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

#include "uvm_forward_decl.h"
#include "uvm_thread_context.h"

#include "uvm_linux.h"
#include "uvm_common.h"

// Thread local storage implementation.
//
// The global data structure that contains the set of active thread contexts
// is a table of UVM_THREAD_CONTEXT_TABLE_SIZE entries of type
// uvm_thread_context_table_entry_t.
// Each entry contains a small array of UVM_THREAD_CONTEXT_ARRAY_SIZE entries,
// a red-black tree, and a lock protecting the tree.
//
// The thread_context_non_interrupt_table_entry() function maps the current task
// (i.e. the current thread context) to a table entry. That function also
// recommends a position within the entry's array, but that index can be safely
// ignored: the thread context can be located in any array slot, or in the
// red-black tree.
//
// The described global data structures try to minimize contention among
// threads at two levels. First, thread_context_non_interrupt_table_entry()
// relies on a hash function to evenly spread threads among table entries.
// Second, when several threads are mapped to the same table entry, the same
// hash function spreads them evenly among the array entries, which can
// be independently and atomically updated. If the array is full, the thread
// context of the current task is stored in the red-black tree of the table
// entry, which is protected by a single lock.
//
// Both the table and array entries are cache aligned to avoid false sharing
// overheads due to cache thrashing between concurrent operations on separate
// thread contexts.

#define UVM_THREAD_CONTEXT_ARRAY_SIZE 8

typedef struct  {
    void *acquired[UVM_LOCK_ORDER_COUNT];
} uvm_thread_context_lock_acquired_t;

typedef struct  {
    // If zero, the entry is empty. Otherwise, task is equal to the value of
    // get_current() for the thread associated with thread_context;
    atomic64_t task;

    uvm_thread_context_t *thread_context;
} ____cacheline_aligned_in_smp uvm_thread_context_array_entry_t;

// The thread's context information is stored in the array or the red-black
// tree.
typedef struct  {
    // Small array where thread contexts are stored first. Each array entry
    // can be atomically claimed or released.
    uvm_thread_context_array_entry_t array[UVM_THREAD_CONTEXT_ARRAY_SIZE];

    // Red-black tree, used when the array is full. A red-black tree is chosen
    // because additions and removals are frequent operations: every time the
    // UVM module is entered, there is one addition, one removal, and one
    // lookup. The same UVM call may result on additional lookups.
    struct rb_root tree;

    // Spinlock protecting the tree. A raw lock is chosen because UVM locks
    // rely on thread context information to be available for lock tracking.
    spinlock_t tree_lock;
} ____cacheline_aligned_in_smp uvm_thread_context_table_entry_t;

// Global data structure containing all the active thread contexts
static uvm_thread_context_table_entry_t g_thread_context_table[UVM_THREAD_CONTEXT_TABLE_SIZE];

static bool g_thread_context_table_initialized __read_mostly = false;

// Per CPU context wrapper, used for interrupt context. Zero initialized.
static DEFINE_PER_CPU(uvm_thread_context_wrapper_t, interrupt_thread_context_wrapper);

// Array of acquired locks, used in the interrupt path. The non-interrupt path
// allocates the array when locking for the first time.
static DEFINE_PER_CPU(uvm_thread_context_lock_acquired_t, interrupt_thread_context_lock_acquired);

static void thread_context_non_interrupt_remove(uvm_thread_context_t *thread_context,
                                                uvm_thread_context_table_entry_t *thread_context_entry);

bool uvm_thread_context_wrapper_is_used(void)
{
    // The wrapper contains lock information. While uvm_record_lock_X
    // routines are a no-op outside of debug mode, unit tests do invoke their
    // internal counterparts __uvm_record_lock_X. To add coverage, lock
    // information is made available in develop and release modes if the
    // builtin tests are enabled.
    return UVM_IS_DEBUG() || uvm_enable_builtin_tests;
}

bool uvm_thread_context_global_initialized(void)
{
    return g_thread_context_table_initialized;
}

void uvm_thread_context_global_init(void)
{
    size_t table_index;

    UVM_ASSERT(!uvm_thread_context_global_initialized());

    for (table_index = 0; table_index < UVM_THREAD_CONTEXT_TABLE_SIZE; table_index++) {
        uvm_thread_context_table_entry_t *table_entry = g_thread_context_table + table_index;

        spin_lock_init(&table_entry->tree_lock);
        table_entry->tree = RB_ROOT;
    }

    g_thread_context_table_initialized = true;
}

void uvm_thread_context_global_exit(void)
{
    size_t table_index;
    uvm_thread_context_t *curr_thread_context = uvm_thread_context();

    UVM_ASSERT(uvm_thread_context_global_initialized());

    // Search for thread contexts that were added but never removed.
    // There should be one thread context left: the one added by the UVM module
    // exit routine that invoked this function. To prevent the exit routine from
    // deleting its thread context after deinitialization of the global table,
    // it is deleted here. uvm_thread_context_remove will detect that the global
    // shutdown already happened and skip.
    for (table_index = 0; table_index < UVM_THREAD_CONTEXT_TABLE_SIZE; table_index++) {
        size_t array_index;
        struct rb_node *node;
        uvm_thread_context_table_entry_t *table_entry = g_thread_context_table + table_index;

        for (array_index = 0; array_index < UVM_THREAD_CONTEXT_ARRAY_SIZE; array_index++) {
            uvm_thread_context_t *thread_context;
            uvm_thread_context_array_entry_t *array_entry = table_entry->array + array_index;

            NvU64 task = atomic64_read(&array_entry->task);

            if (task == 0)
                continue;

            thread_context = array_entry->thread_context;

            UVM_ASSERT_MSG(thread_context == curr_thread_context,
                           "Left-over thread_context 0x%llx task 0x%llx\n",
                           (NvU64) thread_context,
                           (NvU64) thread_context->task);

            thread_context_non_interrupt_remove(thread_context, table_entry);
        }

        node = rb_first(&table_entry->tree);

        while (node) {
            uvm_thread_context_t *thread_context = rb_entry(node, uvm_thread_context_t, node);

            UVM_ASSERT_MSG(thread_context == curr_thread_context,
                           "Left-over thread_context 0x%llx task 0x%llx\n",
                           (NvU64) thread_context,
                           (NvU64) thread_context->task);

            thread_context_non_interrupt_remove(thread_context, table_entry);
            node = rb_first(&table_entry->tree);
        }
    }

    g_thread_context_table_initialized = false;
}

static uvm_thread_context_t *thread_context_non_interrupt_tree_search(struct rb_root *root, struct task_struct *task)
{
    struct rb_node *node = root->rb_node;
    uintptr_t task_uintptr = (uintptr_t) task;

    while (node) {
       uvm_thread_context_t *thread_context = rb_entry(node, uvm_thread_context_t, node);
       uintptr_t thread_context_task_uintptr = (uintptr_t) thread_context->task;

       if (thread_context_task_uintptr == task_uintptr)
           return thread_context;

       node = (thread_context_task_uintptr > task_uintptr)? node->rb_left : node->rb_right;
    }

    return NULL;
}

static bool thread_context_non_interrupt_tree_insert(struct rb_root *root, uvm_thread_context_t *new_thread_context)
{
    struct rb_node **node_ptr = &root->rb_node;
    struct rb_node *node = root->rb_node;
    struct rb_node *parent = NULL;
    const struct task_struct *task = new_thread_context->task;
    uintptr_t task_uintptr = (uintptr_t) task;

    while (node) {
        uvm_thread_context_t *thread_context = rb_entry(node, uvm_thread_context_t, node);
        uintptr_t thread_context_task_uintptr = (uintptr_t) thread_context->task;

       if (thread_context_task_uintptr == task_uintptr)
            return false;

        parent = node;
        node_ptr = (thread_context_task_uintptr > task_uintptr) ? &node->rb_left : &node->rb_right;
        node = *node_ptr;
    }

    rb_link_node(&new_thread_context->node, parent, node_ptr);
    rb_insert_color(&new_thread_context->node, root);

    return true;
}

static void thread_context_lock_interrupt_patch_acquired(uvm_thread_context_lock_t *context_lock)
{
    uvm_thread_context_lock_acquired_t *thread_context_lock_acquired;

    UVM_ASSERT(in_interrupt());
    UVM_ASSERT(context_lock->acquired == NULL);

    // Stich the preallocated, per-CPU array to the thread context lock.
    thread_context_lock_acquired = &get_cpu_var(interrupt_thread_context_lock_acquired);
    put_cpu_var(interrupt_thread_context_lock_acquired);
    context_lock->acquired = (void**) thread_context_lock_acquired;
}

static uvm_thread_context_lock_t *thread_context_lock_of(uvm_thread_context_t *thread_context)
{
    uvm_thread_context_wrapper_t *thread_context_wrapper;
    uvm_thread_context_lock_t *context_lock;

    if (!uvm_thread_context_wrapper_is_used())
        return NULL;

    thread_context_wrapper = container_of(thread_context, uvm_thread_context_wrapper_t, context);
    context_lock = &thread_context_wrapper->context_lock;

    // When the wrapper is used, the thread context lock is always present but
    // its acquired locks array may not, due to a failed allocation. Instead of
    // working around the missing array, pretend that the entire lock context
    // does not exist. This situation can only happen in non-interrupt paths.
    if (context_lock->acquired == NULL) {
        if (in_interrupt())
            thread_context_lock_interrupt_patch_acquired(context_lock);
        else
            return NULL;
    }

    return context_lock;
}

static void thread_context_non_interrupt_init(uvm_thread_context_t *thread_context)
{
    UVM_ASSERT(!in_interrupt());

    thread_context->array_index = UVM_THREAD_CONTEXT_ARRAY_SIZE;

    if (uvm_thread_context_wrapper_is_used()) {
        uvm_thread_context_wrapper_t *thread_context_wrapper;
        uvm_thread_context_lock_t *context_lock;

        thread_context_wrapper = container_of(thread_context, uvm_thread_context_wrapper_t, context);
        context_lock = &thread_context_wrapper->context_lock;

        memset(context_lock, 0, sizeof(*context_lock));

        // If this allocation fails, the lock context will appear as not
        // present, but the rest of the thread context is usable.
        context_lock->acquired = kmalloc(sizeof(context_lock->acquired[0]) * UVM_LOCK_ORDER_COUNT, NV_UVM_GFP_FLAGS);
    }
}

static void thread_context_non_interrupt_deinit(uvm_thread_context_t *thread_context)
{
    uvm_thread_context_lock_t *context_lock;

    UVM_ASSERT(!in_interrupt());

    context_lock = thread_context_lock_of(thread_context);
    if (context_lock != NULL) {
        UVM_ASSERT(__uvm_check_all_unlocked(context_lock));

        kfree(context_lock->acquired);
        context_lock->acquired = NULL;
    }
}

// Return the table entry and array index within that entry where the thread
// context of the current task is located.
//
// The array index should be interpreted as a hint: the thread context of the
// current taks may be stored at a different array index, or in the tree.
static uvm_thread_context_table_entry_t *thread_context_non_interrupt_table_entry(size_t *array_index_hint)
{
    size_t table_index;
    NvU64 current_ptr = (NvU64) current;
    NvU32 hash = jhash_2words((NvU32) current_ptr, (NvU32) (current_ptr >> 32), 0);

    BUILD_BUG_ON(UVM_THREAD_CONTEXT_TABLE_SIZE > (1 << 16));
    BUILD_BUG_ON(UVM_THREAD_CONTEXT_ARRAY_SIZE > (1 << 16));
    UVM_ASSERT(!in_interrupt());

    // The upper 16 bits of the hash value index the table; the lower 16
    // index the array
    table_index = (hash >> 16) % UVM_THREAD_CONTEXT_TABLE_SIZE;

    if (array_index_hint != NULL)
        *array_index_hint = hash % UVM_THREAD_CONTEXT_ARRAY_SIZE;

    return g_thread_context_table + table_index;
}

static uvm_thread_context_t *thread_context_non_interrupt(void)
{
    unsigned long flags;
    size_t i, array_index;
    uvm_thread_context_t *thread_context;
    uvm_thread_context_table_entry_t *table_entry = thread_context_non_interrupt_table_entry(&array_index);

    for (i = array_index; i < (UVM_THREAD_CONTEXT_ARRAY_SIZE + array_index); i++) {
        size_t curr_array_index = i % UVM_THREAD_CONTEXT_ARRAY_SIZE;
        uvm_thread_context_array_entry_t *array_entry = table_entry->array + curr_array_index;

        if (atomic64_read(&array_entry->task) == (NvU64) current) {
            thread_context = array_entry->thread_context;

            UVM_ASSERT(thread_context != NULL);
            UVM_ASSERT(thread_context->array_index == curr_array_index);

            return thread_context;
        }
    }

    spin_lock_irqsave(&table_entry->tree_lock, flags);
    thread_context = thread_context_non_interrupt_tree_search(&table_entry->tree, current);
    spin_unlock_irqrestore(&table_entry->tree_lock, flags);

    return thread_context;
}

static uvm_thread_context_t *thread_context_interrupt(void)
{
    uvm_thread_context_wrapper_t *thread_context_wrapper;

    // As we are in interrupt anyway it would be best to just use this_cpu_ptr()
    // but it was added in 2.6.33 and the interface is non-trivial to implement
    // prior to that.
    thread_context_wrapper = &get_cpu_var(interrupt_thread_context_wrapper);
    put_cpu_var(interrupt_thread_context_wrapper);

    return &thread_context_wrapper->context;
}

static uvm_thread_context_t *thread_context_current(void)
{
    return in_interrupt() ? thread_context_interrupt() : thread_context_non_interrupt();
}

bool uvm_thread_context_present(void)
{
    return thread_context_current() != NULL;
}

uvm_thread_context_t *uvm_thread_context(void)
{
    uvm_thread_context_t *thread_context = thread_context_current();

    // If this assertion fires is probably because an entry point into the
    // UVM module has not been wrapped with a UVM_ENTRY_X macro. The entry point
    // to wrap is the first nvidia-uvm function in the error call stack printed
    // by the assertion.
    UVM_ASSERT(thread_context != NULL);

    return thread_context;
}

// The addition logic takes into account that there may be a different thread
// context already associated with the given task. This happens in the uncommon
// case of re-entering the UVM module. Therefore, it is worth approaching the
// addition in a optimistic (speculative) fashion: if a slot is empty in the
// array, it is immediately taken. Should we discover later on that the task
// already has a thread context associated with it in the rest of the array or
// the tree, the previously claimed array slot is released.
static bool thread_context_non_interrupt_add(uvm_thread_context_t *thread_context,
                                             uvm_thread_context_table_entry_t *table_entry,
                                             size_t array_index_hint)
{
    size_t i;
    NvU64 task;
    unsigned long flags;
    bool added;

    UVM_ASSERT(!in_interrupt());
    UVM_ASSERT(thread_context != NULL);
    UVM_ASSERT(table_entry != NULL);
    UVM_ASSERT(table_entry - g_thread_context_table >= 0);
    UVM_ASSERT(table_entry - g_thread_context_table < UVM_THREAD_CONTEXT_TABLE_SIZE);
    UVM_ASSERT(array_index_hint < UVM_THREAD_CONTEXT_ARRAY_SIZE);

    thread_context_non_interrupt_init(thread_context);
    UVM_ASSERT(thread_context->array_index == UVM_THREAD_CONTEXT_ARRAY_SIZE);

    task = (NvU64) thread_context->task;
    UVM_ASSERT(task > 0);

    for (i = array_index_hint; i < (array_index_hint + UVM_THREAD_CONTEXT_ARRAY_SIZE); i++) {
        const size_t curr_array_index = i % UVM_THREAD_CONTEXT_ARRAY_SIZE;
        uvm_thread_context_array_entry_t *array_entry = table_entry->array + curr_array_index;

        if (thread_context->array_index == UVM_THREAD_CONTEXT_ARRAY_SIZE) {
            NvU64 old = atomic64_cmpxchg(&array_entry->task, 0, task);

            // Task already added a different thread context. The current thread
            // context has not been inserted but needs to be freed.
            if (old == task) {
                thread_context_non_interrupt_deinit(thread_context);
                return false;
            }

            // Speculatively add the current thread context.
            if (old == 0)
                thread_context->array_index = curr_array_index;
        }
        else if (atomic64_read(&array_entry->task) == task) {

            // Task already added a different thread context to the array, so
            // undo the speculative insertion
            atomic64_set(&table_entry->array[thread_context->array_index].task, 0);
            thread_context_non_interrupt_deinit(thread_context);

            return false;
        }
    }

    spin_lock_irqsave(&table_entry->tree_lock, flags);

    if (thread_context->array_index == UVM_THREAD_CONTEXT_ARRAY_SIZE) {

        // If the task already added a different thread context to the tree,
        // there is nothing to undo because the current thread context has not
        // been inserted.
        added = thread_context_non_interrupt_tree_insert(&table_entry->tree, thread_context);
    }
    else if (thread_context_non_interrupt_tree_search(&table_entry->tree, thread_context->task) != NULL) {

        // Task already added a different thread context to the tree, so undo
        // the speculative insertion
        atomic64_set(&table_entry->array[thread_context->array_index].task, 0);

        added = false;
    }
    else {

        // Speculative insertion succeeded: a thread context associated with the
        // same task has not been found in the array or the tree.
        table_entry->array[thread_context->array_index].thread_context = thread_context;
        added = true;
    }

    if (!added)
        thread_context_non_interrupt_deinit(thread_context);

    spin_unlock_irqrestore(&table_entry->tree_lock, flags);
    return added;
}

bool uvm_thread_context_add(uvm_thread_context_t *thread_context)
{
    uvm_thread_context_table_entry_t *table_entry;
    size_t array_index;

    UVM_ASSERT(thread_context != NULL);
    UVM_ASSERT(!in_interrupt());

    // Initialize the thread context table. This can only happen when loading
    // the UVM module
    if (!uvm_thread_context_global_initialized())
        uvm_thread_context_global_init();

    thread_context->task = current;
    thread_context->ignore_hmm_invalidate_va_block = NULL;
    table_entry = thread_context_non_interrupt_table_entry(&array_index);
    return thread_context_non_interrupt_add(thread_context, table_entry, array_index);
}

bool uvm_thread_context_add_at(uvm_thread_context_t *thread_context, size_t table_index)
{
    uvm_thread_context_table_entry_t *table_entry;

    UVM_ASSERT(uvm_enable_builtin_tests != 0);
    UVM_ASSERT(uvm_thread_context_global_initialized());

    table_entry = g_thread_context_table + table_index;
    return thread_context_non_interrupt_add(thread_context, table_entry, 0);
}

static void thread_context_non_interrupt_remove(uvm_thread_context_t *thread_context,
                                                uvm_thread_context_table_entry_t *table_entry)
{
    NvU32 array_index;

    UVM_ASSERT(!in_interrupt());
    UVM_ASSERT(thread_context != NULL);
    UVM_ASSERT(table_entry != NULL);
    UVM_ASSERT(table_entry - g_thread_context_table >= 0);
    UVM_ASSERT(table_entry - g_thread_context_table < UVM_THREAD_CONTEXT_TABLE_SIZE);

    array_index = thread_context->array_index;
    UVM_ASSERT(array_index <= UVM_THREAD_CONTEXT_ARRAY_SIZE);

    // We cannot use RB_EMPTY_NODE to determine if the thread context is in the
    // tree, because the tree lock is not held and we haven't called RB_CLEAR_NODE.
    // If the thread context is indeed in the tree, concurrent operations on
    // the parent pointer/color of the thread context's node could result in
    // RB_EMPTY_NODE(thread_context->node) being true.
    if (array_index != UVM_THREAD_CONTEXT_ARRAY_SIZE) {

        uvm_thread_context_array_entry_t *array_entry = table_entry->array + array_index;

        UVM_ASSERT(array_index < UVM_THREAD_CONTEXT_ARRAY_SIZE);
        UVM_ASSERT(atomic64_read(&array_entry->task) == (NvU64) thread_context->task);

        // Clear the task. The memory barrier prevents the write from being
        // moved before a previous (in program order) write to the entry's
        // thread_context field in thread_context_non_interrupt_add.
        //
        // A more detailed explanation about why the memory barrier is needed
        // before an atomic write, and why we are not using a different flavor
        // of atomic write such as atomic64_set_release, can be found in
        // uvm_gpu_semaphore.c:update_completed_value_locked().
        smp_mb__before_atomic();
        atomic64_set(&array_entry->task, 0);
    }
    else {
        unsigned long flags;

        spin_lock_irqsave(&table_entry->tree_lock, flags);
        rb_erase(&thread_context->node, &table_entry->tree);
        spin_unlock_irqrestore(&table_entry->tree_lock, flags);
    }

    thread_context_non_interrupt_deinit(thread_context);
}

void uvm_thread_context_remove(uvm_thread_context_t *thread_context)
{
    uvm_thread_context_table_entry_t *table_entry;

    UVM_ASSERT(thread_context != NULL);
    UVM_ASSERT(!in_interrupt());

    // If the thread context table has been deinitialized, then we must be in
    // the UVM module unload path, and the thread context added during the call
    // of uvm_exit has already been removed in the global deinitialization.
    if (!uvm_thread_context_global_initialized())
        return;

    UVM_ASSERT(thread_context->task == current);
    UVM_ASSERT(uvm_thread_context() == thread_context);

    table_entry = thread_context_non_interrupt_table_entry(NULL);
    thread_context_non_interrupt_remove(thread_context, table_entry);
}

void uvm_thread_context_remove_at(uvm_thread_context_t *thread_context, size_t table_index)
{
    uvm_thread_context_table_entry_t *table_entry = g_thread_context_table + table_index;

    UVM_ASSERT(uvm_enable_builtin_tests != 0);

    thread_context_non_interrupt_remove(thread_context, table_entry);
}

// Move operation
//  -Lock information is copied to the destination, and cleared in the source.
//  -Locations in the global array or tree are not copied nor cleared, since
//   they may be needed for a later removal of the source, and are no longer
//   valid after it.
//  -When adding new members to the thread context, consider if they need to be
//   moved
static void thread_context_move(uvm_thread_context_t *dst, uvm_thread_context_t *src)
{
    uvm_thread_context_lock_t *src_context_lock, *dst_context_lock;

    UVM_ASSERT(uvm_enable_builtin_tests != 0);

    src_context_lock = thread_context_lock_of(src);
    dst_context_lock = thread_context_lock_of(dst);

    if ((dst_context_lock != NULL) && (src_context_lock != NULL)) {
        size_t acquired_size = sizeof(src_context_lock->acquired[0]) * UVM_LOCK_ORDER_COUNT;

        dst_context_lock->skip_lock_tracking = src_context_lock->skip_lock_tracking;
        src_context_lock->skip_lock_tracking = false;

        // Note that the locks are not released, even when they appear as such
        // if we query the source thread context. They are still acquired in the
        // destination context.
        bitmap_copy(dst_context_lock->acquired_lock_orders,
                    src_context_lock->acquired_lock_orders,
                    UVM_LOCK_ORDER_COUNT);
        bitmap_zero(src_context_lock->acquired_lock_orders, UVM_LOCK_ORDER_COUNT);

        bitmap_copy(dst_context_lock->exclusive_acquired_lock_orders,
                    src_context_lock->exclusive_acquired_lock_orders,
                    UVM_LOCK_ORDER_COUNT);
        bitmap_zero(src_context_lock->exclusive_acquired_lock_orders, UVM_LOCK_ORDER_COUNT);

        bitmap_copy(dst_context_lock->out_of_order_acquired_lock_orders,
                    src_context_lock->out_of_order_acquired_lock_orders,
                    UVM_LOCK_ORDER_COUNT);
        bitmap_zero(src_context_lock->out_of_order_acquired_lock_orders, UVM_LOCK_ORDER_COUNT);

        memcpy(dst_context_lock->acquired, src_context_lock->acquired, acquired_size);
    }
}

void uvm_thread_context_save(uvm_thread_context_t *dst)
{
    thread_context_non_interrupt_init(dst);
    thread_context_move(dst, uvm_thread_context());
}

void uvm_thread_context_restore(uvm_thread_context_t *src)
{
    thread_context_move(uvm_thread_context(), src);
    thread_context_non_interrupt_deinit(src);
}

uvm_thread_context_lock_t *uvm_thread_context_lock_get(void)
{
    return thread_context_lock_of(uvm_thread_context());
}

void uvm_thread_context_lock_disable_tracking(void)
{
    uvm_thread_context_lock_t *context_lock = thread_context_lock_of(uvm_thread_context());

    if (context_lock == NULL)
        return;

    ++context_lock->skip_lock_tracking;

    UVM_ASSERT(context_lock->skip_lock_tracking != 0);
}

void uvm_thread_context_lock_enable_tracking(void)
{
    uvm_thread_context_lock_t *context_lock = thread_context_lock_of(uvm_thread_context());

    if (context_lock == NULL)
        return;

    UVM_ASSERT(context_lock->skip_lock_tracking > 0);

    --context_lock->skip_lock_tracking;
}
