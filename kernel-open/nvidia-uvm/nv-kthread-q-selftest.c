/*******************************************************************************
    Copyright (c) 2016 NVIDIA Corporation

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

#include "nv-kthread-q.h"
#include <linux/vmalloc.h>
#include <linux/kthread.h>
#include <linux/string.h>
#include <linux/completion.h>
#include <linux/module.h>
#include <linux/cpumask.h>
#include <linux/mm.h>

// If NV_BUILD_MODULE_INSTANCES is not defined, do it here in order to avoid
// build warnings/errors when including nv-linux.h as it expects the definition
// to be present.
#ifndef NV_BUILD_MODULE_INSTANCES
#define NV_BUILD_MODULE_INSTANCES 1
#endif
#include "nv-linux.h"

// Below are just a very few lines of printing and test assertion support.
// It is important to avoid dependencies on other modules, because nv-kthread-q
// and its self test are supposed to only require:
//
//    -- Linux kernel functions and macros
//
// In order to avoid external dependencies (specifically, NV_STATUS codes), all
// functions in this file return non-zero upon failure, and zero upon success.

#ifndef NVIDIA_PRETTY_PRINTING_PREFIX
    #define NVIDIA_PRETTY_PRINTING_PREFIX "nvidia: "
#endif

// This prints even on release builds:
#define NVQ_TEST_PRINT(fmt, ...)                                               \
    printk(KERN_INFO NVIDIA_PRETTY_PRINTING_PREFIX "%s:%u[pid:%d] " fmt,       \
           __FUNCTION__,                                                       \
           __LINE__,                                                           \
           current->pid,                                                       \
           ##__VA_ARGS__)

// Caution: This macro will return out of the current scope
#define TEST_CHECK_RET(cond)                                                   \
    do {                                                                       \
        if (unlikely(!(cond))) {                                               \
            NVQ_TEST_PRINT("Test check failed, condition '%s' not true\n",     \
                           #cond);                                             \
            on_nvq_assert();                                                   \
            return -1;                                                         \
        }                                                                      \
    } while(0)

// Most test failures will do things such as just hang or crash. However, in
// order to detect bugs that are less fatal, simply count how many queue items
// actually ran.

#define NUM_Q_ITEMS_IN_BASIC_TEST       6
#define NUM_RESCHEDULE_CALLBACKS        10
#define NUM_TEST_Q_ITEMS                (100 * 1000)
#define NUM_TEST_KTHREADS               8
#define NUM_Q_ITEMS_IN_MULTITHREAD_TEST (NUM_TEST_Q_ITEMS * NUM_TEST_KTHREADS)

// This exists in order to have a function to place a breakpoint on:
void on_nvq_assert(void)
{
    (void)NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Basic start-stop test

typedef struct basic_start_stop_args
{
    int                 value_to_write;
    int                 *where_to_write;
} basic_start_stop_args_t;

static void _basic_start_stop_callback(void *args)
{
    basic_start_stop_args_t *start_stop_args = (basic_start_stop_args_t*)args;

    *start_stop_args->where_to_write = start_stop_args->value_to_write;
}

static int _basic_start_stop_test(void)
{
    int i, was_scheduled;
    int result = 0;
    nv_kthread_q_item_t q_item[NUM_Q_ITEMS_IN_BASIC_TEST];
    int callback_values_written[NUM_Q_ITEMS_IN_BASIC_TEST];
    basic_start_stop_args_t start_stop_args[NUM_Q_ITEMS_IN_BASIC_TEST];
    nv_kthread_q_t local_q;

    // Do a redudant stop to ensure stop is supported on zero initialized memory
    // No crash should occur
    memset(&local_q, 0, sizeof(nv_kthread_q_t));
    nv_kthread_q_stop(&local_q);

    // Do a quick start-stop cycle first:
    result = nv_kthread_q_init(&local_q, "q_to_stop");
    TEST_CHECK_RET(result == 0);
    nv_kthread_q_stop(&local_q);

    // call another q_stop and it shouldn't crash and should return fine
    nv_kthread_q_stop(&local_q);

    memset(&start_stop_args, 0, sizeof(start_stop_args));
    memset(callback_values_written, 0, sizeof(callback_values_written));

    // All the callback arguments point to the same nv_kthread_q:
    for (i = 0; i < NUM_Q_ITEMS_IN_BASIC_TEST; ++i) {
        start_stop_args[i].value_to_write = i;
        start_stop_args[i].where_to_write = &callback_values_written[i];
    }

    result = nv_kthread_q_init(&local_q, "basic_q");
    TEST_CHECK_RET(result == 0);

    // Launch 3 items, then flush the queue.
    //
    // Each iteration sends a different instance of args to the callback
    // function.
    for (i = 0; i < 3; ++i) {
        nv_kthread_q_item_init(&q_item[i],
                               _basic_start_stop_callback,
                               &start_stop_args[i]);

        was_scheduled = nv_kthread_q_schedule_q_item(&local_q, &q_item[i]);
        result |= (!was_scheduled);
    }

    // It is legal to flush more than once, so flush twice in a row:
    nv_kthread_q_flush(&local_q);
    nv_kthread_q_flush(&local_q);

    // Launch the remaining items, then stop (which flushes) the queue:
    for (i = 3; i < NUM_Q_ITEMS_IN_BASIC_TEST; ++i) {
        nv_kthread_q_item_init(&q_item[i],
                               _basic_start_stop_callback,
                               &start_stop_args[i]);

        was_scheduled = nv_kthread_q_schedule_q_item(&local_q, &q_item[i]);
        result |= (!was_scheduled);
    }

    nv_kthread_q_stop(&local_q);

    // Verify that all the callbacks ran and wrote their values:
    for (i = 0; i < NUM_Q_ITEMS_IN_BASIC_TEST; ++i)
        TEST_CHECK_RET(callback_values_written[i] == i);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// Multithreaded test

typedef struct multithread_args
{
    nv_kthread_q_t      *test_q;
    atomic_t            *test_wide_accumulator;
    atomic_t             per_thread_accumulator;
} multithread_args_t;

static void _multithread_callback(void *args)
{
    multithread_args_t *multithread_args = (multithread_args_t*)(args);
    atomic_inc(multithread_args->test_wide_accumulator);
    atomic_inc(&multithread_args->per_thread_accumulator);
}

//
// Return values:
// 0:        Success
// -ENOMEM:  vmalloc failed
// -EINVAL:  test failed due to mismatched accumulator counts
//
static int _multithreaded_q_kthread_function(void *args)
{
    int i, was_scheduled;
    int result = 0;
    int per_thread_count;
    int test_wide_count;
    multithread_args_t *multithread_args = (multithread_args_t*)args;
    nv_kthread_q_item_t *q_items;
    size_t alloc_size = NUM_TEST_Q_ITEMS * sizeof(nv_kthread_q_item_t);

    q_items = vmalloc(alloc_size);
    if (!q_items) {
        result = -ENOMEM;
        goto done;
    }

    memset(q_items, 0, alloc_size);

    for (i = 0; i < NUM_TEST_Q_ITEMS; ++i) {
        nv_kthread_q_item_init(&q_items[i],
                               _multithread_callback,
                               multithread_args);

        was_scheduled = nv_kthread_q_schedule_q_item(multithread_args->test_q,
                                                     &q_items[i]);
        result |= (!was_scheduled);
    }

    nv_kthread_q_flush(multithread_args->test_q);

    per_thread_count = atomic_read(&multithread_args->per_thread_accumulator);
    if (per_thread_count != NUM_TEST_Q_ITEMS) {
        NVQ_TEST_PRINT("per_thread_count: Expected: %d, actual: %d\n",
                       NUM_TEST_Q_ITEMS, per_thread_count);
        goto done;
    }

    test_wide_count = atomic_read(multithread_args->test_wide_accumulator);
    if (test_wide_count < NUM_TEST_Q_ITEMS) {
        NVQ_TEST_PRINT("test_wide_count: Expected: >= %d, actual: %d\n",
                       NUM_TEST_Q_ITEMS, test_wide_count);
        goto done;
    }

done:
    if (q_items)
        vfree(q_items);

    while (!kthread_should_stop())
        schedule();

    return result;
}

static int _multithreaded_q_test(void)
{
    int i, j;
    int result = 0;
    struct task_struct *kthreads[NUM_TEST_KTHREADS];
    multithread_args_t multithread_args[NUM_TEST_KTHREADS];
    nv_kthread_q_t local_q;
    atomic_t       local_accumulator;

    memset(multithread_args, 0, sizeof(multithread_args));
    memset(kthreads, 0, sizeof(kthreads));
    atomic_set(&local_accumulator, 0);

    result = nv_kthread_q_init(&local_q, "multithread_test_q");
    TEST_CHECK_RET(result == 0);

    for (i = 0; i < NUM_TEST_KTHREADS; ++i) {
        multithread_args[i].test_q                = &local_q;
        multithread_args[i].test_wide_accumulator = &local_accumulator;

        kthreads[i] = kthread_run(_multithreaded_q_kthread_function,
                                  &multithread_args[i],
                                  "nvq_test_kthread");

        if (IS_ERR(kthreads[i]))
            goto failed;
    }

    // Stop all of the test kthreads, then stop the queue. Collect any
    // non-zero (failure) return values from the kthreads, and use those
    // later to report a test failure.
    for (i = 0; i < NUM_TEST_KTHREADS; ++i) {
        result |= kthread_stop(kthreads[i]);
    }

    nv_kthread_q_stop(&local_q);

    TEST_CHECK_RET(atomic_read(&local_accumulator) ==
                   NUM_Q_ITEMS_IN_MULTITHREAD_TEST);
    return result;

failed:
    NVQ_TEST_PRINT("kthread_run[%d] failed: errno: %ld\n",
                   i, PTR_ERR(kthreads[i]));

    // Stop any threads that had successfully started:
    for (j = 0; j < i; ++j)
        kthread_stop(kthreads[j]);

    nv_kthread_q_stop(&local_q);
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Self-rescheduling test

typedef struct resched_args
{
    nv_kthread_q_t      test_q;
    nv_kthread_q_item_t q_item;
    atomic_t            accumulator;
    atomic_t            stop_rescheduling_callbacks;
    int                 test_failure;
} resched_args_t;

static void _reschedule_callback(void *args)
{
    int was_scheduled;
    resched_args_t *resched_args = (resched_args_t*)args;

    // This test promises to add one to accumulator, for each time through.
    atomic_inc(&resched_args->accumulator);

    if (atomic_read(&resched_args->stop_rescheduling_callbacks) == 0) {
        nv_kthread_q_item_init(&resched_args->q_item,
                               _reschedule_callback,
                               resched_args);

        was_scheduled = nv_kthread_q_schedule_q_item(&resched_args->test_q,
                                                     &resched_args->q_item);
        if (!was_scheduled) {
            resched_args->test_failure = 1;
        }
    }

    // Ensure thread relinquishes control else we hang in single-core environments
    schedule();
}

// Verify that re-scheduling the same q_item, from within its own
// callback, works.
static int _reschedule_same_item_from_its_own_callback_test(void)
{
    int was_scheduled;
    int result = 0;
    resched_args_t resched_args;

    memset(&resched_args, 0, sizeof(resched_args));

    result = nv_kthread_q_init(&resched_args.test_q, "resched_test_q");
    TEST_CHECK_RET(result == 0);

    nv_kthread_q_item_init(&resched_args.q_item,
                           _reschedule_callback,
                           &resched_args);

    was_scheduled = nv_kthread_q_schedule_q_item(&resched_args.test_q,
                                                 &resched_args.q_item);
    result |= (!was_scheduled);

    // Wait for a few callback items to run
    while(atomic_read(&resched_args.accumulator) < NUM_RESCHEDULE_CALLBACKS)
        schedule();

    // Stop the callbacks from rescheduling themselves. This requires two
    // levels of flushing: one flush to wait for any callbacks that missed
    // the .stop_rescheduling_callbacks change, and another for any pending
    // callbacks that were scheduled from within the callback.
    atomic_set(&resched_args.stop_rescheduling_callbacks, 1);

    // Stop the queue. This is guaranteed to do a (double) flush, and that
    // flush takes care of any pending callbacks that we rescheduled from
    // within the callback function.
    nv_kthread_q_stop(&resched_args.test_q);

    return (result || resched_args.test_failure);
}

////////////////////////////////////////////////////////////////////////////////
// Rescheduling the exact same q_item test

typedef struct same_q_item_args
{
    atomic_t test_accumulator;
} same_q_item_args_t;

static void _same_q_item_callback(void *args)
{
    same_q_item_args_t *same_q_item_args = (same_q_item_args_t*)(args);
    atomic_inc(&same_q_item_args->test_accumulator);
}

static int _same_q_item_test(void)
{
    int result, i;
    int num_scheduled = 0;
    same_q_item_args_t  same_q_item_args;
    nv_kthread_q_t      local_q;
    nv_kthread_q_item_t q_item;

    memset(&same_q_item_args, 0, sizeof(same_q_item_args));

    result = nv_kthread_q_init(&local_q, "same_q_item_test_q");
    TEST_CHECK_RET(result == 0);

    nv_kthread_q_item_init(&q_item,
                           _same_q_item_callback,
                           &same_q_item_args);

    // Attempt to queue up many copies of the same q_item, then stop the queue.
    // This is an attempt to launch enough q_items that at least some of them
    // end up being pending in the queue, and exercise the "if already pending"
    // logic.
    //
    // Some manual testing indicates that launching 1000 q_items in a tight loop
    // causes between 1 and 20 copies to run. Obviously this is extremely
    // dependent on the particular test machine and kernel and more, but it
    // shows that 1000 is not unreasonable.
    for (i = 0; i < 1000; ++i)
        num_scheduled += nv_kthread_q_schedule_q_item(&local_q, &q_item);

    nv_kthread_q_stop(&local_q);

    // At least one item will have run, but not necessarily any more than that.
    TEST_CHECK_RET(atomic_read(&same_q_item_args.test_accumulator) >= 1);
    TEST_CHECK_RET(atomic_read(&same_q_item_args.test_accumulator) == num_scheduled);

    return 0;
}

// Returns true if any of the stack pages are not resident on the indicated node.
static bool stack_mismatch(const struct task_struct *thread, int preferred_node)
{
    unsigned num_stack_pages, i;
    char* stack = (char*) thread->stack;

    // If the stack has not been allocated using vmalloc, the physical pages
    // are all on the same node, so just check the first page
    if (!is_vmalloc_addr(stack)) {
        struct page *page = virt_to_page(stack);
        int node = page_to_nid(page);

        return node != preferred_node;
    }

    num_stack_pages = THREAD_SIZE >> PAGE_SHIFT;

    // The physical pages backing the stack may be discontiguous, so check them
    // all.
    for (i = 0; i < num_stack_pages; i++) {
        char *curr_stack_page = stack + i * PAGE_SIZE;
        struct page *page = vmalloc_to_page(curr_stack_page);
        int node = page_to_nid(page);

        if (node != preferred_node)
            return true;
    }

    return false;
}

static void _check_cpu_affinity_callback(void *args)
{
    struct task_struct *thread = get_current();
    int *preferred_node = (int *) args;
    int *ret = preferred_node + 1;

    *ret = stack_mismatch(thread, *preferred_node);
}

// Verify that the stack of the kernel thread created by
// nv_kthread_q_init_on_node is resident on the specified NUMA node.
//
// nv_kthread_q_init_on_node does not guarantee that the thread's stack
// will be resident on the passed node, but in practice the preference is mostly
// honored so we invoke the function multiple times and allow a percentage of
// failures per node.
static int _check_cpu_affinity_test(void)
{
    int result, node;
    nv_kthread_q_t local_q;

    for_each_online_node(node) {
        unsigned i;
        const unsigned max_i = 100;
        unsigned stack_mismatches = 0;

        // Allow up to 20% of the stacks to be resident on a node different from
        // the one requested.
        const int alloc_mismatch_percentage = 20;

        // Only test on CPU nodes which have memory
        if (!nv_numa_node_has_memory(node) || !node_state(node, N_CPU))
            continue;

        for (i = 0; i < max_i; i++) {
            unsigned j;
            int thread_args[2];
            nv_kthread_q_item_t q_item;
            char q_name[64];

            nv_kthread_q_item_init(&q_item, _check_cpu_affinity_callback, thread_args);
            snprintf(q_name, sizeof(q_name), "test_q_%d", node);
            result = nv_kthread_q_init_on_node(&local_q, q_name, node);
            TEST_CHECK_RET(result == 0);

            // The second entry contains the value returned by the callback:
            // 0 if no mismatch found, and 1 otherwise.
            thread_args[0] = node;
            thread_args[1] = 0;

            // Run several iterations to ensure that the thread's stack does
            // not migrate after initialization.
            for (j = 0; j < 25; j++) {
                result = nv_kthread_q_schedule_q_item(&local_q, &q_item);

                // nv_kthread_q_schedule_q_item() returns non-zero value if the
                // item was successfully scheduled.
                if (result == 0) {
                    nv_kthread_q_stop(&local_q);
                    TEST_CHECK_RET(false);
                }

                nv_kthread_q_flush(&local_q);

                // Count as failure if any of the stack pages is resident on a
                // another node on any iteration.
                if (thread_args[1] == 1) {
                    stack_mismatches++;
                    break;
                }
            }

            nv_kthread_q_stop(&local_q);

            if ((100 * stack_mismatches / max_i) > alloc_mismatch_percentage)
                TEST_CHECK_RET(false);
        }
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Top-level test entry point

int nv_kthread_q_run_self_test(void)
{
    int result;

    result = _basic_start_stop_test();
    TEST_CHECK_RET(result == 0);

    result = _reschedule_same_item_from_its_own_callback_test();
    TEST_CHECK_RET(result == 0);

    result = _multithreaded_q_test();
    TEST_CHECK_RET(result == 0);

    result = _same_q_item_test();
    TEST_CHECK_RET(result == 0);

    result = _check_cpu_affinity_test();
    TEST_CHECK_RET(result == 0);

    return 0;
}
