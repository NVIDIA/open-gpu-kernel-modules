/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KCL_LINUX_TIMEKEEPING_H
#define _KCL_LINUX_TIMEKEEPING_H
#include <linux/ktime.h>

#ifndef HAVE_KTIME_GET_NS
static inline u64 ktime_get_ns(void)
{
	return ktime_to_ns(ktime_get());
}
#endif

#if !defined(HAVE_KTIME_GET_BOOTTIME_NS)
#if defined(HAVE_KTIME_GET_NS)
static inline u64 ktime_get_boottime_ns(void)
{
	return ktime_get_boot_ns();
}
#else
static inline u64 ktime_get_boottime_ns(void)
{
	struct timespec time;

	get_monotonic_boottime(&time);
	return (u64)timespec_to_ns(&time);
}
#endif /* HAVE_KTIME_GET_NS */
#endif /* HAVE_KTIME_GET_BOOTTIME_NS */

#if !defined(HAVE_KTIME_GET_RAW_NS)
static inline u64 ktime_get_raw_ns(void)
{
	struct timespec time;

	getrawmonotonic(&time);
	return (u64)timespec_to_ns(&time);
}
#endif

#ifndef HAVE_KTIME_GET_REAL_SECONDS
static inline time64_t ktime_get_real_seconds(void)
{
	struct timeval ts;

	do_gettimeofday(&ts);
	return (time64_t)ts.tv_sec;
}
#endif

#if !defined(HAVE_KTIME_GET_MONO_FAST_NS)
static inline u64 ktime_get_mono_fast_ns(void)
{
	return ktime_to_ns(ktime_get());
}
#endif

#ifndef HAVE_JIFFIES64_TO_MSECS
extern u64 jiffies64_to_msecs(u64 j);
#endif

#endif
