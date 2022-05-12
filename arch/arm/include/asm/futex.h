/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_ARM_FUTEX_H
#define _ASM_ARM_FUTEX_H

#ifdef __KERNEL__

#include <linux/futex.h>
#include <linux/uaccess.h>
#include <asm/errno.h>

#define __futex_atomic_ex_table(err_reg)			\
	"3:\n"							\
	"	.pushsection __ex_table,\"a\"\n"		\
	"	.align	3\n"					\
	"	.long	1b, 4f, 2b, 4f\n"			\
	"	.popsection\n"					\
	"	.pushsection .text.fixup,\"ax\"\n"		\
	"	.align	2\n"					\
	"4:	mov	%0, " err_reg "\n"			\
	"	b	3b\n"					\
	"	.popsection"

#ifdef CONFIG_SMP

#define __futex_atomic_op(insn, ret, oldval, tmp, uaddr, oparg)	\
({								\
	unsigned int __ua_flags;				\
	smp_mb();						\
	prefetchw(uaddr);					\
	__ua_flags = uaccess_save_and_enable();			\
	__asm__ __volatile__(					\
	"1:	ldrex	%1, [%3]\n"				\
	"	" insn "\n"					\
	"2:	strex	%2, %0, [%3]\n"				\
	"	teq	%2, #0\n"				\
	"	bne	1b\n"					\
	"	mov	%0, #0\n"				\
	__futex_atomic_ex_table("%5")				\
	: "=&r" (ret), "=&r" (oldval), "=&r" (tmp)		\
	: "r" (uaddr), "r" (oparg), "Ir" (-EFAULT)		\
	: "cc", "memory");					\
	uaccess_restore(__ua_flags);				\
})

static inline int
futex_atomic_cmpxchg_inatomic(u32 *uval, u32 __user *uaddr,
			      u32 oldval, u32 newval)
{
	unsigned int __ua_flags;
	int ret;
	u32 val;

	if (!access_ok(uaddr, sizeof(u32)))
		return -EFAULT;

	smp_mb();
	/* Prefetching cannot fault */
	prefetchw(uaddr);
	__ua_flags = uaccess_save_and_enable();
	__asm__ __volatile__("@futex_atomic_cmpxchg_inatomic\n"
	"1:	ldrex	%1, [%4]\n"
	"	teq	%1, %2\n"
	"	ite	eq	@ explicit IT needed for the 2b label\n"
	"2:	strexeq	%0, %3, [%4]\n"
	"	movne	%0, #0\n"
	"	teq	%0, #0\n"
	"	bne	1b\n"
	__futex_atomic_ex_table("%5")
	: "=&r" (ret), "=&r" (val)
	: "r" (oldval), "r" (newval), "r" (uaddr), "Ir" (-EFAULT)
	: "cc", "memory");
	uaccess_restore(__ua_flags);
	smp_mb();

	*uval = val;
	return ret;
}

#else /* !SMP, we can work around lack of atomic ops by disabling preemption */

#include <linux/preempt.h>
#include <asm/domain.h>

#define __futex_atomic_op(insn, ret, oldval, tmp, uaddr, oparg)	\
({								\
	unsigned int __ua_flags = uaccess_save_and_enable();	\
	__asm__ __volatile__(					\
	"1:	" TUSER(ldr) "	%1, [%3]\n"			\
	"	" insn "\n"					\
	"2:	" TUSER(str) "	%0, [%3]\n"			\
	"	mov	%0, #0\n"				\
	__futex_atomic_ex_table("%5")				\
	: "=&r" (ret), "=&r" (oldval), "=&r" (tmp)		\
	: "r" (uaddr), "r" (oparg), "Ir" (-EFAULT)		\
	: "cc", "memory");					\
	uaccess_restore(__ua_flags);				\
})

static inline int
futex_atomic_cmpxchg_inatomic(u32 *uval, u32 __user *uaddr,
			      u32 oldval, u32 newval)
{
	unsigned int __ua_flags;
	int ret = 0;
	u32 val;

	if (!access_ok(uaddr, sizeof(u32)))
		return -EFAULT;

	preempt_disable();
	__ua_flags = uaccess_save_and_enable();
	__asm__ __volatile__("@futex_atomic_cmpxchg_inatomic\n"
	"	.syntax unified\n"
	"1:	" TUSER(ldr) "	%1, [%4]\n"
	"	teq	%1, %2\n"
	"	it	eq	@ explicit IT needed for the 2b label\n"
	"2:	" TUSERCOND(str, eq) "	%3, [%4]\n"
	__futex_atomic_ex_table("%5")
	: "+r" (ret), "=&r" (val)
	: "r" (oldval), "r" (newval), "r" (uaddr), "Ir" (-EFAULT)
	: "cc", "memory");
	uaccess_restore(__ua_flags);

	*uval = val;
	preempt_enable();

	return ret;
}

#endif /* !SMP */

static inline int
arch_futex_atomic_op_inuser(int op, int oparg, int *oval, u32 __user *uaddr)
{
	int oldval = 0, ret, tmp;

	if (!access_ok(uaddr, sizeof(u32)))
		return -EFAULT;

#ifndef CONFIG_SMP
	preempt_disable();
#endif

	switch (op) {
	case FUTEX_OP_SET:
		__futex_atomic_op("mov	%0, %4", ret, oldval, tmp, uaddr, oparg);
		break;
	case FUTEX_OP_ADD:
		__futex_atomic_op("add	%0, %1, %4", ret, oldval, tmp, uaddr, oparg);
		break;
	case FUTEX_OP_OR:
		__futex_atomic_op("orr	%0, %1, %4", ret, oldval, tmp, uaddr, oparg);
		break;
	case FUTEX_OP_ANDN:
		__futex_atomic_op("and	%0, %1, %4", ret, oldval, tmp, uaddr, ~oparg);
		break;
	case FUTEX_OP_XOR:
		__futex_atomic_op("eor	%0, %1, %4", ret, oldval, tmp, uaddr, oparg);
		break;
	default:
		ret = -ENOSYS;
	}

#ifndef CONFIG_SMP
	preempt_enable();
#endif

	/*
	 * Store unconditionally. If ret != 0 the extra store is the least
	 * of the worries but GCC cannot figure out that __futex_atomic_op()
	 * is either setting ret to -EFAULT or storing the old value in
	 * oldval which results in a uninitialized warning at the call site.
	 */
	*oval = oldval;

	return ret;
}

#endif /* __KERNEL__ */
#endif /* _ASM_ARM_FUTEX_H */
