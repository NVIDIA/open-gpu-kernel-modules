/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_JIFFIES_H
#define _LINUX_JIFFIES_H

#include <linux/cache.h>
#include <linux/limits.h>
#include <linux/math64.h>
#include <linux/minmax.h>
#include <linux/types.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <vdso/jiffies.h>
#include <asm/param.h>			/* for HZ */
#include <generated/timeconst.h>

/*
 * The following defines establish the engineering parameters of the PLL
 * model. The HZ variable establishes the timer interrupt frequency, 100 Hz
 * for the SunOS kernel, 256 Hz for the Ultrix kernel and 1024 Hz for the
 * OSF/1 kernel. The SHIFT_HZ define expresses the same value as the
 * nearest power of two in order to avoid hardware multiply operations.
 */
#if HZ >= 12 && HZ < 24
# define SHIFT_HZ	4
#elif HZ >= 24 && HZ < 48
# define SHIFT_HZ	5
#elif HZ >= 48 && HZ < 96
# define SHIFT_HZ	6
#elif HZ >= 96 && HZ < 192
# define SHIFT_HZ	7
#elif HZ >= 192 && HZ < 384
# define SHIFT_HZ	8
#elif HZ >= 384 && HZ < 768
# define SHIFT_HZ	9
#elif HZ >= 768 && HZ < 1536
# define SHIFT_HZ	10
#elif HZ >= 1536 && HZ < 3072
# define SHIFT_HZ	11
#elif HZ >= 3072 && HZ < 6144
# define SHIFT_HZ	12
#elif HZ >= 6144 && HZ < 12288
# define SHIFT_HZ	13
#else
# error Invalid value of HZ.
#endif

/* Suppose we want to divide two numbers NOM and DEN: NOM/DEN, then we can
 * improve accuracy by shifting LSH bits, hence calculating:
 *     (NOM << LSH) / DEN
 * This however means trouble for large NOM, because (NOM << LSH) may no
 * longer fit in 32 bits. The following way of calculating this gives us
 * some slack, under the following conditions:
 *   - (NOM / DEN) fits in (32 - LSH) bits.
 *   - (NOM % DEN) fits in (32 - LSH) bits.
 */
#define SH_DIV(NOM,DEN,LSH) (   (((NOM) / (DEN)) << (LSH))              \
                             + ((((NOM) % (DEN)) << (LSH)) + (DEN) / 2) / (DEN))

/* LATCH is used in the interval timer and ftape setup. */
#define LATCH ((CLOCK_TICK_RATE + HZ/2) / HZ)	/* For divider */

extern int register_refined_jiffies(long clock_tick_rate);

/* TICK_USEC is the time between ticks in usec assuming SHIFTED_HZ */
#define TICK_USEC ((USEC_PER_SEC + HZ/2) / HZ)

/* USER_TICK_USEC is the time between ticks in usec assuming fake USER_HZ */
#define USER_TICK_USEC ((1000000UL + USER_HZ/2) / USER_HZ)

#ifndef __jiffy_arch_data
#define __jiffy_arch_data
#endif

/*
 * The 64-bit value is not atomic - you MUST NOT read it
 * without sampling the sequence number in jiffies_lock.
 * get_jiffies_64() will do this for you as appropriate.
 */
extern u64 __cacheline_aligned_in_smp jiffies_64;
extern unsigned long volatile __cacheline_aligned_in_smp __jiffy_arch_data jiffies;

#if (BITS_PER_LONG < 64)
u64 get_jiffies_64(void);
#else
static inline u64 get_jiffies_64(void)
{
	return (u64)jiffies;
}
#endif

/*
 *	These inlines deal with timer wrapping correctly. You are 
 *	strongly encouraged to use them
 *	1. Because people otherwise forget
 *	2. Because if the timer wrap changes in future you won't have to
 *	   alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
#define time_after(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)((b) - (a)) < 0))
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)((a) - (b)) >= 0))
#define time_before_eq(a,b)	time_after_eq(b,a)

/*
 * Calculate whether a is in the range of [b, c].
 */
#define time_in_range(a,b,c) \
	(time_after_eq(a,b) && \
	 time_before_eq(a,c))

/*
 * Calculate whether a is in the range of [b, c).
 */
#define time_in_range_open(a,b,c) \
	(time_after_eq(a,b) && \
	 time_before(a,c))

/* Same as above, but does so with platform independent 64bit types.
 * These must be used when utilizing jiffies_64 (i.e. return value of
 * get_jiffies_64() */
#define time_after64(a,b)	\
	(typecheck(__u64, a) &&	\
	 typecheck(__u64, b) && \
	 ((__s64)((b) - (a)) < 0))
#define time_before64(a,b)	time_after64(b,a)

#define time_after_eq64(a,b)	\
	(typecheck(__u64, a) && \
	 typecheck(__u64, b) && \
	 ((__s64)((a) - (b)) >= 0))
#define time_before_eq64(a,b)	time_after_eq64(b,a)

#define time_in_range64(a, b, c) \
	(time_after_eq64(a, b) && \
	 time_before_eq64(a, c))

/*
 * These four macros compare jiffies and 'a' for convenience.
 */

/* time_is_before_jiffies(a) return true if a is before jiffies */
#define time_is_before_jiffies(a) time_after(jiffies, a)
#define time_is_before_jiffies64(a) time_after64(get_jiffies_64(), a)

/* time_is_after_jiffies(a) return true if a is after jiffies */
#define time_is_after_jiffies(a) time_before(jiffies, a)
#define time_is_after_jiffies64(a) time_before64(get_jiffies_64(), a)

/* time_is_before_eq_jiffies(a) return true if a is before or equal to jiffies*/
#define time_is_before_eq_jiffies(a) time_after_eq(jiffies, a)
#define time_is_before_eq_jiffies64(a) time_after_eq64(get_jiffies_64(), a)

/* time_is_after_eq_jiffies(a) return true if a is after or equal to jiffies*/
#define time_is_after_eq_jiffies(a) time_before_eq(jiffies, a)
#define time_is_after_eq_jiffies64(a) time_before_eq64(get_jiffies_64(), a)

/*
 * Have the 32 bit jiffies value wrap 5 minutes after boot
 * so jiffies wrap bugs show up earlier.
 */
#define INITIAL_JIFFIES ((unsigned long)(unsigned int) (-300*HZ))

/*
 * Change timeval to jiffies, trying to avoid the
 * most obvious overflows..
 *
 * And some not so obvious.
 *
 * Note that we don't want to return LONG_MAX, because
 * for various timeout reasons we often end up having
 * to wait "jiffies+1" in order to guarantee that we wait
 * at _least_ "jiffies" - so "jiffies+1" had better still
 * be positive.
 */
#define MAX_JIFFY_OFFSET ((LONG_MAX >> 1)-1)

extern unsigned long preset_lpj;

/*
 * We want to do realistic conversions of time so we need to use the same
 * values the update wall clock code uses as the jiffies size.  This value
 * is: TICK_NSEC (which is defined in timex.h).  This
 * is a constant and is in nanoseconds.  We will use scaled math
 * with a set of scales defined here as SEC_JIFFIE_SC,  USEC_JIFFIE_SC and
 * NSEC_JIFFIE_SC.  Note that these defines contain nothing but
 * constants and so are computed at compile time.  SHIFT_HZ (computed in
 * timex.h) adjusts the scaling for different HZ values.

 * Scaled math???  What is that?
 *
 * Scaled math is a way to do integer math on values that would,
 * otherwise, either overflow, underflow, or cause undesired div
 * instructions to appear in the execution path.  In short, we "scale"
 * up the operands so they take more bits (more precision, less
 * underflow), do the desired operation and then "scale" the result back
 * by the same amount.  If we do the scaling by shifting we avoid the
 * costly mpy and the dastardly div instructions.

 * Suppose, for example, we want to convert from seconds to jiffies
 * where jiffies is defined in nanoseconds as NSEC_PER_JIFFIE.  The
 * simple math is: jiff = (sec * NSEC_PER_SEC) / NSEC_PER_JIFFIE; We
 * observe that (NSEC_PER_SEC / NSEC_PER_JIFFIE) is a constant which we
 * might calculate at compile time, however, the result will only have
 * about 3-4 bits of precision (less for smaller values of HZ).
 *
 * So, we scale as follows:
 * jiff = (sec) * (NSEC_PER_SEC / NSEC_PER_JIFFIE);
 * jiff = ((sec) * ((NSEC_PER_SEC * SCALE)/ NSEC_PER_JIFFIE)) / SCALE;
 * Then we make SCALE a power of two so:
 * jiff = ((sec) * ((NSEC_PER_SEC << SCALE)/ NSEC_PER_JIFFIE)) >> SCALE;
 * Now we define:
 * #define SEC_CONV = ((NSEC_PER_SEC << SCALE)/ NSEC_PER_JIFFIE))
 * jiff = (sec * SEC_CONV) >> SCALE;
 *
 * Often the math we use will expand beyond 32-bits so we tell C how to
 * do this and pass the 64-bit result of the mpy through the ">> SCALE"
 * which should take the result back to 32-bits.  We want this expansion
 * to capture as much precision as possible.  At the same time we don't
 * want to overflow so we pick the SCALE to avoid this.  In this file,
 * that means using a different scale for each range of HZ values (as
 * defined in timex.h).
 *
 * For those who want to know, gcc will give a 64-bit result from a "*"
 * operator if the result is a long long AND at least one of the
 * operands is cast to long long (usually just prior to the "*" so as
 * not to confuse it into thinking it really has a 64-bit operand,
 * which, buy the way, it can do, but it takes more code and at least 2
 * mpys).

 * We also need to be aware that one second in nanoseconds is only a
 * couple of bits away from overflowing a 32-bit word, so we MUST use
 * 64-bits to get the full range time in nanoseconds.

 */

/*
 * Here are the scales we will use.  One for seconds, nanoseconds and
 * microseconds.
 *
 * Within the limits of cpp we do a rough cut at the SEC_JIFFIE_SC and
 * check if the sign bit is set.  If not, we bump the shift count by 1.
 * (Gets an extra bit of precision where we can use it.)
 * We know it is set for HZ = 1024 and HZ = 100 not for 1000.
 * Haven't tested others.

 * Limits of cpp (for #if expressions) only long (no long long), but
 * then we only need the most signicant bit.
 */

#define SEC_JIFFIE_SC (31 - SHIFT_HZ)
#if !((((NSEC_PER_SEC << 2) / TICK_NSEC) << (SEC_JIFFIE_SC - 2)) & 0x80000000)
#undef SEC_JIFFIE_SC
#define SEC_JIFFIE_SC (32 - SHIFT_HZ)
#endif
#define NSEC_JIFFIE_SC (SEC_JIFFIE_SC + 29)
#define SEC_CONVERSION ((unsigned long)((((u64)NSEC_PER_SEC << SEC_JIFFIE_SC) +\
                                TICK_NSEC -1) / (u64)TICK_NSEC))

#define NSEC_CONVERSION ((unsigned long)((((u64)1 << NSEC_JIFFIE_SC) +\
                                        TICK_NSEC -1) / (u64)TICK_NSEC))
/*
 * The maximum jiffie value is (MAX_INT >> 1).  Here we translate that
 * into seconds.  The 64-bit case will overflow if we are not careful,
 * so use the messy SH_DIV macro to do it.  Still all constants.
 */
#if BITS_PER_LONG < 64
# define MAX_SEC_IN_JIFFIES \
	(long)((u64)((u64)MAX_JIFFY_OFFSET * TICK_NSEC) / NSEC_PER_SEC)
#else	/* take care of overflow on 64 bits machines */
# define MAX_SEC_IN_JIFFIES \
	(SH_DIV((MAX_JIFFY_OFFSET >> SEC_JIFFIE_SC) * TICK_NSEC, NSEC_PER_SEC, 1) - 1)

#endif

/*
 * Convert various time units to each other:
 */
extern unsigned int jiffies_to_msecs(const unsigned long j);
extern unsigned int jiffies_to_usecs(const unsigned long j);

static inline u64 jiffies_to_nsecs(const unsigned long j)
{
	return (u64)jiffies_to_usecs(j) * NSEC_PER_USEC;
}

extern u64 jiffies64_to_nsecs(u64 j);
extern u64 jiffies64_to_msecs(u64 j);

extern unsigned long __msecs_to_jiffies(const unsigned int m);
#if HZ <= MSEC_PER_SEC && !(MSEC_PER_SEC % HZ)
/*
 * HZ is equal to or smaller than 1000, and 1000 is a nice round
 * multiple of HZ, divide with the factor between them, but round
 * upwards:
 */
static inline unsigned long _msecs_to_jiffies(const unsigned int m)
{
	return (m + (MSEC_PER_SEC / HZ) - 1) / (MSEC_PER_SEC / HZ);
}
#elif HZ > MSEC_PER_SEC && !(HZ % MSEC_PER_SEC)
/*
 * HZ is larger than 1000, and HZ is a nice round multiple of 1000 -
 * simply multiply with the factor between them.
 *
 * But first make sure the multiplication result cannot overflow:
 */
static inline unsigned long _msecs_to_jiffies(const unsigned int m)
{
	if (m > jiffies_to_msecs(MAX_JIFFY_OFFSET))
		return MAX_JIFFY_OFFSET;
	return m * (HZ / MSEC_PER_SEC);
}
#else
/*
 * Generic case - multiply, round and divide. But first check that if
 * we are doing a net multiplication, that we wouldn't overflow:
 */
static inline unsigned long _msecs_to_jiffies(const unsigned int m)
{
	if (HZ > MSEC_PER_SEC && m > jiffies_to_msecs(MAX_JIFFY_OFFSET))
		return MAX_JIFFY_OFFSET;

	return (MSEC_TO_HZ_MUL32 * m + MSEC_TO_HZ_ADJ32) >> MSEC_TO_HZ_SHR32;
}
#endif
/**
 * msecs_to_jiffies: - convert milliseconds to jiffies
 * @m:	time in milliseconds
 *
 * conversion is done as follows:
 *
 * - negative values mean 'infinite timeout' (MAX_JIFFY_OFFSET)
 *
 * - 'too large' values [that would result in larger than
 *   MAX_JIFFY_OFFSET values] mean 'infinite timeout' too.
 *
 * - all other values are converted to jiffies by either multiplying
 *   the input value by a factor or dividing it with a factor and
 *   handling any 32-bit overflows.
 *   for the details see __msecs_to_jiffies()
 *
 * msecs_to_jiffies() checks for the passed in value being a constant
 * via __builtin_constant_p() allowing gcc to eliminate most of the
 * code, __msecs_to_jiffies() is called if the value passed does not
 * allow constant folding and the actual conversion must be done at
 * runtime.
 * the HZ range specific helpers _msecs_to_jiffies() are called both
 * directly here and from __msecs_to_jiffies() in the case where
 * constant folding is not possible.
 */
static __always_inline unsigned long msecs_to_jiffies(const unsigned int m)
{
	if (__builtin_constant_p(m)) {
		if ((int)m < 0)
			return MAX_JIFFY_OFFSET;
		return _msecs_to_jiffies(m);
	} else {
		return __msecs_to_jiffies(m);
	}
}

extern unsigned long __usecs_to_jiffies(const unsigned int u);
#if !(USEC_PER_SEC % HZ)
static inline unsigned long _usecs_to_jiffies(const unsigned int u)
{
	return (u + (USEC_PER_SEC / HZ) - 1) / (USEC_PER_SEC / HZ);
}
#else
static inline unsigned long _usecs_to_jiffies(const unsigned int u)
{
	return (USEC_TO_HZ_MUL32 * u + USEC_TO_HZ_ADJ32)
		>> USEC_TO_HZ_SHR32;
}
#endif

/**
 * usecs_to_jiffies: - convert microseconds to jiffies
 * @u:	time in microseconds
 *
 * conversion is done as follows:
 *
 * - 'too large' values [that would result in larger than
 *   MAX_JIFFY_OFFSET values] mean 'infinite timeout' too.
 *
 * - all other values are converted to jiffies by either multiplying
 *   the input value by a factor or dividing it with a factor and
 *   handling any 32-bit overflows as for msecs_to_jiffies.
 *
 * usecs_to_jiffies() checks for the passed in value being a constant
 * via __builtin_constant_p() allowing gcc to eliminate most of the
 * code, __usecs_to_jiffies() is called if the value passed does not
 * allow constant folding and the actual conversion must be done at
 * runtime.
 * the HZ range specific helpers _usecs_to_jiffies() are called both
 * directly here and from __msecs_to_jiffies() in the case where
 * constant folding is not possible.
 */
static __always_inline unsigned long usecs_to_jiffies(const unsigned int u)
{
	if (__builtin_constant_p(u)) {
		if (u > jiffies_to_usecs(MAX_JIFFY_OFFSET))
			return MAX_JIFFY_OFFSET;
		return _usecs_to_jiffies(u);
	} else {
		return __usecs_to_jiffies(u);
	}
}

extern unsigned long timespec64_to_jiffies(const struct timespec64 *value);
extern void jiffies_to_timespec64(const unsigned long jiffies,
				  struct timespec64 *value);
extern clock_t jiffies_to_clock_t(unsigned long x);
static inline clock_t jiffies_delta_to_clock_t(long delta)
{
	return jiffies_to_clock_t(max(0L, delta));
}

static inline unsigned int jiffies_delta_to_msecs(long delta)
{
	return jiffies_to_msecs(max(0L, delta));
}

extern unsigned long clock_t_to_jiffies(unsigned long x);
extern u64 jiffies_64_to_clock_t(u64 x);
extern u64 nsec_to_clock_t(u64 x);
extern u64 nsecs_to_jiffies64(u64 n);
extern unsigned long nsecs_to_jiffies(u64 n);

#define TIMESTAMP_SIZE	30

#endif
