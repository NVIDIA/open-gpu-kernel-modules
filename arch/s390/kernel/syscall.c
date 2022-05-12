// SPDX-License-Identifier: GPL-2.0
/*
 *  S390 version
 *    Copyright IBM Corp. 1999, 2000
 *    Author(s): Martin Schwidefsky (schwidefsky@de.ibm.com),
 *               Thomas Spatzier (tspat@de.ibm.com)
 *
 *  Derived from "arch/i386/kernel/sys_i386.c"
 *
 *  This file contains various random system calls that
 *  have a non-standard calling sequence on the Linux/s390
 *  platform.
 */

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/smp.h>
#include <linux/sem.h>
#include <linux/msg.h>
#include <linux/shm.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <linux/mman.h>
#include <linux/file.h>
#include <linux/utsname.h>
#include <linux/personality.h>
#include <linux/unistd.h>
#include <linux/ipc.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/thread_info.h>
#include <linux/entry-common.h>

#include <asm/ptrace.h>
#include <asm/vtime.h>

#include "entry.h"

/*
 * Perform the mmap() system call. Linux for S/390 isn't able to handle more
 * than 5 system call parameters, so this system call uses a memory block
 * for parameter passing.
 */

struct s390_mmap_arg_struct {
	unsigned long addr;
	unsigned long len;
	unsigned long prot;
	unsigned long flags;
	unsigned long fd;
	unsigned long offset;
};

SYSCALL_DEFINE1(mmap2, struct s390_mmap_arg_struct __user *, arg)
{
	struct s390_mmap_arg_struct a;
	int error = -EFAULT;

	if (copy_from_user(&a, arg, sizeof(a)))
		goto out;
	error = ksys_mmap_pgoff(a.addr, a.len, a.prot, a.flags, a.fd, a.offset);
out:
	return error;
}

#ifdef CONFIG_SYSVIPC
/*
 * sys_ipc() is the de-multiplexer for the SysV IPC calls.
 */
SYSCALL_DEFINE5(s390_ipc, uint, call, int, first, unsigned long, second,
		unsigned long, third, void __user *, ptr)
{
	if (call >> 16)
		return -EINVAL;
	/* The s390 sys_ipc variant has only five parameters instead of six
	 * like the generic variant. The only difference is the handling of
	 * the SEMTIMEDOP subcall where on s390 the third parameter is used
	 * as a pointer to a struct timespec where the generic variant uses
	 * the fifth parameter.
	 * Therefore we can call the generic variant by simply passing the
	 * third parameter also as fifth parameter.
	 */
	return ksys_ipc(call, first, second, third, ptr, third);
}
#endif /* CONFIG_SYSVIPC */

SYSCALL_DEFINE1(s390_personality, unsigned int, personality)
{
	unsigned int ret = current->personality;

	if (personality(current->personality) == PER_LINUX32 &&
	    personality(personality) == PER_LINUX)
		personality |= PER_LINUX32;

	if (personality != 0xffffffff)
		set_personality(personality);

	if (personality(ret) == PER_LINUX32)
		ret &= ~PER_LINUX32;

	return ret;
}

SYSCALL_DEFINE0(ni_syscall)
{
	return -ENOSYS;
}

void do_syscall(struct pt_regs *regs)
{
	unsigned long nr;

	nr = regs->int_code & 0xffff;
	if (!nr) {
		nr = regs->gprs[1] & 0xffff;
		regs->int_code &= ~0xffffUL;
		regs->int_code |= nr;
	}

	regs->gprs[2] = nr;

	nr = syscall_enter_from_user_mode_work(regs, nr);

	/*
	 * In the s390 ptrace ABI, both the syscall number and the return value
	 * use gpr2. However, userspace puts the syscall number either in the
	 * svc instruction itself, or uses gpr1. To make at least skipping syscalls
	 * work, the ptrace code sets PIF_SYSCALL_RET_SET, which is checked here
	 * and if set, the syscall will be skipped.
	 */
	if (!test_pt_regs_flag(regs, PIF_SYSCALL_RET_SET)) {
		regs->gprs[2] = -ENOSYS;
		if (likely(nr < NR_syscalls))
			regs->gprs[2] = current->thread.sys_call_table[nr](regs);
	} else {
		clear_pt_regs_flag(regs, PIF_SYSCALL_RET_SET);
	}
	syscall_exit_to_user_mode_work(regs);
}

void noinstr __do_syscall(struct pt_regs *regs, int per_trap)
{
	add_random_kstack_offset();
	enter_from_user_mode(regs);

	memcpy(&regs->gprs[8], S390_lowcore.save_area_sync, 8 * sizeof(unsigned long));
	memcpy(&regs->int_code, &S390_lowcore.svc_ilc, sizeof(regs->int_code));
	regs->psw = S390_lowcore.svc_old_psw;

	update_timer_sys();

	local_irq_enable();
	regs->orig_gpr2 = regs->gprs[2];

	if (per_trap)
		set_thread_flag(TIF_PER_TRAP);

	for (;;) {
		regs->flags = 0;
		set_pt_regs_flag(regs, PIF_SYSCALL);
		do_syscall(regs);
		if (!test_pt_regs_flag(regs, PIF_SYSCALL_RESTART))
			break;
		local_irq_enable();
	}
	exit_to_user_mode();
}
