/*
 * Access to user system call parameters and results
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * See asm-generic/syscall.h for descriptions of what we must do here.
 *
 * Copyright (C) 2012 Ralf Baechle <ralf@linux-mips.org>
 */

#ifndef __ASM_MIPS_SYSCALL_H
#define __ASM_MIPS_SYSCALL_H

#include <linux/compiler.h>
#include <uapi/linux/audit.h>
#include <linux/elf-em.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/ptrace.h>
#include <asm/unistd.h>

#ifndef __NR_syscall /* Only defined if _MIPS_SIM == _MIPS_SIM_ABI32 */
#define __NR_syscall 4000
#endif

static inline bool mips_syscall_is_indirect(struct task_struct *task,
					    struct pt_regs *regs)
{
	/* O32 ABI syscall() - Either 64-bit with O32 or 32-bit */
	return (IS_ENABLED(CONFIG_32BIT) ||
		test_tsk_thread_flag(task, TIF_32BIT_REGS)) &&
		(regs->regs[2] == __NR_syscall);
}

static inline long syscall_get_nr(struct task_struct *task,
				  struct pt_regs *regs)
{
	return current_thread_info()->syscall;
}

static inline void mips_syscall_update_nr(struct task_struct *task,
					  struct pt_regs *regs)
{
	/*
	 * v0 is the system call number, except for O32 ABI syscall(), where it
	 * ends up in a0.
	 */
	if (mips_syscall_is_indirect(task, regs))
		task_thread_info(task)->syscall = regs->regs[4];
	else
		task_thread_info(task)->syscall = regs->regs[2];
}

static inline void mips_get_syscall_arg(unsigned long *arg,
	struct task_struct *task, struct pt_regs *regs, unsigned int n)
{
	unsigned long usp __maybe_unused = regs->regs[29];

	switch (n) {
	case 0: case 1: case 2: case 3:
		*arg = regs->regs[4 + n];

		return;

#ifdef CONFIG_32BIT
	case 4: case 5: case 6: case 7:
		get_user(*arg, (int *)usp + n);
		return;
#endif

#ifdef CONFIG_64BIT
	case 4: case 5: case 6: case 7:
#ifdef CONFIG_MIPS32_O32
		if (test_tsk_thread_flag(task, TIF_32BIT_REGS))
			get_user(*arg, (int *)usp + n);
		else
#endif
			*arg = regs->regs[4 + n];

		return;
#endif

	default:
		BUG();
	}

	unreachable();
}

static inline long syscall_get_error(struct task_struct *task,
				     struct pt_regs *regs)
{
	return regs->regs[7] ? -regs->regs[2] : 0;
}

static inline long syscall_get_return_value(struct task_struct *task,
					    struct pt_regs *regs)
{
	return regs->regs[2];
}

static inline void syscall_rollback(struct task_struct *task,
				    struct pt_regs *regs)
{
	/* Do nothing */
}

static inline void syscall_set_return_value(struct task_struct *task,
					    struct pt_regs *regs,
					    int error, long val)
{
	if (error) {
		regs->regs[2] = -error;
		regs->regs[7] = 1;
	} else {
		regs->regs[2] = val;
		regs->regs[7] = 0;
	}
}

static inline void syscall_get_arguments(struct task_struct *task,
					 struct pt_regs *regs,
					 unsigned long *args)
{
	unsigned int i = 0;
	unsigned int n = 6;

	/* O32 ABI syscall() */
	if (mips_syscall_is_indirect(task, regs))
		i++;

	while (n--)
		mips_get_syscall_arg(args++, task, regs, i++);
}

extern const unsigned long sys_call_table[];
extern const unsigned long sys32_call_table[];
extern const unsigned long sysn32_call_table[];

static inline int syscall_get_arch(struct task_struct *task)
{
	int arch = AUDIT_ARCH_MIPS;
#ifdef CONFIG_64BIT
	if (!test_tsk_thread_flag(task, TIF_32BIT_REGS)) {
		arch |= __AUDIT_ARCH_64BIT;
		/* N32 sets only TIF_32BIT_ADDR */
		if (test_tsk_thread_flag(task, TIF_32BIT_ADDR))
			arch |= __AUDIT_ARCH_CONVENTION_MIPS64_N32;
	}
#endif
#if defined(__LITTLE_ENDIAN)
	arch |=  __AUDIT_ARCH_LE;
#endif
	return arch;
}

#endif	/* __ASM_MIPS_SYSCALL_H */
