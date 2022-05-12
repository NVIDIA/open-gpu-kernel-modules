// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *    PARISC Architecture-dependent parts of process handling
 *    based on the work for i386
 *
 *    Copyright (C) 1999-2003 Matthew Wilcox <willy at parisc-linux.org>
 *    Copyright (C) 2000 Martin K Petersen <mkp at mkp.net>
 *    Copyright (C) 2000 John Marvin <jsm at parisc-linux.org>
 *    Copyright (C) 2000 David Huggins-Daines <dhd with pobox.org>
 *    Copyright (C) 2000-2003 Paul Bame <bame at parisc-linux.org>
 *    Copyright (C) 2000 Philipp Rumpf <prumpf with tux.org>
 *    Copyright (C) 2000 David Kennedy <dkennedy with linuxcare.com>
 *    Copyright (C) 2000 Richard Hirst <rhirst with parisc-linux.org>
 *    Copyright (C) 2000 Grant Grundler <grundler with parisc-linux.org>
 *    Copyright (C) 2001 Alan Modra <amodra at parisc-linux.org>
 *    Copyright (C) 2001-2002 Ryan Bradetich <rbrad at parisc-linux.org>
 *    Copyright (C) 2001-2014 Helge Deller <deller@gmx.de>
 *    Copyright (C) 2002 Randolph Chung <tausq with parisc-linux.org>
 */

#include <stdarg.h>

#include <linux/elf.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/cpu.h>
#include <linux/module.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <linux/sched/debug.h>
#include <linux/sched/task.h>
#include <linux/sched/task_stack.h>
#include <linux/slab.h>
#include <linux/stddef.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>
#include <linux/uaccess.h>
#include <linux/rcupdate.h>
#include <linux/random.h>
#include <linux/nmi.h>

#include <asm/io.h>
#include <asm/asm-offsets.h>
#include <asm/assembly.h>
#include <asm/pdc.h>
#include <asm/pdc_chassis.h>
#include <asm/unwind.h>
#include <asm/sections.h>

#define COMMAND_GLOBAL  F_EXTEND(0xfffe0030)
#define CMD_RESET       5       /* reset any module */

/*
** The Wright Brothers and Gecko systems have a H/W problem
** (Lasi...'nuf said) may cause a broadcast reset to lockup
** the system. An HVERSION dependent PDC call was developed
** to perform a "safe", platform specific broadcast reset instead
** of kludging up all the code.
**
** Older machines which do not implement PDC_BROADCAST_RESET will
** return (with an error) and the regular broadcast reset can be
** issued. Obviously, if the PDC does implement PDC_BROADCAST_RESET
** the PDC call will not return (the system will be reset).
*/
void machine_restart(char *cmd)
{
#ifdef FASTBOOT_SELFTEST_SUPPORT
	/*
	 ** If user has modified the Firmware Selftest Bitmap,
	 ** run the tests specified in the bitmap after the
	 ** system is rebooted w/PDC_DO_RESET.
	 **
	 ** ftc_bitmap = 0x1AUL "Skip destructive memory tests"
	 **
	 ** Using "directed resets" at each processor with the MEM_TOC
	 ** vector cleared will also avoid running destructive
	 ** memory self tests. (Not implemented yet)
	 */
	if (ftc_bitmap) {
		pdc_do_firm_test_reset(ftc_bitmap);
	}
#endif
	/* set up a new led state on systems shipped with a LED State panel */
	pdc_chassis_send_status(PDC_CHASSIS_DIRECT_SHUTDOWN);
	
	/* "Normal" system reset */
	pdc_do_reset();

	/* Nope...box should reset with just CMD_RESET now */
	gsc_writel(CMD_RESET, COMMAND_GLOBAL);

	/* Wait for RESET to lay us to rest. */
	while (1) ;

}

void (*chassis_power_off)(void);

/*
 * This routine is called from sys_reboot to actually turn off the
 * machine 
 */
void machine_power_off(void)
{
	/* If there is a registered power off handler, call it. */
	if (chassis_power_off)
		chassis_power_off();

	/* Put the soft power button back under hardware control.
	 * If the user had already pressed the power button, the
	 * following call will immediately power off. */
	pdc_soft_power_button(0);
	
	pdc_chassis_send_status(PDC_CHASSIS_DIRECT_SHUTDOWN);

	/* ipmi_poweroff may have been installed. */
	if (pm_power_off)
		pm_power_off();
		
	/* It seems we have no way to power the system off via
	 * software. The user has to press the button himself. */

	printk(KERN_EMERG "System shut down completed.\n"
	       "Please power this system off now.");

	/* prevent soft lockup/stalled CPU messages for endless loop. */
	rcu_sysrq_start();
	lockup_detector_soft_poweroff();
	for (;;);
}

void (*pm_power_off)(void);
EXPORT_SYMBOL(pm_power_off);

void machine_halt(void)
{
	machine_power_off();
}

void flush_thread(void)
{
	/* Only needs to handle fpu stuff or perf monitors.
	** REVISIT: several arches implement a "lazy fpu state".
	*/
}

void release_thread(struct task_struct *dead_task)
{
}

/*
 * Idle thread support
 *
 * Detect when running on QEMU with SeaBIOS PDC Firmware and let
 * QEMU idle the host too.
 */

int running_on_qemu __ro_after_init;
EXPORT_SYMBOL(running_on_qemu);

void __cpuidle arch_cpu_idle_dead(void)
{
	/* nop on real hardware, qemu will offline CPU. */
	asm volatile("or %%r31,%%r31,%%r31\n":::);
}

void __cpuidle arch_cpu_idle(void)
{
	raw_local_irq_enable();

	/* nop on real hardware, qemu will idle sleep. */
	asm volatile("or %%r10,%%r10,%%r10\n":::);
}

static int __init parisc_idle_init(void)
{
	if (!running_on_qemu)
		cpu_idle_poll_ctrl(1);

	return 0;
}
arch_initcall(parisc_idle_init);

/*
 * Copy architecture-specific thread state
 */
int
copy_thread(unsigned long clone_flags, unsigned long usp,
	    unsigned long kthread_arg, struct task_struct *p, unsigned long tls)
{
	struct pt_regs *cregs = &(p->thread.regs);
	void *stack = task_stack_page(p);
	
	/* We have to use void * instead of a function pointer, because
	 * function pointers aren't a pointer to the function on 64-bit.
	 * Make them const so the compiler knows they live in .text */
	extern void * const ret_from_kernel_thread;
	extern void * const child_return;

	if (unlikely(p->flags & (PF_KTHREAD | PF_IO_WORKER))) {
		/* kernel thread */
		memset(cregs, 0, sizeof(struct pt_regs));
		if (!usp) /* idle thread */
			return 0;
		/* Must exit via ret_from_kernel_thread in order
		 * to call schedule_tail()
		 */
		cregs->ksp = (unsigned long)stack + THREAD_SZ_ALGN + FRAME_SIZE;
		cregs->kpc = (unsigned long) &ret_from_kernel_thread;
		/*
		 * Copy function and argument to be called from
		 * ret_from_kernel_thread.
		 */
#ifdef CONFIG_64BIT
		cregs->gr[27] = ((unsigned long *)usp)[3];
		cregs->gr[26] = ((unsigned long *)usp)[2];
#else
		cregs->gr[26] = usp;
#endif
		cregs->gr[25] = kthread_arg;
	} else {
		/* user thread */
		/* usp must be word aligned.  This also prevents users from
		 * passing in the value 1 (which is the signal for a special
		 * return for a kernel thread) */
		if (usp) {
			usp = ALIGN(usp, 4);
			if (likely(usp))
				cregs->gr[30] = usp;
		}
		cregs->ksp = (unsigned long)stack + THREAD_SZ_ALGN + FRAME_SIZE;
		cregs->kpc = (unsigned long) &child_return;

		/* Setup thread TLS area */
		if (clone_flags & CLONE_SETTLS)
			cregs->cr27 = tls;
	}

	return 0;
}

unsigned long
get_wchan(struct task_struct *p)
{
	struct unwind_frame_info info;
	unsigned long ip;
	int count = 0;

	if (!p || p == current || p->state == TASK_RUNNING)
		return 0;

	/*
	 * These bracket the sleeping functions..
	 */

	unwind_frame_init_from_blocked_task(&info, p);
	do {
		if (unwind_once(&info) < 0)
			return 0;
		if (p->state == TASK_RUNNING)
                        return 0;
		ip = info.ip;
		if (!in_sched_functions(ip))
			return ip;
	} while (count++ < MAX_UNWIND_ENTRIES);
	return 0;
}

#ifdef CONFIG_64BIT
void *dereference_function_descriptor(void *ptr)
{
	Elf64_Fdesc *desc = ptr;
	void *p;

	if (!get_kernel_nofault(p, (void *)&desc->addr))
		ptr = p;
	return ptr;
}

void *dereference_kernel_function_descriptor(void *ptr)
{
	if (ptr < (void *)__start_opd ||
			ptr >= (void *)__end_opd)
		return ptr;

	return dereference_function_descriptor(ptr);
}
#endif

static inline unsigned long brk_rnd(void)
{
	return (get_random_int() & BRK_RND_MASK) << PAGE_SHIFT;
}

unsigned long arch_randomize_brk(struct mm_struct *mm)
{
	unsigned long ret = PAGE_ALIGN(mm->brk + brk_rnd());

	if (ret < mm->brk)
		return mm->brk;
	return ret;
}
