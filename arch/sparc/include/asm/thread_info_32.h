/* SPDX-License-Identifier: GPL-2.0 */
/*
 * thread_info.h: sparc low-level thread information
 * adapted from the ppc version by Pete Zaitcev, which was
 * adapted from the i386 version by Paul Mackerras
 *
 * Copyright (C) 2002  David Howells (dhowells@redhat.com)
 * Copyright (c) 2002  Pete Zaitcev (zaitcev@yahoo.com)
 * - Incorporating suggestions made by Linus Torvalds and Dave Miller
 */

#ifndef _ASM_THREAD_INFO_H
#define _ASM_THREAD_INFO_H

#ifdef __KERNEL__

#ifndef __ASSEMBLY__

#include <asm/ptrace.h>
#include <asm/page.h>

/*
 * Low level task data.
 *
 * If you change this, change the TI_* offsets below to match.
 */
#define NSWINS 8
struct thread_info {
	unsigned long		uwinmask;
	struct task_struct	*task;		/* main task structure */
	unsigned long		flags;		/* low level flags */
	int			cpu;		/* cpu we're on */
	int			preempt_count;	/* 0 => preemptable,
						   <0 => BUG */
	int			softirq_count;
	int			hardirq_count;

	u32 __unused;

	/* Context switch saved kernel state. */
	unsigned long ksp;	/* ... ksp __attribute__ ((aligned (8))); */
	unsigned long kpc;
	unsigned long kpsr;
	unsigned long kwim;

	/* A place to store user windows and stack pointers
	 * when the stack needs inspection.
	 */
	struct reg_window32	reg_window[NSWINS];	/* align for ldd! */
	unsigned long		rwbuf_stkptrs[NSWINS];
	unsigned long		w_saved;
};

/*
 * macros/functions for gaining access to the thread information structure
 */
#define INIT_THREAD_INFO(tsk)				\
{							\
	.uwinmask	=	0,			\
	.task		=	&tsk,			\
	.flags		=	0,			\
	.cpu		=	0,			\
	.preempt_count	=	INIT_PREEMPT_COUNT,	\
}

/* how to get the thread information struct from C */
register struct thread_info *current_thread_info_reg asm("g6");
#define current_thread_info()   (current_thread_info_reg)

/*
 * thread information allocation
 */
#define THREAD_SIZE_ORDER  1

#endif /* __ASSEMBLY__ */

/* Size of kernel stack for each process */
#define THREAD_SIZE		(2 * PAGE_SIZE)

/*
 * Offsets in thread_info structure, used in assembly code
 * The "#define REGWIN_SZ 0x40" was abolished, so no multiplications.
 */
#define TI_UWINMASK	0x00	/* uwinmask */
#define TI_TASK		0x04
#define TI_FLAGS	0x08
#define TI_CPU		0x0c
#define TI_PREEMPT	0x10	/* preempt_count */
#define TI_SOFTIRQ	0x14	/* softirq_count */
#define TI_HARDIRQ	0x18	/* hardirq_count */
#define TI_KSP		0x20	/* ksp */
#define TI_KPC		0x24	/* kpc (ldd'ed with kpc) */
#define TI_KPSR		0x28	/* kpsr */
#define TI_KWIM		0x2c	/* kwim (ldd'ed with kpsr) */
#define TI_REG_WINDOW	0x30
#define TI_RWIN_SPTRS	0x230
#define TI_W_SAVED	0x250

/*
 * thread information flag bit numbers
 */
#define TIF_SYSCALL_TRACE	0	/* syscall trace active */
#define TIF_NOTIFY_RESUME	1	/* callback before returning to user */
#define TIF_SIGPENDING		2	/* signal pending */
#define TIF_NEED_RESCHED	3	/* rescheduling necessary */
#define TIF_RESTORE_SIGMASK	4	/* restore signal mask in do_signal() */
#define TIF_NOTIFY_SIGNAL	5	/* signal notifications exist */
#define TIF_USEDFPU		8	/* FPU was used by this task
					 * this quantum (SMP) */
#define TIF_POLLING_NRFLAG	9	/* true if poll_idle() is polling
					 * TIF_NEED_RESCHED */
#define TIF_MEMDIE		10	/* is terminating due to OOM killer */

/* as above, but as bit values */
#define _TIF_SYSCALL_TRACE	(1<<TIF_SYSCALL_TRACE)
#define _TIF_NOTIFY_RESUME	(1<<TIF_NOTIFY_RESUME)
#define _TIF_SIGPENDING		(1<<TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1<<TIF_NEED_RESCHED)
#define _TIF_NOTIFY_SIGNAL	(1<<TIF_NOTIFY_SIGNAL)
#define _TIF_USEDFPU		(1<<TIF_USEDFPU)
#define _TIF_POLLING_NRFLAG	(1<<TIF_POLLING_NRFLAG)

#define _TIF_DO_NOTIFY_RESUME_MASK	(_TIF_NOTIFY_RESUME | \
					 _TIF_SIGPENDING | _TIF_NOTIFY_SIGNAL)

#define is_32bit_task()	(1)

#endif /* __KERNEL__ */

#endif /* _ASM_THREAD_INFO_H */
