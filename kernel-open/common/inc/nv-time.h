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
#include <linux/ktime.h>

#include <nvstatus.h>

#define NV_MAX_ISR_DELAY_US           20000
#define NV_MAX_ISR_DELAY_MS           (NV_MAX_ISR_DELAY_US / 1000)
#define NV_NSECS_TO_JIFFIES(nsec)     ((nsec) * HZ / 1000000000)

#if !defined(NV_TIMESPEC64_PRESENT)
struct timespec64 {
    __s64 tv_sec;
    long  tv_nsec;
};
#endif

#if !defined(NV_KTIME_GET_RAW_TS64_PRESENT)
static inline void ktime_get_raw_ts64(struct timespec64 *ts64)
{
    struct timespec ts;
    getrawmonotonic(&ts);
    ts64->tv_sec = ts.tv_sec;
    ts64->tv_nsec = ts.tv_nsec;
}
#endif

#if !defined(NV_KTIME_GET_REAL_TS64_PRESENT)
static inline void ktime_get_real_ts64(struct timespec64 *ts64)
{
    struct timeval tv;
    do_gettimeofday(&tv);
    ts64->tv_sec = tv.tv_sec;
    ts64->tv_nsec = tv.tv_usec * (NvU64) NSEC_PER_USEC;
}
#endif

static NvBool nv_timer_less_than
(
    const struct timespec64 *a,
    const struct timespec64 *b
)
{
    return (a->tv_sec == b->tv_sec) ? (a->tv_nsec < b->tv_nsec)
                                    : (a->tv_sec < b->tv_sec);
}

#if !defined(NV_TIMESPEC64_PRESENT)
static inline struct timespec64 timespec64_add
(
    const struct timespec64    a,
    const struct timespec64    b
)
{
    struct timespec64 result;

    result.tv_sec = a.tv_sec + b.tv_sec;
    result.tv_nsec = a.tv_nsec + b.tv_nsec;
    while (result.tv_nsec >= NSEC_PER_SEC)
    {
        ++result.tv_sec;
        result.tv_nsec -= NSEC_PER_SEC;
    }
    return result;
}

static inline struct timespec64  timespec64_sub
(
    const struct timespec64    a,
    const struct timespec64    b
)
{
    struct timespec64 result;

    result.tv_sec = a.tv_sec - b.tv_sec;
    result.tv_nsec = a.tv_nsec - b.tv_nsec;
    while (result.tv_nsec < 0)
    {
        --(result.tv_sec);
        result.tv_nsec += NSEC_PER_SEC;
    }
    return result;
}

static inline s64 timespec64_to_ns(struct timespec64 *ts)
{
    return ((s64) ts->tv_sec *  NSEC_PER_SEC) + ts->tv_nsec;
}
#endif

static inline NvU64 nv_ktime_get_raw_ns(void)
{
    struct timespec64 ts;
    ktime_get_raw_ts64(&ts);
    return (NvU64)timespec64_to_ns(&ts);
}

// #define NV_CHECK_DELAY_ACCURACY 1

/*
 * It is generally a bad idea to use udelay() to wait for more than
 * a few milliseconds. Since the caller is most likely not aware of
 * this, we use mdelay() for any full millisecond to be safe.
 */
static inline NV_STATUS nv_sleep_us(unsigned int us)
{

    unsigned long mdelay_safe_msec;
    unsigned long usec;

#ifdef NV_CHECK_DELAY_ACCURACY
    struct timespec64 tm1, tm2, tm_diff;

    ktime_get_raw_ts64(&tm1);
#endif

    if (in_irq() && (us > NV_MAX_ISR_DELAY_US))
        return NV_ERR_GENERIC;

    mdelay_safe_msec = us / 1000;
    if (mdelay_safe_msec)
        mdelay(mdelay_safe_msec);

    usec = us % 1000;
    if (usec)
        udelay(usec);

#ifdef NV_CHECK_DELAY_ACCURACY
    ktime_get_raw_ts64(&tm2);
    tm_diff = timespec64_sub(tm2, tm1);
    pr_info("NVRM: delay of %d usec results in actual delay of 0x%llu nsec\n",
             us, timespec64_to_ns(&tm_diff));
#endif
    return NV_OK;
}

/*
 * Sleep for specified milliseconds. Yields the CPU to scheduler.
 *
 * On Linux, a jiffie represents the time passed in between two timer
 * interrupts. The number of jiffies per second (HZ) varies across the
 * supported platforms. On i386, where HZ is 100, a timer interrupt is
 * generated every 10ms. NV_MSECS_TO_JIFFIES should be accurate independent of
 * the actual value of HZ; any partial jiffies will be 'floor'ed, the
 * remainder will be accounted for with mdelay().
 */
static inline NV_STATUS nv_sleep_ms(unsigned int ms)
{
    NvU64 ns;
    unsigned long jiffies;
    unsigned long mdelay_safe_msec;
    struct timespec64 tm_end, tm_aux;
#ifdef NV_CHECK_DELAY_ACCURACY
    struct timespec64 tm_start;
#endif

    ktime_get_raw_ts64(&tm_aux);
#ifdef NV_CHECK_DELAY_ACCURACY
    tm_start = tm_aux;
#endif

    if (in_irq() && (ms > NV_MAX_ISR_DELAY_MS))
    {
        return NV_ERR_GENERIC;
    }

    if (irqs_disabled() || in_interrupt() || in_atomic())
    {
        mdelay(ms);
        return NV_OK;
    }

    ns = ms * (NvU64) NSEC_PER_MSEC;
    tm_end.tv_nsec = ns;
    tm_end.tv_sec = 0;
    tm_end = timespec64_add(tm_aux, tm_end);

    /* do we have a full jiffie to wait? */
    jiffies = NV_NSECS_TO_JIFFIES(ns);

    if (jiffies)
    {
        //
        // If we have at least one full jiffy to wait, give up
        // up the CPU; since we may be rescheduled before
        // the requested timeout has expired, loop until less
        // than a jiffie of the desired delay remains.
        //
        set_current_state(TASK_INTERRUPTIBLE);
        do
        {
            schedule_timeout(jiffies);
            ktime_get_raw_ts64(&tm_aux);
            if (nv_timer_less_than(&tm_aux, &tm_end))
            {
                tm_aux = timespec64_sub(tm_end, tm_aux);
                ns = (NvU64) timespec64_to_ns(&tm_aux);
            }
            else
                ns = 0;
        } while ((jiffies = NV_NSECS_TO_JIFFIES(ns)) != 0);
    }

    if (ns > (NvU64) NSEC_PER_MSEC)
    {
        mdelay_safe_msec = ns / (NvU64) NSEC_PER_MSEC;
        mdelay(mdelay_safe_msec);
        ns %= (NvU64) NSEC_PER_MSEC;
    }
    if (ns)
    {
        ndelay(ns);
    }
#ifdef NV_CHECK_DELAY_ACCURACY
    ktime_get_raw_ts64(&tm_aux);
    tm_aux = timespec64_sub(tm_aux, tm_start);
    pr_info("NVRM: delay of %d msec results in actual delay of %lld.%09ld sec\n",
             ms, tm_aux.tv_sec, tm_aux.tv_nsec);
#endif
    return NV_OK;
}

#endif // __NV_TIME_H__
