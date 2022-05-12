// SPDX-License-Identifier: GPL-2.0
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task_stack.h>
#include <linux/perf_event.h>
#include <linux/bug.h>
#include <linux/stddef.h>
#include <asm/perf_regs.h>
#include <asm/ptrace.h>

#ifdef CONFIG_X86_32
#define PERF_REG_X86_MAX PERF_REG_X86_32_MAX
#else
#define PERF_REG_X86_MAX PERF_REG_X86_64_MAX
#endif

#define PT_REGS_OFFSET(id, r) [id] = offsetof(struct pt_regs, r)

static unsigned int pt_regs_offset[PERF_REG_X86_MAX] = {
	PT_REGS_OFFSET(PERF_REG_X86_AX, ax),
	PT_REGS_OFFSET(PERF_REG_X86_BX, bx),
	PT_REGS_OFFSET(PERF_REG_X86_CX, cx),
	PT_REGS_OFFSET(PERF_REG_X86_DX, dx),
	PT_REGS_OFFSET(PERF_REG_X86_SI, si),
	PT_REGS_OFFSET(PERF_REG_X86_DI, di),
	PT_REGS_OFFSET(PERF_REG_X86_BP, bp),
	PT_REGS_OFFSET(PERF_REG_X86_SP, sp),
	PT_REGS_OFFSET(PERF_REG_X86_IP, ip),
	PT_REGS_OFFSET(PERF_REG_X86_FLAGS, flags),
	PT_REGS_OFFSET(PERF_REG_X86_CS, cs),
	PT_REGS_OFFSET(PERF_REG_X86_SS, ss),
#ifdef CONFIG_X86_32
	PT_REGS_OFFSET(PERF_REG_X86_DS, ds),
	PT_REGS_OFFSET(PERF_REG_X86_ES, es),
	PT_REGS_OFFSET(PERF_REG_X86_FS, fs),
	PT_REGS_OFFSET(PERF_REG_X86_GS, gs),
#else
	/*
	 * The pt_regs struct does not store
	 * ds, es, fs, gs in 64 bit mode.
	 */
	(unsigned int) -1,
	(unsigned int) -1,
	(unsigned int) -1,
	(unsigned int) -1,
#endif
#ifdef CONFIG_X86_64
	PT_REGS_OFFSET(PERF_REG_X86_R8, r8),
	PT_REGS_OFFSET(PERF_REG_X86_R9, r9),
	PT_REGS_OFFSET(PERF_REG_X86_R10, r10),
	PT_REGS_OFFSET(PERF_REG_X86_R11, r11),
	PT_REGS_OFFSET(PERF_REG_X86_R12, r12),
	PT_REGS_OFFSET(PERF_REG_X86_R13, r13),
	PT_REGS_OFFSET(PERF_REG_X86_R14, r14),
	PT_REGS_OFFSET(PERF_REG_X86_R15, r15),
#endif
};

u64 perf_reg_value(struct pt_regs *regs, int idx)
{
	struct x86_perf_regs *perf_regs;

	if (idx >= PERF_REG_X86_XMM0 && idx < PERF_REG_X86_XMM_MAX) {
		perf_regs = container_of(regs, struct x86_perf_regs, regs);
		if (!perf_regs->xmm_regs)
			return 0;
		return perf_regs->xmm_regs[idx - PERF_REG_X86_XMM0];
	}

	if (WARN_ON_ONCE(idx >= ARRAY_SIZE(pt_regs_offset)))
		return 0;

	return regs_get_register(regs, pt_regs_offset[idx]);
}

#define PERF_REG_X86_RESERVED	(((1ULL << PERF_REG_X86_XMM0) - 1) & \
				 ~((1ULL << PERF_REG_X86_MAX) - 1))

#ifdef CONFIG_X86_32
#define REG_NOSUPPORT ((1ULL << PERF_REG_X86_R8) | \
		       (1ULL << PERF_REG_X86_R9) | \
		       (1ULL << PERF_REG_X86_R10) | \
		       (1ULL << PERF_REG_X86_R11) | \
		       (1ULL << PERF_REG_X86_R12) | \
		       (1ULL << PERF_REG_X86_R13) | \
		       (1ULL << PERF_REG_X86_R14) | \
		       (1ULL << PERF_REG_X86_R15))

int perf_reg_validate(u64 mask)
{
	if (!mask || (mask & (REG_NOSUPPORT | PERF_REG_X86_RESERVED)))
		return -EINVAL;

	return 0;
}

u64 perf_reg_abi(struct task_struct *task)
{
	return PERF_SAMPLE_REGS_ABI_32;
}

void perf_get_regs_user(struct perf_regs *regs_user,
			struct pt_regs *regs)
{
	regs_user->regs = task_pt_regs(current);
	regs_user->abi = perf_reg_abi(current);
}
#else /* CONFIG_X86_64 */
#define REG_NOSUPPORT ((1ULL << PERF_REG_X86_DS) | \
		       (1ULL << PERF_REG_X86_ES) | \
		       (1ULL << PERF_REG_X86_FS) | \
		       (1ULL << PERF_REG_X86_GS))

int perf_reg_validate(u64 mask)
{
	if (!mask || (mask & (REG_NOSUPPORT | PERF_REG_X86_RESERVED)))
		return -EINVAL;

	return 0;
}

u64 perf_reg_abi(struct task_struct *task)
{
	if (!user_64bit_mode(task_pt_regs(task)))
		return PERF_SAMPLE_REGS_ABI_32;
	else
		return PERF_SAMPLE_REGS_ABI_64;
}

static DEFINE_PER_CPU(struct pt_regs, nmi_user_regs);

void perf_get_regs_user(struct perf_regs *regs_user,
			struct pt_regs *regs)
{
	struct pt_regs *regs_user_copy = this_cpu_ptr(&nmi_user_regs);
	struct pt_regs *user_regs = task_pt_regs(current);

	if (!in_nmi()) {
		regs_user->regs = user_regs;
		regs_user->abi = perf_reg_abi(current);
		return;
	}

	/*
	 * If we're in an NMI that interrupted task_pt_regs setup, then
	 * we can't sample user regs at all.  This check isn't really
	 * sufficient, though, as we could be in an NMI inside an interrupt
	 * that happened during task_pt_regs setup.
	 */
	if (regs->sp > (unsigned long)&user_regs->r11 &&
	    regs->sp <= (unsigned long)(user_regs + 1)) {
		regs_user->abi = PERF_SAMPLE_REGS_ABI_NONE;
		regs_user->regs = NULL;
		return;
	}

	/*
	 * These registers are always saved on 64-bit syscall entry.
	 * On 32-bit entry points, they are saved too except r8..r11.
	 */
	regs_user_copy->ip = user_regs->ip;
	regs_user_copy->ax = user_regs->ax;
	regs_user_copy->cx = user_regs->cx;
	regs_user_copy->dx = user_regs->dx;
	regs_user_copy->si = user_regs->si;
	regs_user_copy->di = user_regs->di;
	regs_user_copy->r8 = user_regs->r8;
	regs_user_copy->r9 = user_regs->r9;
	regs_user_copy->r10 = user_regs->r10;
	regs_user_copy->r11 = user_regs->r11;
	regs_user_copy->orig_ax = user_regs->orig_ax;
	regs_user_copy->flags = user_regs->flags;
	regs_user_copy->sp = user_regs->sp;
	regs_user_copy->cs = user_regs->cs;
	regs_user_copy->ss = user_regs->ss;
	/*
	 * Store user space frame-pointer value on sample
	 * to facilitate stack unwinding for cases when
	 * user space executable code has such support
	 * enabled at compile time:
	 */
	regs_user_copy->bp = user_regs->bp;

	regs_user_copy->bx = -1;
	regs_user_copy->r12 = -1;
	regs_user_copy->r13 = -1;
	regs_user_copy->r14 = -1;
	regs_user_copy->r15 = -1;
	/*
	 * For this to be at all useful, we need a reasonable guess for
	 * the ABI.  Be careful: we're in NMI context, and we're
	 * considering current to be the current task, so we should
	 * be careful not to look at any other percpu variables that might
	 * change during context switches.
	 */
	regs_user->abi = user_64bit_mode(user_regs) ?
		PERF_SAMPLE_REGS_ABI_64 : PERF_SAMPLE_REGS_ABI_32;

	regs_user->regs = regs_user_copy;
}
#endif /* CONFIG_X86_32 */
