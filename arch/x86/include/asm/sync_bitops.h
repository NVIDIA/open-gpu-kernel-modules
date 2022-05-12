/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_SYNC_BITOPS_H
#define _ASM_X86_SYNC_BITOPS_H

/*
 * Copyright 1992, Linus Torvalds.
 */

/*
 * These have to be done with inline assembly: that way the bit-setting
 * is guaranteed to be atomic. All bit operations return 0 if the bit
 * was cleared before the operation and != 0 if it was not.
 *
 * bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 */

#include <asm/rmwcc.h>

#define ADDR (*(volatile long *)addr)

/**
 * sync_set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 *
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static inline void sync_set_bit(long nr, volatile unsigned long *addr)
{
	asm volatile("lock; " __ASM_SIZE(bts) " %1,%0"
		     : "+m" (ADDR)
		     : "Ir" (nr)
		     : "memory");
}

/**
 * sync_clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * sync_clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_atomic() and/or smp_mb__after_atomic()
 * in order to ensure changes are visible on other processors.
 */
static inline void sync_clear_bit(long nr, volatile unsigned long *addr)
{
	asm volatile("lock; " __ASM_SIZE(btr) " %1,%0"
		     : "+m" (ADDR)
		     : "Ir" (nr)
		     : "memory");
}

/**
 * sync_change_bit - Toggle a bit in memory
 * @nr: Bit to change
 * @addr: Address to start counting from
 *
 * sync_change_bit() is atomic and may not be reordered.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static inline void sync_change_bit(long nr, volatile unsigned long *addr)
{
	asm volatile("lock; " __ASM_SIZE(btc) " %1,%0"
		     : "+m" (ADDR)
		     : "Ir" (nr)
		     : "memory");
}

/**
 * sync_test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static inline bool sync_test_and_set_bit(long nr, volatile unsigned long *addr)
{
	return GEN_BINARY_RMWcc("lock; " __ASM_SIZE(bts), *addr, c, "Ir", nr);
}

/**
 * sync_test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static inline int sync_test_and_clear_bit(long nr, volatile unsigned long *addr)
{
	return GEN_BINARY_RMWcc("lock; " __ASM_SIZE(btr), *addr, c, "Ir", nr);
}

/**
 * sync_test_and_change_bit - Change a bit and return its old value
 * @nr: Bit to change
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static inline int sync_test_and_change_bit(long nr, volatile unsigned long *addr)
{
	return GEN_BINARY_RMWcc("lock; " __ASM_SIZE(btc), *addr, c, "Ir", nr);
}

#define sync_test_bit(nr, addr) test_bit(nr, addr)

#undef ADDR

#endif /* _ASM_X86_SYNC_BITOPS_H */
