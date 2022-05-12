/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Process/processor support for the Hexagon architecture
 *
 * Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.
 */

#ifndef _ASM_PROCESSOR_H
#define _ASM_PROCESSOR_H

#ifndef __ASSEMBLY__

#include <asm/mem-layout.h>
#include <asm/registers.h>
#include <asm/hexagon_vm.h>

/*  task_struct, defined elsewhere, is the "process descriptor" */
struct task_struct;

extern void start_thread(struct pt_regs *, unsigned long, unsigned long);

/*
 * thread_struct is supposed to be for context switch data.
 * Specifically, to hold the state necessary to perform switch_to...
 */
struct thread_struct {
	void *switch_sp;
};

/*
 * initializes thread_struct
 * The only thing we have in there is switch_sp
 * which doesn't really need to be initialized.
 */

#define INIT_THREAD { \
}

#define cpu_relax() __vmyield()

/*
 * Decides where the kernel will search for a free chunk of vm space during
 * mmaps.
 * See also arch_get_unmapped_area.
 * Doesn't affect if you have MAX_FIXED in the page flags set though...
 *
 * Apparently the convention is that ld.so will ask for "unmapped" private
 * memory to be allocated SOMEWHERE, but it also asks for memory explicitly
 * via MAP_FIXED at the lower * addresses starting at VA=0x0.
 *
 * If the two requests collide, you get authentic segfaulting action, so
 * you have to kick the "unmapped" base requests higher up.
 */
#define TASK_UNMAPPED_BASE	(PAGE_ALIGN(TASK_SIZE/3))


#define task_pt_regs(task) \
	((struct pt_regs *)(task_stack_page(task) + THREAD_SIZE) - 1)

#define KSTK_EIP(tsk) (pt_elr(task_pt_regs(tsk)))
#define KSTK_ESP(tsk) (pt_psp(task_pt_regs(tsk)))

/*  Free all resources held by a thread; defined in process.c  */
extern void release_thread(struct task_struct *dead_task);

/* Get wait channel for task P.  */
extern unsigned long get_wchan(struct task_struct *p);

/*  The following stuff is pretty HEXAGON specific.  */

/*  This is really just here for __switch_to.
    Offsets are pulled via asm-offsets.c  */

/*
 * No real reason why VM and native switch stacks should be different.
 * Ultimately this should merge.  Note that Rev C. ABI called out only
 * R24-27 as callee saved GPRs needing explicit attention (R29-31 being
 * dealt with automagically by allocframe), but the current ABI has
 * more, R16-R27.  By saving more, the worst case is that we waste some
 * cycles if building with the old compilers.
 */

struct hexagon_switch_stack {
	union {
		struct {
			unsigned long r16;
			unsigned long r17;
		};
		unsigned long long	r1716;
	};
	union {
		struct {
			unsigned long r18;
			unsigned long r19;
		};
		unsigned long long	r1918;
	};
	union {
		struct {
			unsigned long r20;
			unsigned long r21;
		};
		unsigned long long	r2120;
	};
	union {
		struct {
			unsigned long r22;
			unsigned long r23;
		};
		unsigned long long	r2322;
	};
	union {
		struct {
			unsigned long r24;
			unsigned long r25;
		};
		unsigned long long	r2524;
	};
	union {
		struct {
			unsigned long r26;
			unsigned long r27;
		};
		unsigned long long	r2726;
	};

	unsigned long		fp;
	unsigned long		lr;
};

#endif /* !__ASSEMBLY__ */

#endif
