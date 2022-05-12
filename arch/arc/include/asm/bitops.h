/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 */

#ifndef _ASM_BITOPS_H
#define _ASM_BITOPS_H

#ifndef _LINUX_BITOPS_H
#error only <linux/bitops.h> can be included directly
#endif

#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <linux/compiler.h>
#include <asm/barrier.h>
#ifndef CONFIG_ARC_HAS_LLSC
#include <asm/smp.h>
#endif

#ifdef CONFIG_ARC_HAS_LLSC

/*
 * Hardware assisted Atomic-R-M-W
 */

#define BIT_OP(op, c_op, asm_op)					\
static inline void op##_bit(unsigned long nr, volatile unsigned long *m)\
{									\
	unsigned int temp;						\
									\
	m += nr >> 5;							\
									\
	nr &= 0x1f;							\
									\
	__asm__ __volatile__(						\
	"1:	llock       %0, [%1]		\n"			\
	"	" #asm_op " %0, %0, %2	\n"				\
	"	scond       %0, [%1]		\n"			\
	"	bnz         1b			\n"			\
	: "=&r"(temp)	/* Early clobber, to prevent reg reuse */	\
	: "r"(m),	/* Not "m": llock only supports reg direct addr mode */	\
	  "ir"(nr)							\
	: "cc");							\
}

/*
 * Semantically:
 *    Test the bit
 *    if clear
 *        set it and return 0 (old value)
 *    else
 *        return 1 (old value).
 *
 * Since ARC lacks a equivalent h/w primitive, the bit is set unconditionally
 * and the old value of bit is returned
 */
#define TEST_N_BIT_OP(op, c_op, asm_op)					\
static inline int test_and_##op##_bit(unsigned long nr, volatile unsigned long *m)\
{									\
	unsigned long old, temp;					\
									\
	m += nr >> 5;							\
									\
	nr &= 0x1f;							\
									\
	/*								\
	 * Explicit full memory barrier needed before/after as		\
	 * LLOCK/SCOND themselves don't provide any such smenatic	\
	 */								\
	smp_mb();							\
									\
	__asm__ __volatile__(						\
	"1:	llock       %0, [%2]	\n"				\
	"	" #asm_op " %1, %0, %3	\n"				\
	"	scond       %1, [%2]	\n"				\
	"	bnz         1b		\n"				\
	: "=&r"(old), "=&r"(temp)					\
	: "r"(m), "ir"(nr)						\
	: "cc");							\
									\
	smp_mb();							\
									\
	return (old & (1 << nr)) != 0;					\
}

#else /* !CONFIG_ARC_HAS_LLSC */

/*
 * Non hardware assisted Atomic-R-M-W
 * Locking would change to irq-disabling only (UP) and spinlocks (SMP)
 *
 * There's "significant" micro-optimization in writing our own variants of
 * bitops (over generic variants)
 *
 * (1) The generic APIs have "signed" @nr while we have it "unsigned"
 *     This avoids extra code to be generated for pointer arithmatic, since
 *     is "not sure" that index is NOT -ve
 * (2) Utilize the fact that ARCompact bit fidding insn (BSET/BCLR/ASL) etc
 *     only consider bottom 5 bits of @nr, so NO need to mask them off.
 *     (GCC Quirk: however for constant @nr we still need to do the masking
 *             at compile time)
 */

#define BIT_OP(op, c_op, asm_op)					\
static inline void op##_bit(unsigned long nr, volatile unsigned long *m)\
{									\
	unsigned long temp, flags;					\
	m += nr >> 5;							\
									\
	/*								\
	 * spin lock/unlock provide the needed smp_mb() before/after	\
	 */								\
	bitops_lock(flags);						\
									\
	temp = *m;							\
	*m = temp c_op (1UL << (nr & 0x1f));					\
									\
	bitops_unlock(flags);						\
}

#define TEST_N_BIT_OP(op, c_op, asm_op)					\
static inline int test_and_##op##_bit(unsigned long nr, volatile unsigned long *m)\
{									\
	unsigned long old, flags;					\
	m += nr >> 5;							\
									\
	bitops_lock(flags);						\
									\
	old = *m;							\
	*m = old c_op (1UL << (nr & 0x1f));				\
									\
	bitops_unlock(flags);						\
									\
	return (old & (1UL << (nr & 0x1f))) != 0;			\
}

#endif

/***************************************
 * Non atomic variants
 **************************************/

#define __BIT_OP(op, c_op, asm_op)					\
static inline void __##op##_bit(unsigned long nr, volatile unsigned long *m)	\
{									\
	unsigned long temp;						\
	m += nr >> 5;							\
									\
	temp = *m;							\
	*m = temp c_op (1UL << (nr & 0x1f));				\
}

#define __TEST_N_BIT_OP(op, c_op, asm_op)				\
static inline int __test_and_##op##_bit(unsigned long nr, volatile unsigned long *m)\
{									\
	unsigned long old;						\
	m += nr >> 5;							\
									\
	old = *m;							\
	*m = old c_op (1UL << (nr & 0x1f));				\
									\
	return (old & (1UL << (nr & 0x1f))) != 0;			\
}

#define BIT_OPS(op, c_op, asm_op)					\
									\
	/* set_bit(), clear_bit(), change_bit() */			\
	BIT_OP(op, c_op, asm_op)					\
									\
	/* test_and_set_bit(), test_and_clear_bit(), test_and_change_bit() */\
	TEST_N_BIT_OP(op, c_op, asm_op)					\
									\
	/* __set_bit(), __clear_bit(), __change_bit() */		\
	__BIT_OP(op, c_op, asm_op)					\
									\
	/* __test_and_set_bit(), __test_and_clear_bit(), __test_and_change_bit() */\
	__TEST_N_BIT_OP(op, c_op, asm_op)

BIT_OPS(set, |, bset)
BIT_OPS(clear, & ~, bclr)
BIT_OPS(change, ^, bxor)

/*
 * This routine doesn't need to be atomic.
 */
static inline int
test_bit(unsigned int nr, const volatile unsigned long *addr)
{
	unsigned long mask;

	addr += nr >> 5;

	mask = 1UL << (nr & 0x1f);

	return ((mask & *addr) != 0);
}

#ifdef CONFIG_ISA_ARCOMPACT

/*
 * Count the number of zeros, starting from MSB
 * Helper for fls( ) friends
 * This is a pure count, so (1-32) or (0-31) doesn't apply
 * It could be 0 to 32, based on num of 0's in there
 * clz(0x8000_0000) = 0, clz(0xFFFF_FFFF)=0, clz(0) = 32, clz(1) = 31
 */
static inline __attribute__ ((const)) int clz(unsigned int x)
{
	unsigned int res;

	__asm__ __volatile__(
	"	norm.f  %0, %1		\n"
	"	mov.n   %0, 0		\n"
	"	add.p   %0, %0, 1	\n"
	: "=r"(res)
	: "r"(x)
	: "cc");

	return res;
}

static inline int constant_fls(unsigned int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u))
		r -= 1;
	return r;
}

/*
 * fls = Find Last Set in word
 * @result: [1-32]
 * fls(1) = 1, fls(0x80000000) = 32, fls(0) = 0
 */
static inline __attribute__ ((const)) int fls(unsigned int x)
{
	if (__builtin_constant_p(x))
	       return constant_fls(x);

	return 32 - clz(x);
}

/*
 * __fls: Similar to fls, but zero based (0-31)
 */
static inline __attribute__ ((const)) int __fls(unsigned long x)
{
	if (!x)
		return 0;
	else
		return fls(x) - 1;
}

/*
 * ffs = Find First Set in word (LSB to MSB)
 * @result: [1-32], 0 if all 0's
 */
#define ffs(x)	({ unsigned long __t = (x); fls(__t & -__t); })

/*
 * __ffs: Similar to ffs, but zero based (0-31)
 */
static inline __attribute__ ((const)) unsigned long __ffs(unsigned long word)
{
	if (!word)
		return word;

	return ffs(word) - 1;
}

#else	/* CONFIG_ISA_ARCV2 */

/*
 * fls = Find Last Set in word
 * @result: [1-32]
 * fls(1) = 1, fls(0x80000000) = 32, fls(0) = 0
 */
static inline __attribute__ ((const)) int fls(unsigned long x)
{
	int n;

	asm volatile(
	"	fls.f	%0, %1		\n"  /* 0:31; 0(Z) if src 0 */
	"	add.nz	%0, %0, 1	\n"  /* 0:31 -> 1:32 */
	: "=r"(n)	/* Early clobber not needed */
	: "r"(x)
	: "cc");

	return n;
}

/*
 * __fls: Similar to fls, but zero based (0-31). Also 0 if no bit set
 */
static inline __attribute__ ((const)) int __fls(unsigned long x)
{
	/* FLS insn has exactly same semantics as the API */
	return	__builtin_arc_fls(x);
}

/*
 * ffs = Find First Set in word (LSB to MSB)
 * @result: [1-32], 0 if all 0's
 */
static inline __attribute__ ((const)) int ffs(unsigned long x)
{
	int n;

	asm volatile(
	"	ffs.f	%0, %1		\n"  /* 0:31; 31(Z) if src 0 */
	"	add.nz	%0, %0, 1	\n"  /* 0:31 -> 1:32 */
	"	mov.z	%0, 0		\n"  /* 31(Z)-> 0 */
	: "=r"(n)	/* Early clobber not needed */
	: "r"(x)
	: "cc");

	return n;
}

/*
 * __ffs: Similar to ffs, but zero based (0-31)
 */
static inline __attribute__ ((const)) unsigned long __ffs(unsigned long x)
{
	unsigned long n;

	asm volatile(
	"	ffs.f	%0, %1		\n"  /* 0:31; 31(Z) if src 0 */
	"	mov.z	%0, 0		\n"  /* 31(Z)-> 0 */
	: "=r"(n)
	: "r"(x)
	: "cc");

	return n;

}

#endif	/* CONFIG_ISA_ARCOMPACT */

/*
 * ffz = Find First Zero in word.
 * @return:[0-31], 32 if all 1's
 */
#define ffz(x)	__ffs(~(x))

#include <asm-generic/bitops/hweight.h>
#include <asm-generic/bitops/fls64.h>
#include <asm-generic/bitops/sched.h>
#include <asm-generic/bitops/lock.h>

#include <asm-generic/bitops/find.h>
#include <asm-generic/bitops/le.h>
#include <asm-generic/bitops/ext2-atomic-setbit.h>

#endif /* !__ASSEMBLY__ */

#endif
