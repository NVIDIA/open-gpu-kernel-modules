// SPDX-License-Identifier: GPL-2.0-only
/* By Ross Biro 1/23/92 */
/*
 * Pentium III FXSR, SSE support
 *	Gareth Hughes <gareth@valinux.com>, May 2000
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task_stack.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/ptrace.h>
#include <linux/tracehook.h>
#include <linux/user.h>
#include <linux/elf.h>
#include <linux/security.h>
#include <linux/audit.h>
#include <linux/seccomp.h>
#include <linux/signal.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <linux/rcupdate.h>
#include <linux/export.h>
#include <linux/context_tracking.h>
#include <linux/nospec.h>

#include <linux/uaccess.h>
#include <asm/processor.h>
#include <asm/fpu/internal.h>
#include <asm/fpu/signal.h>
#include <asm/fpu/regset.h>
#include <asm/debugreg.h>
#include <asm/ldt.h>
#include <asm/desc.h>
#include <asm/prctl.h>
#include <asm/proto.h>
#include <asm/hw_breakpoint.h>
#include <asm/traps.h>
#include <asm/syscall.h>
#include <asm/fsgsbase.h>
#include <asm/io_bitmap.h>

#include "tls.h"

enum x86_regset {
	REGSET_GENERAL,
	REGSET_FP,
	REGSET_XFP,
	REGSET_IOPERM64 = REGSET_XFP,
	REGSET_XSTATE,
	REGSET_TLS,
	REGSET_IOPERM32,
};

struct pt_regs_offset {
	const char *name;
	int offset;
};

#define REG_OFFSET_NAME(r) {.name = #r, .offset = offsetof(struct pt_regs, r)}
#define REG_OFFSET_END {.name = NULL, .offset = 0}

static const struct pt_regs_offset regoffset_table[] = {
#ifdef CONFIG_X86_64
	REG_OFFSET_NAME(r15),
	REG_OFFSET_NAME(r14),
	REG_OFFSET_NAME(r13),
	REG_OFFSET_NAME(r12),
	REG_OFFSET_NAME(r11),
	REG_OFFSET_NAME(r10),
	REG_OFFSET_NAME(r9),
	REG_OFFSET_NAME(r8),
#endif
	REG_OFFSET_NAME(bx),
	REG_OFFSET_NAME(cx),
	REG_OFFSET_NAME(dx),
	REG_OFFSET_NAME(si),
	REG_OFFSET_NAME(di),
	REG_OFFSET_NAME(bp),
	REG_OFFSET_NAME(ax),
#ifdef CONFIG_X86_32
	REG_OFFSET_NAME(ds),
	REG_OFFSET_NAME(es),
	REG_OFFSET_NAME(fs),
	REG_OFFSET_NAME(gs),
#endif
	REG_OFFSET_NAME(orig_ax),
	REG_OFFSET_NAME(ip),
	REG_OFFSET_NAME(cs),
	REG_OFFSET_NAME(flags),
	REG_OFFSET_NAME(sp),
	REG_OFFSET_NAME(ss),
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
 * regs_query_register_name() - query register name from its offset
 * @offset:	the offset of a register in struct pt_regs.
 *
 * regs_query_register_name() returns the name of a register from its
 * offset in struct pt_regs. If the @offset is invalid, this returns NULL;
 */
const char *regs_query_register_name(unsigned int offset)
{
	const struct pt_regs_offset *roff;
	for (roff = regoffset_table; roff->name != NULL; roff++)
		if (roff->offset == offset)
			return roff->name;
	return NULL;
}

/*
 * does not yet catch signals sent when the child dies.
 * in exit.c or in signal.c.
 */

/*
 * Determines which flags the user has access to [1 = access, 0 = no access].
 */
#define FLAG_MASK_32		((unsigned long)			\
				 (X86_EFLAGS_CF | X86_EFLAGS_PF |	\
				  X86_EFLAGS_AF | X86_EFLAGS_ZF |	\
				  X86_EFLAGS_SF | X86_EFLAGS_TF |	\
				  X86_EFLAGS_DF | X86_EFLAGS_OF |	\
				  X86_EFLAGS_RF | X86_EFLAGS_AC))

/*
 * Determines whether a value may be installed in a segment register.
 */
static inline bool invalid_selector(u16 value)
{
	return unlikely(value != 0 && (value & SEGMENT_RPL_MASK) != USER_RPL);
}

#ifdef CONFIG_X86_32

#define FLAG_MASK		FLAG_MASK_32

static unsigned long *pt_regs_access(struct pt_regs *regs, unsigned long regno)
{
	BUILD_BUG_ON(offsetof(struct pt_regs, bx) != 0);
	return &regs->bx + (regno >> 2);
}

static u16 get_segment_reg(struct task_struct *task, unsigned long offset)
{
	/*
	 * Returning the value truncates it to 16 bits.
	 */
	unsigned int retval;
	if (offset != offsetof(struct user_regs_struct, gs))
		retval = *pt_regs_access(task_pt_regs(task), offset);
	else {
		if (task == current)
			retval = get_user_gs(task_pt_regs(task));
		else
			retval = task_user_gs(task);
	}
	return retval;
}

static int set_segment_reg(struct task_struct *task,
			   unsigned long offset, u16 value)
{
	if (WARN_ON_ONCE(task == current))
		return -EIO;

	/*
	 * The value argument was already truncated to 16 bits.
	 */
	if (invalid_selector(value))
		return -EIO;

	/*
	 * For %cs and %ss we cannot permit a null selector.
	 * We can permit a bogus selector as long as it has USER_RPL.
	 * Null selectors are fine for other segment registers, but
	 * we will never get back to user mode with invalid %cs or %ss
	 * and will take the trap in iret instead.  Much code relies
	 * on user_mode() to distinguish a user trap frame (which can
	 * safely use invalid selectors) from a kernel trap frame.
	 */
	switch (offset) {
	case offsetof(struct user_regs_struct, cs):
	case offsetof(struct user_regs_struct, ss):
		if (unlikely(value == 0))
			return -EIO;
		fallthrough;

	default:
		*pt_regs_access(task_pt_regs(task), offset) = value;
		break;

	case offsetof(struct user_regs_struct, gs):
		task_user_gs(task) = value;
	}

	return 0;
}

#else  /* CONFIG_X86_64 */

#define FLAG_MASK		(FLAG_MASK_32 | X86_EFLAGS_NT)

static unsigned long *pt_regs_access(struct pt_regs *regs, unsigned long offset)
{
	BUILD_BUG_ON(offsetof(struct pt_regs, r15) != 0);
	return &regs->r15 + (offset / sizeof(regs->r15));
}

static u16 get_segment_reg(struct task_struct *task, unsigned long offset)
{
	/*
	 * Returning the value truncates it to 16 bits.
	 */
	unsigned int seg;

	switch (offset) {
	case offsetof(struct user_regs_struct, fs):
		if (task == current) {
			/* Older gas can't assemble movq %?s,%r?? */
			asm("movl %%fs,%0" : "=r" (seg));
			return seg;
		}
		return task->thread.fsindex;
	case offsetof(struct user_regs_struct, gs):
		if (task == current) {
			asm("movl %%gs,%0" : "=r" (seg));
			return seg;
		}
		return task->thread.gsindex;
	case offsetof(struct user_regs_struct, ds):
		if (task == current) {
			asm("movl %%ds,%0" : "=r" (seg));
			return seg;
		}
		return task->thread.ds;
	case offsetof(struct user_regs_struct, es):
		if (task == current) {
			asm("movl %%es,%0" : "=r" (seg));
			return seg;
		}
		return task->thread.es;

	case offsetof(struct user_regs_struct, cs):
	case offsetof(struct user_regs_struct, ss):
		break;
	}
	return *pt_regs_access(task_pt_regs(task), offset);
}

static int set_segment_reg(struct task_struct *task,
			   unsigned long offset, u16 value)
{
	if (WARN_ON_ONCE(task == current))
		return -EIO;

	/*
	 * The value argument was already truncated to 16 bits.
	 */
	if (invalid_selector(value))
		return -EIO;

	/*
	 * Writes to FS and GS will change the stored selector.  Whether
	 * this changes the segment base as well depends on whether
	 * FSGSBASE is enabled.
	 */

	switch (offset) {
	case offsetof(struct user_regs_struct,fs):
		task->thread.fsindex = value;
		break;
	case offsetof(struct user_regs_struct,gs):
		task->thread.gsindex = value;
		break;
	case offsetof(struct user_regs_struct,ds):
		task->thread.ds = value;
		break;
	case offsetof(struct user_regs_struct,es):
		task->thread.es = value;
		break;

		/*
		 * Can't actually change these in 64-bit mode.
		 */
	case offsetof(struct user_regs_struct,cs):
		if (unlikely(value == 0))
			return -EIO;
		task_pt_regs(task)->cs = value;
		break;
	case offsetof(struct user_regs_struct,ss):
		if (unlikely(value == 0))
			return -EIO;
		task_pt_regs(task)->ss = value;
		break;
	}

	return 0;
}

#endif	/* CONFIG_X86_32 */

static unsigned long get_flags(struct task_struct *task)
{
	unsigned long retval = task_pt_regs(task)->flags;

	/*
	 * If the debugger set TF, hide it from the readout.
	 */
	if (test_tsk_thread_flag(task, TIF_FORCED_TF))
		retval &= ~X86_EFLAGS_TF;

	return retval;
}

static int set_flags(struct task_struct *task, unsigned long value)
{
	struct pt_regs *regs = task_pt_regs(task);

	/*
	 * If the user value contains TF, mark that
	 * it was not "us" (the debugger) that set it.
	 * If not, make sure it stays set if we had.
	 */
	if (value & X86_EFLAGS_TF)
		clear_tsk_thread_flag(task, TIF_FORCED_TF);
	else if (test_tsk_thread_flag(task, TIF_FORCED_TF))
		value |= X86_EFLAGS_TF;

	regs->flags = (regs->flags & ~FLAG_MASK) | (value & FLAG_MASK);

	return 0;
}

static int putreg(struct task_struct *child,
		  unsigned long offset, unsigned long value)
{
	switch (offset) {
	case offsetof(struct user_regs_struct, cs):
	case offsetof(struct user_regs_struct, ds):
	case offsetof(struct user_regs_struct, es):
	case offsetof(struct user_regs_struct, fs):
	case offsetof(struct user_regs_struct, gs):
	case offsetof(struct user_regs_struct, ss):
		return set_segment_reg(child, offset, value);

	case offsetof(struct user_regs_struct, flags):
		return set_flags(child, value);

#ifdef CONFIG_X86_64
	case offsetof(struct user_regs_struct,fs_base):
		if (value >= TASK_SIZE_MAX)
			return -EIO;
		x86_fsbase_write_task(child, value);
		return 0;
	case offsetof(struct user_regs_struct,gs_base):
		if (value >= TASK_SIZE_MAX)
			return -EIO;
		x86_gsbase_write_task(child, value);
		return 0;
#endif
	}

	*pt_regs_access(task_pt_regs(child), offset) = value;
	return 0;
}

static unsigned long getreg(struct task_struct *task, unsigned long offset)
{
	switch (offset) {
	case offsetof(struct user_regs_struct, cs):
	case offsetof(struct user_regs_struct, ds):
	case offsetof(struct user_regs_struct, es):
	case offsetof(struct user_regs_struct, fs):
	case offsetof(struct user_regs_struct, gs):
	case offsetof(struct user_regs_struct, ss):
		return get_segment_reg(task, offset);

	case offsetof(struct user_regs_struct, flags):
		return get_flags(task);

#ifdef CONFIG_X86_64
	case offsetof(struct user_regs_struct, fs_base):
		return x86_fsbase_read_task(task);
	case offsetof(struct user_regs_struct, gs_base):
		return x86_gsbase_read_task(task);
#endif
	}

	return *pt_regs_access(task_pt_regs(task), offset);
}

static int genregs_get(struct task_struct *target,
		       const struct user_regset *regset,
		       struct membuf to)
{
	int reg;

	for (reg = 0; to.left; reg++)
		membuf_store(&to, getreg(target, reg * sizeof(unsigned long)));
	return 0;
}

static int genregs_set(struct task_struct *target,
		       const struct user_regset *regset,
		       unsigned int pos, unsigned int count,
		       const void *kbuf, const void __user *ubuf)
{
	int ret = 0;
	if (kbuf) {
		const unsigned long *k = kbuf;
		while (count >= sizeof(*k) && !ret) {
			ret = putreg(target, pos, *k++);
			count -= sizeof(*k);
			pos += sizeof(*k);
		}
	} else {
		const unsigned long  __user *u = ubuf;
		while (count >= sizeof(*u) && !ret) {
			unsigned long word;
			ret = __get_user(word, u++);
			if (ret)
				break;
			ret = putreg(target, pos, word);
			count -= sizeof(*u);
			pos += sizeof(*u);
		}
	}
	return ret;
}

static void ptrace_triggered(struct perf_event *bp,
			     struct perf_sample_data *data,
			     struct pt_regs *regs)
{
	int i;
	struct thread_struct *thread = &(current->thread);

	/*
	 * Store in the virtual DR6 register the fact that the breakpoint
	 * was hit so the thread's debugger will see it.
	 */
	for (i = 0; i < HBP_NUM; i++) {
		if (thread->ptrace_bps[i] == bp)
			break;
	}

	thread->virtual_dr6 |= (DR_TRAP0 << i);
}

/*
 * Walk through every ptrace breakpoints for this thread and
 * build the dr7 value on top of their attributes.
 *
 */
static unsigned long ptrace_get_dr7(struct perf_event *bp[])
{
	int i;
	int dr7 = 0;
	struct arch_hw_breakpoint *info;

	for (i = 0; i < HBP_NUM; i++) {
		if (bp[i] && !bp[i]->attr.disabled) {
			info = counter_arch_bp(bp[i]);
			dr7 |= encode_dr7(i, info->len, info->type);
		}
	}

	return dr7;
}

static int ptrace_fill_bp_fields(struct perf_event_attr *attr,
					int len, int type, bool disabled)
{
	int err, bp_len, bp_type;

	err = arch_bp_generic_fields(len, type, &bp_len, &bp_type);
	if (!err) {
		attr->bp_len = bp_len;
		attr->bp_type = bp_type;
		attr->disabled = disabled;
	}

	return err;
}

static struct perf_event *
ptrace_register_breakpoint(struct task_struct *tsk, int len, int type,
				unsigned long addr, bool disabled)
{
	struct perf_event_attr attr;
	int err;

	ptrace_breakpoint_init(&attr);
	attr.bp_addr = addr;

	err = ptrace_fill_bp_fields(&attr, len, type, disabled);
	if (err)
		return ERR_PTR(err);

	return register_user_hw_breakpoint(&attr, ptrace_triggered,
						 NULL, tsk);
}

static int ptrace_modify_breakpoint(struct perf_event *bp, int len, int type,
					int disabled)
{
	struct perf_event_attr attr = bp->attr;
	int err;

	err = ptrace_fill_bp_fields(&attr, len, type, disabled);
	if (err)
		return err;

	return modify_user_hw_breakpoint(bp, &attr);
}

/*
 * Handle ptrace writes to debug register 7.
 */
static int ptrace_write_dr7(struct task_struct *tsk, unsigned long data)
{
	struct thread_struct *thread = &tsk->thread;
	unsigned long old_dr7;
	bool second_pass = false;
	int i, rc, ret = 0;

	data &= ~DR_CONTROL_RESERVED;
	old_dr7 = ptrace_get_dr7(thread->ptrace_bps);

restore:
	rc = 0;
	for (i = 0; i < HBP_NUM; i++) {
		unsigned len, type;
		bool disabled = !decode_dr7(data, i, &len, &type);
		struct perf_event *bp = thread->ptrace_bps[i];

		if (!bp) {
			if (disabled)
				continue;

			bp = ptrace_register_breakpoint(tsk,
					len, type, 0, disabled);
			if (IS_ERR(bp)) {
				rc = PTR_ERR(bp);
				break;
			}

			thread->ptrace_bps[i] = bp;
			continue;
		}

		rc = ptrace_modify_breakpoint(bp, len, type, disabled);
		if (rc)
			break;
	}

	/* Restore if the first pass failed, second_pass shouldn't fail. */
	if (rc && !WARN_ON(second_pass)) {
		ret = rc;
		data = old_dr7;
		second_pass = true;
		goto restore;
	}

	return ret;
}

/*
 * Handle PTRACE_PEEKUSR calls for the debug register area.
 */
static unsigned long ptrace_get_debugreg(struct task_struct *tsk, int n)
{
	struct thread_struct *thread = &tsk->thread;
	unsigned long val = 0;

	if (n < HBP_NUM) {
		int index = array_index_nospec(n, HBP_NUM);
		struct perf_event *bp = thread->ptrace_bps[index];

		if (bp)
			val = bp->hw.info.address;
	} else if (n == 6) {
		val = thread->virtual_dr6 ^ DR6_RESERVED; /* Flip back to arch polarity */
	} else if (n == 7) {
		val = thread->ptrace_dr7;
	}
	return val;
}

static int ptrace_set_breakpoint_addr(struct task_struct *tsk, int nr,
				      unsigned long addr)
{
	struct thread_struct *t = &tsk->thread;
	struct perf_event *bp = t->ptrace_bps[nr];
	int err = 0;

	if (!bp) {
		/*
		 * Put stub len and type to create an inactive but correct bp.
		 *
		 * CHECKME: the previous code returned -EIO if the addr wasn't
		 * a valid task virtual addr. The new one will return -EINVAL in
		 *  this case.
		 * -EINVAL may be what we want for in-kernel breakpoints users,
		 * but -EIO looks better for ptrace, since we refuse a register
		 * writing for the user. And anyway this is the previous
		 * behaviour.
		 */
		bp = ptrace_register_breakpoint(tsk,
				X86_BREAKPOINT_LEN_1, X86_BREAKPOINT_WRITE,
				addr, true);
		if (IS_ERR(bp))
			err = PTR_ERR(bp);
		else
			t->ptrace_bps[nr] = bp;
	} else {
		struct perf_event_attr attr = bp->attr;

		attr.bp_addr = addr;
		err = modify_user_hw_breakpoint(bp, &attr);
	}

	return err;
}

/*
 * Handle PTRACE_POKEUSR calls for the debug register area.
 */
static int ptrace_set_debugreg(struct task_struct *tsk, int n,
			       unsigned long val)
{
	struct thread_struct *thread = &tsk->thread;
	/* There are no DR4 or DR5 registers */
	int rc = -EIO;

	if (n < HBP_NUM) {
		rc = ptrace_set_breakpoint_addr(tsk, n, val);
	} else if (n == 6) {
		thread->virtual_dr6 = val ^ DR6_RESERVED; /* Flip to positive polarity */
		rc = 0;
	} else if (n == 7) {
		rc = ptrace_write_dr7(tsk, val);
		if (!rc)
			thread->ptrace_dr7 = val;
	}
	return rc;
}

/*
 * These access the current or another (stopped) task's io permission
 * bitmap for debugging or core dump.
 */
static int ioperm_active(struct task_struct *target,
			 const struct user_regset *regset)
{
	struct io_bitmap *iobm = target->thread.io_bitmap;

	return iobm ? DIV_ROUND_UP(iobm->max, regset->size) : 0;
}

static int ioperm_get(struct task_struct *target,
		      const struct user_regset *regset,
		      struct membuf to)
{
	struct io_bitmap *iobm = target->thread.io_bitmap;

	if (!iobm)
		return -ENXIO;

	return membuf_write(&to, iobm->bitmap, IO_BITMAP_BYTES);
}

/*
 * Called by kernel/ptrace.c when detaching..
 *
 * Make sure the single step bit is not set.
 */
void ptrace_disable(struct task_struct *child)
{
	user_disable_single_step(child);
}

#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
static const struct user_regset_view user_x86_32_view; /* Initialized below. */
#endif
#ifdef CONFIG_X86_64
static const struct user_regset_view user_x86_64_view; /* Initialized below. */
#endif

long arch_ptrace(struct task_struct *child, long request,
		 unsigned long addr, unsigned long data)
{
	int ret;
	unsigned long __user *datap = (unsigned long __user *)data;

#ifdef CONFIG_X86_64
	/* This is native 64-bit ptrace() */
	const struct user_regset_view *regset_view = &user_x86_64_view;
#else
	/* This is native 32-bit ptrace() */
	const struct user_regset_view *regset_view = &user_x86_32_view;
#endif

	switch (request) {
	/* read the word at location addr in the USER area. */
	case PTRACE_PEEKUSR: {
		unsigned long tmp;

		ret = -EIO;
		if ((addr & (sizeof(data) - 1)) || addr >= sizeof(struct user))
			break;

		tmp = 0;  /* Default return condition */
		if (addr < sizeof(struct user_regs_struct))
			tmp = getreg(child, addr);
		else if (addr >= offsetof(struct user, u_debugreg[0]) &&
			 addr <= offsetof(struct user, u_debugreg[7])) {
			addr -= offsetof(struct user, u_debugreg[0]);
			tmp = ptrace_get_debugreg(child, addr / sizeof(data));
		}
		ret = put_user(tmp, datap);
		break;
	}

	case PTRACE_POKEUSR: /* write the word at location addr in the USER area */
		ret = -EIO;
		if ((addr & (sizeof(data) - 1)) || addr >= sizeof(struct user))
			break;

		if (addr < sizeof(struct user_regs_struct))
			ret = putreg(child, addr, data);
		else if (addr >= offsetof(struct user, u_debugreg[0]) &&
			 addr <= offsetof(struct user, u_debugreg[7])) {
			addr -= offsetof(struct user, u_debugreg[0]);
			ret = ptrace_set_debugreg(child,
						  addr / sizeof(data), data);
		}
		break;

	case PTRACE_GETREGS:	/* Get all gp regs from the child. */
		return copy_regset_to_user(child,
					   regset_view,
					   REGSET_GENERAL,
					   0, sizeof(struct user_regs_struct),
					   datap);

	case PTRACE_SETREGS:	/* Set all gp regs in the child. */
		return copy_regset_from_user(child,
					     regset_view,
					     REGSET_GENERAL,
					     0, sizeof(struct user_regs_struct),
					     datap);

	case PTRACE_GETFPREGS:	/* Get the child FPU state. */
		return copy_regset_to_user(child,
					   regset_view,
					   REGSET_FP,
					   0, sizeof(struct user_i387_struct),
					   datap);

	case PTRACE_SETFPREGS:	/* Set the child FPU state. */
		return copy_regset_from_user(child,
					     regset_view,
					     REGSET_FP,
					     0, sizeof(struct user_i387_struct),
					     datap);

#ifdef CONFIG_X86_32
	case PTRACE_GETFPXREGS:	/* Get the child extended FPU state. */
		return copy_regset_to_user(child, &user_x86_32_view,
					   REGSET_XFP,
					   0, sizeof(struct user_fxsr_struct),
					   datap) ? -EIO : 0;

	case PTRACE_SETFPXREGS:	/* Set the child extended FPU state. */
		return copy_regset_from_user(child, &user_x86_32_view,
					     REGSET_XFP,
					     0, sizeof(struct user_fxsr_struct),
					     datap) ? -EIO : 0;
#endif

#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
	case PTRACE_GET_THREAD_AREA:
		if ((int) addr < 0)
			return -EIO;
		ret = do_get_thread_area(child, addr,
					(struct user_desc __user *)data);
		break;

	case PTRACE_SET_THREAD_AREA:
		if ((int) addr < 0)
			return -EIO;
		ret = do_set_thread_area(child, addr,
					(struct user_desc __user *)data, 0);
		break;
#endif

#ifdef CONFIG_X86_64
		/* normal 64bit interface to access TLS data.
		   Works just like arch_prctl, except that the arguments
		   are reversed. */
	case PTRACE_ARCH_PRCTL:
		ret = do_arch_prctl_64(child, data, addr);
		break;
#endif

	default:
		ret = ptrace_request(child, request, addr, data);
		break;
	}

	return ret;
}

#ifdef CONFIG_IA32_EMULATION

#include <linux/compat.h>
#include <linux/syscalls.h>
#include <asm/ia32.h>
#include <asm/user32.h>

#define R32(l,q)							\
	case offsetof(struct user32, regs.l):				\
		regs->q = value; break

#define SEG32(rs)							\
	case offsetof(struct user32, regs.rs):				\
		return set_segment_reg(child,				\
				       offsetof(struct user_regs_struct, rs), \
				       value);				\
		break

static int putreg32(struct task_struct *child, unsigned regno, u32 value)
{
	struct pt_regs *regs = task_pt_regs(child);
	int ret;

	switch (regno) {

	SEG32(cs);
	SEG32(ds);
	SEG32(es);

	/*
	 * A 32-bit ptracer on a 64-bit kernel expects that writing
	 * FS or GS will also update the base.  This is needed for
	 * operations like PTRACE_SETREGS to fully restore a saved
	 * CPU state.
	 */

	case offsetof(struct user32, regs.fs):
		ret = set_segment_reg(child,
				      offsetof(struct user_regs_struct, fs),
				      value);
		if (ret == 0)
			child->thread.fsbase =
				x86_fsgsbase_read_task(child, value);
		return ret;

	case offsetof(struct user32, regs.gs):
		ret = set_segment_reg(child,
				      offsetof(struct user_regs_struct, gs),
				      value);
		if (ret == 0)
			child->thread.gsbase =
				x86_fsgsbase_read_task(child, value);
		return ret;

	SEG32(ss);

	R32(ebx, bx);
	R32(ecx, cx);
	R32(edx, dx);
	R32(edi, di);
	R32(esi, si);
	R32(ebp, bp);
	R32(eax, ax);
	R32(eip, ip);
	R32(esp, sp);

	case offsetof(struct user32, regs.orig_eax):
		/*
		 * Warning: bizarre corner case fixup here.  A 32-bit
		 * debugger setting orig_eax to -1 wants to disable
		 * syscall restart.  Make sure that the syscall
		 * restart code sign-extends orig_ax.  Also make sure
		 * we interpret the -ERESTART* codes correctly if
		 * loaded into regs->ax in case the task is not
		 * actually still sitting at the exit from a 32-bit
		 * syscall with TS_COMPAT still set.
		 */
		regs->orig_ax = value;
		if (syscall_get_nr(child, regs) >= 0)
			child->thread_info.status |= TS_I386_REGS_POKED;
		break;

	case offsetof(struct user32, regs.eflags):
		return set_flags(child, value);

	case offsetof(struct user32, u_debugreg[0]) ...
		offsetof(struct user32, u_debugreg[7]):
		regno -= offsetof(struct user32, u_debugreg[0]);
		return ptrace_set_debugreg(child, regno / 4, value);

	default:
		if (regno > sizeof(struct user32) || (regno & 3))
			return -EIO;

		/*
		 * Other dummy fields in the virtual user structure
		 * are ignored
		 */
		break;
	}
	return 0;
}

#undef R32
#undef SEG32

#define R32(l,q)							\
	case offsetof(struct user32, regs.l):				\
		*val = regs->q; break

#define SEG32(rs)							\
	case offsetof(struct user32, regs.rs):				\
		*val = get_segment_reg(child,				\
				       offsetof(struct user_regs_struct, rs)); \
		break

static int getreg32(struct task_struct *child, unsigned regno, u32 *val)
{
	struct pt_regs *regs = task_pt_regs(child);

	switch (regno) {

	SEG32(ds);
	SEG32(es);
	SEG32(fs);
	SEG32(gs);

	R32(cs, cs);
	R32(ss, ss);
	R32(ebx, bx);
	R32(ecx, cx);
	R32(edx, dx);
	R32(edi, di);
	R32(esi, si);
	R32(ebp, bp);
	R32(eax, ax);
	R32(orig_eax, orig_ax);
	R32(eip, ip);
	R32(esp, sp);

	case offsetof(struct user32, regs.eflags):
		*val = get_flags(child);
		break;

	case offsetof(struct user32, u_debugreg[0]) ...
		offsetof(struct user32, u_debugreg[7]):
		regno -= offsetof(struct user32, u_debugreg[0]);
		*val = ptrace_get_debugreg(child, regno / 4);
		break;

	default:
		if (regno > sizeof(struct user32) || (regno & 3))
			return -EIO;

		/*
		 * Other dummy fields in the virtual user structure
		 * are ignored
		 */
		*val = 0;
		break;
	}
	return 0;
}

#undef R32
#undef SEG32

static int genregs32_get(struct task_struct *target,
			 const struct user_regset *regset,
			 struct membuf to)
{
	int reg;

	for (reg = 0; to.left; reg++) {
		u32 val;
		getreg32(target, reg * 4, &val);
		membuf_store(&to, val);
	}
	return 0;
}

static int genregs32_set(struct task_struct *target,
			 const struct user_regset *regset,
			 unsigned int pos, unsigned int count,
			 const void *kbuf, const void __user *ubuf)
{
	int ret = 0;
	if (kbuf) {
		const compat_ulong_t *k = kbuf;
		while (count >= sizeof(*k) && !ret) {
			ret = putreg32(target, pos, *k++);
			count -= sizeof(*k);
			pos += sizeof(*k);
		}
	} else {
		const compat_ulong_t __user *u = ubuf;
		while (count >= sizeof(*u) && !ret) {
			compat_ulong_t word;
			ret = __get_user(word, u++);
			if (ret)
				break;
			ret = putreg32(target, pos, word);
			count -= sizeof(*u);
			pos += sizeof(*u);
		}
	}
	return ret;
}

static long ia32_arch_ptrace(struct task_struct *child, compat_long_t request,
			     compat_ulong_t caddr, compat_ulong_t cdata)
{
	unsigned long addr = caddr;
	unsigned long data = cdata;
	void __user *datap = compat_ptr(data);
	int ret;
	__u32 val;

	switch (request) {
	case PTRACE_PEEKUSR:
		ret = getreg32(child, addr, &val);
		if (ret == 0)
			ret = put_user(val, (__u32 __user *)datap);
		break;

	case PTRACE_POKEUSR:
		ret = putreg32(child, addr, data);
		break;

	case PTRACE_GETREGS:	/* Get all gp regs from the child. */
		return copy_regset_to_user(child, &user_x86_32_view,
					   REGSET_GENERAL,
					   0, sizeof(struct user_regs_struct32),
					   datap);

	case PTRACE_SETREGS:	/* Set all gp regs in the child. */
		return copy_regset_from_user(child, &user_x86_32_view,
					     REGSET_GENERAL, 0,
					     sizeof(struct user_regs_struct32),
					     datap);

	case PTRACE_GETFPREGS:	/* Get the child FPU state. */
		return copy_regset_to_user(child, &user_x86_32_view,
					   REGSET_FP, 0,
					   sizeof(struct user_i387_ia32_struct),
					   datap);

	case PTRACE_SETFPREGS:	/* Set the child FPU state. */
		return copy_regset_from_user(
			child, &user_x86_32_view, REGSET_FP,
			0, sizeof(struct user_i387_ia32_struct), datap);

	case PTRACE_GETFPXREGS:	/* Get the child extended FPU state. */
		return copy_regset_to_user(child, &user_x86_32_view,
					   REGSET_XFP, 0,
					   sizeof(struct user32_fxsr_struct),
					   datap);

	case PTRACE_SETFPXREGS:	/* Set the child extended FPU state. */
		return copy_regset_from_user(child, &user_x86_32_view,
					     REGSET_XFP, 0,
					     sizeof(struct user32_fxsr_struct),
					     datap);

	case PTRACE_GET_THREAD_AREA:
	case PTRACE_SET_THREAD_AREA:
		return arch_ptrace(child, request, addr, data);

	default:
		return compat_ptrace_request(child, request, addr, data);
	}

	return ret;
}
#endif /* CONFIG_IA32_EMULATION */

#ifdef CONFIG_X86_X32_ABI
static long x32_arch_ptrace(struct task_struct *child,
			    compat_long_t request, compat_ulong_t caddr,
			    compat_ulong_t cdata)
{
	unsigned long addr = caddr;
	unsigned long data = cdata;
	void __user *datap = compat_ptr(data);
	int ret;

	switch (request) {
	/* Read 32bits at location addr in the USER area.  Only allow
	   to return the lower 32bits of segment and debug registers.  */
	case PTRACE_PEEKUSR: {
		u32 tmp;

		ret = -EIO;
		if ((addr & (sizeof(data) - 1)) || addr >= sizeof(struct user) ||
		    addr < offsetof(struct user_regs_struct, cs))
			break;

		tmp = 0;  /* Default return condition */
		if (addr < sizeof(struct user_regs_struct))
			tmp = getreg(child, addr);
		else if (addr >= offsetof(struct user, u_debugreg[0]) &&
			 addr <= offsetof(struct user, u_debugreg[7])) {
			addr -= offsetof(struct user, u_debugreg[0]);
			tmp = ptrace_get_debugreg(child, addr / sizeof(data));
		}
		ret = put_user(tmp, (__u32 __user *)datap);
		break;
	}

	/* Write the word at location addr in the USER area.  Only allow
	   to update segment and debug registers with the upper 32bits
	   zero-extended. */
	case PTRACE_POKEUSR:
		ret = -EIO;
		if ((addr & (sizeof(data) - 1)) || addr >= sizeof(struct user) ||
		    addr < offsetof(struct user_regs_struct, cs))
			break;

		if (addr < sizeof(struct user_regs_struct))
			ret = putreg(child, addr, data);
		else if (addr >= offsetof(struct user, u_debugreg[0]) &&
			 addr <= offsetof(struct user, u_debugreg[7])) {
			addr -= offsetof(struct user, u_debugreg[0]);
			ret = ptrace_set_debugreg(child,
						  addr / sizeof(data), data);
		}
		break;

	case PTRACE_GETREGS:	/* Get all gp regs from the child. */
		return copy_regset_to_user(child,
					   &user_x86_64_view,
					   REGSET_GENERAL,
					   0, sizeof(struct user_regs_struct),
					   datap);

	case PTRACE_SETREGS:	/* Set all gp regs in the child. */
		return copy_regset_from_user(child,
					     &user_x86_64_view,
					     REGSET_GENERAL,
					     0, sizeof(struct user_regs_struct),
					     datap);

	case PTRACE_GETFPREGS:	/* Get the child FPU state. */
		return copy_regset_to_user(child,
					   &user_x86_64_view,
					   REGSET_FP,
					   0, sizeof(struct user_i387_struct),
					   datap);

	case PTRACE_SETFPREGS:	/* Set the child FPU state. */
		return copy_regset_from_user(child,
					     &user_x86_64_view,
					     REGSET_FP,
					     0, sizeof(struct user_i387_struct),
					     datap);

	default:
		return compat_ptrace_request(child, request, addr, data);
	}

	return ret;
}
#endif

#ifdef CONFIG_COMPAT
long compat_arch_ptrace(struct task_struct *child, compat_long_t request,
			compat_ulong_t caddr, compat_ulong_t cdata)
{
#ifdef CONFIG_X86_X32_ABI
	if (!in_ia32_syscall())
		return x32_arch_ptrace(child, request, caddr, cdata);
#endif
#ifdef CONFIG_IA32_EMULATION
	return ia32_arch_ptrace(child, request, caddr, cdata);
#else
	return 0;
#endif
}
#endif	/* CONFIG_COMPAT */

#ifdef CONFIG_X86_64

static struct user_regset x86_64_regsets[] __ro_after_init = {
	[REGSET_GENERAL] = {
		.core_note_type = NT_PRSTATUS,
		.n = sizeof(struct user_regs_struct) / sizeof(long),
		.size = sizeof(long), .align = sizeof(long),
		.regset_get = genregs_get, .set = genregs_set
	},
	[REGSET_FP] = {
		.core_note_type = NT_PRFPREG,
		.n = sizeof(struct user_i387_struct) / sizeof(long),
		.size = sizeof(long), .align = sizeof(long),
		.active = regset_xregset_fpregs_active, .regset_get = xfpregs_get, .set = xfpregs_set
	},
	[REGSET_XSTATE] = {
		.core_note_type = NT_X86_XSTATE,
		.size = sizeof(u64), .align = sizeof(u64),
		.active = xstateregs_active, .regset_get = xstateregs_get,
		.set = xstateregs_set
	},
	[REGSET_IOPERM64] = {
		.core_note_type = NT_386_IOPERM,
		.n = IO_BITMAP_LONGS,
		.size = sizeof(long), .align = sizeof(long),
		.active = ioperm_active, .regset_get = ioperm_get
	},
};

static const struct user_regset_view user_x86_64_view = {
	.name = "x86_64", .e_machine = EM_X86_64,
	.regsets = x86_64_regsets, .n = ARRAY_SIZE(x86_64_regsets)
};

#else  /* CONFIG_X86_32 */

#define user_regs_struct32	user_regs_struct
#define genregs32_get		genregs_get
#define genregs32_set		genregs_set

#endif	/* CONFIG_X86_64 */

#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
static struct user_regset x86_32_regsets[] __ro_after_init = {
	[REGSET_GENERAL] = {
		.core_note_type = NT_PRSTATUS,
		.n = sizeof(struct user_regs_struct32) / sizeof(u32),
		.size = sizeof(u32), .align = sizeof(u32),
		.regset_get = genregs32_get, .set = genregs32_set
	},
	[REGSET_FP] = {
		.core_note_type = NT_PRFPREG,
		.n = sizeof(struct user_i387_ia32_struct) / sizeof(u32),
		.size = sizeof(u32), .align = sizeof(u32),
		.active = regset_fpregs_active, .regset_get = fpregs_get, .set = fpregs_set
	},
	[REGSET_XFP] = {
		.core_note_type = NT_PRXFPREG,
		.n = sizeof(struct user32_fxsr_struct) / sizeof(u32),
		.size = sizeof(u32), .align = sizeof(u32),
		.active = regset_xregset_fpregs_active, .regset_get = xfpregs_get, .set = xfpregs_set
	},
	[REGSET_XSTATE] = {
		.core_note_type = NT_X86_XSTATE,
		.size = sizeof(u64), .align = sizeof(u64),
		.active = xstateregs_active, .regset_get = xstateregs_get,
		.set = xstateregs_set
	},
	[REGSET_TLS] = {
		.core_note_type = NT_386_TLS,
		.n = GDT_ENTRY_TLS_ENTRIES, .bias = GDT_ENTRY_TLS_MIN,
		.size = sizeof(struct user_desc),
		.align = sizeof(struct user_desc),
		.active = regset_tls_active,
		.regset_get = regset_tls_get, .set = regset_tls_set
	},
	[REGSET_IOPERM32] = {
		.core_note_type = NT_386_IOPERM,
		.n = IO_BITMAP_BYTES / sizeof(u32),
		.size = sizeof(u32), .align = sizeof(u32),
		.active = ioperm_active, .regset_get = ioperm_get
	},
};

static const struct user_regset_view user_x86_32_view = {
	.name = "i386", .e_machine = EM_386,
	.regsets = x86_32_regsets, .n = ARRAY_SIZE(x86_32_regsets)
};
#endif

/*
 * This represents bytes 464..511 in the memory layout exported through
 * the REGSET_XSTATE interface.
 */
u64 xstate_fx_sw_bytes[USER_XSTATE_FX_SW_WORDS];

void __init update_regset_xstate_info(unsigned int size, u64 xstate_mask)
{
#ifdef CONFIG_X86_64
	x86_64_regsets[REGSET_XSTATE].n = size / sizeof(u64);
#endif
#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
	x86_32_regsets[REGSET_XSTATE].n = size / sizeof(u64);
#endif
	xstate_fx_sw_bytes[USER_XSTATE_XCR0_WORD] = xstate_mask;
}

/*
 * This is used by the core dump code to decide which regset to dump.  The
 * core dump code writes out the resulting .e_machine and the corresponding
 * regsets.  This is suboptimal if the task is messing around with its CS.L
 * field, but at worst the core dump will end up missing some information.
 *
 * Unfortunately, it is also used by the broken PTRACE_GETREGSET and
 * PTRACE_SETREGSET APIs.  These APIs look at the .regsets field but have
 * no way to make sure that the e_machine they use matches the caller's
 * expectations.  The result is that the data format returned by
 * PTRACE_GETREGSET depends on the returned CS field (and even the offset
 * of the returned CS field depends on its value!) and the data format
 * accepted by PTRACE_SETREGSET is determined by the old CS value.  The
 * upshot is that it is basically impossible to use these APIs correctly.
 *
 * The best way to fix it in the long run would probably be to add new
 * improved ptrace() APIs to read and write registers reliably, possibly by
 * allowing userspace to select the ELF e_machine variant that they expect.
 */
const struct user_regset_view *task_user_regset_view(struct task_struct *task)
{
#ifdef CONFIG_IA32_EMULATION
	if (!user_64bit_mode(task_pt_regs(task)))
#endif
#if defined CONFIG_X86_32 || defined CONFIG_IA32_EMULATION
		return &user_x86_32_view;
#endif
#ifdef CONFIG_X86_64
	return &user_x86_64_view;
#endif
}

void send_sigtrap(struct pt_regs *regs, int error_code, int si_code)
{
	struct task_struct *tsk = current;

	tsk->thread.trap_nr = X86_TRAP_DB;
	tsk->thread.error_code = error_code;

	/* Send us the fake SIGTRAP */
	force_sig_fault(SIGTRAP, si_code,
			user_mode(regs) ? (void __user *)regs->ip : NULL);
}

void user_single_step_report(struct pt_regs *regs)
{
	send_sigtrap(regs, 0, TRAP_BRKPT);
}
