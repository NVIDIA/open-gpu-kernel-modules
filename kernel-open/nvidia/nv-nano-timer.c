/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define  __NO_VERSION__

#include <linux/kernel.h> // For container_of
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/timer.h>
#include "os-interface.h"
#include "nv-linux.h"

#define NV_NANO_TIMER_USE_HRTIMER 1

struct nv_nano_timer
{
#if NV_NANO_TIMER_USE_HRTIMER
    struct hrtimer hr_timer; // This parameter holds linux high resolution timer object
                             // can get replaced with platform specific timer object
#else
    struct timer_list jiffy_timer;
#endif
    nv_linux_state_t *nv_linux_state;
    void (*nv_nano_timer_callback)(struct nv_nano_timer *nv_nstimer);
    void *pTmrEvent;
};

/*!
 * @brief runs nano second resolution timer callback 
*
 * @param[in] nv_nstimer    Pointer to nv_nano_timer_t object
 */
static void
nvidia_nano_timer_callback(
    nv_nano_timer_t *nv_nstimer)
{
    nv_state_t *nv = NULL;
    nv_linux_state_t *nvl = nv_nstimer->nv_linux_state;
    nvidia_stack_t *sp = NULL;

    if (nv_kmem_cache_alloc_stack_atomic(&sp) != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: no cache memory \n");
        return;
    }

    nv = NV_STATE_PTR(nvl);

    if (rm_run_nano_timer_callback(sp, nv, nv_nstimer->pTmrEvent) != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Error in service of callback \n");
    }

    nv_kmem_cache_free_stack(sp);
}

/*!
 * @brief Allocates nano second resolution  timer object
 *
 * @returns nv_nano_timer_t  allocated pointer
 */
static nv_nano_timer_t *nv_alloc_nano_timer(void)
{
    nv_nano_timer_t *nv_nstimer;

    NV_KMALLOC(nv_nstimer, sizeof(nv_nano_timer_t));

    if (nv_nstimer == NULL)
    {
        return NULL;
    }

    memset(nv_nstimer, 0, sizeof(nv_nano_timer_t));

    return nv_nstimer;
}

#if NV_NANO_TIMER_USE_HRTIMER
static enum hrtimer_restart nv_nano_timer_callback_typed_data(struct hrtimer *hrtmr)
{
    struct nv_nano_timer *nv_nstimer =
        container_of(hrtmr, struct nv_nano_timer, hr_timer);

    nv_nstimer->nv_nano_timer_callback(nv_nstimer);

    return HRTIMER_NORESTART;
}
#else
static inline void nv_jiffy_timer_callback_typed_data(struct timer_list *timer)
{
    struct nv_nano_timer *nv_nstimer =
        container_of(timer, struct nv_nano_timer, jiffy_timer);

    nv_nstimer->nv_nano_timer_callback(nv_nstimer);
}

static inline void nv_jiffy_timer_callback_anon_data(unsigned long arg)
{
    struct nv_nano_timer *nv_nstimer = (struct nv_nano_timer *)arg;

    nv_nstimer->nv_nano_timer_callback(nv_nstimer);
}
#endif

/*!
 * @brief Creates & initializes nano second resolution timer object
 *
 * @param[in] nv           Per gpu linux state
 * @param[in] tmrEvent     pointer to TMR_EVENT
 * @param[in] nv_nstimer    Pointer to nv_nano_timer_t object
 */
void NV_API_CALL nv_create_nano_timer(
    nv_state_t *nv,
    void *pTmrEvent,
    nv_nano_timer_t **pnv_nstimer)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nv_nano_timer_t *nv_nstimer = nv_alloc_nano_timer();

    if (nv_nstimer == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Not able to create timer object \n");
        *pnv_nstimer = NULL;
        return;
    }

    nv_nstimer->nv_linux_state = nvl;
    nv_nstimer->pTmrEvent = pTmrEvent;

    nv_nstimer->nv_nano_timer_callback = nvidia_nano_timer_callback;

#if NV_NANO_TIMER_USE_HRTIMER
#if NV_IS_EXPORT_SYMBOL_PRESENT_hrtimer_setup
    hrtimer_setup(&nv_nstimer->hr_timer, &nv_nano_timer_callback_typed_data,
                  CLOCK_MONOTONIC, HRTIMER_MODE_REL);
#else
    hrtimer_init(&nv_nstimer->hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    nv_nstimer->hr_timer.function = nv_nano_timer_callback_typed_data;
#endif // NV_IS_EXPORT_SYMBOL_PRESENT_hrtimer_setup
#else
#if defined(NV_TIMER_SETUP_PRESENT)
    timer_setup(&nv_nstimer->jiffy_timer, nv_jiffy_timer_callback_typed_data, 0);
#else
    init_timer(&nv_nstimer->jiffy_timer);
    nv_nstimer->jiffy_timer.function = nv_jiffy_timer_callback_anon_data;
    nv_nstimer->jiffy_timer.data = (unsigned long)nv_nstimer;
#endif // NV_TIMER_SETUP_PRESENT
#endif // NV_NANO_TIMER_USE_HRTIMER

    *pnv_nstimer = nv_nstimer;
}

/*!
 * @brief Starts nano second resolution timer
 *
 * @param[in] nv           Per gpu linux state
 * @param[in] nv_nstimer   Pointer to nv_nano_timer_t object
 * @param[in] time_ns      Relative time in nano seconds
 */
void NV_API_CALL nv_start_nano_timer(
    nv_state_t *nv,
    nv_nano_timer_t *nv_nstimer,
    NvU64 time_ns)
{
#if NV_NANO_TIMER_USE_HRTIMER
    ktime_t ktime = ktime_set(0, time_ns);
    hrtimer_start(&nv_nstimer->hr_timer, ktime, HRTIMER_MODE_REL);
#else
    unsigned long time_jiffies;
    NvU32 time_us;

    time_us = (NvU32)(time_ns / 1000);
    time_jiffies = usecs_to_jiffies(time_us);
    mod_timer(&nv_nstimer->jiffy_timer, jiffies + time_jiffies);
#endif
}

/*!
 * @brief Cancels nano second resolution timer
 *
 * @param[in] nv           Per gpu linux state
 * @param[in] nv_nstimer   Pointer to nv_nano_timer_t object
 */
void NV_API_CALL nv_cancel_nano_timer(
    nv_state_t *nv,
    nv_nano_timer_t *nv_nstimer)
{
#if NV_NANO_TIMER_USE_HRTIMER
    hrtimer_cancel(&nv_nstimer->hr_timer);
#else
    nv_timer_delete_sync(&nv_nstimer->jiffy_timer);
#endif

}

/*!
 * @brief Cancels & deletes nano second resolution timer object
 *
 * @param[in] nv           Per gpu linux state
 * @param[in] nv_nstimer   Pointer to nv_nano_timer_t object
 */
void NV_API_CALL nv_destroy_nano_timer(
    nv_state_t *nv,
    nv_nano_timer_t *nv_nstimer)
{
    nv_cancel_nano_timer(nv, nv_nstimer);
    NV_KFREE(nv_nstimer, sizeof(nv_nano_timer_t));
}
