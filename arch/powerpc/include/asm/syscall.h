/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Access to user system call parameters and results
 *
 * Copyright (C) 2008 Red Hat, Inc.  All rights reserved.
 *
 * See asm-generic/syscall.h for descriptions of what we must do here.
 */

#ifndef _ASM_SYSCALL_H
#define _ASM_SYSCALL_H	1

#include <uapi/linux/audit.h>
#include <linux/sched.h>
#include <linux/thread_info.h>

/* ftrace syscalls requires exporting the sys_call_table */
extern const unsigned long sys_call_table[];
extern const unsigned long compat_sys_call_table[];

static inline int syscall_get_nr(struct task_struct *task, struct pt_regs *regs)
{
	/*
	 * Note that we are returning an int here. That means 0xffffffff, ie.
	 * 32-bit negative 1, will be interpreted as -1 on a 64-bit kernel.
	 * This is important for seccomp so that compat tasks can set r0 = -1
	 * to reject the syscall.
	 */
	if (trap_is_syscall(regs))
		return regs->gpr[0];
	else
		return -1;
}

static inline void syscall_rollback(struct task_struct *task,
				    struct pt_regs *regs)
{
	regs->gpr[3] = regs->orig_gpr3;
}

static inline long syscall_get_error(struct task_struct *task,
				     struct pt_regs *regs)
{
	if (trap_is_scv(regs)) {
		unsigned long error = regs->gpr[3];

		return IS_ERR_VALUE(error) ? error : 0;
	} else {
		/*
		 * If the system call failed,
		 * regs->gpr[3] contains a positive ERRORCODE.
		 */
		return (regs->ccr & 0x10000000UL) ? -regs->gpr[3] : 0;
	}
}

static inline long syscall_get_return_value(struct task_struct *task,
					    struct pt_regs *regs)
{
	return regs->gpr[3];
}

static inline void syscall_set_return_value(struct task_struct *task,
					    struct pt_regs *regs,
					    int error, long val)
{
	if (trap_is_scv(regs)) {
		regs->gpr[3] = (long) error ?: val;
	} else {
		/*
		 * In the general case it's not obvious that we must deal with
		 * CCR here, as the syscall exit path will also do that for us.
		 * However there are some places, eg. the signal code, which
		 * check ccr to decide if the value in r3 is actually an error.
		 */
		if (error) {
			regs->ccr |= 0x10000000L;
			regs->gpr[3] = error;
		} else {
			regs->ccr &= ~0x10000000L;
			regs->gpr[3] = val;
		}
	}
}

static inline void syscall_get_arguments(struct task_struct *task,
					 struct pt_regs *regs,
					 unsigned long *args)
{
	unsigned long val, mask = -1UL;
	unsigned int n = 6;

#ifdef CONFIG_COMPAT
	if (test_tsk_thread_flag(task, TIF_32BIT))
		mask = 0xffffffff;
#endif
	while (n--) {
		if (n == 0)
			val = regs->orig_gpr3;
		else
			val = regs->gpr[3 + n];

		args[n] = val & mask;
	}
}

static inline void syscall_set_arguments(struct task_struct *task,
					 struct pt_regs *regs,
					 const unsigned long *args)
{
	memcpy(&regs->gpr[3], args, 6 * sizeof(args[0]));

	/* Also copy the first argument into orig_gpr3 */
	regs->orig_gpr3 = args[0];
}

static inline int syscall_get_arch(struct task_struct *task)
{
	int arch;

	if (IS_ENABLED(CONFIG_PPC64) && !test_tsk_thread_flag(task, TIF_32BIT))
		arch = AUDIT_ARCH_PPC64;
	else
		arch = AUDIT_ARCH_PPC;

#ifdef __LITTLE_ENDIAN__
	arch |= __AUDIT_ARCH_LE;
#endif
	return arch;
}
#endif	/* _ASM_SYSCALL_H */
