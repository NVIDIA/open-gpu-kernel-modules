/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _TIMEKEEPING_INTERNAL_H
#define _TIMEKEEPING_INTERNAL_H

#include <linux/clocksource.h>
#include <linux/spinlock.h>
#include <linux/time.h>

/*
 * timekeeping debug functions
 */
#ifdef CONFIG_DEBUG_FS
extern void tk_debug_account_sleep_time(const struct timespec64 *t);
#else
#define tk_debug_account_sleep_time(x)
#endif

#ifdef CONFIG_CLOCKSOURCE_VALIDATE_LAST_CYCLE
static inline u64 clocksource_delta(u64 now, u64 last, u64 mask)
{
	u64 ret = (now - last) & mask;

	/*
	 * Prevent time going backwards by checking the MSB of mask in
	 * the result. If set, return 0.
	 */
	return ret & ~(mask >> 1) ? 0 : ret;
}
#else
static inline u64 clocksource_delta(u64 now, u64 last, u64 mask)
{
	return (now - last) & mask;
}
#endif

/* Semi public for serialization of non timekeeper VDSO updates. */
extern raw_spinlock_t timekeeper_lock;

#endif /* _TIMEKEEPING_INTERNAL_H */
