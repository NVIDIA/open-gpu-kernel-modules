/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_POWERPC_SLICE_H
#define _ASM_POWERPC_SLICE_H

#ifdef CONFIG_PPC_BOOK3S_64
#include <asm/book3s/64/slice.h>
#endif

#ifndef __ASSEMBLY__

struct mm_struct;

#ifdef CONFIG_PPC_MM_SLICES

#ifdef CONFIG_HUGETLB_PAGE
#define HAVE_ARCH_HUGETLB_UNMAPPED_AREA
#endif
#define HAVE_ARCH_UNMAPPED_AREA
#define HAVE_ARCH_UNMAPPED_AREA_TOPDOWN

unsigned long slice_get_unmapped_area(unsigned long addr, unsigned long len,
				      unsigned long flags, unsigned int psize,
				      int topdown);

unsigned int get_slice_psize(struct mm_struct *mm, unsigned long addr);

void slice_set_range_psize(struct mm_struct *mm, unsigned long start,
			   unsigned long len, unsigned int psize);

void slice_init_new_context_exec(struct mm_struct *mm);
void slice_setup_new_exec(void);

#else /* CONFIG_PPC_MM_SLICES */

static inline void slice_init_new_context_exec(struct mm_struct *mm) {}

static inline unsigned int get_slice_psize(struct mm_struct *mm, unsigned long addr)
{
	return 0;
}

#endif /* CONFIG_PPC_MM_SLICES */

#endif /* __ASSEMBLY__ */

#endif /* _ASM_POWERPC_SLICE_H */
