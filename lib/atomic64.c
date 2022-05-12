// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Generic implementation of 64-bit atomics using spinlocks,
 * useful on processors that don't have 64-bit atomic instructions.
 *
 * Copyright © 2009 Paul Mackerras, IBM Corp. <paulus@au1.ibm.com>
 */
#include <linux/types.h>
#include <linux/cache.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/export.h>
#include <linux/atomic.h>

/*
 * We use a hashed array of spinlocks to provide exclusive access
 * to each atomic64_t variable.  Since this is expected to used on
 * systems with small numbers of CPUs (<= 4 or so), we use a
 * relatively small array of 16 spinlocks to avoid wasting too much
 * memory on the spinlock array.
 */
#define NR_LOCKS	16

/*
 * Ensure each lock is in a separate cacheline.
 */
static union {
	raw_spinlock_t lock;
	char pad[L1_CACHE_BYTES];
} atomic64_lock[NR_LOCKS] __cacheline_aligned_in_smp = {
	[0 ... (NR_LOCKS - 1)] = {
		.lock =  __RAW_SPIN_LOCK_UNLOCKED(atomic64_lock.lock),
	},
};

static inline raw_spinlock_t *lock_addr(const atomic64_t *v)
{
	unsigned long addr = (unsigned long) v;

	addr >>= L1_CACHE_SHIFT;
	addr ^= (addr >> 8) ^ (addr >> 16);
	return &atomic64_lock[addr & (NR_LOCKS - 1)].lock;
}

s64 atomic64_read(const atomic64_t *v)
{
	unsigned long flags;
	raw_spinlock_t *lock = lock_addr(v);
	s64 val;

	raw_spin_lock_irqsave(lock, flags);
	val = v->counter;
	raw_spin_unlock_irqrestore(lock, flags);
	return val;
}
EXPORT_SYMBOL(atomic64_read);

void atomic64_set(atomic64_t *v, s64 i)
{
	unsigned long flags;
	raw_spinlock_t *lock = lock_addr(v);

	raw_spin_lock_irqsave(lock, flags);
	v->counter = i;
	raw_spin_unlock_irqrestore(lock, flags);
}
EXPORT_SYMBOL(atomic64_set);

#define ATOMIC64_OP(op, c_op)						\
void atomic64_##op(s64 a, atomic64_t *v)				\
{									\
	unsigned long flags;						\
	raw_spinlock_t *lock = lock_addr(v);				\
									\
	raw_spin_lock_irqsave(lock, flags);				\
	v->counter c_op a;						\
	raw_spin_unlock_irqrestore(lock, flags);			\
}									\
EXPORT_SYMBOL(atomic64_##op);

#define ATOMIC64_OP_RETURN(op, c_op)					\
s64 atomic64_##op##_return(s64 a, atomic64_t *v)			\
{									\
	unsigned long flags;						\
	raw_spinlock_t *lock = lock_addr(v);				\
	s64 val;							\
									\
	raw_spin_lock_irqsave(lock, flags);				\
	val = (v->counter c_op a);					\
	raw_spin_unlock_irqrestore(lock, flags);			\
	return val;							\
}									\
EXPORT_SYMBOL(atomic64_##op##_return);

#define ATOMIC64_FETCH_OP(op, c_op)					\
s64 atomic64_fetch_##op(s64 a, atomic64_t *v)				\
{									\
	unsigned long flags;						\
	raw_spinlock_t *lock = lock_addr(v);				\
	s64 val;							\
									\
	raw_spin_lock_irqsave(lock, flags);				\
	val = v->counter;						\
	v->counter c_op a;						\
	raw_spin_unlock_irqrestore(lock, flags);			\
	return val;							\
}									\
EXPORT_SYMBOL(atomic64_fetch_##op);

#define ATOMIC64_OPS(op, c_op)						\
	ATOMIC64_OP(op, c_op)						\
	ATOMIC64_OP_RETURN(op, c_op)					\
	ATOMIC64_FETCH_OP(op, c_op)

ATOMIC64_OPS(add, +=)
ATOMIC64_OPS(sub, -=)

#undef ATOMIC64_OPS
#define ATOMIC64_OPS(op, c_op)						\
	ATOMIC64_OP(op, c_op)						\
	ATOMIC64_OP_RETURN(op, c_op)					\
	ATOMIC64_FETCH_OP(op, c_op)

ATOMIC64_OPS(and, &=)
ATOMIC64_OPS(or, |=)
ATOMIC64_OPS(xor, ^=)

#undef ATOMIC64_OPS
#undef ATOMIC64_FETCH_OP
#undef ATOMIC64_OP_RETURN
#undef ATOMIC64_OP

s64 atomic64_dec_if_positive(atomic64_t *v)
{
	unsigned long flags;
	raw_spinlock_t *lock = lock_addr(v);
	s64 val;

	raw_spin_lock_irqsave(lock, flags);
	val = v->counter - 1;
	if (val >= 0)
		v->counter = val;
	raw_spin_unlock_irqrestore(lock, flags);
	return val;
}
EXPORT_SYMBOL(atomic64_dec_if_positive);

s64 atomic64_cmpxchg(atomic64_t *v, s64 o, s64 n)
{
	unsigned long flags;
	raw_spinlock_t *lock = lock_addr(v);
	s64 val;

	raw_spin_lock_irqsave(lock, flags);
	val = v->counter;
	if (val == o)
		v->counter = n;
	raw_spin_unlock_irqrestore(lock, flags);
	return val;
}
EXPORT_SYMBOL(atomic64_cmpxchg);

s64 atomic64_xchg(atomic64_t *v, s64 new)
{
	unsigned long flags;
	raw_spinlock_t *lock = lock_addr(v);
	s64 val;

	raw_spin_lock_irqsave(lock, flags);
	val = v->counter;
	v->counter = new;
	raw_spin_unlock_irqrestore(lock, flags);
	return val;
}
EXPORT_SYMBOL(atomic64_xchg);

s64 atomic64_fetch_add_unless(atomic64_t *v, s64 a, s64 u)
{
	unsigned long flags;
	raw_spinlock_t *lock = lock_addr(v);
	s64 val;

	raw_spin_lock_irqsave(lock, flags);
	val = v->counter;
	if (val != u)
		v->counter += a;
	raw_spin_unlock_irqrestore(lock, flags);

	return val;
}
EXPORT_SYMBOL(atomic64_fetch_add_unless);
