// SPDX-License-Identifier: GPL-2.0-only
/*
 * This kernel test validates architecture page table helpers and
 * accessors and helps in verifying their continued compliance with
 * expected generic MM semantics.
 *
 * Copyright (C) 2019 ARM Ltd.
 *
 * Author: Anshuman Khandual <anshuman.khandual@arm.com>
 */
#define pr_fmt(fmt) "debug_vm_pgtable: [%-25s]: " fmt, __func__

#include <linux/gfp.h>
#include <linux/highmem.h>
#include <linux/hugetlb.h>
#include <linux/kernel.h>
#include <linux/kconfig.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/pfn_t.h>
#include <linux/printk.h>
#include <linux/pgtable.h>
#include <linux/random.h>
#include <linux/spinlock.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/start_kernel.h>
#include <linux/sched/mm.h>
#include <linux/io.h>
#include <asm/pgalloc.h>
#include <asm/tlbflush.h>

/*
 * Please refer Documentation/vm/arch_pgtable_helpers.rst for the semantics
 * expectations that are being validated here. All future changes in here
 * or the documentation need to be in sync.
 */

#define VMFLAGS	(VM_READ|VM_WRITE|VM_EXEC)

/*
 * On s390 platform, the lower 4 bits are used to identify given page table
 * entry type. But these bits might affect the ability to clear entries with
 * pxx_clear() because of how dynamic page table folding works on s390. So
 * while loading up the entries do not change the lower 4 bits. It does not
 * have affect any other platform. Also avoid the 62nd bit on ppc64 that is
 * used to mark a pte entry.
 */
#define S390_SKIP_MASK		GENMASK(3, 0)
#if __BITS_PER_LONG == 64
#define PPC64_SKIP_MASK		GENMASK(62, 62)
#else
#define PPC64_SKIP_MASK		0x0
#endif
#define ARCH_SKIP_MASK (S390_SKIP_MASK | PPC64_SKIP_MASK)
#define RANDOM_ORVALUE (GENMASK(BITS_PER_LONG - 1, 0) & ~ARCH_SKIP_MASK)
#define RANDOM_NZVALUE	GENMASK(7, 0)

static void __init pte_basic_tests(unsigned long pfn, int idx)
{
	pgprot_t prot = protection_map[idx];
	pte_t pte = pfn_pte(pfn, prot);
	unsigned long val = idx, *ptr = &val;

	pr_debug("Validating PTE basic (%pGv)\n", ptr);

	/*
	 * This test needs to be executed after the given page table entry
	 * is created with pfn_pte() to make sure that protection_map[idx]
	 * does not have the dirty bit enabled from the beginning. This is
	 * important for platforms like arm64 where (!PTE_RDONLY) indicate
	 * dirty bit being set.
	 */
	WARN_ON(pte_dirty(pte_wrprotect(pte)));

	WARN_ON(!pte_same(pte, pte));
	WARN_ON(!pte_young(pte_mkyoung(pte_mkold(pte))));
	WARN_ON(!pte_dirty(pte_mkdirty(pte_mkclean(pte))));
	WARN_ON(!pte_write(pte_mkwrite(pte_wrprotect(pte))));
	WARN_ON(pte_young(pte_mkold(pte_mkyoung(pte))));
	WARN_ON(pte_dirty(pte_mkclean(pte_mkdirty(pte))));
	WARN_ON(pte_write(pte_wrprotect(pte_mkwrite(pte))));
	WARN_ON(pte_dirty(pte_wrprotect(pte_mkclean(pte))));
	WARN_ON(!pte_dirty(pte_wrprotect(pte_mkdirty(pte))));
}

static void __init pte_advanced_tests(struct mm_struct *mm,
				      struct vm_area_struct *vma, pte_t *ptep,
				      unsigned long pfn, unsigned long vaddr,
				      pgprot_t prot)
{
	pte_t pte = pfn_pte(pfn, prot);

	/*
	 * Architectures optimize set_pte_at by avoiding TLB flush.
	 * This requires set_pte_at to be not used to update an
	 * existing pte entry. Clear pte before we do set_pte_at
	 */

	pr_debug("Validating PTE advanced\n");
	pte = pfn_pte(pfn, prot);
	set_pte_at(mm, vaddr, ptep, pte);
	ptep_set_wrprotect(mm, vaddr, ptep);
	pte = ptep_get(ptep);
	WARN_ON(pte_write(pte));
	ptep_get_and_clear(mm, vaddr, ptep);
	pte = ptep_get(ptep);
	WARN_ON(!pte_none(pte));

	pte = pfn_pte(pfn, prot);
	pte = pte_wrprotect(pte);
	pte = pte_mkclean(pte);
	set_pte_at(mm, vaddr, ptep, pte);
	pte = pte_mkwrite(pte);
	pte = pte_mkdirty(pte);
	ptep_set_access_flags(vma, vaddr, ptep, pte, 1);
	pte = ptep_get(ptep);
	WARN_ON(!(pte_write(pte) && pte_dirty(pte)));
	ptep_get_and_clear_full(mm, vaddr, ptep, 1);
	pte = ptep_get(ptep);
	WARN_ON(!pte_none(pte));

	pte = pfn_pte(pfn, prot);
	pte = pte_mkyoung(pte);
	set_pte_at(mm, vaddr, ptep, pte);
	ptep_test_and_clear_young(vma, vaddr, ptep);
	pte = ptep_get(ptep);
	WARN_ON(pte_young(pte));
}

static void __init pte_savedwrite_tests(unsigned long pfn, pgprot_t prot)
{
	pte_t pte = pfn_pte(pfn, prot);

	if (!IS_ENABLED(CONFIG_NUMA_BALANCING))
		return;

	pr_debug("Validating PTE saved write\n");
	WARN_ON(!pte_savedwrite(pte_mk_savedwrite(pte_clear_savedwrite(pte))));
	WARN_ON(pte_savedwrite(pte_clear_savedwrite(pte_mk_savedwrite(pte))));
}

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static void __init pmd_basic_tests(unsigned long pfn, int idx)
{
	pgprot_t prot = protection_map[idx];
	pmd_t pmd = pfn_pmd(pfn, prot);
	unsigned long val = idx, *ptr = &val;

	if (!has_transparent_hugepage())
		return;

	pr_debug("Validating PMD basic (%pGv)\n", ptr);

	/*
	 * This test needs to be executed after the given page table entry
	 * is created with pfn_pmd() to make sure that protection_map[idx]
	 * does not have the dirty bit enabled from the beginning. This is
	 * important for platforms like arm64 where (!PTE_RDONLY) indicate
	 * dirty bit being set.
	 */
	WARN_ON(pmd_dirty(pmd_wrprotect(pmd)));


	WARN_ON(!pmd_same(pmd, pmd));
	WARN_ON(!pmd_young(pmd_mkyoung(pmd_mkold(pmd))));
	WARN_ON(!pmd_dirty(pmd_mkdirty(pmd_mkclean(pmd))));
	WARN_ON(!pmd_write(pmd_mkwrite(pmd_wrprotect(pmd))));
	WARN_ON(pmd_young(pmd_mkold(pmd_mkyoung(pmd))));
	WARN_ON(pmd_dirty(pmd_mkclean(pmd_mkdirty(pmd))));
	WARN_ON(pmd_write(pmd_wrprotect(pmd_mkwrite(pmd))));
	WARN_ON(pmd_dirty(pmd_wrprotect(pmd_mkclean(pmd))));
	WARN_ON(!pmd_dirty(pmd_wrprotect(pmd_mkdirty(pmd))));
	/*
	 * A huge page does not point to next level page table
	 * entry. Hence this must qualify as pmd_bad().
	 */
	WARN_ON(!pmd_bad(pmd_mkhuge(pmd)));
}

static void __init pmd_advanced_tests(struct mm_struct *mm,
				      struct vm_area_struct *vma, pmd_t *pmdp,
				      unsigned long pfn, unsigned long vaddr,
				      pgprot_t prot, pgtable_t pgtable)
{
	pmd_t pmd = pfn_pmd(pfn, prot);

	if (!has_transparent_hugepage())
		return;

	pr_debug("Validating PMD advanced\n");
	/* Align the address wrt HPAGE_PMD_SIZE */
	vaddr &= HPAGE_PMD_MASK;

	pgtable_trans_huge_deposit(mm, pmdp, pgtable);

	pmd = pfn_pmd(pfn, prot);
	set_pmd_at(mm, vaddr, pmdp, pmd);
	pmdp_set_wrprotect(mm, vaddr, pmdp);
	pmd = READ_ONCE(*pmdp);
	WARN_ON(pmd_write(pmd));
	pmdp_huge_get_and_clear(mm, vaddr, pmdp);
	pmd = READ_ONCE(*pmdp);
	WARN_ON(!pmd_none(pmd));

	pmd = pfn_pmd(pfn, prot);
	pmd = pmd_wrprotect(pmd);
	pmd = pmd_mkclean(pmd);
	set_pmd_at(mm, vaddr, pmdp, pmd);
	pmd = pmd_mkwrite(pmd);
	pmd = pmd_mkdirty(pmd);
	pmdp_set_access_flags(vma, vaddr, pmdp, pmd, 1);
	pmd = READ_ONCE(*pmdp);
	WARN_ON(!(pmd_write(pmd) && pmd_dirty(pmd)));
	pmdp_huge_get_and_clear_full(vma, vaddr, pmdp, 1);
	pmd = READ_ONCE(*pmdp);
	WARN_ON(!pmd_none(pmd));

	pmd = pmd_mkhuge(pfn_pmd(pfn, prot));
	pmd = pmd_mkyoung(pmd);
	set_pmd_at(mm, vaddr, pmdp, pmd);
	pmdp_test_and_clear_young(vma, vaddr, pmdp);
	pmd = READ_ONCE(*pmdp);
	WARN_ON(pmd_young(pmd));

	/*  Clear the pte entries  */
	pmdp_huge_get_and_clear(mm, vaddr, pmdp);
	pgtable = pgtable_trans_huge_withdraw(mm, pmdp);
}

static void __init pmd_leaf_tests(unsigned long pfn, pgprot_t prot)
{
	pmd_t pmd = pfn_pmd(pfn, prot);

	pr_debug("Validating PMD leaf\n");
	/*
	 * PMD based THP is a leaf entry.
	 */
	pmd = pmd_mkhuge(pmd);
	WARN_ON(!pmd_leaf(pmd));
}

#ifdef CONFIG_HAVE_ARCH_HUGE_VMAP
static void __init pmd_huge_tests(pmd_t *pmdp, unsigned long pfn, pgprot_t prot)
{
	pmd_t pmd;

	if (!arch_vmap_pmd_supported(prot))
		return;

	pr_debug("Validating PMD huge\n");
	/*
	 * X86 defined pmd_set_huge() verifies that the given
	 * PMD is not a populated non-leaf entry.
	 */
	WRITE_ONCE(*pmdp, __pmd(0));
	WARN_ON(!pmd_set_huge(pmdp, __pfn_to_phys(pfn), prot));
	WARN_ON(!pmd_clear_huge(pmdp));
	pmd = READ_ONCE(*pmdp);
	WARN_ON(!pmd_none(pmd));
}
#else /* CONFIG_HAVE_ARCH_HUGE_VMAP */
static void __init pmd_huge_tests(pmd_t *pmdp, unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_HAVE_ARCH_HUGE_VMAP */

static void __init pmd_savedwrite_tests(unsigned long pfn, pgprot_t prot)
{
	pmd_t pmd = pfn_pmd(pfn, prot);

	if (!IS_ENABLED(CONFIG_NUMA_BALANCING))
		return;

	pr_debug("Validating PMD saved write\n");
	WARN_ON(!pmd_savedwrite(pmd_mk_savedwrite(pmd_clear_savedwrite(pmd))));
	WARN_ON(pmd_savedwrite(pmd_clear_savedwrite(pmd_mk_savedwrite(pmd))));
}

#ifdef CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD
static void __init pud_basic_tests(struct mm_struct *mm, unsigned long pfn, int idx)
{
	pgprot_t prot = protection_map[idx];
	pud_t pud = pfn_pud(pfn, prot);
	unsigned long val = idx, *ptr = &val;

	if (!has_transparent_hugepage())
		return;

	pr_debug("Validating PUD basic (%pGv)\n", ptr);

	/*
	 * This test needs to be executed after the given page table entry
	 * is created with pfn_pud() to make sure that protection_map[idx]
	 * does not have the dirty bit enabled from the beginning. This is
	 * important for platforms like arm64 where (!PTE_RDONLY) indicate
	 * dirty bit being set.
	 */
	WARN_ON(pud_dirty(pud_wrprotect(pud)));

	WARN_ON(!pud_same(pud, pud));
	WARN_ON(!pud_young(pud_mkyoung(pud_mkold(pud))));
	WARN_ON(!pud_dirty(pud_mkdirty(pud_mkclean(pud))));
	WARN_ON(pud_dirty(pud_mkclean(pud_mkdirty(pud))));
	WARN_ON(!pud_write(pud_mkwrite(pud_wrprotect(pud))));
	WARN_ON(pud_write(pud_wrprotect(pud_mkwrite(pud))));
	WARN_ON(pud_young(pud_mkold(pud_mkyoung(pud))));
	WARN_ON(pud_dirty(pud_wrprotect(pud_mkclean(pud))));
	WARN_ON(!pud_dirty(pud_wrprotect(pud_mkdirty(pud))));

	if (mm_pmd_folded(mm))
		return;

	/*
	 * A huge page does not point to next level page table
	 * entry. Hence this must qualify as pud_bad().
	 */
	WARN_ON(!pud_bad(pud_mkhuge(pud)));
}

static void __init pud_advanced_tests(struct mm_struct *mm,
				      struct vm_area_struct *vma, pud_t *pudp,
				      unsigned long pfn, unsigned long vaddr,
				      pgprot_t prot)
{
	pud_t pud = pfn_pud(pfn, prot);

	if (!has_transparent_hugepage())
		return;

	pr_debug("Validating PUD advanced\n");
	/* Align the address wrt HPAGE_PUD_SIZE */
	vaddr &= HPAGE_PUD_MASK;

	set_pud_at(mm, vaddr, pudp, pud);
	pudp_set_wrprotect(mm, vaddr, pudp);
	pud = READ_ONCE(*pudp);
	WARN_ON(pud_write(pud));

#ifndef __PAGETABLE_PMD_FOLDED
	pudp_huge_get_and_clear(mm, vaddr, pudp);
	pud = READ_ONCE(*pudp);
	WARN_ON(!pud_none(pud));
#endif /* __PAGETABLE_PMD_FOLDED */
	pud = pfn_pud(pfn, prot);
	pud = pud_wrprotect(pud);
	pud = pud_mkclean(pud);
	set_pud_at(mm, vaddr, pudp, pud);
	pud = pud_mkwrite(pud);
	pud = pud_mkdirty(pud);
	pudp_set_access_flags(vma, vaddr, pudp, pud, 1);
	pud = READ_ONCE(*pudp);
	WARN_ON(!(pud_write(pud) && pud_dirty(pud)));

#ifndef __PAGETABLE_PMD_FOLDED
	pudp_huge_get_and_clear_full(mm, vaddr, pudp, 1);
	pud = READ_ONCE(*pudp);
	WARN_ON(!pud_none(pud));
#endif /* __PAGETABLE_PMD_FOLDED */

	pud = pfn_pud(pfn, prot);
	pud = pud_mkyoung(pud);
	set_pud_at(mm, vaddr, pudp, pud);
	pudp_test_and_clear_young(vma, vaddr, pudp);
	pud = READ_ONCE(*pudp);
	WARN_ON(pud_young(pud));

	pudp_huge_get_and_clear(mm, vaddr, pudp);
}

static void __init pud_leaf_tests(unsigned long pfn, pgprot_t prot)
{
	pud_t pud = pfn_pud(pfn, prot);

	pr_debug("Validating PUD leaf\n");
	/*
	 * PUD based THP is a leaf entry.
	 */
	pud = pud_mkhuge(pud);
	WARN_ON(!pud_leaf(pud));
}

#ifdef CONFIG_HAVE_ARCH_HUGE_VMAP
static void __init pud_huge_tests(pud_t *pudp, unsigned long pfn, pgprot_t prot)
{
	pud_t pud;

	if (!arch_vmap_pud_supported(prot))
		return;

	pr_debug("Validating PUD huge\n");
	/*
	 * X86 defined pud_set_huge() verifies that the given
	 * PUD is not a populated non-leaf entry.
	 */
	WRITE_ONCE(*pudp, __pud(0));
	WARN_ON(!pud_set_huge(pudp, __pfn_to_phys(pfn), prot));
	WARN_ON(!pud_clear_huge(pudp));
	pud = READ_ONCE(*pudp);
	WARN_ON(!pud_none(pud));
}
#else /* !CONFIG_HAVE_ARCH_HUGE_VMAP */
static void __init pud_huge_tests(pud_t *pudp, unsigned long pfn, pgprot_t prot) { }
#endif /* !CONFIG_HAVE_ARCH_HUGE_VMAP */

#else  /* !CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD */
static void __init pud_basic_tests(struct mm_struct *mm, unsigned long pfn, int idx) { }
static void __init pud_advanced_tests(struct mm_struct *mm,
				      struct vm_area_struct *vma, pud_t *pudp,
				      unsigned long pfn, unsigned long vaddr,
				      pgprot_t prot)
{
}
static void __init pud_leaf_tests(unsigned long pfn, pgprot_t prot) { }
static void __init pud_huge_tests(pud_t *pudp, unsigned long pfn, pgprot_t prot)
{
}
#endif /* CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD */
#else  /* !CONFIG_TRANSPARENT_HUGEPAGE */
static void __init pmd_basic_tests(unsigned long pfn, int idx) { }
static void __init pud_basic_tests(struct mm_struct *mm, unsigned long pfn, int idx) { }
static void __init pmd_advanced_tests(struct mm_struct *mm,
				      struct vm_area_struct *vma, pmd_t *pmdp,
				      unsigned long pfn, unsigned long vaddr,
				      pgprot_t prot, pgtable_t pgtable)
{
}
static void __init pud_advanced_tests(struct mm_struct *mm,
				      struct vm_area_struct *vma, pud_t *pudp,
				      unsigned long pfn, unsigned long vaddr,
				      pgprot_t prot)
{
}
static void __init pmd_leaf_tests(unsigned long pfn, pgprot_t prot) { }
static void __init pud_leaf_tests(unsigned long pfn, pgprot_t prot) { }
static void __init pmd_huge_tests(pmd_t *pmdp, unsigned long pfn, pgprot_t prot)
{
}
static void __init pud_huge_tests(pud_t *pudp, unsigned long pfn, pgprot_t prot)
{
}
static void __init pmd_savedwrite_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_TRANSPARENT_HUGEPAGE */

static void __init p4d_basic_tests(unsigned long pfn, pgprot_t prot)
{
	p4d_t p4d;

	pr_debug("Validating P4D basic\n");
	memset(&p4d, RANDOM_NZVALUE, sizeof(p4d_t));
	WARN_ON(!p4d_same(p4d, p4d));
}

static void __init pgd_basic_tests(unsigned long pfn, pgprot_t prot)
{
	pgd_t pgd;

	pr_debug("Validating PGD basic\n");
	memset(&pgd, RANDOM_NZVALUE, sizeof(pgd_t));
	WARN_ON(!pgd_same(pgd, pgd));
}

#ifndef __PAGETABLE_PUD_FOLDED
static void __init pud_clear_tests(struct mm_struct *mm, pud_t *pudp)
{
	pud_t pud = READ_ONCE(*pudp);

	if (mm_pmd_folded(mm))
		return;

	pr_debug("Validating PUD clear\n");
	pud = __pud(pud_val(pud) | RANDOM_ORVALUE);
	WRITE_ONCE(*pudp, pud);
	pud_clear(pudp);
	pud = READ_ONCE(*pudp);
	WARN_ON(!pud_none(pud));
}

static void __init pud_populate_tests(struct mm_struct *mm, pud_t *pudp,
				      pmd_t *pmdp)
{
	pud_t pud;

	if (mm_pmd_folded(mm))
		return;

	pr_debug("Validating PUD populate\n");
	/*
	 * This entry points to next level page table page.
	 * Hence this must not qualify as pud_bad().
	 */
	pud_populate(mm, pudp, pmdp);
	pud = READ_ONCE(*pudp);
	WARN_ON(pud_bad(pud));
}
#else  /* !__PAGETABLE_PUD_FOLDED */
static void __init pud_clear_tests(struct mm_struct *mm, pud_t *pudp) { }
static void __init pud_populate_tests(struct mm_struct *mm, pud_t *pudp,
				      pmd_t *pmdp)
{
}
#endif /* PAGETABLE_PUD_FOLDED */

#ifndef __PAGETABLE_P4D_FOLDED
static void __init p4d_clear_tests(struct mm_struct *mm, p4d_t *p4dp)
{
	p4d_t p4d = READ_ONCE(*p4dp);

	if (mm_pud_folded(mm))
		return;

	pr_debug("Validating P4D clear\n");
	p4d = __p4d(p4d_val(p4d) | RANDOM_ORVALUE);
	WRITE_ONCE(*p4dp, p4d);
	p4d_clear(p4dp);
	p4d = READ_ONCE(*p4dp);
	WARN_ON(!p4d_none(p4d));
}

static void __init p4d_populate_tests(struct mm_struct *mm, p4d_t *p4dp,
				      pud_t *pudp)
{
	p4d_t p4d;

	if (mm_pud_folded(mm))
		return;

	pr_debug("Validating P4D populate\n");
	/*
	 * This entry points to next level page table page.
	 * Hence this must not qualify as p4d_bad().
	 */
	pud_clear(pudp);
	p4d_clear(p4dp);
	p4d_populate(mm, p4dp, pudp);
	p4d = READ_ONCE(*p4dp);
	WARN_ON(p4d_bad(p4d));
}

static void __init pgd_clear_tests(struct mm_struct *mm, pgd_t *pgdp)
{
	pgd_t pgd = READ_ONCE(*pgdp);

	if (mm_p4d_folded(mm))
		return;

	pr_debug("Validating PGD clear\n");
	pgd = __pgd(pgd_val(pgd) | RANDOM_ORVALUE);
	WRITE_ONCE(*pgdp, pgd);
	pgd_clear(pgdp);
	pgd = READ_ONCE(*pgdp);
	WARN_ON(!pgd_none(pgd));
}

static void __init pgd_populate_tests(struct mm_struct *mm, pgd_t *pgdp,
				      p4d_t *p4dp)
{
	pgd_t pgd;

	if (mm_p4d_folded(mm))
		return;

	pr_debug("Validating PGD populate\n");
	/*
	 * This entry points to next level page table page.
	 * Hence this must not qualify as pgd_bad().
	 */
	p4d_clear(p4dp);
	pgd_clear(pgdp);
	pgd_populate(mm, pgdp, p4dp);
	pgd = READ_ONCE(*pgdp);
	WARN_ON(pgd_bad(pgd));
}
#else  /* !__PAGETABLE_P4D_FOLDED */
static void __init p4d_clear_tests(struct mm_struct *mm, p4d_t *p4dp) { }
static void __init pgd_clear_tests(struct mm_struct *mm, pgd_t *pgdp) { }
static void __init p4d_populate_tests(struct mm_struct *mm, p4d_t *p4dp,
				      pud_t *pudp)
{
}
static void __init pgd_populate_tests(struct mm_struct *mm, pgd_t *pgdp,
				      p4d_t *p4dp)
{
}
#endif /* PAGETABLE_P4D_FOLDED */

static void __init pte_clear_tests(struct mm_struct *mm, pte_t *ptep,
				   unsigned long pfn, unsigned long vaddr,
				   pgprot_t prot)
{
	pte_t pte = pfn_pte(pfn, prot);

	pr_debug("Validating PTE clear\n");
#ifndef CONFIG_RISCV
	pte = __pte(pte_val(pte) | RANDOM_ORVALUE);
#endif
	set_pte_at(mm, vaddr, ptep, pte);
	barrier();
	pte_clear(mm, vaddr, ptep);
	pte = ptep_get(ptep);
	WARN_ON(!pte_none(pte));
}

static void __init pmd_clear_tests(struct mm_struct *mm, pmd_t *pmdp)
{
	pmd_t pmd = READ_ONCE(*pmdp);

	pr_debug("Validating PMD clear\n");
	pmd = __pmd(pmd_val(pmd) | RANDOM_ORVALUE);
	WRITE_ONCE(*pmdp, pmd);
	pmd_clear(pmdp);
	pmd = READ_ONCE(*pmdp);
	WARN_ON(!pmd_none(pmd));
}

static void __init pmd_populate_tests(struct mm_struct *mm, pmd_t *pmdp,
				      pgtable_t pgtable)
{
	pmd_t pmd;

	pr_debug("Validating PMD populate\n");
	/*
	 * This entry points to next level page table page.
	 * Hence this must not qualify as pmd_bad().
	 */
	pmd_populate(mm, pmdp, pgtable);
	pmd = READ_ONCE(*pmdp);
	WARN_ON(pmd_bad(pmd));
}

static void __init pte_special_tests(unsigned long pfn, pgprot_t prot)
{
	pte_t pte = pfn_pte(pfn, prot);

	if (!IS_ENABLED(CONFIG_ARCH_HAS_PTE_SPECIAL))
		return;

	pr_debug("Validating PTE special\n");
	WARN_ON(!pte_special(pte_mkspecial(pte)));
}

static void __init pte_protnone_tests(unsigned long pfn, pgprot_t prot)
{
	pte_t pte = pfn_pte(pfn, prot);

	if (!IS_ENABLED(CONFIG_NUMA_BALANCING))
		return;

	pr_debug("Validating PTE protnone\n");
	WARN_ON(!pte_protnone(pte));
	WARN_ON(!pte_present(pte));
}

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static void __init pmd_protnone_tests(unsigned long pfn, pgprot_t prot)
{
	pmd_t pmd = pmd_mkhuge(pfn_pmd(pfn, prot));

	if (!IS_ENABLED(CONFIG_NUMA_BALANCING))
		return;

	pr_debug("Validating PMD protnone\n");
	WARN_ON(!pmd_protnone(pmd));
	WARN_ON(!pmd_present(pmd));
}
#else  /* !CONFIG_TRANSPARENT_HUGEPAGE */
static void __init pmd_protnone_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_TRANSPARENT_HUGEPAGE */

#ifdef CONFIG_ARCH_HAS_PTE_DEVMAP
static void __init pte_devmap_tests(unsigned long pfn, pgprot_t prot)
{
	pte_t pte = pfn_pte(pfn, prot);

	pr_debug("Validating PTE devmap\n");
	WARN_ON(!pte_devmap(pte_mkdevmap(pte)));
}

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static void __init pmd_devmap_tests(unsigned long pfn, pgprot_t prot)
{
	pmd_t pmd = pfn_pmd(pfn, prot);

	pr_debug("Validating PMD devmap\n");
	WARN_ON(!pmd_devmap(pmd_mkdevmap(pmd)));
}

#ifdef CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD
static void __init pud_devmap_tests(unsigned long pfn, pgprot_t prot)
{
	pud_t pud = pfn_pud(pfn, prot);

	pr_debug("Validating PUD devmap\n");
	WARN_ON(!pud_devmap(pud_mkdevmap(pud)));
}
#else  /* !CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD */
static void __init pud_devmap_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD */
#else  /* CONFIG_TRANSPARENT_HUGEPAGE */
static void __init pmd_devmap_tests(unsigned long pfn, pgprot_t prot) { }
static void __init pud_devmap_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_TRANSPARENT_HUGEPAGE */
#else
static void __init pte_devmap_tests(unsigned long pfn, pgprot_t prot) { }
static void __init pmd_devmap_tests(unsigned long pfn, pgprot_t prot) { }
static void __init pud_devmap_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_ARCH_HAS_PTE_DEVMAP */

static void __init pte_soft_dirty_tests(unsigned long pfn, pgprot_t prot)
{
	pte_t pte = pfn_pte(pfn, prot);

	if (!IS_ENABLED(CONFIG_MEM_SOFT_DIRTY))
		return;

	pr_debug("Validating PTE soft dirty\n");
	WARN_ON(!pte_soft_dirty(pte_mksoft_dirty(pte)));
	WARN_ON(pte_soft_dirty(pte_clear_soft_dirty(pte)));
}

static void __init pte_swap_soft_dirty_tests(unsigned long pfn, pgprot_t prot)
{
	pte_t pte = pfn_pte(pfn, prot);

	if (!IS_ENABLED(CONFIG_MEM_SOFT_DIRTY))
		return;

	pr_debug("Validating PTE swap soft dirty\n");
	WARN_ON(!pte_swp_soft_dirty(pte_swp_mksoft_dirty(pte)));
	WARN_ON(pte_swp_soft_dirty(pte_swp_clear_soft_dirty(pte)));
}

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static void __init pmd_soft_dirty_tests(unsigned long pfn, pgprot_t prot)
{
	pmd_t pmd = pfn_pmd(pfn, prot);

	if (!IS_ENABLED(CONFIG_MEM_SOFT_DIRTY))
		return;

	pr_debug("Validating PMD soft dirty\n");
	WARN_ON(!pmd_soft_dirty(pmd_mksoft_dirty(pmd)));
	WARN_ON(pmd_soft_dirty(pmd_clear_soft_dirty(pmd)));
}

static void __init pmd_swap_soft_dirty_tests(unsigned long pfn, pgprot_t prot)
{
	pmd_t pmd = pfn_pmd(pfn, prot);

	if (!IS_ENABLED(CONFIG_MEM_SOFT_DIRTY) ||
		!IS_ENABLED(CONFIG_ARCH_ENABLE_THP_MIGRATION))
		return;

	pr_debug("Validating PMD swap soft dirty\n");
	WARN_ON(!pmd_swp_soft_dirty(pmd_swp_mksoft_dirty(pmd)));
	WARN_ON(pmd_swp_soft_dirty(pmd_swp_clear_soft_dirty(pmd)));
}
#else  /* !CONFIG_ARCH_HAS_PTE_DEVMAP */
static void __init pmd_soft_dirty_tests(unsigned long pfn, pgprot_t prot) { }
static void __init pmd_swap_soft_dirty_tests(unsigned long pfn, pgprot_t prot)
{
}
#endif /* CONFIG_ARCH_HAS_PTE_DEVMAP */

static void __init pte_swap_tests(unsigned long pfn, pgprot_t prot)
{
	swp_entry_t swp;
	pte_t pte;

	pr_debug("Validating PTE swap\n");
	pte = pfn_pte(pfn, prot);
	swp = __pte_to_swp_entry(pte);
	pte = __swp_entry_to_pte(swp);
	WARN_ON(pfn != pte_pfn(pte));
}

#ifdef CONFIG_ARCH_ENABLE_THP_MIGRATION
static void __init pmd_swap_tests(unsigned long pfn, pgprot_t prot)
{
	swp_entry_t swp;
	pmd_t pmd;

	pr_debug("Validating PMD swap\n");
	pmd = pfn_pmd(pfn, prot);
	swp = __pmd_to_swp_entry(pmd);
	pmd = __swp_entry_to_pmd(swp);
	WARN_ON(pfn != pmd_pfn(pmd));
}
#else  /* !CONFIG_ARCH_ENABLE_THP_MIGRATION */
static void __init pmd_swap_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_ARCH_ENABLE_THP_MIGRATION */

static void __init swap_migration_tests(void)
{
	struct page *page;
	swp_entry_t swp;

	if (!IS_ENABLED(CONFIG_MIGRATION))
		return;

	pr_debug("Validating swap migration\n");
	/*
	 * swap_migration_tests() requires a dedicated page as it needs to
	 * be locked before creating a migration entry from it. Locking the
	 * page that actually maps kernel text ('start_kernel') can be real
	 * problematic. Lets allocate a dedicated page explicitly for this
	 * purpose that will be freed subsequently.
	 */
	page = alloc_page(GFP_KERNEL);
	if (!page) {
		pr_err("page allocation failed\n");
		return;
	}

	/*
	 * make_migration_entry() expects given page to be
	 * locked, otherwise it stumbles upon a BUG_ON().
	 */
	__SetPageLocked(page);
	swp = make_migration_entry(page, 1);
	WARN_ON(!is_migration_entry(swp));
	WARN_ON(!is_write_migration_entry(swp));

	make_migration_entry_read(&swp);
	WARN_ON(!is_migration_entry(swp));
	WARN_ON(is_write_migration_entry(swp));

	swp = make_migration_entry(page, 0);
	WARN_ON(!is_migration_entry(swp));
	WARN_ON(is_write_migration_entry(swp));
	__ClearPageLocked(page);
	__free_page(page);
}

#ifdef CONFIG_HUGETLB_PAGE
static void __init hugetlb_basic_tests(unsigned long pfn, pgprot_t prot)
{
	struct page *page;
	pte_t pte;

	pr_debug("Validating HugeTLB basic\n");
	/*
	 * Accessing the page associated with the pfn is safe here,
	 * as it was previously derived from a real kernel symbol.
	 */
	page = pfn_to_page(pfn);
	pte = mk_huge_pte(page, prot);

	WARN_ON(!huge_pte_dirty(huge_pte_mkdirty(pte)));
	WARN_ON(!huge_pte_write(huge_pte_mkwrite(huge_pte_wrprotect(pte))));
	WARN_ON(huge_pte_write(huge_pte_wrprotect(huge_pte_mkwrite(pte))));

#ifdef CONFIG_ARCH_WANT_GENERAL_HUGETLB
	pte = pfn_pte(pfn, prot);

	WARN_ON(!pte_huge(pte_mkhuge(pte)));
#endif /* CONFIG_ARCH_WANT_GENERAL_HUGETLB */
}
#else  /* !CONFIG_HUGETLB_PAGE */
static void __init hugetlb_basic_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_HUGETLB_PAGE */

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
static void __init pmd_thp_tests(unsigned long pfn, pgprot_t prot)
{
	pmd_t pmd;

	if (!has_transparent_hugepage())
		return;

	pr_debug("Validating PMD based THP\n");
	/*
	 * pmd_trans_huge() and pmd_present() must return positive after
	 * MMU invalidation with pmd_mkinvalid(). This behavior is an
	 * optimization for transparent huge page. pmd_trans_huge() must
	 * be true if pmd_page() returns a valid THP to avoid taking the
	 * pmd_lock when others walk over non transhuge pmds (i.e. there
	 * are no THP allocated). Especially when splitting a THP and
	 * removing the present bit from the pmd, pmd_trans_huge() still
	 * needs to return true. pmd_present() should be true whenever
	 * pmd_trans_huge() returns true.
	 */
	pmd = pfn_pmd(pfn, prot);
	WARN_ON(!pmd_trans_huge(pmd_mkhuge(pmd)));

#ifndef __HAVE_ARCH_PMDP_INVALIDATE
	WARN_ON(!pmd_trans_huge(pmd_mkinvalid(pmd_mkhuge(pmd))));
	WARN_ON(!pmd_present(pmd_mkinvalid(pmd_mkhuge(pmd))));
#endif /* __HAVE_ARCH_PMDP_INVALIDATE */
}

#ifdef CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD
static void __init pud_thp_tests(unsigned long pfn, pgprot_t prot)
{
	pud_t pud;

	if (!has_transparent_hugepage())
		return;

	pr_debug("Validating PUD based THP\n");
	pud = pfn_pud(pfn, prot);
	WARN_ON(!pud_trans_huge(pud_mkhuge(pud)));

	/*
	 * pud_mkinvalid() has been dropped for now. Enable back
	 * these tests when it comes back with a modified pud_present().
	 *
	 * WARN_ON(!pud_trans_huge(pud_mkinvalid(pud_mkhuge(pud))));
	 * WARN_ON(!pud_present(pud_mkinvalid(pud_mkhuge(pud))));
	 */
}
#else  /* !CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD */
static void __init pud_thp_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD */
#else  /* !CONFIG_TRANSPARENT_HUGEPAGE */
static void __init pmd_thp_tests(unsigned long pfn, pgprot_t prot) { }
static void __init pud_thp_tests(unsigned long pfn, pgprot_t prot) { }
#endif /* CONFIG_TRANSPARENT_HUGEPAGE */

static unsigned long __init get_random_vaddr(void)
{
	unsigned long random_vaddr, random_pages, total_user_pages;

	total_user_pages = (TASK_SIZE - FIRST_USER_ADDRESS) / PAGE_SIZE;

	random_pages = get_random_long() % total_user_pages;
	random_vaddr = FIRST_USER_ADDRESS + random_pages * PAGE_SIZE;

	return random_vaddr;
}

static int __init debug_vm_pgtable(void)
{
	struct vm_area_struct *vma;
	struct mm_struct *mm;
	pgd_t *pgdp;
	p4d_t *p4dp, *saved_p4dp;
	pud_t *pudp, *saved_pudp;
	pmd_t *pmdp, *saved_pmdp, pmd;
	pte_t *ptep;
	pgtable_t saved_ptep;
	pgprot_t prot, protnone;
	phys_addr_t paddr;
	unsigned long vaddr, pte_aligned, pmd_aligned;
	unsigned long pud_aligned, p4d_aligned, pgd_aligned;
	spinlock_t *ptl = NULL;
	int idx;

	pr_info("Validating architecture page table helpers\n");
	prot = vm_get_page_prot(VMFLAGS);
	vaddr = get_random_vaddr();
	mm = mm_alloc();
	if (!mm) {
		pr_err("mm_struct allocation failed\n");
		return 1;
	}

	/*
	 * __P000 (or even __S000) will help create page table entries with
	 * PROT_NONE permission as required for pxx_protnone_tests().
	 */
	protnone = __P000;

	vma = vm_area_alloc(mm);
	if (!vma) {
		pr_err("vma allocation failed\n");
		return 1;
	}

	/*
	 * PFN for mapping at PTE level is determined from a standard kernel
	 * text symbol. But pfns for higher page table levels are derived by
	 * masking lower bits of this real pfn. These derived pfns might not
	 * exist on the platform but that does not really matter as pfn_pxx()
	 * helpers will still create appropriate entries for the test. This
	 * helps avoid large memory block allocations to be used for mapping
	 * at higher page table levels.
	 */
	paddr = __pa_symbol(&start_kernel);

	pte_aligned = (paddr & PAGE_MASK) >> PAGE_SHIFT;
	pmd_aligned = (paddr & PMD_MASK) >> PAGE_SHIFT;
	pud_aligned = (paddr & PUD_MASK) >> PAGE_SHIFT;
	p4d_aligned = (paddr & P4D_MASK) >> PAGE_SHIFT;
	pgd_aligned = (paddr & PGDIR_MASK) >> PAGE_SHIFT;
	WARN_ON(!pfn_valid(pte_aligned));

	pgdp = pgd_offset(mm, vaddr);
	p4dp = p4d_alloc(mm, pgdp, vaddr);
	pudp = pud_alloc(mm, p4dp, vaddr);
	pmdp = pmd_alloc(mm, pudp, vaddr);
	/*
	 * Allocate pgtable_t
	 */
	if (pte_alloc(mm, pmdp)) {
		pr_err("pgtable allocation failed\n");
		return 1;
	}

	/*
	 * Save all the page table page addresses as the page table
	 * entries will be used for testing with random or garbage
	 * values. These saved addresses will be used for freeing
	 * page table pages.
	 */
	pmd = READ_ONCE(*pmdp);
	saved_p4dp = p4d_offset(pgdp, 0UL);
	saved_pudp = pud_offset(p4dp, 0UL);
	saved_pmdp = pmd_offset(pudp, 0UL);
	saved_ptep = pmd_pgtable(pmd);

	/*
	 * Iterate over the protection_map[] to make sure that all
	 * the basic page table transformation validations just hold
	 * true irrespective of the starting protection value for a
	 * given page table entry.
	 */
	for (idx = 0; idx < ARRAY_SIZE(protection_map); idx++) {
		pte_basic_tests(pte_aligned, idx);
		pmd_basic_tests(pmd_aligned, idx);
		pud_basic_tests(mm, pud_aligned, idx);
	}

	/*
	 * Both P4D and PGD level tests are very basic which do not
	 * involve creating page table entries from the protection
	 * value and the given pfn. Hence just keep them out from
	 * the above iteration for now to save some test execution
	 * time.
	 */
	p4d_basic_tests(p4d_aligned, prot);
	pgd_basic_tests(pgd_aligned, prot);

	pmd_leaf_tests(pmd_aligned, prot);
	pud_leaf_tests(pud_aligned, prot);

	pte_savedwrite_tests(pte_aligned, protnone);
	pmd_savedwrite_tests(pmd_aligned, protnone);

	pte_special_tests(pte_aligned, prot);
	pte_protnone_tests(pte_aligned, protnone);
	pmd_protnone_tests(pmd_aligned, protnone);

	pte_devmap_tests(pte_aligned, prot);
	pmd_devmap_tests(pmd_aligned, prot);
	pud_devmap_tests(pud_aligned, prot);

	pte_soft_dirty_tests(pte_aligned, prot);
	pmd_soft_dirty_tests(pmd_aligned, prot);
	pte_swap_soft_dirty_tests(pte_aligned, prot);
	pmd_swap_soft_dirty_tests(pmd_aligned, prot);

	pte_swap_tests(pte_aligned, prot);
	pmd_swap_tests(pmd_aligned, prot);

	swap_migration_tests();

	pmd_thp_tests(pmd_aligned, prot);
	pud_thp_tests(pud_aligned, prot);

	hugetlb_basic_tests(pte_aligned, prot);

	/*
	 * Page table modifying tests. They need to hold
	 * proper page table lock.
	 */

	ptep = pte_offset_map_lock(mm, pmdp, vaddr, &ptl);
	pte_clear_tests(mm, ptep, pte_aligned, vaddr, prot);
	pte_advanced_tests(mm, vma, ptep, pte_aligned, vaddr, prot);
	pte_unmap_unlock(ptep, ptl);

	ptl = pmd_lock(mm, pmdp);
	pmd_clear_tests(mm, pmdp);
	pmd_advanced_tests(mm, vma, pmdp, pmd_aligned, vaddr, prot, saved_ptep);
	pmd_huge_tests(pmdp, pmd_aligned, prot);
	pmd_populate_tests(mm, pmdp, saved_ptep);
	spin_unlock(ptl);

	ptl = pud_lock(mm, pudp);
	pud_clear_tests(mm, pudp);
	pud_advanced_tests(mm, vma, pudp, pud_aligned, vaddr, prot);
	pud_huge_tests(pudp, pud_aligned, prot);
	pud_populate_tests(mm, pudp, saved_pmdp);
	spin_unlock(ptl);

	spin_lock(&mm->page_table_lock);
	p4d_clear_tests(mm, p4dp);
	pgd_clear_tests(mm, pgdp);
	p4d_populate_tests(mm, p4dp, saved_pudp);
	pgd_populate_tests(mm, pgdp, saved_p4dp);
	spin_unlock(&mm->page_table_lock);

	p4d_free(mm, saved_p4dp);
	pud_free(mm, saved_pudp);
	pmd_free(mm, saved_pmdp);
	pte_free(mm, saved_ptep);

	vm_area_free(vma);
	mm_dec_nr_puds(mm);
	mm_dec_nr_pmds(mm);
	mm_dec_nr_ptes(mm);
	mmdrop(mm);
	return 0;
}
late_initcall(debug_vm_pgtable);
