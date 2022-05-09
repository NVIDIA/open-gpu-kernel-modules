/*
 * SPDX-FileCopyrightText: Copyright (c) 2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_KTHREAD_QUEUE_H__
#define __NV_KTHREAD_QUEUE_H__

#include <linux/types.h>            // atomic_t
#include <linux/list.h>             // list
#include <linux/sched.h>            // task_struct
#include <linux/numa.h>             // NUMA_NO_NODE

#include "conftest.h"

#if defined(NV_LINUX_SEMAPHORE_H_PRESENT)
    #include <linux/semaphore.h>
#else
    #include <asm/semaphore.h>
#endif

////////////////////////////////////////////////////////////////////////////////
// nv_kthread_q:
//
// 1. API and overview
//
//    This "nv_kthread_q" system implements a simple queuing system for deferred
//    work. The nv_kthread_q system has goals and use cases that are similar to
//    the named workqueues in the Linux kernel, but nv_kthread_q is much (10x or
//    so) smaller, simpler--and correspondingly less general. Deferred work
//    items are put into a queue, and run within the context of a dedicated set
//    of kernel threads (kthread).
//
//    In order to avoid confusion with the Linux workqueue system, I have
//    avoided using the term "work", and instead refer to "queues" (also called
//    "q's") and "queue items" (also called "q_items"), in both variable names
//    and comments.
//
//    This module depends only upon the Linux kernel.
//
//    Queue items that are submitted to separate nv_kthread_q instances are
//    guaranteed to be run in different kthreads.
//
//    Queue items that are submitted to the same nv_kthread_q are not guaranteed
//    to be serialized, nor are they guaranteed to run concurrently.
//
// 2. Allocations
//
//    The caller allocates queues and queue items. The nv_kthread_q APIs do
//    the initialization (zeroing and setup) of queues and queue items.
//    Allocation is handled that way, because one of the first use cases is a
//    bottom half interrupt handler, and for that, queue items should be
//    pre-allocated (for example, one per GPU), so that no allocation is
//    required in the top-half interrupt handler. Relevant API calls:
//
// 3. Queue initialization
//
//    nv_kthread_q_init() initializes a queue on the current NUMA node.
//
//    or
//
//    nv_kthread_q_init_on_node() initializes a queue on a specific NUMA node.
//
// 3. Scheduling things for the queue to run
//
//    The nv_kthread_q_schedule_q_item() routine will schedule a q_item to run.
//
// 4. Stopping the queue(s)
//
//    The nv_kthread_q_stop() routine will flush the queue, and safely stop
//    the kthread, before returning.
//
////////////////////////////////////////////////////////////////////////////////

typedef struct nv_kthread_q nv_kthread_q_t;
typedef struct nv_kthread_q_item nv_kthread_q_item_t;

typedef void (*nv_q_func_t)(void *args);

struct nv_kthread_q
{
    struct list_head q_list_head;
    spinlock_t q_lock;

    // This is a counting semaphore. It gets incremented and decremented
    // exactly once for each item that is added to the queue.
    struct semaphore q_sem;
    atomic_t main_loop_should_exit;

    struct task_struct *q_kthread;
};

struct nv_kthread_q_item
{
    struct list_head q_list_node;
    nv_q_func_t function_to_run;
    void *function_args;
};

#if defined(NV_KTHREAD_CREATE_ON_NODE_PRESENT)
    #define NV_KTHREAD_Q_SUPPORTS_AFFINITY() 1
#else
    #define NV_KTHREAD_Q_SUPPORTS_AFFINITY() 0
#endif

#ifndef NUMA_NO_NODE
#define NUMA_NO_NODE (-1)
#endif

#define NV_KTHREAD_NO_NODE NUMA_NO_NODE

//
// The queue must not be used before calling this routine.
//
// The caller allocates an nv_kthread_q_t item. This routine initializes
// the queue, and starts up a kernel thread ("kthread") to service the queue.
// The queue will initially be empty; there is intentionally no way to
// pre-initialize the queue with items to run.
//
// In order to avoid external dependencies (specifically, NV_STATUS codes), this
// returns a Linux kernel (negative) errno on failure, and zero on success. It
// is safe to call nv_kthread_q_stop() on a queue that nv_kthread_q_init()
// failed for.
//
// A short prefix of the qname arg will show up in []'s, via the ps(1) utility.
//
// The kernel thread stack is preferably allocated on the specified NUMA node if
// NUMA-affinity (NV_KTHREAD_Q_SUPPORTS_AFFINITY() == 1) is supported, but
// fallback to another node is possible because kernel allocators do not
// guarantee affinity. Note that NUMA-affinity applies only to
// the kthread stack. This API does not do anything about limiting the CPU
// affinity of the kthread. That is left to the caller.
//
// On kernels, which do not support NUMA-aware kthread stack allocations
// (NV_KTHTREAD_Q_SUPPORTS_AFFINITY() == 0), the API will return -ENOTSUPP
// if the value supplied for 'preferred_node' is anything other than
// NV_KTHREAD_NO_NODE.
//
// Reusing a queue: once a queue is initialized, it must be safely shut down
// (see "Stopping the queue(s)", below), before it can be reused. So, for
// a simple queue use case, the following will work:
//
//     nv_kthread_q_init_on_node(&some_q, "display_name", preferred_node);
//     nv_kthread_q_stop(&some_q);
//     nv_kthread_q_init_on_node(&some_q, "reincarnated", preferred_node);
//     nv_kthread_q_stop(&some_q);
//
int nv_kthread_q_init_on_node(nv_kthread_q_t *q,
                              const char *qname,
                              int preferred_node);

//
// This routine is the same as nv_kthread_q_init_on_node() with the exception
// that the queue stack will be allocated on the NUMA node of the caller.
//
static inline int nv_kthread_q_init(nv_kthread_q_t *q, const char *qname)
{
    return nv_kthread_q_init_on_node(q, qname, NV_KTHREAD_NO_NODE);
}

//
// The caller is responsible for stopping all queues, by calling this routine
// before, for example, kernel module unloading. This nv_kthread_q_stop()
// routine will flush the queue, and safely stop the kthread, before returning.
//
// You may ONLY call nv_kthread_q_stop() once, unless you reinitialize the
// queue in between, as shown in the nv_kthread_q_init() documentation, above.
//
// Do not add any more items to the queue after calling nv_kthread_q_stop.
//
// Calling nv_kthread_q_stop() on a queue which has been zero-initialized or
// for which nv_kthread_q_init() failed, is a no-op.
//
void nv_kthread_q_stop(nv_kthread_q_t *q);

//
// All items that were in the queue before nv_kthread_q_flush was called, and
// all items scheduled by those items, will get run before this function
// returns.
//
// You may NOT call nv_kthread_q_flush() after having called nv_kthread_q_stop.
//
// This actually flushes the queue twice. That ensures that the queue is fully
// flushed, for an important use case: rescheduling from within one's own
// callback. In order to do that safely, you need to:
//
//     -- set a flag that tells the callback to stop rescheduling itself.
//
//     -- call either nv_kthread_q_flush or nv_kthread_q_stop (which internally
//        calls nv_kthread_q_flush). The nv_kthread_q_flush, in turn, actually
//        flushes the queue *twice*. The first flush waits for any callbacks
//        to finish, that missed seeing the "stop_rescheduling" flag. The
//        second flush waits for callbacks that were already scheduled when the
//        first flush finished.
//
void nv_kthread_q_flush(nv_kthread_q_t *q);

// Assigns function_to_run and function_args to the q_item.
//
// This must be called before calling nv_kthread_q_schedule_q_item.
void nv_kthread_q_item_init(nv_kthread_q_item_t *q_item,
                            nv_q_func_t function_to_run,
                            void *function_args);

//
// The caller must have already set up the queue, via nv_kthread_q_init().
// The caller owns the lifetime of the q_item. The nv_kthread_q system runs
// q_items, and adds or removes them from the queue. However, due to the first
// law of q-dynamics, it neither creates nor destroys q_items.
//
// When the callback (the function_to_run argument) is actually run, it is OK
// to free the q_item from within that routine. The nv_kthread_q system
// promises to be done with the q_item before that point.
//
// nv_kthread_q_schedule_q_item may be called from multiple threads at once,
// without danger of corrupting anything. This routine may also be safely
// called from interrupt context, including top-half ISRs.
//
// It is OK to reschedule the same q_item from within its own callback function.
//
// It is also OK to attempt to reschedule the same q_item, if that q_item is
// already pending in the queue. The q_item will not be rescheduled if it is
// already pending.
//
// Returns true (non-zero) if the item was actually scheduled. Returns false if
// the item was not scheduled, which can happen if:
//
//     -- The q_item was already pending in a queue, or
//     -- The queue is shutting down (or not yet started up).
//
int nv_kthread_q_schedule_q_item(nv_kthread_q_t *q,
                                 nv_kthread_q_item_t *q_item);

// Built-in test. Returns -1 if any subtest failed, or 0 upon success.
int nv_kthread_q_run_self_test(void);

#endif // __NV_KTHREAD_QUEUE_H__
