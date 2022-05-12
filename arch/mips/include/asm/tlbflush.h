/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_TLBFLUSH_H
#define __ASM_TLBFLUSH_H

#include <linux/mm.h>

/*
 * TLB flushing:
 *
 *  - flush_tlb_all() flushes all processes TLB entries
 *  - flush_tlb_mm(mm) flushes the specified mm context TLB entries
 *  - flush_tlb_page(vma, vmaddr) flushes one page
 *  - flush_tlb_range(vma, start, end) flushes a range of pages
 *  - flush_tlb_kernel_range(start, end) flushes a range of kernel pages
 */
extern void local_flush_tlb_all(void);
extern void local_flush_tlb_range(struct vm_area_struct *vma,
	unsigned long start, unsigned long end);
extern void local_flush_tlb_kernel_range(unsigned long start,
	unsigned long end);
extern void local_flush_tlb_page(struct vm_area_struct *vma,
	unsigned long page);
extern void local_flush_tlb_one(unsigned long vaddr);

#include <asm/mmu_context.h>

#ifdef CONFIG_SMP

extern void flush_tlb_all(void);
extern void flush_tlb_mm(struct mm_struct *);
extern void flush_tlb_range(struct vm_area_struct *vma, unsigned long,
	unsigned long);
extern void flush_tlb_kernel_range(unsigned long, unsigned long);
extern void flush_tlb_page(struct vm_area_struct *, unsigned long);
extern void flush_tlb_one(unsigned long vaddr);

#else /* CONFIG_SMP */

#define flush_tlb_all()			local_flush_tlb_all()
#define flush_tlb_mm(mm)		drop_mmu_context(mm)
#define flush_tlb_range(vma, vmaddr, end)	local_flush_tlb_range(vma, vmaddr, end)
#define flush_tlb_kernel_range(vmaddr,end) \
	local_flush_tlb_kernel_range(vmaddr, end)
#define flush_tlb_page(vma, page)	local_flush_tlb_page(vma, page)
#define flush_tlb_one(vaddr)		local_flush_tlb_one(vaddr)

#endif /* CONFIG_SMP */

#endif /* __ASM_TLBFLUSH_H */
