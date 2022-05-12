/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * OpenRISC Linux
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * OpenRISC implementation:
 * Copyright (C) 2003 Matjaz Breskvar <phoenix@bsemi.com>
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 * et al.
 */

#ifndef __ASM_OPENRISC_PROCESSOR_H
#define __ASM_OPENRISC_PROCESSOR_H

#include <asm/spr_defs.h>
#include <asm/page.h>
#include <asm/ptrace.h>

#define STACK_TOP       TASK_SIZE
#define STACK_TOP_MAX	STACK_TOP
/* Kernel and user SR register setting */
#define KERNEL_SR (SPR_SR_DME | SPR_SR_IME | SPR_SR_ICE \
		   | SPR_SR_DCE | SPR_SR_SM)
#define USER_SR   (SPR_SR_DME | SPR_SR_IME | SPR_SR_ICE \
		   | SPR_SR_DCE | SPR_SR_IEE | SPR_SR_TEE)

/*
 * User space process size. This is hardcoded into a few places,
 * so don't change it unless you know what you are doing.
 */

#define TASK_SIZE       (0x80000000UL)

/* This decides where the kernel will search for a free chunk of vm
 * space during mmap's.
 */
#define TASK_UNMAPPED_BASE      (TASK_SIZE / 8 * 3)

#ifndef __ASSEMBLY__

struct task_struct;

struct thread_struct {
};

/*
 * At user->kernel entry, the pt_regs struct is stacked on the top of the
 * kernel-stack.  This macro allows us to find those regs for a task.
 * Notice that subsequent pt_regs stackings, like recursive interrupts
 * occurring while we're in the kernel, won't affect this - only the first
 * user->kernel transition registers are reached by this (i.e. not regs
 * for running signal handler)
 */
#define user_regs(thread_info)  (((struct pt_regs *)((unsigned long)(thread_info) + THREAD_SIZE - STACK_FRAME_OVERHEAD)) - 1)

/*
 * Dito but for the currently running task
 */

#define task_pt_regs(task) user_regs(task_thread_info(task))

#define INIT_SP         (sizeof(init_stack) + (unsigned long) &init_stack)

#define INIT_THREAD  { }


#define KSTK_EIP(tsk)   (task_pt_regs(tsk)->pc)
#define KSTK_ESP(tsk)   (task_pt_regs(tsk)->sp)


void start_thread(struct pt_regs *regs, unsigned long nip, unsigned long sp);
void release_thread(struct task_struct *);
unsigned long get_wchan(struct task_struct *p);

#define cpu_relax()     barrier()

#endif /* __ASSEMBLY__ */
#endif /* __ASM_OPENRISC_PROCESSOR_H */
