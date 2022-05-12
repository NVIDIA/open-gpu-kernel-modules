/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 */
#ifndef __ASM_ARC_PAGE_H
#define __ASM_ARC_PAGE_H

#include <uapi/asm/page.h>

#ifdef CONFIG_ARC_HAS_PAE40

#define MAX_POSSIBLE_PHYSMEM_BITS	40
#define PAGE_MASK_PHYS			(0xff00000000ull | PAGE_MASK)

#else /* CONFIG_ARC_HAS_PAE40 */

#define MAX_POSSIBLE_PHYSMEM_BITS	32
#define PAGE_MASK_PHYS			PAGE_MASK

#endif /* CONFIG_ARC_HAS_PAE40 */

#ifndef __ASSEMBLY__

#define clear_page(paddr)		memset((paddr), 0, PAGE_SIZE)
#define copy_user_page(to, from, vaddr, pg)	copy_page(to, from)
#define copy_page(to, from)		memcpy((to), (from), PAGE_SIZE)

struct vm_area_struct;
struct page;

#define __HAVE_ARCH_COPY_USER_HIGHPAGE

void copy_user_highpage(struct page *to, struct page *from,
			unsigned long u_vaddr, struct vm_area_struct *vma);
void clear_user_page(void *to, unsigned long u_vaddr, struct page *page);

#undef STRICT_MM_TYPECHECKS

#ifdef STRICT_MM_TYPECHECKS
/*
 * These are used to make use of C type-checking..
 */
typedef struct {
#ifdef CONFIG_ARC_HAS_PAE40
	unsigned long long pte;
#else
	unsigned long pte;
#endif
} pte_t;
typedef struct {
	unsigned long pgd;
} pgd_t;
typedef struct {
	unsigned long pgprot;
} pgprot_t;

#define pte_val(x)      ((x).pte)
#define pgd_val(x)      ((x).pgd)
#define pgprot_val(x)   ((x).pgprot)

#define __pte(x)        ((pte_t) { (x) })
#define __pgd(x)        ((pgd_t) { (x) })
#define __pgprot(x)     ((pgprot_t) { (x) })

#define pte_pgprot(x) __pgprot(pte_val(x))

#else /* !STRICT_MM_TYPECHECKS */

#ifdef CONFIG_ARC_HAS_PAE40
typedef unsigned long long pte_t;
#else
typedef unsigned long pte_t;
#endif
typedef unsigned long pgd_t;
typedef unsigned long pgprot_t;

#define pte_val(x)	(x)
#define pgd_val(x)	(x)
#define pgprot_val(x)	(x)
#define __pte(x)	(x)
#define __pgd(x)	(x)
#define __pgprot(x)	(x)
#define pte_pgprot(x)	(x)

#endif

typedef pte_t * pgtable_t;

/*
 * Use virt_to_pfn with caution:
 * If used in pte or paddr related macros, it could cause truncation
 * in PAE40 builds
 * As a rule of thumb, only use it in helpers starting with virt_
 * You have been warned !
 */
#define virt_to_pfn(kaddr)	(__pa(kaddr) >> PAGE_SHIFT)

/*
 * When HIGHMEM is enabled we have holes in the memory map so we need
 * pfn_valid() that takes into account the actual extents of the physical
 * memory
 */
#ifdef CONFIG_HIGHMEM

extern unsigned long arch_pfn_offset;
#define ARCH_PFN_OFFSET		arch_pfn_offset

extern int pfn_valid(unsigned long pfn);
#define pfn_valid		pfn_valid

#else /* CONFIG_HIGHMEM */

#define ARCH_PFN_OFFSET		virt_to_pfn(CONFIG_LINUX_RAM_BASE)
#define pfn_valid(pfn)		(((pfn) - ARCH_PFN_OFFSET) < max_mapnr)

#endif /* CONFIG_HIGHMEM */

/*
 * __pa, __va, virt_to_page (ALERT: deprecated, don't use them)
 *
 * These macros have historically been misnamed
 * virt here means link-address/program-address as embedded in object code.
 * And for ARC, link-addr = physical address
 */
#define __pa(vaddr)  ((unsigned long)(vaddr))
#define __va(paddr)  ((void *)((unsigned long)(paddr)))

#define virt_to_page(kaddr)	pfn_to_page(virt_to_pfn(kaddr))
#define virt_addr_valid(kaddr)  pfn_valid(virt_to_pfn(kaddr))

/* Default Permissions for stack/heaps pages (Non Executable) */
#define VM_DATA_DEFAULT_FLAGS	VM_DATA_FLAGS_NON_EXEC

#define WANT_PAGE_VIRTUAL   1

#include <asm-generic/memory_model.h>   /* page_to_pfn, pfn_to_page */
#include <asm-generic/getorder.h>

#endif /* !__ASSEMBLY__ */

#endif
