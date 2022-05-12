// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 * Copyright 2015 Regents of the University of California
 * Copyright 2017 SiFive
 *
 * Copied from arch/tile/kernel/ptrace.c
 */

#include <asm/ptrace.h>
#include <asm/syscall.h>
#include <asm/thread_info.h>
#include <linux/audit.h>
#include <linux/ptrace.h>
#include <linux/elf.h>
#include <linux/regset.h>
#include <linux/sched.h>
#include <linux/sched/task_stack.h>
#include <linux/tracehook.h>

#define CREATE_TRACE_POINTS
#include <trace/events/syscalls.h>

enum riscv_regset {
	REGSET_X,
#ifdef CONFIG_FPU
	REGSET_F,
#endif
};

static int riscv_gpr_get(struct task_struct *target,
			 const struct user_regset *regset,
			 struct membuf to)
{
	return membuf_write(&to, task_pt_regs(target),
			    sizeof(struct user_regs_struct));
}

static int riscv_gpr_set(struct task_struct *target,
			 const struct user_regset *regset,
			 unsigned int pos, unsigned int count,
			 const void *kbuf, const void __user *ubuf)
{
	int ret;
	struct pt_regs *regs;

	regs = task_pt_regs(target);
	ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf, regs, 0, -1);
	return ret;
}

#ifdef CONFIG_FPU
static int riscv_fpr_get(struct task_struct *target,
			 const struct user_regset *regset,
			 struct membuf to)
{
	struct __riscv_d_ext_state *fstate = &target->thread.fstate;

	membuf_write(&to, fstate, offsetof(struct __riscv_d_ext_state, fcsr));
	membuf_store(&to, fstate->fcsr);
	return membuf_zero(&to, 4);	// explicitly pad
}

static int riscv_fpr_set(struct task_struct *target,
			 const struct user_regset *regset,
			 unsigned int pos, unsigned int count,
			 const void *kbuf, const void __user *ubuf)
{
	int ret;
	struct __riscv_d_ext_state *fstate = &target->thread.fstate;

	ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf, fstate, 0,
				 offsetof(struct __riscv_d_ext_state, fcsr));
	if (!ret) {
		ret = user_regset_copyin(&pos, &count, &kbuf, &ubuf, fstate, 0,
					 offsetof(struct __riscv_d_ext_state, fcsr) +
					 sizeof(fstate->fcsr));
	}

	return ret;
}
#endif

static const struct user_regset riscv_user_regset[] = {
	[REGSET_X] = {
		.core_note_type = NT_PRSTATUS,
		.n = ELF_NGREG,
		.size = sizeof(elf_greg_t),
		.align = sizeof(elf_greg_t),
		.regset_get = riscv_gpr_get,
		.set = riscv_gpr_set,
	},
#ifdef CONFIG_FPU
	[REGSET_F] = {
		.core_note_type = NT_PRFPREG,
		.n = ELF_NFPREG,
		.size = sizeof(elf_fpreg_t),
		.align = sizeof(elf_fpreg_t),
		.regset_get = riscv_fpr_get,
		.set = riscv_fpr_set,
	},
#endif
};

static const struct user_regset_view riscv_user_native_view = {
	.name = "riscv",
	.e_machine = EM_RISCV,
	.regsets = riscv_user_regset,
	.n = ARRAY_SIZE(riscv_user_regset),
};

const struct user_regset_view *task_user_regset_view(struct task_struct *task)
{
	return &riscv_user_native_view;
}

struct pt_regs_offset {
	const char *name;
	int offset;
};

#define REG_OFFSET_NAME(r) {.name = #r, .offset = offsetof(struct pt_regs, r)}
#define REG_OFFSET_END {.name = NULL, .offset = 0}

static const struct pt_regs_offset regoffset_table[] = {
	REG_OFFSET_NAME(epc),
	REG_OFFSET_NAME(ra),
	REG_OFFSET_NAME(sp),
	REG_OFFSET_NAME(gp),
	REG_OFFSET_NAME(tp),
	REG_OFFSET_NAME(t0),
	REG_OFFSET_NAME(t1),
	REG_OFFSET_NAME(t2),
	REG_OFFSET_NAME(s0),
	REG_OFFSET_NAME(s1),
	REG_OFFSET_NAME(a0),
	REG_OFFSET_NAME(a1),
	REG_OFFSET_NAME(a2),
	REG_OFFSET_NAME(a3),
	REG_OFFSET_NAME(a4),
	REG_OFFSET_NAME(a5),
	REG_OFFSET_NAME(a6),
	REG_OFFSET_NAME(a7),
	REG_OFFSET_NAME(s2),
	REG_OFFSET_NAME(s3),
	REG_OFFSET_NAME(s4),
	REG_OFFSET_NAME(s5),
	REG_OFFSET_NAME(s6),
	REG_OFFSET_NAME(s7),
	REG_OFFSET_NAME(s8),
	REG_OFFSET_NAME(s9),
	REG_OFFSET_NAME(s10),
	REG_OFFSET_NAME(s11),
	REG_OFFSET_NAME(t3),
	REG_OFFSET_NAME(t4),
	REG_OFFSET_NAME(t5),
	REG_OFFSET_NAME(t6),
	REG_OFFSET_NAME(status),
	REG_OFFSET_NAME(badaddr),
	REG_OFFSET_NAME(cause),
	REG_OFFSET_NAME(orig_a0),
	REG_OFFSET_END,
};

/**
 * regs_query_register_offset() - query register offset from its name
 * @name:	the name of a register
 *
 * regs_query_register_offset() returns the offset of a register in struct
 * pt_regs from its name. If the name is invalid, this returns -EINVAL;
 */
int regs_query_register_offset(const char *name)
{
	const struct pt_regs_offset *roff;

	for (roff = regoffset_table; roff->name != NULL; roff++)
		if (!strcmp(roff->name, name))
			return roff->offset;
	return -EINVAL;
}

/**
 * regs_within_kernel_stack() - check the address in the stack
 * @regs:      pt_regs which contains kernel stack pointer.
 * @addr:      address which is checked.
 *
 * regs_within_kernel_stack() checks @addr is within the kernel stack page(s).
 * If @addr is within the kernel stack, it returns true. If not, returns false.
 */
static bool regs_within_kernel_stack(struct pt_regs *regs, unsigned long addr)
{
	return (addr & ~(THREAD_SIZE - 1))  ==
		(kernel_stack_pointer(regs) & ~(THREAD_SIZE - 1));
}

/**
 * regs_get_kernel_stack_nth() - get Nth entry of the stack
 * @regs:	pt_regs which contains kernel stack pointer.
 * @n:		stack entry number.
 *
 * regs_get_kernel_stack_nth() returns @n th entry of the kernel stack which
 * is specified by @regs. If the @n th entry is NOT in the kernel stack,
 * this returns 0.
 */
unsigned long regs_get_kernel_stack_nth(struct pt_regs *regs, unsigned int n)
{
	unsigned long *addr = (unsigned long *)kernel_stack_pointer(regs);

	addr += n;
	if (regs_within_kernel_stack(regs, (unsigned long)addr))
		return *addr;
	else
		return 0;
}

void ptrace_disable(struct task_struct *child)
{
	clear_tsk_thread_flag(child, TIF_SYSCALL_TRACE);
}

long arch_ptrace(struct task_struct *child, long request,
		 unsigned long addr, unsigned long data)
{
	long ret = -EIO;

	switch (request) {
	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

/*
 * Allows PTRACE_SYSCALL to work.  These are called from entry.S in
 * {handle,ret_from}_syscall.
 */
__visible int do_syscall_trace_enter(struct pt_regs *regs)
{
	if (test_thread_flag(TIF_SYSCALL_TRACE))
		if (tracehook_report_syscall_entry(regs))
			return -1;

	/*
	 * Do the secure computing after ptrace; failures should be fast.
	 * If this fails we might have return value in a0 from seccomp
	 * (via SECCOMP_RET_ERRNO/TRACE).
	 */
	if (secure_computing() == -1)
		return -1;

#ifdef CONFIG_HAVE_SYSCALL_TRACEPOINTS
	if (test_thread_flag(TIF_SYSCALL_TRACEPOINT))
		trace_sys_enter(regs, syscall_get_nr(current, regs));
#endif

	audit_syscall_entry(regs->a7, regs->a0, regs->a1, regs->a2, regs->a3);
	return 0;
}

__visible void do_syscall_trace_exit(struct pt_regs *regs)
{
	audit_syscall_exit(regs);

	if (test_thread_flag(TIF_SYSCALL_TRACE))
		tracehook_report_syscall_exit(regs, 0);

#ifdef CONFIG_HAVE_SYSCALL_TRACEPOINTS
	if (test_thread_flag(TIF_SYSCALL_TRACEPOINT))
		trace_sys_exit(regs, regs_return_value(regs));
#endif
}
