/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2000 - 2003 Jeff Dike (jdike@addtoit.com)
 * Copyright 2003 PathScale, Inc.
 */

#ifndef __UM_PAGE_H
#define __UM_PAGE_H

#include <linux/const.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT	12
#define PAGE_SIZE	(_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#ifndef __ASSEMBLY__

struct page;

#include <linux/pfn.h>
#include <linux/types.h>
#include <asm/vm-flags.h>

/*
 * These are used to make use of C type-checking..
 */

#define clear_page(page)	memset((void *)(page), 0, PAGE_SIZE)
#define copy_page(to,from)	memcpy((void *)(to), (void *)(from), PAGE_SIZE)

#define clear_user_page(page, vaddr, pg)	clear_page(page)
#define copy_user_page(to, from, vaddr, pg)	copy_page(to, from)

#if defined(CONFIG_3_LEVEL_PGTABLES) && !defined(CONFIG_64BIT)

typedef struct { unsigned long pte; } pte_t;
typedef struct { unsigned long pmd; } pmd_t;
typedef struct { unsigned long pgd; } pgd_t;
#define pte_val(p) ((p).pte)

#define pte_get_bits(p, bits) ((p).pte & (bits))
#define pte_set_bits(p, bits) ((p).pte |= (bits))
#define pte_clear_bits(p, bits) ((p).pte &= ~(bits))
#define pte_copy(to, from) ({ (to).pte = (from).pte; })
#define pte_is_zero(p) (!((p).pte & ~_PAGE_NEWPAGE))
#define pte_set_val(p, phys, prot) \
	({ (p).pte = (phys) | pgprot_val(prot); })

#define pmd_val(x)	((x).pmd)
#define __pmd(x) ((pmd_t) { (x) } )

typedef unsigned long long phys_t;

#else

typedef struct { unsigned long pte; } pte_t;
typedef struct { unsigned long pgd; } pgd_t;

#ifdef CONFIG_3_LEVEL_PGTABLES
typedef struct { unsigned long pmd; } pmd_t;
#define pmd_val(x)	((x).pmd)
#define __pmd(x) ((pmd_t) { (x) } )
#endif

#define pte_val(x)	((x).pte)


#define pte_get_bits(p, bits) ((p).pte & (bits))
#define pte_set_bits(p, bits) ((p).pte |= (bits))
#define pte_clear_bits(p, bits) ((p).pte &= ~(bits))
#define pte_copy(to, from) ((to).pte = (from).pte)
#define pte_is_zero(p) (!((p).pte & ~_PAGE_NEWPAGE))
#define pte_set_val(p, phys, prot) (p).pte = (phys | pgprot_val(prot))

typedef unsigned long phys_t;

#endif

typedef struct { unsigned long pgprot; } pgprot_t;

typedef struct page *pgtable_t;

#define pgd_val(x)	((x).pgd)
#define pgprot_val(x)	((x).pgprot)

#define __pte(x) ((pte_t) { (x) } )
#define __pgd(x) ((pgd_t) { (x) } )
#define __pgprot(x)	((pgprot_t) { (x) } )

extern unsigned long uml_physmem;

#define PAGE_OFFSET (uml_physmem)
#define KERNELBASE PAGE_OFFSET

#define __va_space (8*1024*1024)

#include <mem.h>

/* Cast to unsigned long before casting to void * to avoid a warning from
 * mmap_kmem about cutting a long long down to a void *.  Not sure that
 * casting is the right thing, but 32-bit UML can't have 64-bit virtual
 * addresses
 */
#define __pa(virt) to_phys((void *) (unsigned long) (virt))
#define __va(phys) to_virt((unsigned long) (phys))

#define phys_to_pfn(p) ((p) >> PAGE_SHIFT)
#define pfn_to_phys(pfn) PFN_PHYS(pfn)

#define pfn_valid(pfn) ((pfn) < max_mapnr)
#define virt_addr_valid(v) pfn_valid(phys_to_pfn(__pa(v)))

#include <asm-generic/memory_model.h>
#include <asm-generic/getorder.h>

#endif	/* __ASSEMBLY__ */

#ifdef CONFIG_X86_32
#define __HAVE_ARCH_GATE_AREA 1
#endif

#endif	/* __UM_PAGE_H */
