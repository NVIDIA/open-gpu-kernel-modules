/* SPDX-License-Identifier: GPL-2.0 */
/*
 * linux/include/asm/timex.h
 *
 * sparc64 architecture timex specifications
 */
#ifndef _ASMsparc64_TIMEX_H
#define _ASMsparc64_TIMEX_H

#include <asm/timer.h>

#define CLOCK_TICK_RATE	1193180 /* Underlying HZ */

/* Getting on the cycle counter on sparc64. */
typedef unsigned long cycles_t;
#define get_cycles()	tick_ops->get_tick()

#define ARCH_HAS_READ_CURRENT_TIMER

#endif
