/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_LOCK_H_
#define _NV_LOCK_H_

#include "conftest.h"

#include <linux/spinlock.h>
#include <linux/rwsem.h>
#include <linux/sched.h> /* signal_pending, cond_resched */

#if defined(NV_LINUX_SCHED_SIGNAL_H_PRESENT)
#include <linux/sched/signal.h>     /* signal_pending for kernels >= 4.11 */
#endif

#if defined(NV_LINUX_SEMAPHORE_H_PRESENT)
#include <linux/semaphore.h>
#else
#include <asm/semaphore.h>
#endif

#if defined(CONFIG_PREEMPT_RT) || defined(CONFIG_PREEMPT_RT_FULL)
typedef raw_spinlock_t            nv_spinlock_t;
#define NV_SPIN_LOCK_INIT(lock)   raw_spin_lock_init(lock)
#define NV_SPIN_LOCK_IRQ(lock)    raw_spin_lock_irq(lock)
#define NV_SPIN_UNLOCK_IRQ(lock)  raw_spin_unlock_irq(lock)
#define NV_SPIN_LOCK_IRQSAVE(lock,flags) raw_spin_lock_irqsave(lock,flags)
#define NV_SPIN_UNLOCK_IRQRESTORE(lock,flags) raw_spin_unlock_irqrestore(lock,flags)
#define NV_SPIN_LOCK(lock)        raw_spin_lock(lock)
#define NV_SPIN_UNLOCK(lock)      raw_spin_unlock(lock)
#define NV_SPIN_UNLOCK_WAIT(lock) raw_spin_unlock_wait(lock)
#else
typedef spinlock_t                nv_spinlock_t;
#define NV_SPIN_LOCK_INIT(lock)   spin_lock_init(lock)
#define NV_SPIN_LOCK_IRQ(lock)    spin_lock_irq(lock)
#define NV_SPIN_UNLOCK_IRQ(lock)  spin_unlock_irq(lock)
#define NV_SPIN_LOCK_IRQSAVE(lock,flags) spin_lock_irqsave(lock,flags)
#define NV_SPIN_UNLOCK_IRQRESTORE(lock,flags) spin_unlock_irqrestore(lock,flags)
#define NV_SPIN_LOCK(lock)        spin_lock(lock)
#define NV_SPIN_UNLOCK(lock)      spin_unlock(lock)
#define NV_SPIN_UNLOCK_WAIT(lock) spin_unlock_wait(lock)
#endif

#if defined(NV_CONFIG_PREEMPT_RT)
#define NV_INIT_SEMA(sema, val) sema_init(sema,val)
#else
#if !defined(__SEMAPHORE_INITIALIZER) && defined(__COMPAT_SEMAPHORE_INITIALIZER)
#define __SEMAPHORE_INITIALIZER __COMPAT_SEMAPHORE_INITIALIZER
#endif
#define NV_INIT_SEMA(sema, val)                    \
    {                                              \
        struct semaphore __sema =                  \
            __SEMAPHORE_INITIALIZER(*(sema), val); \
        *(sema) = __sema;                          \
    }
#endif
#define NV_INIT_MUTEX(mutex) NV_INIT_SEMA(mutex, 1)

static inline int nv_down_read_interruptible(struct rw_semaphore *lock)
{
    while (!down_read_trylock(lock))
    {
        if (signal_pending(current))
            return -EINTR;
        cond_resched();
    }
    return 0;
}


#endif  /* _NV_LOCK_H_ */
