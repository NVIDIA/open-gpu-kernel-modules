/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  S390 version
 *    Copyright IBM Corp. 1999
 *
 *  Derived from "include/asm-i386/timex.h"
 *    Copyright (C) 1992, Linus Torvalds
 */

#ifndef _ASM_S390_TIMEX_H
#define _ASM_S390_TIMEX_H

#include <linux/preempt.h>
#include <linux/time64.h>
#include <asm/lowcore.h>

/* The value of the TOD clock for 1.1.1970. */
#define TOD_UNIX_EPOCH 0x7d91048bca000000ULL

extern u64 clock_comparator_max;

union tod_clock {
	__uint128_t val;
	struct {
		__uint128_t ei	:  8; /* epoch index */
		__uint128_t tod : 64; /* bits 0-63 of tod clock */
		__uint128_t	: 40;
		__uint128_t pf	: 16; /* programmable field */
	};
	struct {
		__uint128_t eitod : 72; /* epoch index + bits 0-63 tod clock */
		__uint128_t	  : 56;
	};
	struct {
		__uint128_t us	: 60; /* micro-seconds */
		__uint128_t sus	: 12; /* sub-microseconds */
		__uint128_t	: 56;
	};
} __packed;

/* Inline functions for clock register access. */
static inline int set_tod_clock(__u64 time)
{
	int cc;

	asm volatile(
		"   sck   %1\n"
		"   ipm   %0\n"
		"   srl   %0,28\n"
		: "=d" (cc) : "Q" (time) : "cc");
	return cc;
}

static inline int store_tod_clock_ext_cc(union tod_clock *clk)
{
	int cc;

	asm volatile(
		"   stcke  %1\n"
		"   ipm   %0\n"
		"   srl   %0,28\n"
		: "=d" (cc), "=Q" (*clk) : : "cc");
	return cc;
}

static inline void store_tod_clock_ext(union tod_clock *tod)
{
	asm volatile("stcke %0" : "=Q" (*tod) : : "cc");
}

static inline void set_clock_comparator(__u64 time)
{
	asm volatile("sckc %0" : : "Q" (time));
}

static inline void set_tod_programmable_field(u16 val)
{
	register unsigned long reg0 asm("0") = val;

	asm volatile("sckpf" : : "d" (reg0));
}

void clock_comparator_work(void);

void __init time_early_init(void);

extern unsigned char ptff_function_mask[16];

/* Function codes for the ptff instruction. */
#define PTFF_QAF	0x00	/* query available functions */
#define PTFF_QTO	0x01	/* query tod offset */
#define PTFF_QSI	0x02	/* query steering information */
#define PTFF_QUI	0x04	/* query UTC information */
#define PTFF_ATO	0x40	/* adjust tod offset */
#define PTFF_STO	0x41	/* set tod offset */
#define PTFF_SFS	0x42	/* set fine steering rate */
#define PTFF_SGS	0x43	/* set gross steering rate */

/* Query TOD offset result */
struct ptff_qto {
	unsigned long physical_clock;
	unsigned long tod_offset;
	unsigned long logical_tod_offset;
	unsigned long tod_epoch_difference;
} __packed;

static inline int ptff_query(unsigned int nr)
{
	unsigned char *ptr;

	ptr = ptff_function_mask + (nr >> 3);
	return (*ptr & (0x80 >> (nr & 7))) != 0;
}

/* Query UTC information result */
struct ptff_qui {
	unsigned int tm : 2;
	unsigned int ts : 2;
	unsigned int : 28;
	unsigned int pad_0x04;
	unsigned long leap_event;
	short old_leap;
	short new_leap;
	unsigned int pad_0x14;
	unsigned long prt[5];
	unsigned long cst[3];
	unsigned int skew;
	unsigned int pad_0x5c[41];
} __packed;

/*
 * ptff - Perform timing facility function
 * @ptff_block: Pointer to ptff parameter block
 * @len: Length of parameter block
 * @func: Function code
 * Returns: Condition code (0 on success)
 */
#define ptff(ptff_block, len, func)					\
({									\
	struct addrtype { char _[len]; };				\
	register unsigned int reg0 asm("0") = func;			\
	register unsigned long reg1 asm("1") = (unsigned long) (ptff_block);\
	int rc;								\
									\
	asm volatile(							\
		"	.word	0x0104\n"				\
		"	ipm	%0\n"					\
		"	srl	%0,28\n"				\
		: "=d" (rc), "+m" (*(struct addrtype *) reg1)		\
		: "d" (reg0), "d" (reg1) : "cc");			\
	rc;								\
})

static inline unsigned long local_tick_disable(void)
{
	unsigned long old;

	old = S390_lowcore.clock_comparator;
	S390_lowcore.clock_comparator = clock_comparator_max;
	set_clock_comparator(S390_lowcore.clock_comparator);
	return old;
}

static inline void local_tick_enable(unsigned long comp)
{
	S390_lowcore.clock_comparator = comp;
	set_clock_comparator(S390_lowcore.clock_comparator);
}

#define CLOCK_TICK_RATE		1193180 /* Underlying HZ */

typedef unsigned long cycles_t;

static inline unsigned long get_tod_clock(void)
{
	union tod_clock clk;

	store_tod_clock_ext(&clk);
	return clk.tod;
}

static inline unsigned long get_tod_clock_fast(void)
{
#ifdef CONFIG_HAVE_MARCH_Z9_109_FEATURES
	unsigned long clk;

	asm volatile("stckf %0" : "=Q" (clk) : : "cc");
	return clk;
#else
	return get_tod_clock();
#endif
}

static inline cycles_t get_cycles(void)
{
	return (cycles_t) get_tod_clock() >> 2;
}

int get_phys_clock(unsigned long *clock);
void init_cpu_timer(void);

extern union tod_clock tod_clock_base;

/**
 * get_clock_monotonic - returns current time in clock rate units
 *
 * The clock and tod_clock_base get changed via stop_machine.
 * Therefore preemption must be disabled, otherwise the returned
 * value is not guaranteed to be monotonic.
 */
static inline unsigned long get_tod_clock_monotonic(void)
{
	unsigned long tod;

	preempt_disable_notrace();
	tod = get_tod_clock() - tod_clock_base.tod;
	preempt_enable_notrace();
	return tod;
}

/**
 * tod_to_ns - convert a TOD format value to nanoseconds
 * @todval: to be converted TOD format value
 * Returns: number of nanoseconds that correspond to the TOD format value
 *
 * Converting a 64 Bit TOD format value to nanoseconds means that the value
 * must be divided by 4.096. In order to achieve that we multiply with 125
 * and divide by 512:
 *
 *    ns = (todval * 125) >> 9;
 *
 * In order to avoid an overflow with the multiplication we can rewrite this.
 * With a split todval == 2^9 * th + tl (th upper 55 bits, tl lower 9 bits)
 * we end up with
 *
 *    ns = ((2^9 * th + tl) * 125 ) >> 9;
 * -> ns = (th * 125) + ((tl * 125) >> 9);
 *
 */
static inline unsigned long tod_to_ns(unsigned long todval)
{
	return ((todval >> 9) * 125) + (((todval & 0x1ff) * 125) >> 9);
}

/**
 * tod_after - compare two 64 bit TOD values
 * @a: first 64 bit TOD timestamp
 * @b: second 64 bit TOD timestamp
 *
 * Returns: true if a is later than b
 */
static inline int tod_after(unsigned long a, unsigned long b)
{
	if (MACHINE_HAS_SCC)
		return (long) a > (long) b;
	return a > b;
}

/**
 * tod_after_eq - compare two 64 bit TOD values
 * @a: first 64 bit TOD timestamp
 * @b: second 64 bit TOD timestamp
 *
 * Returns: true if a is later than b
 */
static inline int tod_after_eq(unsigned long a, unsigned long b)
{
	if (MACHINE_HAS_SCC)
		return (long) a >= (long) b;
	return a >= b;
}

#endif
