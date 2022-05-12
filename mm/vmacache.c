// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2014 Davidlohr Bueso.
 */
#include <linux/sched/signal.h>
#include <linux/sched/task.h>
#include <linux/mm.h>
#include <linux/vmacache.h>

/*
 * Hash based on the pmd of addr if configured with MMU, which provides a good
 * hit rate for workloads with spatial locality.  Otherwise, use pages.
 */
#ifdef CONFIG_MMU
#define VMACACHE_SHIFT	PMD_SHIFT
#else
#define VMACACHE_SHIFT	PAGE_SHIFT
#endif
#define VMACACHE_HASH(addr) ((addr >> VMACACHE_SHIFT) & VMACACHE_MASK)

/*
 * This task may be accessing a foreign mm via (for example)
 * get_user_pages()->find_vma().  The vmacache is task-local and this
 * task's vmacache pertains to a different mm (ie, its own).  There is
 * nothing we can do here.
 *
 * Also handle the case where a kernel thread has adopted this mm via
 * kthread_use_mm(). That kernel thread's vmacache is not applicable to this mm.
 */
static inline bool vmacache_valid_mm(struct mm_struct *mm)
{
	return current->mm == mm && !(current->flags & PF_KTHREAD);
}

void vmacache_update(unsigned long addr, struct vm_area_struct *newvma)
{
	if (vmacache_valid_mm(newvma->vm_mm))
		current->vmacache.vmas[VMACACHE_HASH(addr)] = newvma;
}

static bool vmacache_valid(struct mm_struct *mm)
{
	struct task_struct *curr;

	if (!vmacache_valid_mm(mm))
		return false;

	curr = current;
	if (mm->vmacache_seqnum != curr->vmacache.seqnum) {
		/*
		 * First attempt will always be invalid, initialize
		 * the new cache for this task here.
		 */
		curr->vmacache.seqnum = mm->vmacache_seqnum;
		vmacache_flush(curr);
		return false;
	}
	return true;
}

struct vm_area_struct *vmacache_find(struct mm_struct *mm, unsigned long addr)
{
	int idx = VMACACHE_HASH(addr);
	int i;

	count_vm_vmacache_event(VMACACHE_FIND_CALLS);

	if (!vmacache_valid(mm))
		return NULL;

	for (i = 0; i < VMACACHE_SIZE; i++) {
		struct vm_area_struct *vma = current->vmacache.vmas[idx];

		if (vma) {
#ifdef CONFIG_DEBUG_VM_VMACACHE
			if (WARN_ON_ONCE(vma->vm_mm != mm))
				break;
#endif
			if (vma->vm_start <= addr && vma->vm_end > addr) {
				count_vm_vmacache_event(VMACACHE_FIND_HITS);
				return vma;
			}
		}
		if (++idx == VMACACHE_SIZE)
			idx = 0;
	}

	return NULL;
}

#ifndef CONFIG_MMU
struct vm_area_struct *vmacache_find_exact(struct mm_struct *mm,
					   unsigned long start,
					   unsigned long end)
{
	int idx = VMACACHE_HASH(start);
	int i;

	count_vm_vmacache_event(VMACACHE_FIND_CALLS);

	if (!vmacache_valid(mm))
		return NULL;

	for (i = 0; i < VMACACHE_SIZE; i++) {
		struct vm_area_struct *vma = current->vmacache.vmas[idx];

		if (vma && vma->vm_start == start && vma->vm_end == end) {
			count_vm_vmacache_event(VMACACHE_FIND_HITS);
			return vma;
		}
		if (++idx == VMACACHE_SIZE)
			idx = 0;
	}

	return NULL;
}
#endif
