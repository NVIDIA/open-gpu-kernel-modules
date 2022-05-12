/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_POWERPC_BOOK3S_64_HASH_H
#define _ASM_POWERPC_BOOK3S_64_HASH_H
#ifdef __KERNEL__

#include <asm/asm-const.h>

/*
 * Common bits between 4K and 64K pages in a linux-style PTE.
 * Additional bits may be defined in pgtable-hash64-*.h
 *
 */
#define H_PTE_NONE_MASK		_PAGE_HPTEFLAGS

#ifdef CONFIG_PPC_64K_PAGES
#include <asm/book3s/64/hash-64k.h>
#else
#include <asm/book3s/64/hash-4k.h>
#endif

/* Bits to set in a PMD/PUD/PGD entry valid bit*/
#define HASH_PMD_VAL_BITS		(0x8000000000000000UL)
#define HASH_PUD_VAL_BITS		(0x8000000000000000UL)
#define HASH_PGD_VAL_BITS		(0x8000000000000000UL)

/*
 * Size of EA range mapped by our pagetables.
 */
#define H_PGTABLE_EADDR_SIZE	(H_PTE_INDEX_SIZE + H_PMD_INDEX_SIZE + \
				 H_PUD_INDEX_SIZE + H_PGD_INDEX_SIZE + PAGE_SHIFT)
#define H_PGTABLE_RANGE		(ASM_CONST(1) << H_PGTABLE_EADDR_SIZE)
/*
 * Top 2 bits are ignored in page table walk.
 */
#define EA_MASK			(~(0xcUL << 60))

/*
 * We store the slot details in the second half of page table.
 * Increase the pud level table so that hugetlb ptes can be stored
 * at pud level.
 */
#if defined(CONFIG_HUGETLB_PAGE) &&  defined(CONFIG_PPC_64K_PAGES)
#define H_PUD_CACHE_INDEX	(H_PUD_INDEX_SIZE + 1)
#else
#define H_PUD_CACHE_INDEX	(H_PUD_INDEX_SIZE)
#endif

/*
 * +------------------------------+
 * |                              |
 * |                              |
 * |                              |
 * +------------------------------+  Kernel virtual map end (0xc00e000000000000)
 * |                              |
 * |                              |
 * |      512TB/16TB of vmemmap   |
 * |                              |
 * |                              |
 * +------------------------------+  Kernel vmemmap  start
 * |                              |
 * |      512TB/16TB of IO map    |
 * |                              |
 * +------------------------------+  Kernel IO map start
 * |                              |
 * |      512TB/16TB of vmap      |
 * |                              |
 * +------------------------------+  Kernel virt start (0xc008000000000000)
 * |                              |
 * |                              |
 * |                              |
 * +------------------------------+  Kernel linear (0xc.....)
 */

#define H_VMALLOC_START		H_KERN_VIRT_START
#define H_VMALLOC_SIZE		H_KERN_MAP_SIZE
#define H_VMALLOC_END		(H_VMALLOC_START + H_VMALLOC_SIZE)

#define H_KERN_IO_START		H_VMALLOC_END
#define H_KERN_IO_SIZE		H_KERN_MAP_SIZE
#define H_KERN_IO_END		(H_KERN_IO_START + H_KERN_IO_SIZE)

#define H_VMEMMAP_START		H_KERN_IO_END
#define H_VMEMMAP_SIZE		H_KERN_MAP_SIZE
#define H_VMEMMAP_END		(H_VMEMMAP_START + H_VMEMMAP_SIZE)

#define NON_LINEAR_REGION_ID(ea)	((((unsigned long)ea - H_KERN_VIRT_START) >> REGION_SHIFT) + 2)

/*
 * Region IDs
 */
#define USER_REGION_ID		0
#define LINEAR_MAP_REGION_ID	1
#define VMALLOC_REGION_ID	NON_LINEAR_REGION_ID(H_VMALLOC_START)
#define IO_REGION_ID		NON_LINEAR_REGION_ID(H_KERN_IO_START)
#define VMEMMAP_REGION_ID	NON_LINEAR_REGION_ID(H_VMEMMAP_START)
#define INVALID_REGION_ID	(VMEMMAP_REGION_ID + 1)

/*
 * Defines the address of the vmemap area, in its own region on
 * hash table CPUs.
 */
#ifdef CONFIG_PPC_MM_SLICES
#define HAVE_ARCH_UNMAPPED_AREA
#define HAVE_ARCH_UNMAPPED_AREA_TOPDOWN
#endif /* CONFIG_PPC_MM_SLICES */

/* PTEIDX nibble */
#define _PTEIDX_SECONDARY	0x8
#define _PTEIDX_GROUP_IX	0x7

#define H_PMD_BAD_BITS		(PTE_TABLE_SIZE-1)
#define H_PUD_BAD_BITS		(PMD_TABLE_SIZE-1)

#ifndef __ASSEMBLY__
static inline int get_region_id(unsigned long ea)
{
	int region_id;
	int id = (ea >> 60UL);

	if (id == 0)
		return USER_REGION_ID;

	if (id != (PAGE_OFFSET >> 60))
		return INVALID_REGION_ID;

	if (ea < H_KERN_VIRT_START)
		return LINEAR_MAP_REGION_ID;

	BUILD_BUG_ON(NON_LINEAR_REGION_ID(H_VMALLOC_START) != 2);

	region_id = NON_LINEAR_REGION_ID(ea);
	return region_id;
}

#define	hash__pmd_bad(pmd)		(pmd_val(pmd) & H_PMD_BAD_BITS)
#define	hash__pud_bad(pud)		(pud_val(pud) & H_PUD_BAD_BITS)
static inline int hash__p4d_bad(p4d_t p4d)
{
	return (p4d_val(p4d) == 0);
}
#ifdef CONFIG_STRICT_KERNEL_RWX
extern void hash__mark_rodata_ro(void);
extern void hash__mark_initmem_nx(void);
#endif

extern void hpte_need_flush(struct mm_struct *mm, unsigned long addr,
			    pte_t *ptep, unsigned long pte, int huge);
unsigned long htab_convert_pte_flags(unsigned long pteflags, unsigned long flags);
/* Atomic PTE updates */
static inline unsigned long hash__pte_update(struct mm_struct *mm,
					 unsigned long addr,
					 pte_t *ptep, unsigned long clr,
					 unsigned long set,
					 int huge)
{
	__be64 old_be, tmp_be;
	unsigned long old;

	__asm__ __volatile__(
	"1:	ldarx	%0,0,%3		# pte_update\n\
	and.	%1,%0,%6\n\
	bne-	1b \n\
	andc	%1,%0,%4 \n\
	or	%1,%1,%7\n\
	stdcx.	%1,0,%3 \n\
	bne-	1b"
	: "=&r" (old_be), "=&r" (tmp_be), "=m" (*ptep)
	: "r" (ptep), "r" (cpu_to_be64(clr)), "m" (*ptep),
	  "r" (cpu_to_be64(H_PAGE_BUSY)), "r" (cpu_to_be64(set))
	: "cc" );
	/* huge pages use the old page table lock */
	if (!huge)
		assert_pte_locked(mm, addr);

	old = be64_to_cpu(old_be);
	if (old & H_PAGE_HASHPTE)
		hpte_need_flush(mm, addr, ptep, old, huge);

	return old;
}

/* Set the dirty and/or accessed bits atomically in a linux PTE, this
 * function doesn't need to flush the hash entry
 */
static inline void hash__ptep_set_access_flags(pte_t *ptep, pte_t entry)
{
	__be64 old, tmp, val, mask;

	mask = cpu_to_be64(_PAGE_DIRTY | _PAGE_ACCESSED | _PAGE_READ | _PAGE_WRITE |
			   _PAGE_EXEC | _PAGE_SOFT_DIRTY);

	val = pte_raw(entry) & mask;

	__asm__ __volatile__(
	"1:	ldarx	%0,0,%4\n\
		and.	%1,%0,%6\n\
		bne-	1b \n\
		or	%0,%3,%0\n\
		stdcx.	%0,0,%4\n\
		bne-	1b"
	:"=&r" (old), "=&r" (tmp), "=m" (*ptep)
	:"r" (val), "r" (ptep), "m" (*ptep), "r" (cpu_to_be64(H_PAGE_BUSY))
	:"cc");
}

static inline int hash__pte_same(pte_t pte_a, pte_t pte_b)
{
	return (((pte_raw(pte_a) ^ pte_raw(pte_b)) & ~cpu_to_be64(_PAGE_HPTEFLAGS)) == 0);
}

static inline int hash__pte_none(pte_t pte)
{
	return (pte_val(pte) & ~H_PTE_NONE_MASK) == 0;
}

unsigned long pte_get_hash_gslot(unsigned long vpn, unsigned long shift,
		int ssize, real_pte_t rpte, unsigned int subpg_index);

/* This low level function performs the actual PTE insertion
 * Setting the PTE depends on the MMU type and other factors. It's
 * an horrible mess that I'm not going to try to clean up now but
 * I'm keeping it in one place rather than spread around
 */
static inline void hash__set_pte_at(struct mm_struct *mm, unsigned long addr,
				  pte_t *ptep, pte_t pte, int percpu)
{
	/*
	 * Anything else just stores the PTE normally. That covers all 64-bit
	 * cases, and 32-bit non-hash with 32-bit PTEs.
	 */
	*ptep = pte;
}

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
extern void hpte_do_hugepage_flush(struct mm_struct *mm, unsigned long addr,
				   pmd_t *pmdp, unsigned long old_pmd);
#else
static inline void hpte_do_hugepage_flush(struct mm_struct *mm,
					  unsigned long addr, pmd_t *pmdp,
					  unsigned long old_pmd)
{
	WARN(1, "%s called with THP disabled\n", __func__);
}
#endif /* CONFIG_TRANSPARENT_HUGEPAGE */


int hash__map_kernel_page(unsigned long ea, unsigned long pa, pgprot_t prot);
extern int __meminit hash__vmemmap_create_mapping(unsigned long start,
					      unsigned long page_size,
					      unsigned long phys);
extern void hash__vmemmap_remove_mapping(unsigned long start,
				     unsigned long page_size);

int hash__create_section_mapping(unsigned long start, unsigned long end,
				 int nid, pgprot_t prot);
int hash__remove_section_mapping(unsigned long start, unsigned long end);

#endif /* !__ASSEMBLY__ */
#endif /* __KERNEL__ */
#endif /* _ASM_POWERPC_BOOK3S_64_HASH_H */
