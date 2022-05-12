/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 */

#ifndef __ASM_ARC_TLBFLUSH__
#define __ASM_ARC_TLBFLUSH__

#include <linux/mm.h>

void local_flush_tlb_all(void);
void local_flush_tlb_mm(struct mm_struct *mm);
void local_flush_tlb_page(struct vm_area_struct *vma, unsigned long page);
void local_flush_tlb_kernel_range(unsigned long start, unsigned long end);
void local_flush_tlb_range(struct vm_area_struct *vma,
			   unsigned long start, unsigned long end);
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
void local_flush_pmd_tlb_range(struct vm_area_struct *vma, unsigned long start,
			       unsigned long end);
#endif

#ifndef CONFIG_SMP
#define flush_tlb_range(vma, s, e)	local_flush_tlb_range(vma, s, e)
#define flush_tlb_page(vma, page)	local_flush_tlb_page(vma, page)
#define flush_tlb_kernel_range(s, e)	local_flush_tlb_kernel_range(s, e)
#define flush_tlb_all()			local_flush_tlb_all()
#define flush_tlb_mm(mm)		local_flush_tlb_mm(mm)
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
#define flush_pmd_tlb_range(vma, s, e)	local_flush_pmd_tlb_range(vma, s, e)
#endif
#else
extern void flush_tlb_range(struct vm_area_struct *vma, unsigned long start,
							 unsigned long end);
extern void flush_tlb_page(struct vm_area_struct *vma, unsigned long page);
extern void flush_tlb_kernel_range(unsigned long start, unsigned long end);
extern void flush_tlb_all(void);
extern void flush_tlb_mm(struct mm_struct *mm);
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
extern void flush_pmd_tlb_range(struct vm_area_struct *vma, unsigned long start, unsigned long end);
#endif
#endif /* CONFIG_SMP */
#endif
