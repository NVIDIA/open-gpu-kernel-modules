/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2001, 2002, MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 * Copyright (c) 2003  Maciej W. Rozycki
 *
 * include/asm-mips/time.h
 *     header file for the new style time.c file and time services.
 */
#ifndef _ASM_TIME_H
#define _ASM_TIME_H

#include <linux/rtc.h>
#include <linux/spinlock.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>

extern spinlock_t rtc_lock;

/*
 * board specific routines required by time_init().
 */
extern void plat_time_init(void);

/*
 * mips_hpt_frequency - must be set if you intend to use an R4k-compatible
 * counter as a timer interrupt source.
 */
extern unsigned int mips_hpt_frequency;

/*
 * The performance counter IRQ on MIPS is a close relative to the timer IRQ
 * so it lives here.
 */
extern int (*perf_irq)(void);
extern int __weak get_c0_perfcount_int(void);

/*
 * Initialize the calling CPU's compare interrupt as clockevent device
 */
extern unsigned int get_c0_compare_int(void);
extern int r4k_clockevent_init(void);

static inline int mips_clockevent_init(void)
{
#ifdef CONFIG_CEVT_R4K
	return r4k_clockevent_init();
#else
	return -ENXIO;
#endif
}

/*
 * Initialize the count register as a clocksource
 */
extern int init_r4k_clocksource(void);

static inline int init_mips_clocksource(void)
{
#ifdef CONFIG_CSRC_R4K
	return init_r4k_clocksource();
#else
	return 0;
#endif
}

static inline void clockevent_set_clock(struct clock_event_device *cd,
					unsigned int clock)
{
	clockevents_calc_mult_shift(cd, clock, 4);
}

#endif /* _ASM_TIME_H */
