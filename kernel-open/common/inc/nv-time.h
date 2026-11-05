/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NV_TIME_H__
#define __NV_TIME_H__

#include "conftest.h"
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

#include <nvstatus.h>

#define NV_MAX_ISR_DELAY_US           20000
#define NV_MAX_ISR_DELAY_MS           (NV_MAX_ISR_DELAY_US / 1000)
#define NV_MSEC_TO_JIFFIES_FLOOR(ms)  ((NvU64)(ms) * HZ / MSEC_PER_SEC)

/*
 * in_hardirq() was added in v5.11-rc1 (2020-12-15) to replace in_irq().
 * Fall back to in_irq() for older kernels that don't have in_hardirq().
 */
static inline NvBool nv_in_hardirq(void)
{
#if defined(in_hardirq)
    return in_hardirq();
#else
    return in_irq();
#endif
}

static inline ktime_t nv_ktime_get_raw(void)
{
    return ktime_get_raw();
}

static inline NvU64 nv_ktime_get_raw_ns(void)
{
    return ktime_get_raw_ns();
}

/*
 * udelay() may overflow the counter on some platforms. mdelay() is a safe
 * wrapper to use.
 */
static inline void __nv_delay_usec(unsigned int usec)
{
    unsigned long msec = usec / USEC_PER_MSEC;

    usec %= USEC_PER_MSEC;

    if (msec)
        mdelay(msec);

    if (usec)
        udelay(usec);
}

static inline NV_STATUS nv_delay_us(unsigned int us)
{
    if (nv_in_hardirq() && (us > NV_MAX_ISR_DELAY_US))
        return NV_ERR_GENERIC;

    __nv_delay_usec(us);

    return NV_OK;
}

/*
 * Sleep for specified milliseconds. Yields the CPU to scheduler.
 *
 * On Linux, a jiffy represents the time passed between two timer interrupts.
 * The number of jiffies per second (HZ) varies across the supported
 * platforms.
 *
 * In atomic context it's just a busy-loop with mdelay(). In non-atomic context,
 * it may sleep with schedule_timeout().
 */
static inline NV_STATUS nv_sleep_ms(unsigned int ms)
{
    long jiffies_timeout;
    ktime_t end, delta;

    end = ktime_add_ms(nv_ktime_get_raw(), ms);

    if (nv_in_hardirq() && (ms > NV_MAX_ISR_DELAY_MS))
    {
        return NV_ERR_GENERIC;
    }

    if (irqs_disabled() || in_interrupt() || in_atomic())
    {
        mdelay(ms);
        return NV_OK;
    }

    /* Do we have a full jiffy to wait? */
    jiffies_timeout = NV_MSEC_TO_JIFFIES_FLOOR(ms);
    while (jiffies_timeout)
    {
        set_current_state(TASK_INTERRUPTIBLE);
        jiffies_timeout = schedule_timeout(jiffies_timeout);
        if (ktime_after(nv_ktime_get_raw(), end))
            return NV_OK;
    }

    delta = ktime_sub(end, nv_ktime_get_raw());
    if (ktime_to_ns(delta) <= 0)
        return NV_OK;
    __nv_delay_usec(ktime_to_us(delta));

    delta = ktime_sub(end, nv_ktime_get_raw());
    if (ktime_to_ns(delta) <= 0)
        return NV_OK;
    ndelay(ktime_to_ns(delta));

    return NV_OK;
}

#endif // __NV_TIME_H__
