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

#ifndef __NV_KTHREAD_QUEUE_OS_H__
#define __NV_KTHREAD_QUEUE_OS_H__

#include <linux/types.h>            // atomic_t
#include <linux/list.h>             // list
#include <linux/sched.h>            // task_struct
#include <linux/numa.h>             // NUMA_NO_NODE
#include <linux/semaphore.h>

#include "conftest.h"

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


#ifndef NUMA_NO_NODE
#define NUMA_NO_NODE (-1)
#endif

#define NV_KTHREAD_NO_NODE NUMA_NO_NODE

#endif
