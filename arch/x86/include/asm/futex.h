/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_FUTEX_H
#define _ASM_X86_FUTEX_H

#ifdef __KERNEL__

#include <linux/futex.h>
#include <linux/uaccess.h>

#include <asm/asm.h>
#include <asm/errno.h>
#include <asm/processor.h>
#include <asm/smap.h>

#define unsafe_atomic_op1(insn, oval, uaddr, oparg, label)	\
do {								\
	int oldval = 0, ret;					\
	asm volatile("1:\t" insn "\n"				\
		     "2:\n"					\
		     "\t.section .fixup,\"ax\"\n"		\
		     "3:\tmov\t%3, %1\n"			\
		     "\tjmp\t2b\n"				\
		     "\t.previous\n"				\
		     _ASM_EXTABLE_UA(1b, 3b)			\
		     : "=r" (oldval), "=r" (ret), "+m" (*uaddr)	\
		     : "i" (-EFAULT), "0" (oparg), "1" (0));	\
	if (ret)						\
		goto label;					\
	*oval = oldval;						\
} while(0)


#define unsafe_atomic_op2(insn, oval, uaddr, oparg, label)	\
do {								\
	int oldval = 0, ret, tem;				\
	asm volatile("1:\tmovl	%2, %0\n"			\
		     "2:\tmovl\t%0, %3\n"			\
		     "\t" insn "\n"				\
		     "3:\t" LOCK_PREFIX "cmpxchgl %3, %2\n"	\
		     "\tjnz\t2b\n"				\
		     "4:\n"					\
		     "\t.section .fixup,\"ax\"\n"		\
		     "5:\tmov\t%5, %1\n"			\
		     "\tjmp\t4b\n"				\
		     "\t.previous\n"				\
		     _ASM_EXTABLE_UA(1b, 5b)			\
		     _ASM_EXTABLE_UA(3b, 5b)			\
		     : "=&a" (oldval), "=&r" (ret),		\
		       "+m" (*uaddr), "=&r" (tem)		\
		     : "r" (oparg), "i" (-EFAULT), "1" (0));	\
	if (ret)						\
		goto label;					\
	*oval = oldval;						\
} while(0)

static __always_inline int arch_futex_atomic_op_inuser(int op, int oparg, int *oval,
		u32 __user *uaddr)
{
	if (!user_access_begin(uaddr, sizeof(u32)))
		return -EFAULT;

	switch (op) {
	case FUTEX_OP_SET:
		unsafe_atomic_op1("xchgl %0, %2", oval, uaddr, oparg, Efault);
		break;
	case FUTEX_OP_ADD:
		unsafe_atomic_op1(LOCK_PREFIX "xaddl %0, %2", oval,
				   uaddr, oparg, Efault);
		break;
	case FUTEX_OP_OR:
		unsafe_atomic_op2("orl %4, %3", oval, uaddr, oparg, Efault);
		break;
	case FUTEX_OP_ANDN:
		unsafe_atomic_op2("andl %4, %3", oval, uaddr, ~oparg, Efault);
		break;
	case FUTEX_OP_XOR:
		unsafe_atomic_op2("xorl %4, %3", oval, uaddr, oparg, Efault);
		break;
	default:
		user_access_end();
		return -ENOSYS;
	}
	user_access_end();
	return 0;
Efault:
	user_access_end();
	return -EFAULT;
}

static inline int futex_atomic_cmpxchg_inatomic(u32 *uval, u32 __user *uaddr,
						u32 oldval, u32 newval)
{
	int ret = 0;

	if (!user_access_begin(uaddr, sizeof(u32)))
		return -EFAULT;
	asm volatile("\n"
		"1:\t" LOCK_PREFIX "cmpxchgl %4, %2\n"
		"2:\n"
		"\t.section .fixup, \"ax\"\n"
		"3:\tmov     %3, %0\n"
		"\tjmp     2b\n"
		"\t.previous\n"
		_ASM_EXTABLE_UA(1b, 3b)
		: "+r" (ret), "=a" (oldval), "+m" (*uaddr)
		: "i" (-EFAULT), "r" (newval), "1" (oldval)
		: "memory"
	);
	user_access_end();
	*uval = oldval;
	return ret;
}

#endif
#endif /* _ASM_X86_FUTEX_H */
