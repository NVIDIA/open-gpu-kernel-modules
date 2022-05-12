/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_IA64_TLBFLUSH_H
#define _ASM_IA64_TLBFLUSH_H

/*
 * Copyright (C) 2002 Hewlett-Packard Co
 *	David Mosberger-Tang <davidm@hpl.hp.com>
 */


#include <linux/mm.h>

#include <asm/intrinsics.h>
#include <asm/mmu_context.h>
#include <asm/page.h>

struct ia64_tr_entry {
	u64 ifa;
	u64 itir;
	u64 pte;
	u64 rr;
}; /*Record for tr entry!*/

extern int ia64_itr_entry(u64 target_mask, u64 va, u64 pte, u64 log_size);
extern void ia64_ptr_entry(u64 target_mask, int slot);
extern struct ia64_tr_entry *ia64_idtrs[NR_CPUS];

/*
 region register macros
*/
#define RR_TO_VE(val)   (((val) >> 0) & 0x0000000000000001)
#define RR_VE(val)     (((val) & 0x0000000000000001) << 0)
#define RR_VE_MASK     0x0000000000000001L
#define RR_VE_SHIFT    0
#define RR_TO_PS(val)  (((val) >> 2) & 0x000000000000003f)
#define RR_PS(val)     (((val) & 0x000000000000003f) << 2)
#define RR_PS_MASK     0x00000000000000fcL
#define RR_PS_SHIFT    2
#define RR_RID_MASK    0x00000000ffffff00L
#define RR_TO_RID(val)         ((val >> 8) & 0xffffff)

/*
 * Now for some TLB flushing routines.  This is the kind of stuff that
 * can be very expensive, so try to avoid them whenever possible.
 */
extern void setup_ptcg_sem(int max_purges, int from_palo);

/*
 * Flush everything (kernel mapping may also have changed due to
 * vmalloc/vfree).
 */
extern void local_flush_tlb_all (void);

#ifdef CONFIG_SMP
  extern void smp_flush_tlb_all (void);
  extern void smp_flush_tlb_mm (struct mm_struct *mm);
  extern void smp_flush_tlb_cpumask (cpumask_t xcpumask);
# define flush_tlb_all()	smp_flush_tlb_all()
#else
# define flush_tlb_all()	local_flush_tlb_all()
# define smp_flush_tlb_cpumask(m) local_flush_tlb_all()
#endif

static inline void
local_finish_flush_tlb_mm (struct mm_struct *mm)
{
	if (mm == current->active_mm)
		activate_context(mm);
}

/*
 * Flush a specified user mapping.  This is called, e.g., as a result of fork() and
 * exit().  fork() ends up here because the copy-on-write mechanism needs to write-protect
 * the PTEs of the parent task.
 */
static inline void
flush_tlb_mm (struct mm_struct *mm)
{
	if (!mm)
		return;

	set_bit(mm->context, ia64_ctx.flushmap);
	mm->context = 0;

	if (atomic_read(&mm->mm_users) == 0)
		return;		/* happens as a result of exit_mmap() */

#ifdef CONFIG_SMP
	smp_flush_tlb_mm(mm);
#else
	local_finish_flush_tlb_mm(mm);
#endif
}

extern void flush_tlb_range (struct vm_area_struct *vma, unsigned long start, unsigned long end);

/*
 * Page-granular tlb flush.
 */
static inline void
flush_tlb_page (struct vm_area_struct *vma, unsigned long addr)
{
#ifdef CONFIG_SMP
	flush_tlb_range(vma, (addr & PAGE_MASK), (addr & PAGE_MASK) + PAGE_SIZE);
#else
	if (vma->vm_mm == current->active_mm)
		ia64_ptcl(addr, (PAGE_SHIFT << 2));
	else
		vma->vm_mm->context = 0;
#endif
}

/*
 * Flush the local TLB. Invoked from another cpu using an IPI.
 */
#ifdef CONFIG_SMP
void smp_local_flush_tlb(void);
#else
#define smp_local_flush_tlb()
#endif

static inline void flush_tlb_kernel_range(unsigned long start,
					  unsigned long end)
{
	flush_tlb_all();	/* XXX fix me */
}

#endif /* _ASM_IA64_TLBFLUSH_H */
