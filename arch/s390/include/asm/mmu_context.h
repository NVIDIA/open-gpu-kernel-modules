/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  S390 version
 *
 *  Derived from "include/asm-i386/mmu_context.h"
 */

#ifndef __S390_MMU_CONTEXT_H
#define __S390_MMU_CONTEXT_H

#include <asm/pgalloc.h>
#include <linux/uaccess.h>
#include <linux/mm_types.h>
#include <asm/tlbflush.h>
#include <asm/ctl_reg.h>
#include <asm-generic/mm_hooks.h>

#define init_new_context init_new_context
static inline int init_new_context(struct task_struct *tsk,
				   struct mm_struct *mm)
{
	unsigned long asce_type, init_entry;

	spin_lock_init(&mm->context.lock);
	INIT_LIST_HEAD(&mm->context.pgtable_list);
	INIT_LIST_HEAD(&mm->context.gmap_list);
	cpumask_clear(&mm->context.cpu_attach_mask);
	atomic_set(&mm->context.flush_count, 0);
	atomic_set(&mm->context.is_protected, 0);
	mm->context.gmap_asce = 0;
	mm->context.flush_mm = 0;
#ifdef CONFIG_PGSTE
	mm->context.alloc_pgste = page_table_allocate_pgste ||
		test_thread_flag(TIF_PGSTE) ||
		(current->mm && current->mm->context.alloc_pgste);
	mm->context.has_pgste = 0;
	mm->context.uses_skeys = 0;
	mm->context.uses_cmm = 0;
	mm->context.allow_gmap_hpage_1m = 0;
#endif
	switch (mm->context.asce_limit) {
	default:
		/*
		 * context created by exec, the value of asce_limit can
		 * only be zero in this case
		 */
		VM_BUG_ON(mm->context.asce_limit);
		/* continue as 3-level task */
		mm->context.asce_limit = _REGION2_SIZE;
		fallthrough;
	case _REGION2_SIZE:
		/* forked 3-level task */
		init_entry = _REGION3_ENTRY_EMPTY;
		asce_type = _ASCE_TYPE_REGION3;
		break;
	case TASK_SIZE_MAX:
		/* forked 5-level task */
		init_entry = _REGION1_ENTRY_EMPTY;
		asce_type = _ASCE_TYPE_REGION1;
		break;
	case _REGION1_SIZE:
		/* forked 4-level task */
		init_entry = _REGION2_ENTRY_EMPTY;
		asce_type = _ASCE_TYPE_REGION2;
		break;
	}
	mm->context.asce = __pa(mm->pgd) | _ASCE_TABLE_LENGTH |
			   _ASCE_USER_BITS | asce_type;
	crst_table_init((unsigned long *) mm->pgd, init_entry);
	return 0;
}

static inline void switch_mm(struct mm_struct *prev, struct mm_struct *next,
			     struct task_struct *tsk)
{
	int cpu = smp_processor_id();

	if (next == &init_mm)
		S390_lowcore.user_asce = s390_invalid_asce;
	else
		S390_lowcore.user_asce = next->context.asce;
	cpumask_set_cpu(cpu, &next->context.cpu_attach_mask);
	/* Clear previous user-ASCE from CR7 */
	__ctl_load(s390_invalid_asce, 7, 7);
	if (prev != next)
		cpumask_clear_cpu(cpu, &prev->context.cpu_attach_mask);
}

#define finish_arch_post_lock_switch finish_arch_post_lock_switch
static inline void finish_arch_post_lock_switch(void)
{
	struct task_struct *tsk = current;
	struct mm_struct *mm = tsk->mm;

	if (mm) {
		preempt_disable();
		while (atomic_read(&mm->context.flush_count))
			cpu_relax();
		cpumask_set_cpu(smp_processor_id(), mm_cpumask(mm));
		__tlb_flush_mm_lazy(mm);
		preempt_enable();
	}
	__ctl_load(S390_lowcore.user_asce, 7, 7);
}

#define activate_mm activate_mm
static inline void activate_mm(struct mm_struct *prev,
                               struct mm_struct *next)
{
	switch_mm(prev, next, current);
	cpumask_set_cpu(smp_processor_id(), mm_cpumask(next));
	__ctl_load(S390_lowcore.user_asce, 7, 7);
}

#include <asm-generic/mmu_context.h>

#endif /* __S390_MMU_CONTEXT_H */
