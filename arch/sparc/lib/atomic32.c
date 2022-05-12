// SPDX-License-Identifier: GPL-2.0
/*
 * atomic32.c: 32-bit atomic_t implementation
 *
 * Copyright (C) 2004 Keith M Wesolowski
 * Copyright (C) 2007 Kyle McMartin
 * 
 * Based on asm-parisc/atomic.h Copyright (C) 2000 Philipp Rumpf
 */

#include <linux/atomic.h>
#include <linux/spinlock.h>
#include <linux/module.h>

#ifdef CONFIG_SMP
#define ATOMIC_HASH_SIZE	4
#define ATOMIC_HASH(a)	(&__atomic_hash[(((unsigned long)a)>>8) & (ATOMIC_HASH_SIZE-1)])

spinlock_t __atomic_hash[ATOMIC_HASH_SIZE] = {
	[0 ... (ATOMIC_HASH_SIZE-1)] = __SPIN_LOCK_UNLOCKED(__atomic_hash)
};

#else /* SMP */

static DEFINE_SPINLOCK(dummy);
#define ATOMIC_HASH_SIZE	1
#define ATOMIC_HASH(a)		(&dummy)

#endif /* SMP */

#define ATOMIC_FETCH_OP(op, c_op)					\
int atomic_fetch_##op(int i, atomic_t *v)				\
{									\
	int ret;							\
	unsigned long flags;						\
	spin_lock_irqsave(ATOMIC_HASH(v), flags);			\
									\
	ret = v->counter;						\
	v->counter c_op i;						\
									\
	spin_unlock_irqrestore(ATOMIC_HASH(v), flags);			\
	return ret;							\
}									\
EXPORT_SYMBOL(atomic_fetch_##op);

#define ATOMIC_OP_RETURN(op, c_op)					\
int atomic_##op##_return(int i, atomic_t *v)				\
{									\
	int ret;							\
	unsigned long flags;						\
	spin_lock_irqsave(ATOMIC_HASH(v), flags);			\
									\
	ret = (v->counter c_op i);					\
									\
	spin_unlock_irqrestore(ATOMIC_HASH(v), flags);			\
	return ret;							\
}									\
EXPORT_SYMBOL(atomic_##op##_return);

ATOMIC_OP_RETURN(add, +=)

ATOMIC_FETCH_OP(add, +=)
ATOMIC_FETCH_OP(and, &=)
ATOMIC_FETCH_OP(or, |=)
ATOMIC_FETCH_OP(xor, ^=)

#undef ATOMIC_FETCH_OP
#undef ATOMIC_OP_RETURN

int atomic_xchg(atomic_t *v, int new)
{
	int ret;
	unsigned long flags;

	spin_lock_irqsave(ATOMIC_HASH(v), flags);
	ret = v->counter;
	v->counter = new;
	spin_unlock_irqrestore(ATOMIC_HASH(v), flags);
	return ret;
}
EXPORT_SYMBOL(atomic_xchg);

int atomic_cmpxchg(atomic_t *v, int old, int new)
{
	int ret;
	unsigned long flags;

	spin_lock_irqsave(ATOMIC_HASH(v), flags);
	ret = v->counter;
	if (likely(ret == old))
		v->counter = new;

	spin_unlock_irqrestore(ATOMIC_HASH(v), flags);
	return ret;
}
EXPORT_SYMBOL(atomic_cmpxchg);

int atomic_fetch_add_unless(atomic_t *v, int a, int u)
{
	int ret;
	unsigned long flags;

	spin_lock_irqsave(ATOMIC_HASH(v), flags);
	ret = v->counter;
	if (ret != u)
		v->counter += a;
	spin_unlock_irqrestore(ATOMIC_HASH(v), flags);
	return ret;
}
EXPORT_SYMBOL(atomic_fetch_add_unless);

/* Atomic operations are already serializing */
void atomic_set(atomic_t *v, int i)
{
	unsigned long flags;

	spin_lock_irqsave(ATOMIC_HASH(v), flags);
	v->counter = i;
	spin_unlock_irqrestore(ATOMIC_HASH(v), flags);
}
EXPORT_SYMBOL(atomic_set);

unsigned long ___set_bit(unsigned long *addr, unsigned long mask)
{
	unsigned long old, flags;

	spin_lock_irqsave(ATOMIC_HASH(addr), flags);
	old = *addr;
	*addr = old | mask;
	spin_unlock_irqrestore(ATOMIC_HASH(addr), flags);

	return old & mask;
}
EXPORT_SYMBOL(___set_bit);

unsigned long ___clear_bit(unsigned long *addr, unsigned long mask)
{
	unsigned long old, flags;

	spin_lock_irqsave(ATOMIC_HASH(addr), flags);
	old = *addr;
	*addr = old & ~mask;
	spin_unlock_irqrestore(ATOMIC_HASH(addr), flags);

	return old & mask;
}
EXPORT_SYMBOL(___clear_bit);

unsigned long ___change_bit(unsigned long *addr, unsigned long mask)
{
	unsigned long old, flags;

	spin_lock_irqsave(ATOMIC_HASH(addr), flags);
	old = *addr;
	*addr = old ^ mask;
	spin_unlock_irqrestore(ATOMIC_HASH(addr), flags);

	return old & mask;
}
EXPORT_SYMBOL(___change_bit);

unsigned long __cmpxchg_u32(volatile u32 *ptr, u32 old, u32 new)
{
	unsigned long flags;
	u32 prev;

	spin_lock_irqsave(ATOMIC_HASH(ptr), flags);
	if ((prev = *ptr) == old)
		*ptr = new;
	spin_unlock_irqrestore(ATOMIC_HASH(ptr), flags);

	return (unsigned long)prev;
}
EXPORT_SYMBOL(__cmpxchg_u32);

u64 __cmpxchg_u64(u64 *ptr, u64 old, u64 new)
{
	unsigned long flags;
	u64 prev;

	spin_lock_irqsave(ATOMIC_HASH(ptr), flags);
	if ((prev = *ptr) == old)
		*ptr = new;
	spin_unlock_irqrestore(ATOMIC_HASH(ptr), flags);

	return prev;
}
EXPORT_SYMBOL(__cmpxchg_u64);

unsigned long __xchg_u32(volatile u32 *ptr, u32 new)
{
	unsigned long flags;
	u32 prev;

	spin_lock_irqsave(ATOMIC_HASH(ptr), flags);
	prev = *ptr;
	*ptr = new;
	spin_unlock_irqrestore(ATOMIC_HASH(ptr), flags);

	return (unsigned long)prev;
}
EXPORT_SYMBOL(__xchg_u32);
