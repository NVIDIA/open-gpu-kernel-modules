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
#ifndef __NV_TIMER_H__
#define __NV_TIMER_H__

#include <linux/timer.h>
#include <linux/kernel.h> // For container_of

#include "conftest.h"

struct nv_timer
{
    struct timer_list kernel_timer;
    void (*nv_timer_callback)(struct nv_timer *nv_timer);
};

static inline void nv_timer_callback_typed_data(struct timer_list *timer)
{
    struct nv_timer *nv_timer =
        container_of(timer, struct nv_timer, kernel_timer);

    nv_timer->nv_timer_callback(nv_timer);
}

static inline void nv_timer_callback_anon_data(unsigned long arg)
{
    struct nv_timer *nv_timer = (struct nv_timer *)arg;

    nv_timer->nv_timer_callback(nv_timer);
}

static inline void nv_timer_setup(struct nv_timer *nv_timer,
                                  void (*callback)(struct nv_timer *nv_timer))
{
    nv_timer->nv_timer_callback = callback;

#if defined(NV_TIMER_SETUP_PRESENT)
    timer_setup(&nv_timer->kernel_timer, nv_timer_callback_typed_data, 0);
#else
    init_timer(&nv_timer->kernel_timer);
    nv_timer->kernel_timer.function = nv_timer_callback_anon_data;
    nv_timer->kernel_timer.data = (unsigned long)nv_timer;
#endif
}

#endif // __NV_TIMER_H__
