// SPDX-License-Identifier: GPL-2.0
/*
 *  linux/arch/alpha/kernel/process.c
 *
 *  Copyright (C) 1995  Linus Torvalds
 */

/*
 * This file handles the architecture-dependent parts of process handling.
 */

#include <linux/errno.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/debug.h>
#include <linux/sched/task.h>
#include <linux/sched/task_stack.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/ptrace.h>
#include <linux/user.h>
#include <linux/time.h>
#include <linux/major.h>
#include <linux/stat.h>
#include <linux/vt.h>
#include <linux/mman.h>
#include <linux/elfcore.h>
#include <linux/reboot.h>
#include <linux/tty.h>
#include <linux/console.h>
#include <linux/slab.h>
#include <linux/rcupdate.h>

#include <asm/reg.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <asm/hwrpb.h>
#include <asm/fpu.h>

#include "proto.h"
#include "pci_impl.h"

/*
 * Power off function, if any
 */
void (*pm_power_off)(void) = machine_power_off;
EXPORT_SYMBOL(pm_power_off);

#ifdef CONFIG_ALPHA_WTINT
/*
 * Sleep the CPU.
 * EV6, LCA45 and QEMU know how to power down, skipping N timer interrupts.
 */
void arch_cpu_idle(void)
{
	wtint(0);
	raw_local_irq_enable();
}

void arch_cpu_idle_dead(void)
{
	wtint(INT_MAX);
}
#endif /* ALPHA_WTINT */

struct halt_info {
	int mode;
	char *restart_cmd;
};

static void
common_shutdown_1(void *generic_ptr)
{
	struct halt_info *how = (struct halt_info *)generic_ptr;
	struct percpu_struct *cpup;
	unsigned long *pflags, flags;
	int cpuid = smp_processor_id();

	/* No point in taking interrupts anymore. */
	local_irq_disable();

	cpup = (struct percpu_struct *)
			((unsigned long)hwrpb + hwrpb->processor_offset
			 + hwrpb->processor_size * cpuid);
	pflags = &cpup->flags;
	flags = *pflags;

	/* Clear reason to "default"; clear "bootstrap in progress". */
	flags &= ~0x00ff0001UL;

#ifdef CONFIG_SMP
	/* Secondaries halt here. */
	if (cpuid != boot_cpuid) {
		flags |= 0x00040000UL; /* "remain halted" */
		*pflags = flags;
		set_cpu_present(cpuid, false);
		set_cpu_possible(cpuid, false);
		halt();
	}
#endif

	if (how->mode == LINUX_REBOOT_CMD_RESTART) {
		if (!how->restart_cmd) {
			flags |= 0x00020000UL; /* "cold bootstrap" */
		} else {
			/* For SRM, we could probably set environment
			   variables to get this to work.  We'd have to
			   delay this until after srm_paging_stop unless
			   we ever got srm_fixup working.

			   At the moment, SRM will use the last boot device,
			   but the file and flags will be the defaults, when
			   doing a "warm" bootstrap.  */
			flags |= 0x00030000UL; /* "warm bootstrap" */
		}
	} else {
		flags |= 0x00040000UL; /* "remain halted" */
	}
	*pflags = flags;

#ifdef CONFIG_SMP
	/* Wait for the secondaries to halt. */
	set_cpu_present(boot_cpuid, false);
	set_cpu_possible(boot_cpuid, false);
	while (cpumask_weight(cpu_present_mask))
		barrier();
#endif

	/* If booted from SRM, reset some of the original environment. */
	if (alpha_using_srm) {
#ifdef CONFIG_DUMMY_CONSOLE
		/* If we've gotten here after SysRq-b, leave interrupt
		   context before taking over the console. */
		if (in_irq())
			irq_exit();
		/* This has the effect of resetting the VGA video origin.  */
		console_lock();
		do_take_over_console(&dummy_con, 0, MAX_NR_CONSOLES-1, 1);
		console_unlock();
#endif
		pci_restore_srm_config();
		set_hae(srm_hae);
	}

	if (alpha_mv.kill_arch)
		alpha_mv.kill_arch(how->mode);

	if (! alpha_using_srm && how->mode != LINUX_REBOOT_CMD_RESTART) {
		/* Unfortunately, since MILO doesn't currently understand
		   the hwrpb bits above, we can't reliably halt the 
		   processor and keep it halted.  So just loop.  */
		return;
	}

	if (alpha_using_srm)
		srm_paging_stop();

	halt();
}

static void
common_shutdown(int mode, char *restart_cmd)
{
	struct halt_info args;
	args.mode = mode;
	args.restart_cmd = restart_cmd;
	on_each_cpu(common_shutdown_1, &args, 0);
}

void
machine_restart(char *restart_cmd)
{
	common_shutdown(LINUX_REBOOT_CMD_RESTART, restart_cmd);
}


void
machine_halt(void)
{
	common_shutdown(LINUX_REBOOT_CMD_HALT, NULL);
}


void
machine_power_off(void)
{
	common_shutdown(LINUX_REBOOT_CMD_POWER_OFF, NULL);
}


/* Used by sysrq-p, among others.  I don't believe r9-r15 are ever
   saved in the context it's used.  */

void
show_regs(struct pt_regs *regs)
{
	show_regs_print_info(KERN_DEFAULT);
	dik_show_regs(regs, NULL);
}

/*
 * Re-start a thread when doing execve()
 */
void
start_thread(struct pt_regs * regs, unsigned long pc, unsigned long sp)
{
	regs->pc = pc;
	regs->ps = 8;
	wrusp(sp);
}
EXPORT_SYMBOL(start_thread);

void
flush_thread(void)
{
	/* Arrange for each exec'ed process to start off with a clean slate
	   with respect to the FPU.  This is all exceptions disabled.  */
	current_thread_info()->ieee_state = 0;
	wrfpcr(FPCR_DYN_NORMAL | ieee_swcr_to_fpcr(0));

	/* Clean slate for TLS.  */
	current_thread_info()->pcb.unique = 0;
}

void
release_thread(struct task_struct *dead_task)
{
}

/*
 * Copy architecture-specific thread state
 */
int copy_thread(unsigned long clone_flags, unsigned long usp,
		unsigned long kthread_arg, struct task_struct *p,
		unsigned long tls)
{
	extern void ret_from_fork(void);
	extern void ret_from_kernel_thread(void);

	struct thread_info *childti = task_thread_info(p);
	struct pt_regs *childregs = task_pt_regs(p);
	struct pt_regs *regs = current_pt_regs();
	struct switch_stack *childstack, *stack;

	childstack = ((struct switch_stack *) childregs) - 1;
	childti->pcb.ksp = (unsigned long) childstack;
	childti->pcb.flags = 1;	/* set FEN, clear everything else */

	if (unlikely(p->flags & (PF_KTHREAD | PF_IO_WORKER))) {
		/* kernel thread */
		memset(childstack, 0,
			sizeof(struct switch_stack) + sizeof(struct pt_regs));
		childstack->r26 = (unsigned long) ret_from_kernel_thread;
		childstack->r9 = usp;	/* function */
		childstack->r10 = kthread_arg;
		childregs->hae = alpha_mv.hae_cache,
		childti->pcb.usp = 0;
		return 0;
	}
	/* Note: if CLONE_SETTLS is not set, then we must inherit the
	   value from the parent, which will have been set by the block
	   copy in dup_task_struct.  This is non-intuitive, but is
	   required for proper operation in the case of a threaded
	   application calling fork.  */
	if (clone_flags & CLONE_SETTLS)
		childti->pcb.unique = tls;
	else
		regs->r20 = 0;	/* OSF/1 has some strange fork() semantics.  */
	childti->pcb.usp = usp ?: rdusp();
	*childregs = *regs;
	childregs->r0 = 0;
	childregs->r19 = 0;
	childregs->r20 = 1;	/* OSF/1 has some strange fork() semantics.  */
	stack = ((struct switch_stack *) regs) - 1;
	*childstack = *stack;
	childstack->r26 = (unsigned long) ret_from_fork;
	return 0;
}

/*
 * Fill in the user structure for a ELF core dump.
 */
void
dump_elf_thread(elf_greg_t *dest, struct pt_regs *pt, struct thread_info *ti)
{
	/* switch stack follows right below pt_regs: */
	struct switch_stack * sw = ((struct switch_stack *) pt) - 1;

	dest[ 0] = pt->r0;
	dest[ 1] = pt->r1;
	dest[ 2] = pt->r2;
	dest[ 3] = pt->r3;
	dest[ 4] = pt->r4;
	dest[ 5] = pt->r5;
	dest[ 6] = pt->r6;
	dest[ 7] = pt->r7;
	dest[ 8] = pt->r8;
	dest[ 9] = sw->r9;
	dest[10] = sw->r10;
	dest[11] = sw->r11;
	dest[12] = sw->r12;
	dest[13] = sw->r13;
	dest[14] = sw->r14;
	dest[15] = sw->r15;
	dest[16] = pt->r16;
	dest[17] = pt->r17;
	dest[18] = pt->r18;
	dest[19] = pt->r19;
	dest[20] = pt->r20;
	dest[21] = pt->r21;
	dest[22] = pt->r22;
	dest[23] = pt->r23;
	dest[24] = pt->r24;
	dest[25] = pt->r25;
	dest[26] = pt->r26;
	dest[27] = pt->r27;
	dest[28] = pt->r28;
	dest[29] = pt->gp;
	dest[30] = ti == current_thread_info() ? rdusp() : ti->pcb.usp;
	dest[31] = pt->pc;

	/* Once upon a time this was the PS value.  Which is stupid
	   since that is always 8 for usermode.  Usurped for the more
	   useful value of the thread's UNIQUE field.  */
	dest[32] = ti->pcb.unique;
}
EXPORT_SYMBOL(dump_elf_thread);

int
dump_elf_task(elf_greg_t *dest, struct task_struct *task)
{
	dump_elf_thread(dest, task_pt_regs(task), task_thread_info(task));
	return 1;
}
EXPORT_SYMBOL(dump_elf_task);

int
dump_elf_task_fp(elf_fpreg_t *dest, struct task_struct *task)
{
	struct switch_stack *sw = (struct switch_stack *)task_pt_regs(task) - 1;
	memcpy(dest, sw->fp, 32 * 8);
	return 1;
}
EXPORT_SYMBOL(dump_elf_task_fp);

/*
 * Return saved PC of a blocked thread.  This assumes the frame
 * pointer is the 6th saved long on the kernel stack and that the
 * saved return address is the first long in the frame.  This all
 * holds provided the thread blocked through a call to schedule() ($15
 * is the frame pointer in schedule() and $15 is saved at offset 48 by
 * entry.S:do_switch_stack).
 *
 * Under heavy swap load I've seen this lose in an ugly way.  So do
 * some extra sanity checking on the ranges we expect these pointers
 * to be in so that we can fail gracefully.  This is just for ps after
 * all.  -- r~
 */

static unsigned long
thread_saved_pc(struct task_struct *t)
{
	unsigned long base = (unsigned long)task_stack_page(t);
	unsigned long fp, sp = task_thread_info(t)->pcb.ksp;

	if (sp > base && sp+6*8 < base + 16*1024) {
		fp = ((unsigned long*)sp)[6];
		if (fp > sp && fp < base + 16*1024)
			return *(unsigned long *)fp;
	}

	return 0;
}

unsigned long
get_wchan(struct task_struct *p)
{
	unsigned long schedule_frame;
	unsigned long pc;
	if (!p || p == current || p->state == TASK_RUNNING)
		return 0;
	/*
	 * This one depends on the frame size of schedule().  Do a
	 * "disass schedule" in gdb to find the frame size.  Also, the
	 * code assumes that sleep_on() follows immediately after
	 * interruptible_sleep_on() and that add_timer() follows
	 * immediately after interruptible_sleep().  Ugly, isn't it?
	 * Maybe adding a wchan field to task_struct would be better,
	 * after all...
	 */

	pc = thread_saved_pc(p);
	if (in_sched_functions(pc)) {
		schedule_frame = ((unsigned long *)task_thread_info(p)->pcb.ksp)[6];
		return ((unsigned long *)schedule_frame)[12];
	}
	return pc;
}
