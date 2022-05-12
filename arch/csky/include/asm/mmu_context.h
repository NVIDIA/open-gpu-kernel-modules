/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __ASM_CSKY_MMU_CONTEXT_H
#define __ASM_CSKY_MMU_CONTEXT_H

#include <asm-generic/mm_hooks.h>
#include <asm/setup.h>
#include <asm/page.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>

#include <linux/errno.h>
#include <linux/sched.h>
#include <abi/ckmmu.h>

#define ASID_MASK		((1 << CONFIG_CPU_ASID_BITS) - 1)
#define cpu_asid(mm)		(atomic64_read(&mm->context.asid) & ASID_MASK)

#define init_new_context(tsk,mm)	({ atomic64_set(&(mm)->context.asid, 0); 0; })

void check_and_switch_context(struct mm_struct *mm, unsigned int cpu);

static inline void
switch_mm(struct mm_struct *prev, struct mm_struct *next,
	  struct task_struct *tsk)
{
	unsigned int cpu = smp_processor_id();

	if (prev != next)
		check_and_switch_context(next, cpu);

	setup_pgd(next->pgd, next->context.asid.counter);

	flush_icache_deferred(next);
}

#include <asm-generic/mmu_context.h>

#endif /* __ASM_CSKY_MMU_CONTEXT_H */
