/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * A stand-alone ticket spinlock implementation for use by the non-VHE
 * KVM hypervisor code running at EL2.
 *
 * Copyright (C) 2020 Google LLC
 * Author: Will Deacon <will@kernel.org>
 *
 * Heavily based on the implementation removed by c11090474d70 which was:
 * Copyright (C) 2012 ARM Ltd.
 */

#ifndef __ARM64_KVM_NVHE_SPINLOCK_H__
#define __ARM64_KVM_NVHE_SPINLOCK_H__

#include <asm/alternative.h>
#include <asm/lse.h>

typedef union hyp_spinlock {
	u32	__val;
	struct {
#ifdef __AARCH64EB__
		u16 next, owner;
#else
		u16 owner, next;
#endif
	};
} hyp_spinlock_t;

#define hyp_spin_lock_init(l)						\
do {									\
	*(l) = (hyp_spinlock_t){ .__val = 0 };				\
} while (0)

static inline void hyp_spin_lock(hyp_spinlock_t *lock)
{
	u32 tmp;
	hyp_spinlock_t lockval, newval;

	asm volatile(
	/* Atomically increment the next ticket. */
	ARM64_LSE_ATOMIC_INSN(
	/* LL/SC */
"	prfm	pstl1strm, %3\n"
"1:	ldaxr	%w0, %3\n"
"	add	%w1, %w0, #(1 << 16)\n"
"	stxr	%w2, %w1, %3\n"
"	cbnz	%w2, 1b\n",
	/* LSE atomics */
"	mov	%w2, #(1 << 16)\n"
"	ldadda	%w2, %w0, %3\n"
	__nops(3))

	/* Did we get the lock? */
"	eor	%w1, %w0, %w0, ror #16\n"
"	cbz	%w1, 3f\n"
	/*
	 * No: spin on the owner. Send a local event to avoid missing an
	 * unlock before the exclusive load.
	 */
"	sevl\n"
"2:	wfe\n"
"	ldaxrh	%w2, %4\n"
"	eor	%w1, %w2, %w0, lsr #16\n"
"	cbnz	%w1, 2b\n"
	/* We got the lock. Critical section starts here. */
"3:"
	: "=&r" (lockval), "=&r" (newval), "=&r" (tmp), "+Q" (*lock)
	: "Q" (lock->owner)
	: "memory");
}

static inline void hyp_spin_unlock(hyp_spinlock_t *lock)
{
	u64 tmp;

	asm volatile(
	ARM64_LSE_ATOMIC_INSN(
	/* LL/SC */
	"	ldrh	%w1, %0\n"
	"	add	%w1, %w1, #1\n"
	"	stlrh	%w1, %0",
	/* LSE atomics */
	"	mov	%w1, #1\n"
	"	staddlh	%w1, %0\n"
	__nops(1))
	: "=Q" (lock->owner), "=&r" (tmp)
	:
	: "memory");
}

#endif /* __ARM64_KVM_NVHE_SPINLOCK_H__ */
