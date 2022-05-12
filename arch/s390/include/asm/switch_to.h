/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright IBM Corp. 1999, 2009
 *
 * Author(s): Martin Schwidefsky <schwidefsky@de.ibm.com>
 */

#ifndef __ASM_SWITCH_TO_H
#define __ASM_SWITCH_TO_H

#include <linux/thread_info.h>
#include <asm/fpu/api.h>
#include <asm/ptrace.h>
#include <asm/guarded_storage.h>

extern struct task_struct *__switch_to(void *, void *);
extern void update_cr_regs(struct task_struct *task);

static inline void save_access_regs(unsigned int *acrs)
{
	typedef struct { int _[NUM_ACRS]; } acrstype;

	asm volatile("stam 0,15,%0" : "=Q" (*(acrstype *)acrs));
}

static inline void restore_access_regs(unsigned int *acrs)
{
	typedef struct { int _[NUM_ACRS]; } acrstype;

	asm volatile("lam 0,15,%0" : : "Q" (*(acrstype *)acrs));
}

#define switch_to(prev, next, last) do {				\
	/* save_fpu_regs() sets the CIF_FPU flag, which enforces	\
	 * a restore of the floating point / vector registers as	\
	 * soon as the next task returns to user space			\
	 */								\
	save_fpu_regs();						\
	save_access_regs(&prev->thread.acrs[0]);			\
	save_ri_cb(prev->thread.ri_cb);					\
	save_gs_cb(prev->thread.gs_cb);					\
	update_cr_regs(next);						\
	restore_access_regs(&next->thread.acrs[0]);			\
	restore_ri_cb(next->thread.ri_cb, prev->thread.ri_cb);		\
	restore_gs_cb(next->thread.gs_cb);				\
	prev = __switch_to(prev, next);					\
} while (0)

#endif /* __ASM_SWITCH_TO_H */
