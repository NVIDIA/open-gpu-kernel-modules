// SPDX-License-Identifier: GPL-2.0-only
/*
 * CPU-agnostic AMD IO page table allocator.
 *
 * Copyright (C) 2020 Advanced Micro Devices, Inc.
 * Author: Suravee Suthikulpanit <suravee.suthikulpanit@amd.com>
 */

#define pr_fmt(fmt)     "AMD-Vi: " fmt
#define dev_fmt(fmt)    pr_fmt(fmt)

#include <linux/atomic.h>
#include <linux/bitops.h>
#include <linux/io-pgtable.h>
#include <linux/kernel.h>
#include <linux/sizes.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>

#include <asm/barrier.h>

#include "amd_iommu_types.h"
#include "amd_iommu.h"

static void v1_tlb_flush_all(void *cookie)
{
}

static void v1_tlb_flush_walk(unsigned long iova, size_t size,
				  size_t granule, void *cookie)
{
}

static void v1_tlb_add_page(struct iommu_iotlb_gather *gather,
					 unsigned long iova, size_t granule,
					 void *cookie)
{
}

static const struct iommu_flush_ops v1_flush_ops = {
	.tlb_flush_all	= v1_tlb_flush_all,
	.tlb_flush_walk = v1_tlb_flush_walk,
	.tlb_add_page	= v1_tlb_add_page,
};

/*
 * Helper function to get the first pte of a large mapping
 */
static u64 *first_pte_l7(u64 *pte, unsigned long *page_size,
			 unsigned long *count)
{
	unsigned long pte_mask, pg_size, cnt;
	u64 *fpte;

	pg_size  = PTE_PAGE_SIZE(*pte);
	cnt      = PAGE_SIZE_PTE_COUNT(pg_size);
	pte_mask = ~((cnt << 3) - 1);
	fpte     = (u64 *)(((unsigned long)pte) & pte_mask);

	if (page_size)
		*page_size = pg_size;

	if (count)
		*count = cnt;

	return fpte;
}

/****************************************************************************
 *
 * The functions below are used the create the page table mappings for
 * unity mapped regions.
 *
 ****************************************************************************/

static void free_page_list(struct page *freelist)
{
	while (freelist != NULL) {
		unsigned long p = (unsigned long)page_address(freelist);

		freelist = freelist->freelist;
		free_page(p);
	}
}

static struct page *free_pt_page(unsigned long pt, struct page *freelist)
{
	struct page *p = virt_to_page((void *)pt);

	p->freelist = freelist;

	return p;
}

#define DEFINE_FREE_PT_FN(LVL, FN)						\
static struct page *free_pt_##LVL (unsigned long __pt, struct page *freelist)	\
{										\
	unsigned long p;							\
	u64 *pt;								\
	int i;									\
										\
	pt = (u64 *)__pt;							\
										\
	for (i = 0; i < 512; ++i) {						\
		/* PTE present? */						\
		if (!IOMMU_PTE_PRESENT(pt[i]))					\
			continue;						\
										\
		/* Large PTE? */						\
		if (PM_PTE_LEVEL(pt[i]) == 0 ||					\
		    PM_PTE_LEVEL(pt[i]) == 7)					\
			continue;						\
										\
		p = (unsigned long)IOMMU_PTE_PAGE(pt[i]);			\
		freelist = FN(p, freelist);					\
	}									\
										\
	return free_pt_page((unsigned long)pt, freelist);			\
}

DEFINE_FREE_PT_FN(l2, free_pt_page)
DEFINE_FREE_PT_FN(l3, free_pt_l2)
DEFINE_FREE_PT_FN(l4, free_pt_l3)
DEFINE_FREE_PT_FN(l5, free_pt_l4)
DEFINE_FREE_PT_FN(l6, free_pt_l5)

static struct page *free_sub_pt(unsigned long root, int mode,
				struct page *freelist)
{
	switch (mode) {
	case PAGE_MODE_NONE:
	case PAGE_MODE_7_LEVEL:
		break;
	case PAGE_MODE_1_LEVEL:
		freelist = free_pt_page(root, freelist);
		break;
	case PAGE_MODE_2_LEVEL:
		freelist = free_pt_l2(root, freelist);
		break;
	case PAGE_MODE_3_LEVEL:
		freelist = free_pt_l3(root, freelist);
		break;
	case PAGE_MODE_4_LEVEL:
		freelist = free_pt_l4(root, freelist);
		break;
	case PAGE_MODE_5_LEVEL:
		freelist = free_pt_l5(root, freelist);
		break;
	case PAGE_MODE_6_LEVEL:
		freelist = free_pt_l6(root, freelist);
		break;
	default:
		BUG();
	}

	return freelist;
}

void amd_iommu_domain_set_pgtable(struct protection_domain *domain,
				  u64 *root, int mode)
{
	u64 pt_root;

	/* lowest 3 bits encode pgtable mode */
	pt_root = mode & 7;
	pt_root |= (u64)root;

	amd_iommu_domain_set_pt_root(domain, pt_root);
}

/*
 * This function is used to add another level to an IO page table. Adding
 * another level increases the size of the address space by 9 bits to a size up
 * to 64 bits.
 */
static bool increase_address_space(struct protection_domain *domain,
				   unsigned long address,
				   gfp_t gfp)
{
	unsigned long flags;
	bool ret = true;
	u64 *pte;

	pte = (void *)get_zeroed_page(gfp);
	if (!pte)
		return false;

	spin_lock_irqsave(&domain->lock, flags);

	if (address <= PM_LEVEL_SIZE(domain->iop.mode))
		goto out;

	ret = false;
	if (WARN_ON_ONCE(domain->iop.mode == PAGE_MODE_6_LEVEL))
		goto out;

	*pte = PM_LEVEL_PDE(domain->iop.mode, iommu_virt_to_phys(domain->iop.root));

	domain->iop.root  = pte;
	domain->iop.mode += 1;
	amd_iommu_update_and_flush_device_table(domain);
	amd_iommu_domain_flush_complete(domain);

	/*
	 * Device Table needs to be updated and flushed before the new root can
	 * be published.
	 */
	amd_iommu_domain_set_pgtable(domain, pte, domain->iop.mode);

	pte = NULL;
	ret = true;

out:
	spin_unlock_irqrestore(&domain->lock, flags);
	free_page((unsigned long)pte);

	return ret;
}

static u64 *alloc_pte(struct protection_domain *domain,
		      unsigned long address,
		      unsigned long page_size,
		      u64 **pte_page,
		      gfp_t gfp,
		      bool *updated)
{
	int level, end_lvl;
	u64 *pte, *page;

	BUG_ON(!is_power_of_2(page_size));

	while (address > PM_LEVEL_SIZE(domain->iop.mode)) {
		/*
		 * Return an error if there is no memory to update the
		 * page-table.
		 */
		if (!increase_address_space(domain, address, gfp))
			return NULL;
	}


	level   = domain->iop.mode - 1;
	pte     = &domain->iop.root[PM_LEVEL_INDEX(level, address)];
	address = PAGE_SIZE_ALIGN(address, page_size);
	end_lvl = PAGE_SIZE_LEVEL(page_size);

	while (level > end_lvl) {
		u64 __pte, __npte;
		int pte_level;

		__pte     = *pte;
		pte_level = PM_PTE_LEVEL(__pte);

		/*
		 * If we replace a series of large PTEs, we need
		 * to tear down all of them.
		 */
		if (IOMMU_PTE_PRESENT(__pte) &&
		    pte_level == PAGE_MODE_7_LEVEL) {
			unsigned long count, i;
			u64 *lpte;

			lpte = first_pte_l7(pte, NULL, &count);

			/*
			 * Unmap the replicated PTEs that still match the
			 * original large mapping
			 */
			for (i = 0; i < count; ++i)
				cmpxchg64(&lpte[i], __pte, 0ULL);

			*updated = true;
			continue;
		}

		if (!IOMMU_PTE_PRESENT(__pte) ||
		    pte_level == PAGE_MODE_NONE) {
			page = (u64 *)get_zeroed_page(gfp);

			if (!page)
				return NULL;

			__npte = PM_LEVEL_PDE(level, iommu_virt_to_phys(page));

			/* pte could have been changed somewhere. */
			if (cmpxchg64(pte, __pte, __npte) != __pte)
				free_page((unsigned long)page);
			else if (IOMMU_PTE_PRESENT(__pte))
				*updated = true;

			continue;
		}

		/* No level skipping support yet */
		if (pte_level != level)
			return NULL;

		level -= 1;

		pte = IOMMU_PTE_PAGE(__pte);

		if (pte_page && level == end_lvl)
			*pte_page = pte;

		pte = &pte[PM_LEVEL_INDEX(level, address)];
	}

	return pte;
}

/*
 * This function checks if there is a PTE for a given dma address. If
 * there is one, it returns the pointer to it.
 */
static u64 *fetch_pte(struct amd_io_pgtable *pgtable,
		      unsigned long address,
		      unsigned long *page_size)
{
	int level;
	u64 *pte;

	*page_size = 0;

	if (address > PM_LEVEL_SIZE(pgtable->mode))
		return NULL;

	level	   =  pgtable->mode - 1;
	pte	   = &pgtable->root[PM_LEVEL_INDEX(level, address)];
	*page_size =  PTE_LEVEL_PAGE_SIZE(level);

	while (level > 0) {

		/* Not Present */
		if (!IOMMU_PTE_PRESENT(*pte))
			return NULL;

		/* Large PTE */
		if (PM_PTE_LEVEL(*pte) == 7 ||
		    PM_PTE_LEVEL(*pte) == 0)
			break;

		/* No level skipping support yet */
		if (PM_PTE_LEVEL(*pte) != level)
			return NULL;

		level -= 1;

		/* Walk to the next level */
		pte	   = IOMMU_PTE_PAGE(*pte);
		pte	   = &pte[PM_LEVEL_INDEX(level, address)];
		*page_size = PTE_LEVEL_PAGE_SIZE(level);
	}

	/*
	 * If we have a series of large PTEs, make
	 * sure to return a pointer to the first one.
	 */
	if (PM_PTE_LEVEL(*pte) == PAGE_MODE_7_LEVEL)
		pte = first_pte_l7(pte, page_size, NULL);

	return pte;
}

static struct page *free_clear_pte(u64 *pte, u64 pteval, struct page *freelist)
{
	unsigned long pt;
	int mode;

	while (cmpxchg64(pte, pteval, 0) != pteval) {
		pr_warn("AMD-Vi: IOMMU pte changed since we read it\n");
		pteval = *pte;
	}

	if (!IOMMU_PTE_PRESENT(pteval))
		return freelist;

	pt   = (unsigned long)IOMMU_PTE_PAGE(pteval);
	mode = IOMMU_PTE_MODE(pteval);

	return free_sub_pt(pt, mode, freelist);
}

/*
 * Generic mapping functions. It maps a physical address into a DMA
 * address space. It allocates the page table pages if necessary.
 * In the future it can be extended to a generic mapping function
 * supporting all features of AMD IOMMU page tables like level skipping
 * and full 64 bit address spaces.
 */
static int iommu_v1_map_page(struct io_pgtable_ops *ops, unsigned long iova,
			  phys_addr_t paddr, size_t size, int prot, gfp_t gfp)
{
	struct protection_domain *dom = io_pgtable_ops_to_domain(ops);
	struct page *freelist = NULL;
	bool updated = false;
	u64 __pte, *pte;
	int ret, i, count;

	BUG_ON(!IS_ALIGNED(iova, size));
	BUG_ON(!IS_ALIGNED(paddr, size));

	ret = -EINVAL;
	if (!(prot & IOMMU_PROT_MASK))
		goto out;

	count = PAGE_SIZE_PTE_COUNT(size);
	pte   = alloc_pte(dom, iova, size, NULL, gfp, &updated);

	ret = -ENOMEM;
	if (!pte)
		goto out;

	for (i = 0; i < count; ++i)
		freelist = free_clear_pte(&pte[i], pte[i], freelist);

	if (freelist != NULL)
		updated = true;

	if (count > 1) {
		__pte = PAGE_SIZE_PTE(__sme_set(paddr), size);
		__pte |= PM_LEVEL_ENC(7) | IOMMU_PTE_PR | IOMMU_PTE_FC;
	} else
		__pte = __sme_set(paddr) | IOMMU_PTE_PR | IOMMU_PTE_FC;

	if (prot & IOMMU_PROT_IR)
		__pte |= IOMMU_PTE_IR;
	if (prot & IOMMU_PROT_IW)
		__pte |= IOMMU_PTE_IW;

	for (i = 0; i < count; ++i)
		pte[i] = __pte;

	ret = 0;

out:
	if (updated) {
		unsigned long flags;

		spin_lock_irqsave(&dom->lock, flags);
		/*
		 * Flush domain TLB(s) and wait for completion. Any Device-Table
		 * Updates and flushing already happened in
		 * increase_address_space().
		 */
		amd_iommu_domain_flush_tlb_pde(dom);
		amd_iommu_domain_flush_complete(dom);
		spin_unlock_irqrestore(&dom->lock, flags);
	}

	/* Everything flushed out, free pages now */
	free_page_list(freelist);

	return ret;
}

static unsigned long iommu_v1_unmap_page(struct io_pgtable_ops *ops,
				      unsigned long iova,
				      size_t size,
				      struct iommu_iotlb_gather *gather)
{
	struct amd_io_pgtable *pgtable = io_pgtable_ops_to_data(ops);
	unsigned long long unmapped;
	unsigned long unmap_size;
	u64 *pte;

	BUG_ON(!is_power_of_2(size));

	unmapped = 0;

	while (unmapped < size) {
		pte = fetch_pte(pgtable, iova, &unmap_size);
		if (pte) {
			int i, count;

			count = PAGE_SIZE_PTE_COUNT(unmap_size);
			for (i = 0; i < count; i++)
				pte[i] = 0ULL;
		}

		iova = (iova & ~(unmap_size - 1)) + unmap_size;
		unmapped += unmap_size;
	}

	BUG_ON(unmapped && !is_power_of_2(unmapped));

	return unmapped;
}

static phys_addr_t iommu_v1_iova_to_phys(struct io_pgtable_ops *ops, unsigned long iova)
{
	struct amd_io_pgtable *pgtable = io_pgtable_ops_to_data(ops);
	unsigned long offset_mask, pte_pgsize;
	u64 *pte, __pte;

	if (pgtable->mode == PAGE_MODE_NONE)
		return iova;

	pte = fetch_pte(pgtable, iova, &pte_pgsize);

	if (!pte || !IOMMU_PTE_PRESENT(*pte))
		return 0;

	offset_mask = pte_pgsize - 1;
	__pte	    = __sme_clr(*pte & PM_ADDR_MASK);

	return (__pte & ~offset_mask) | (iova & offset_mask);
}

/*
 * ----------------------------------------------------
 */
static void v1_free_pgtable(struct io_pgtable *iop)
{
	struct amd_io_pgtable *pgtable = container_of(iop, struct amd_io_pgtable, iop);
	struct protection_domain *dom;
	struct page *freelist = NULL;
	unsigned long root;

	if (pgtable->mode == PAGE_MODE_NONE)
		return;

	dom = container_of(pgtable, struct protection_domain, iop);

	/* Update data structure */
	amd_iommu_domain_clr_pt_root(dom);

	/* Make changes visible to IOMMUs */
	amd_iommu_domain_update(dom);

	/* Page-table is not visible to IOMMU anymore, so free it */
	BUG_ON(pgtable->mode < PAGE_MODE_NONE ||
	       pgtable->mode > PAGE_MODE_6_LEVEL);

	root = (unsigned long)pgtable->root;
	freelist = free_sub_pt(root, pgtable->mode, freelist);

	free_page_list(freelist);
}

static struct io_pgtable *v1_alloc_pgtable(struct io_pgtable_cfg *cfg, void *cookie)
{
	struct amd_io_pgtable *pgtable = io_pgtable_cfg_to_data(cfg);

	cfg->pgsize_bitmap  = AMD_IOMMU_PGSIZES,
	cfg->ias            = IOMMU_IN_ADDR_BIT_SIZE,
	cfg->oas            = IOMMU_OUT_ADDR_BIT_SIZE,
	cfg->tlb            = &v1_flush_ops;

	pgtable->iop.ops.map          = iommu_v1_map_page;
	pgtable->iop.ops.unmap        = iommu_v1_unmap_page;
	pgtable->iop.ops.iova_to_phys = iommu_v1_iova_to_phys;

	return &pgtable->iop;
}

struct io_pgtable_init_fns io_pgtable_amd_iommu_v1_init_fns = {
	.alloc	= v1_alloc_pgtable,
	.free	= v1_free_pgtable,
};
