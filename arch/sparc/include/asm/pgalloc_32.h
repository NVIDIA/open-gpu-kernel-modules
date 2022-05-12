/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _SPARC_PGALLOC_H
#define _SPARC_PGALLOC_H

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pgtable.h>

#include <asm/pgtsrmmu.h>
#include <asm/vaddrs.h>
#include <asm/page.h>

struct page;

void *srmmu_get_nocache(int size, int align);
void srmmu_free_nocache(void *addr, int size);

extern struct resource sparc_iomap;

pgd_t *get_pgd_fast(void);
static inline void free_pgd_fast(pgd_t *pgd)
{
	srmmu_free_nocache(pgd, SRMMU_PGD_TABLE_SIZE);
}

#define pgd_free(mm, pgd)	free_pgd_fast(pgd)
#define pgd_alloc(mm)	get_pgd_fast()

static inline void pud_set(pud_t * pudp, pmd_t * pmdp)
{
	unsigned long pa = __nocache_pa(pmdp);

	set_pte((pte_t *)pudp, __pte((SRMMU_ET_PTD | (pa >> 4))));
}

#define pud_populate(MM, PGD, PMD)      pud_set(PGD, PMD)

static inline pmd_t *pmd_alloc_one(struct mm_struct *mm,
				   unsigned long address)
{
	return srmmu_get_nocache(SRMMU_PMD_TABLE_SIZE,
				 SRMMU_PMD_TABLE_SIZE);
}

static inline void free_pmd_fast(pmd_t * pmd)
{
	srmmu_free_nocache(pmd, SRMMU_PMD_TABLE_SIZE);
}

#define pmd_free(mm, pmd)		free_pmd_fast(pmd)
#define __pmd_free_tlb(tlb, pmd, addr)	pmd_free((tlb)->mm, pmd)

#define pmd_populate(mm, pmd, pte)	pmd_set(pmd, pte)
#define pmd_pgtable(pmd)		(pgtable_t)__pmd_page(pmd)

void pmd_set(pmd_t *pmdp, pte_t *ptep);
#define pmd_populate_kernel		pmd_populate

pgtable_t pte_alloc_one(struct mm_struct *mm);

static inline pte_t *pte_alloc_one_kernel(struct mm_struct *mm)
{
	return srmmu_get_nocache(SRMMU_PTE_TABLE_SIZE,
				 SRMMU_PTE_TABLE_SIZE);
}


static inline void free_pte_fast(pte_t *pte)
{
	srmmu_free_nocache(pte, SRMMU_PTE_TABLE_SIZE);
}

#define pte_free_kernel(mm, pte)	free_pte_fast(pte)

void pte_free(struct mm_struct * mm, pgtable_t pte);
#define __pte_free_tlb(tlb, pte, addr)	pte_free((tlb)->mm, pte)

#endif /* _SPARC_PGALLOC_H */
