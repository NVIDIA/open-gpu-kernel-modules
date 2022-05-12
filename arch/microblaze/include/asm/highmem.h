/* SPDX-License-Identifier: GPL-2.0 */
/*
 * highmem.h: virtual kernel memory mappings for high memory
 *
 * Used in CONFIG_HIGHMEM systems for memory pages which
 * are not addressable by direct kernel virtual addresses.
 *
 * Copyright (C) 1999 Gerhard Wichert, Siemens AG
 *		      Gerhard.Wichert@pdb.siemens.de
 *
 *
 * Redesigned the x86 32-bit VM architecture to deal with
 * up to 16 Terabyte physical memory. With current x86 CPUs
 * we now support up to 64 Gigabytes physical RAM.
 *
 * Copyright (C) 1999 Ingo Molnar <mingo@redhat.com>
 */
#ifndef _ASM_HIGHMEM_H
#define _ASM_HIGHMEM_H

#ifdef __KERNEL__

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <asm/fixmap.h>

extern pte_t *pkmap_page_table;

/*
 * Right now we initialize only a single pte table. It can be extended
 * easily, subsequent pte tables have to be allocated in one physical
 * chunk of RAM.
 */
/*
 * We use one full pte table with 4K pages. And with 16K/64K/256K pages pte
 * table covers enough memory (32MB/512MB/2GB resp.), so that both FIXMAP
 * and PKMAP can be placed in a single pte table. We use 512 pages for PKMAP
 * in case of 16K/64K/256K page sizes.
 */

#define PKMAP_ORDER	PTE_SHIFT
#define LAST_PKMAP	(1 << PKMAP_ORDER)

#define PKMAP_BASE	((FIXADDR_START - PAGE_SIZE * (LAST_PKMAP + 1)) \
								& PMD_MASK)

#define LAST_PKMAP_MASK	(LAST_PKMAP - 1)
#define PKMAP_NR(virt)  ((virt - PKMAP_BASE) >> PAGE_SHIFT)
#define PKMAP_ADDR(nr)  (PKMAP_BASE + ((nr) << PAGE_SHIFT))

#define flush_cache_kmaps()	{ flush_icache(); flush_dcache(); }

#define arch_kmap_local_post_map(vaddr, pteval)	\
	local_flush_tlb_page(NULL, vaddr);
#define arch_kmap_local_post_unmap(vaddr)	\
	local_flush_tlb_page(NULL, vaddr);

#endif /* __KERNEL__ */

#endif /* _ASM_HIGHMEM_H */
