// SPDX-License-Identifier: GPL-2.0-only
/*
 * Stack trace management functions
 *
 *  Copyright (C) 2006 Atsushi Nemoto <anemo@mba.ocn.ne.jp>
 */
#include <linux/sched.h>
#include <linux/sched/debug.h>
#include <linux/sched/task_stack.h>
#include <linux/stacktrace.h>
#include <linux/export.h>
#include <asm/stacktrace.h>

/*
 * Save stack-backtrace addresses into a stack_trace buffer:
 */
static void save_raw_context_stack(struct stack_trace *trace,
	unsigned long reg29, int savesched)
{
	unsigned long *sp = (unsigned long *)reg29;
	unsigned long addr;

	while (!kstack_end(sp)) {
		addr = *sp++;
		if (__kernel_text_address(addr) &&
		    (savesched || !in_sched_functions(addr))) {
			if (trace->skip > 0)
				trace->skip--;
			else
				trace->entries[trace->nr_entries++] = addr;
			if (trace->nr_entries >= trace->max_entries)
				break;
		}
	}
}

static void save_context_stack(struct stack_trace *trace,
	struct task_struct *tsk, struct pt_regs *regs, int savesched)
{
	unsigned long sp = regs->regs[29];
#ifdef CONFIG_KALLSYMS
	unsigned long ra = regs->regs[31];
	unsigned long pc = regs->cp0_epc;

	if (raw_show_trace || !__kernel_text_address(pc)) {
		unsigned long stack_page =
			(unsigned long)task_stack_page(tsk);
		if (stack_page && sp >= stack_page &&
		    sp <= stack_page + THREAD_SIZE - 32)
			save_raw_context_stack(trace, sp, savesched);
		return;
	}
	do {
		if (savesched || !in_sched_functions(pc)) {
			if (trace->skip > 0)
				trace->skip--;
			else
				trace->entries[trace->nr_entries++] = pc;
			if (trace->nr_entries >= trace->max_entries)
				break;
		}
		pc = unwind_stack(tsk, &sp, pc, &ra);
	} while (pc);
#else
	save_raw_context_stack(trace, sp, savesched);
#endif
}

/*
 * Save stack-backtrace addresses into a stack_trace buffer.
 */
void save_stack_trace(struct stack_trace *trace)
{
	save_stack_trace_tsk(current, trace);
}
EXPORT_SYMBOL_GPL(save_stack_trace);

void save_stack_trace_tsk(struct task_struct *tsk, struct stack_trace *trace)
{
	struct pt_regs dummyregs;
	struct pt_regs *regs = &dummyregs;

	WARN_ON(trace->nr_entries || !trace->max_entries);

	if (tsk != current) {
		regs->regs[29] = tsk->thread.reg29;
		regs->regs[31] = 0;
		regs->cp0_epc = tsk->thread.reg31;
	} else
		prepare_frametrace(regs);
	save_context_stack(trace, tsk, regs, tsk == current);
}
EXPORT_SYMBOL_GPL(save_stack_trace_tsk);
