/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nv-kthread-q.h"
#include "nv-list-helpers.h"

#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <linux/module.h>
#include <linux/mm.h>

#if defined(NV_LINUX_BUG_H_PRESENT)
    #include <linux/bug.h>
#else
    #include <asm/bug.h>
#endif

// Today's implementation is a little simpler and more limited than the
// API description allows for in nv-kthread-q.h. Details include:
//
// 1. Each nv_kthread_q instance is a first-in, first-out queue.
//
// 2. Each nv_kthread_q instance is serviced by exactly one kthread.
//
// You can create any number of queues, each of which gets its own
// named kernel thread (kthread). You can then insert arbitrary functions
// into the queue, and those functions will be run in the context of the
// queue's kthread.

#ifndef WARN
    // Only *really* old kernels (2.6.9) end up here. Just use a simple printk
    // to implement this, because such kernels won't be supported much longer.
    #define WARN(condition, format...) ({                    \
        int __ret_warn_on = !!(condition);                   \
        if (unlikely(__ret_warn_on))                         \
            printk(KERN_ERR format);                         \
        unlikely(__ret_warn_on);                             \
    })
#endif

#define NVQ_WARN(fmt, ...)                                   \
    do {                                                     \
        if (in_interrupt()) {                                \
            WARN(1, "nv_kthread_q: [in interrupt]: " fmt,    \
            ##__VA_ARGS__);                                  \
        }                                                    \
        else {                                               \
            WARN(1, "nv_kthread_q: task: %s: " fmt,          \
                 current->comm,                              \
                 ##__VA_ARGS__);                             \
        }                                                    \
    } while (0)

static int _main_loop(void *args)
{
    nv_kthread_q_t *q = (nv_kthread_q_t *)args;
    nv_kthread_q_item_t *q_item = NULL;
    unsigned long flags;

    while (1) {
        // Normally this thread is never interrupted. However,
        // down_interruptible (instead of down) is called here,
        // in order to avoid being classified as a potentially
        // hung task, by the kernel watchdog.
        while (down_interruptible(&q->q_sem))
            NVQ_WARN("Interrupted during semaphore wait\n");

        if (atomic_read(&q->main_loop_should_exit))
            break;

        spin_lock_irqsave(&q->q_lock, flags);

        // The q_sem semaphore prevents us from getting here unless there is
        // at least one item in the list, so an empty list indicates a bug.
        if (unlikely(list_empty(&q->q_list_head))) {
            spin_unlock_irqrestore(&q->q_lock, flags);
            NVQ_WARN("_main_loop: Empty queue: q: 0x%p\n", q);
            continue;
        }

        // Consume one item from the queue
        q_item = list_first_entry(&q->q_list_head,
                                   nv_kthread_q_item_t,
                                   q_list_node);

        list_del_init(&q_item->q_list_node);

        spin_unlock_irqrestore(&q->q_lock, flags);

        // Run the item
        q_item->function_to_run(q_item->function_args);

        // Make debugging a little simpler by clearing this between runs:
        q_item = NULL;
    }

    while (!kthread_should_stop())
        schedule();

    return 0;
}

void nv_kthread_q_stop(nv_kthread_q_t *q)
{
    // check if queue has been properly initialized
    if (unlikely(!q->q_kthread))
        return;

    nv_kthread_q_flush(q);

    // If this assertion fires, then a caller likely either broke the API rules,
    // by adding items after calling nv_kthread_q_stop, or possibly messed up
    // with inadequate flushing of self-rescheduling q_items.
    if (unlikely(!list_empty(&q->q_list_head)))
        NVQ_WARN("list not empty after flushing\n");

    if (likely(!atomic_read(&q->main_loop_should_exit))) {

        atomic_set(&q->main_loop_should_exit, 1);

        // Wake up the kthread so that it can see that it needs to stop:
        up(&q->q_sem);

        kthread_stop(q->q_kthread);
        q->q_kthread = NULL;
    }
}

// When CONFIG_VMAP_STACK is defined, the kernel thread stack allocator used by
// kthread_create_on_node relies on a 2 entry, per-core cache to minimize
// vmalloc invocations. The cache is NUMA-unaware, so when there is a hit, the
// stack location ends up being a function of the core assigned to the current
// thread, instead of being a function of the specified NUMA node. The cache was
// added to the kernel in commit ac496bf48d97f2503eaa353996a4dd5e4383eaf0
// ("fork: Optimize task creation by caching two thread stacks per CPU if
// CONFIG_VMAP_STACK=y")
//
// To work around the problematic cache, we create up to three kernel threads
//   -If the first thread's stack is resident on the preferred node, return this
//    thread.
//   -Otherwise, create a second thread. If its stack is resident on the
//    preferred node, stop the first thread and return this one.
//   -Otherwise, create a third thread. The stack allocator does not find a
//    cached stack, and so falls back to vmalloc, which takes the NUMA hint into
//    consideration. The first two threads are then stopped.
//
// When CONFIG_VMAP_STACK is not defined, the first kernel thread is returned.
//
// This function is never invoked when there is no NUMA preference (preferred
// node is NUMA_NO_NODE).
static struct task_struct *thread_create_on_node(int (*threadfn)(void *data),
                                                 nv_kthread_q_t *q,
                                                 int preferred_node,
                                                 const char *q_name)
{

    unsigned i, j;
    static const unsigned attempts = 3;
    struct task_struct *thread[3];

    for (i = 0;; i++) {
        struct page *stack;

        thread[i] = kthread_create_on_node(threadfn, q, preferred_node, q_name);

        if (unlikely(IS_ERR(thread[i]))) {

            // Instead of failing, pick the previous thread, even if its
            // stack is not allocated on the preferred node.
            if (i > 0)
                i--;

            break;
        }

        // vmalloc is not used to allocate the stack, so simply return the
        // thread, even if its stack may not be allocated on the preferred node
        if (!is_vmalloc_addr(thread[i]->stack))
            break;

        // Ran out of attempts - return thread even if its stack may not be
        // allocated on the preferred node
        if ((i == (attempts - 1)))
            break;

        // Get the NUMA node where the first page of the stack is resident. If
        // it is the preferred node, select this thread.
        stack = vmalloc_to_page(thread[i]->stack);
        if (page_to_nid(stack) == preferred_node)
            break;
    }

    for (j = i; j > 0; j--)
        kthread_stop(thread[j - 1]);

    return thread[i];
}

int nv_kthread_q_init_on_node(nv_kthread_q_t *q, const char *q_name, int preferred_node)
{
    memset(q, 0, sizeof(*q));

    INIT_LIST_HEAD(&q->q_list_head);
    spin_lock_init(&q->q_lock);
    sema_init(&q->q_sem, 0);

    if (preferred_node == NV_KTHREAD_NO_NODE) {
        q->q_kthread = kthread_create(_main_loop, q, q_name);
    }
    else {
        q->q_kthread = thread_create_on_node(_main_loop, q, preferred_node, q_name);
    }

    if (IS_ERR(q->q_kthread)) {
        int err = PTR_ERR(q->q_kthread);

        // Clear q_kthread before returning so that nv_kthread_q_stop() can be
        // safely called on it making error handling easier.
        q->q_kthread = NULL;

        return err;
    }

    wake_up_process(q->q_kthread);

    return 0;
}

int nv_kthread_q_init(nv_kthread_q_t *q, const char *qname)
{
    return nv_kthread_q_init_on_node(q, qname, NV_KTHREAD_NO_NODE);
}

// Returns true (non-zero) if the item was actually scheduled, and false if the
// item was already pending in a queue.
static int _raw_q_schedule(nv_kthread_q_t *q, nv_kthread_q_item_t *q_item)
{
    unsigned long flags;
    int ret = 1;

    spin_lock_irqsave(&q->q_lock, flags);

    if (likely(list_empty(&q_item->q_list_node)))
        list_add_tail(&q_item->q_list_node, &q->q_list_head);
    else
        ret = 0;

    spin_unlock_irqrestore(&q->q_lock, flags);

    if (likely(ret))
        up(&q->q_sem);

    return ret;
}

void nv_kthread_q_item_init(nv_kthread_q_item_t *q_item,
                            nv_q_func_t function_to_run,
                            void *function_args)
{
    INIT_LIST_HEAD(&q_item->q_list_node);
    q_item->function_to_run = function_to_run;
    q_item->function_args   = function_args;
}

// Returns true (non-zero) if the q_item got scheduled, false otherwise.
int nv_kthread_q_schedule_q_item(nv_kthread_q_t *q,
                                 nv_kthread_q_item_t *q_item)
{
    if (unlikely(atomic_read(&q->main_loop_should_exit))) {
        NVQ_WARN("Not allowed: nv_kthread_q_schedule_q_item was "
                   "called with a non-alive q: 0x%p\n", q);
        return 0;
    }

    return _raw_q_schedule(q, q_item);
}

static void _q_flush_function(void *args)
{
    struct completion *completion = (struct completion *)args;
    complete(completion);
}


static void _raw_q_flush(nv_kthread_q_t *q)
{
    nv_kthread_q_item_t q_item;
    DECLARE_COMPLETION_ONSTACK(completion);

    nv_kthread_q_item_init(&q_item, _q_flush_function, &completion);

    _raw_q_schedule(q, &q_item);

    // Wait for the flush item to run. Once it has run, then all of the
    // previously queued items in front of it will have run, so that means
    // the flush is complete.
    wait_for_completion(&completion);
}

void nv_kthread_q_flush(nv_kthread_q_t *q)
{
    if (unlikely(atomic_read(&q->main_loop_should_exit))) {
        NVQ_WARN("Not allowed: nv_kthread_q_flush was called after "
                   "nv_kthread_q_stop. q: 0x%p\n", q);
        return;
    }

    // This 2x flush is not a typing mistake. The queue really does have to be
    // flushed twice, in order to take care of the case of a q_item that
    // reschedules itself.
    _raw_q_flush(q);
    _raw_q_flush(q);
}
