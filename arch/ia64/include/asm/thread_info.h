/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2002-2003 Hewlett-Packard Co
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 */
#ifndef _ASM_IA64_THREAD_INFO_H
#define _ASM_IA64_THREAD_INFO_H

#ifndef ASM_OFFSETS_C
#include <asm/asm-offsets.h>
#endif
#include <asm/processor.h>
#include <asm/ptrace.h>

#define THREAD_SIZE			KERNEL_STACK_SIZE

#ifndef __ASSEMBLY__

/*
 * On IA-64, we want to keep the task structure and kernel stack together, so they can be
 * mapped by a single TLB entry and so they can be addressed by the "current" pointer
 * without having to do pointer masking.
 */
struct thread_info {
	struct task_struct *task;	/* XXX not really needed, except for dup_task_struct() */
	__u32 flags;			/* thread_info flags (see TIF_*) */
	__u32 cpu;			/* current CPU */
	__u32 last_cpu;			/* Last CPU thread ran on */
	__u32 status;			/* Thread synchronous flags */
	mm_segment_t addr_limit;	/* user-level address space limit */
	int preempt_count;		/* 0=premptable, <0=BUG; will also serve as bh-counter */
#ifdef CONFIG_VIRT_CPU_ACCOUNTING_NATIVE
	__u64 utime;
	__u64 stime;
	__u64 gtime;
	__u64 hardirq_time;
	__u64 softirq_time;
	__u64 idle_time;
	__u64 ac_stamp;
	__u64 ac_leave;
	__u64 ac_stime;
	__u64 ac_utime;
#endif
};

#define INIT_THREAD_INFO(tsk)			\
{						\
	.task		= &tsk,			\
	.flags		= 0,			\
	.cpu		= 0,			\
	.addr_limit	= KERNEL_DS,		\
	.preempt_count	= INIT_PREEMPT_COUNT,	\
}

#ifndef ASM_OFFSETS_C
/* how to get the thread information struct from C */
#define current_thread_info()	((struct thread_info *) ((char *) current + IA64_TASK_SIZE))
#define alloc_thread_stack_node(tsk, node)	\
		((unsigned long *) ((char *) (tsk) + IA64_TASK_SIZE))
#define task_thread_info(tsk)	((struct thread_info *) ((char *) (tsk) + IA64_TASK_SIZE))
#else
#define current_thread_info()	((struct thread_info *) 0)
#define alloc_thread_stack_node(tsk, node)	((unsigned long *) 0)
#define task_thread_info(tsk)	((struct thread_info *) 0)
#endif
#define free_thread_stack(tsk)	/* nothing */
#define task_stack_page(tsk)	((void *)(tsk))

#define __HAVE_THREAD_FUNCTIONS
#ifdef CONFIG_VIRT_CPU_ACCOUNTING_NATIVE
#define setup_thread_stack(p, org)			\
	*task_thread_info(p) = *task_thread_info(org);	\
	task_thread_info(p)->ac_stime = 0;		\
	task_thread_info(p)->ac_utime = 0;		\
	task_thread_info(p)->task = (p);
#else
#define setup_thread_stack(p, org) \
	*task_thread_info(p) = *task_thread_info(org); \
	task_thread_info(p)->task = (p);
#endif
#define end_of_stack(p) (unsigned long *)((void *)(p) + IA64_RBS_OFFSET)

#define alloc_task_struct_node(node)						\
({										\
	struct page *page = alloc_pages_node(node, GFP_KERNEL | __GFP_COMP,	\
					     KERNEL_STACK_SIZE_ORDER);		\
	struct task_struct *ret = page ? page_address(page) : NULL;		\
										\
	ret;									\
})
#define free_task_struct(tsk)	free_pages((unsigned long) (tsk), KERNEL_STACK_SIZE_ORDER)

#endif /* !__ASSEMBLY */

/*
 * thread information flags
 * - these are process state flags that various assembly files may need to access
 * - pending work-to-be-done flags are in least-significant 16 bits, other flags
 *   in top 16 bits
 */
#define TIF_SIGPENDING		0	/* signal pending */
#define TIF_NEED_RESCHED	1	/* rescheduling necessary */
#define TIF_SYSCALL_TRACE	2	/* syscall trace active */
#define TIF_SYSCALL_AUDIT	3	/* syscall auditing active */
#define TIF_SINGLESTEP		4	/* restore singlestep on return to user mode */
#define TIF_NOTIFY_SIGNAL	5	/* signal notification exist */
#define TIF_NOTIFY_RESUME	6	/* resumption notification requested */
#define TIF_MEMDIE		17	/* is terminating due to OOM killer */
#define TIF_MCA_INIT		18	/* this task is processing MCA or INIT */
#define TIF_DB_DISABLED		19	/* debug trap disabled for fsyscall */
#define TIF_RESTORE_RSE		21	/* user RBS is newer than kernel RBS */
#define TIF_POLLING_NRFLAG	22	/* idle is polling for TIF_NEED_RESCHED */

#define _TIF_SYSCALL_TRACE	(1 << TIF_SYSCALL_TRACE)
#define _TIF_SYSCALL_AUDIT	(1 << TIF_SYSCALL_AUDIT)
#define _TIF_SINGLESTEP		(1 << TIF_SINGLESTEP)
#define _TIF_SYSCALL_TRACEAUDIT	(_TIF_SYSCALL_TRACE|_TIF_SYSCALL_AUDIT|_TIF_SINGLESTEP)
#define _TIF_NOTIFY_RESUME	(1 << TIF_NOTIFY_RESUME)
#define _TIF_NOTIFY_SIGNAL	(1 << TIF_NOTIFY_SIGNAL)
#define _TIF_SIGPENDING		(1 << TIF_SIGPENDING)
#define _TIF_NEED_RESCHED	(1 << TIF_NEED_RESCHED)
#define _TIF_MCA_INIT		(1 << TIF_MCA_INIT)
#define _TIF_DB_DISABLED	(1 << TIF_DB_DISABLED)
#define _TIF_RESTORE_RSE	(1 << TIF_RESTORE_RSE)
#define _TIF_POLLING_NRFLAG	(1 << TIF_POLLING_NRFLAG)

/* "work to do on user-return" bits */
#define TIF_ALLWORK_MASK	(_TIF_SIGPENDING|_TIF_NOTIFY_RESUME|_TIF_SYSCALL_AUDIT|\
				 _TIF_NEED_RESCHED|_TIF_SYSCALL_TRACE|_TIF_NOTIFY_SIGNAL)
/* like TIF_ALLWORK_BITS but sans TIF_SYSCALL_TRACE or TIF_SYSCALL_AUDIT */
#define TIF_WORK_MASK		(TIF_ALLWORK_MASK&~(_TIF_SYSCALL_TRACE|_TIF_SYSCALL_AUDIT))

#endif /* _ASM_IA64_THREAD_INFO_H */
