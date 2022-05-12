// SPDX-License-Identifier: GPL-2.0

/*
 * Copyright 2016-2019 HabanaLabs, Ltd.
 * All Rights Reserved.
 */

#include "../habanalabs.h"
#include "../../include/hw_ip/mmu/mmu_general.h"

#include <linux/slab.h>

static inline u64 get_phys_addr(struct hl_ctx *ctx, u64 shadow_addr);

static struct pgt_info *get_pgt_info(struct hl_ctx *ctx, u64 hop_addr)
{
	struct pgt_info *pgt_info = NULL;

	hash_for_each_possible(ctx->mmu_shadow_hash, pgt_info, node,
				(unsigned long) hop_addr)
		if (hop_addr == pgt_info->shadow_addr)
			break;

	return pgt_info;
}

static void _free_hop(struct hl_ctx *ctx, struct pgt_info *pgt_info)
{
	struct hl_device *hdev = ctx->hdev;

	gen_pool_free(hdev->mmu_priv.dr.mmu_pgt_pool, pgt_info->phys_addr,
			hdev->asic_prop.mmu_hop_table_size);
	hash_del(&pgt_info->node);
	kfree((u64 *) (uintptr_t) pgt_info->shadow_addr);
	kfree(pgt_info);
}

static void free_hop(struct hl_ctx *ctx, u64 hop_addr)
{
	struct pgt_info *pgt_info = get_pgt_info(ctx, hop_addr);

	_free_hop(ctx, pgt_info);
}

static u64 alloc_hop(struct hl_ctx *ctx)
{
	struct hl_device *hdev = ctx->hdev;
	struct asic_fixed_properties *prop = &hdev->asic_prop;
	struct pgt_info *pgt_info;
	u64 phys_addr, shadow_addr;

	pgt_info = kmalloc(sizeof(*pgt_info), GFP_KERNEL);
	if (!pgt_info)
		return ULLONG_MAX;

	phys_addr = (u64) gen_pool_alloc(hdev->mmu_priv.dr.mmu_pgt_pool,
					prop->mmu_hop_table_size);
	if (!phys_addr) {
		dev_err(hdev->dev, "failed to allocate page\n");
		goto pool_add_err;
	}

	shadow_addr = (u64) (uintptr_t) kzalloc(prop->mmu_hop_table_size,
						GFP_KERNEL);
	if (!shadow_addr)
		goto shadow_err;

	pgt_info->phys_addr = phys_addr;
	pgt_info->shadow_addr = shadow_addr;
	pgt_info->ctx = ctx;
	pgt_info->num_of_ptes = 0;
	hash_add(ctx->mmu_shadow_hash, &pgt_info->node, shadow_addr);

	return shadow_addr;

shadow_err:
	gen_pool_free(hdev->mmu_priv.dr.mmu_pgt_pool, phys_addr,
			prop->mmu_hop_table_size);
pool_add_err:
	kfree(pgt_info);

	return ULLONG_MAX;
}

static inline u64 get_phys_hop0_addr(struct hl_ctx *ctx)
{
	return ctx->hdev->asic_prop.mmu_pgt_addr +
			(ctx->asid * ctx->hdev->asic_prop.mmu_hop_table_size);
}

static inline u64 get_hop0_addr(struct hl_ctx *ctx)
{
	return (u64) (uintptr_t) ctx->hdev->mmu_priv.dr.mmu_shadow_hop0 +
			(ctx->asid * ctx->hdev->asic_prop.mmu_hop_table_size);
}

static void flush(struct hl_ctx *ctx)
{
	/* flush all writes from all cores to reach PCI */
	mb();
	ctx->hdev->asic_funcs->read_pte(ctx->hdev, get_phys_hop0_addr(ctx));
}

/* transform the value to physical address when writing to H/W */
static inline void write_pte(struct hl_ctx *ctx, u64 shadow_pte_addr, u64 val)
{
	/*
	 * The value to write is actually the address of the next shadow hop +
	 * flags at the 12 LSBs.
	 * Hence in order to get the value to write to the physical PTE, we
	 * clear the 12 LSBs and translate the shadow hop to its associated
	 * physical hop, and add back the original 12 LSBs.
	 */
	u64 phys_val = get_phys_addr(ctx, val & HOP_PHYS_ADDR_MASK) |
				(val & FLAGS_MASK);

	ctx->hdev->asic_funcs->write_pte(ctx->hdev,
					get_phys_addr(ctx, shadow_pte_addr),
					phys_val);

	*(u64 *) (uintptr_t) shadow_pte_addr = val;
}

/* do not transform the value to physical address when writing to H/W */
static inline void write_final_pte(struct hl_ctx *ctx, u64 shadow_pte_addr,
					u64 val)
{
	ctx->hdev->asic_funcs->write_pte(ctx->hdev,
					get_phys_addr(ctx, shadow_pte_addr),
					val);
	*(u64 *) (uintptr_t) shadow_pte_addr = val;
}

/* clear the last and present bits */
static inline void clear_pte(struct hl_ctx *ctx, u64 pte_addr)
{
	/* no need to transform the value to physical address */
	write_final_pte(ctx, pte_addr, 0);
}

static inline void get_pte(struct hl_ctx *ctx, u64 hop_addr)
{
	get_pgt_info(ctx, hop_addr)->num_of_ptes++;
}

/*
 * put_pte - decrement the num of ptes and free the hop if possible
 *
 * @ctx: pointer to the context structure
 * @hop_addr: addr of the hop
 *
 * This function returns the number of ptes left on this hop. If the number is
 * 0, it means the pte was freed.
 */
static inline int put_pte(struct hl_ctx *ctx, u64 hop_addr)
{
	struct pgt_info *pgt_info = get_pgt_info(ctx, hop_addr);
	int num_of_ptes_left;

	pgt_info->num_of_ptes--;

	/*
	 * Need to save the number of ptes left because free_hop might free
	 * the pgt_info
	 */
	num_of_ptes_left = pgt_info->num_of_ptes;
	if (!num_of_ptes_left)
		_free_hop(ctx, pgt_info);

	return num_of_ptes_left;
}

static inline u64 get_hopN_pte_addr(struct hl_ctx *ctx, u64 hop_addr,
					u64 virt_addr, u64 mask, u64 shift)
{
	return hop_addr + ctx->hdev->asic_prop.mmu_pte_size *
			((virt_addr & mask) >> shift);
}

static inline u64 get_hop0_pte_addr(struct hl_ctx *ctx,
					struct hl_mmu_properties *mmu_prop,
					u64 hop_addr, u64 vaddr)
{
	return get_hopN_pte_addr(ctx, hop_addr, vaddr, mmu_prop->hop0_mask,
					mmu_prop->hop0_shift);
}

static inline u64 get_hop1_pte_addr(struct hl_ctx *ctx,
					struct hl_mmu_properties *mmu_prop,
					u64 hop_addr, u64 vaddr)
{
	return get_hopN_pte_addr(ctx, hop_addr, vaddr, mmu_prop->hop1_mask,
					mmu_prop->hop1_shift);
}

static inline u64 get_hop2_pte_addr(struct hl_ctx *ctx,
					struct hl_mmu_properties *mmu_prop,
					u64 hop_addr, u64 vaddr)
{
	return get_hopN_pte_addr(ctx, hop_addr, vaddr, mmu_prop->hop2_mask,
					mmu_prop->hop2_shift);
}

static inline u64 get_hop3_pte_addr(struct hl_ctx *ctx,
					struct hl_mmu_properties *mmu_prop,
					u64 hop_addr, u64 vaddr)
{
	return get_hopN_pte_addr(ctx, hop_addr, vaddr, mmu_prop->hop3_mask,
					mmu_prop->hop3_shift);
}

static inline u64 get_hop4_pte_addr(struct hl_ctx *ctx,
					struct hl_mmu_properties *mmu_prop,
					u64 hop_addr, u64 vaddr)
{
	return get_hopN_pte_addr(ctx, hop_addr, vaddr, mmu_prop->hop4_mask,
					mmu_prop->hop4_shift);
}

static inline u64 get_next_hop_addr(struct hl_ctx *ctx, u64 curr_pte)
{
	if (curr_pte & PAGE_PRESENT_MASK)
		return curr_pte & HOP_PHYS_ADDR_MASK;
	else
		return ULLONG_MAX;
}

static inline u64 get_alloc_next_hop_addr(struct hl_ctx *ctx, u64 curr_pte,
						bool *is_new_hop)
{
	u64 hop_addr = get_next_hop_addr(ctx, curr_pte);

	if (hop_addr == ULLONG_MAX) {
		hop_addr = alloc_hop(ctx);
		*is_new_hop = (hop_addr != ULLONG_MAX);
	}

	return hop_addr;
}

/* translates shadow address inside hop to a physical address */
static inline u64 get_phys_addr(struct hl_ctx *ctx, u64 shadow_addr)
{
	u64 page_mask = (ctx->hdev->asic_prop.mmu_hop_table_size - 1);
	u64 shadow_hop_addr = shadow_addr & ~page_mask;
	u64 pte_offset = shadow_addr & page_mask;
	u64 phys_hop_addr;

	if (shadow_hop_addr != get_hop0_addr(ctx))
		phys_hop_addr = get_pgt_info(ctx, shadow_hop_addr)->phys_addr;
	else
		phys_hop_addr = get_phys_hop0_addr(ctx);

	return phys_hop_addr + pte_offset;
}

static int dram_default_mapping_init(struct hl_ctx *ctx)
{
	struct hl_device *hdev = ctx->hdev;
	struct asic_fixed_properties *prop = &hdev->asic_prop;
	u64 num_of_hop3, total_hops, hop0_addr, hop1_addr, hop2_addr,
		hop2_pte_addr, hop3_pte_addr, pte_val;
	int rc, i, j, hop3_allocated = 0;

	if ((!prop->dram_supports_virtual_memory) ||
			(!hdev->dram_default_page_mapping) ||
			(ctx->asid == HL_KERNEL_ASID_ID))
		return 0;

	num_of_hop3 = prop->dram_size_for_default_page_mapping;
	do_div(num_of_hop3, prop->dram_page_size);
	do_div(num_of_hop3, PTE_ENTRIES_IN_HOP);

	/* add hop1 and hop2 */
	total_hops = num_of_hop3 + 2;

	ctx->dram_default_hops = kzalloc(HL_PTE_SIZE * total_hops,  GFP_KERNEL);
	if (!ctx->dram_default_hops)
		return -ENOMEM;

	hop0_addr = get_hop0_addr(ctx);

	hop1_addr = alloc_hop(ctx);
	if (hop1_addr == ULLONG_MAX) {
		dev_err(hdev->dev, "failed to alloc hop 1\n");
		rc = -ENOMEM;
		goto hop1_err;
	}

	ctx->dram_default_hops[total_hops - 1] = hop1_addr;

	hop2_addr = alloc_hop(ctx);
	if (hop2_addr == ULLONG_MAX) {
		dev_err(hdev->dev, "failed to alloc hop 2\n");
		rc = -ENOMEM;
		goto hop2_err;
	}

	ctx->dram_default_hops[total_hops - 2] = hop2_addr;

	for (i = 0 ; i < num_of_hop3 ; i++) {
		ctx->dram_default_hops[i] = alloc_hop(ctx);
		if (ctx->dram_default_hops[i] == ULLONG_MAX) {
			dev_err(hdev->dev, "failed to alloc hop 3, i: %d\n", i);
			rc = -ENOMEM;
			goto hop3_err;
		}
		hop3_allocated++;
	}

	/* need only pte 0 in hops 0 and 1 */
	pte_val = (hop1_addr & HOP_PHYS_ADDR_MASK) | PAGE_PRESENT_MASK;
	write_pte(ctx, hop0_addr, pte_val);

	pte_val = (hop2_addr & HOP_PHYS_ADDR_MASK) | PAGE_PRESENT_MASK;
	write_pte(ctx, hop1_addr, pte_val);
	get_pte(ctx, hop1_addr);

	hop2_pte_addr = hop2_addr;
	for (i = 0 ; i < num_of_hop3 ; i++) {
		pte_val = (ctx->dram_default_hops[i] & HOP_PHYS_ADDR_MASK) |
				PAGE_PRESENT_MASK;
		write_pte(ctx, hop2_pte_addr, pte_val);
		get_pte(ctx, hop2_addr);
		hop2_pte_addr += HL_PTE_SIZE;
	}

	pte_val = (prop->mmu_dram_default_page_addr & HOP_PHYS_ADDR_MASK) |
			LAST_MASK | PAGE_PRESENT_MASK;

	for (i = 0 ; i < num_of_hop3 ; i++) {
		hop3_pte_addr = ctx->dram_default_hops[i];
		for (j = 0 ; j < PTE_ENTRIES_IN_HOP ; j++) {
			write_final_pte(ctx, hop3_pte_addr, pte_val);
			get_pte(ctx, ctx->dram_default_hops[i]);
			hop3_pte_addr += HL_PTE_SIZE;
		}
	}

	flush(ctx);

	return 0;

hop3_err:
	for (i = 0 ; i < hop3_allocated ; i++)
		free_hop(ctx, ctx->dram_default_hops[i]);

	free_hop(ctx, hop2_addr);
hop2_err:
	free_hop(ctx, hop1_addr);
hop1_err:
	kfree(ctx->dram_default_hops);

	return rc;
}

static void dram_default_mapping_fini(struct hl_ctx *ctx)
{
	struct hl_device *hdev = ctx->hdev;
	struct asic_fixed_properties *prop = &hdev->asic_prop;
	u64 num_of_hop3, total_hops, hop0_addr, hop1_addr, hop2_addr,
		hop2_pte_addr, hop3_pte_addr;
	int i, j;

	if ((!prop->dram_supports_virtual_memory) ||
			(!hdev->dram_default_page_mapping) ||
			(ctx->asid == HL_KERNEL_ASID_ID))
		return;

	num_of_hop3 = prop->dram_size_for_default_page_mapping;
	do_div(num_of_hop3, prop->dram_page_size);
	do_div(num_of_hop3, PTE_ENTRIES_IN_HOP);

	hop0_addr = get_hop0_addr(ctx);
	/* add hop1 and hop2 */
	total_hops = num_of_hop3 + 2;
	hop1_addr = ctx->dram_default_hops[total_hops - 1];
	hop2_addr = ctx->dram_default_hops[total_hops - 2];

	for (i = 0 ; i < num_of_hop3 ; i++) {
		hop3_pte_addr = ctx->dram_default_hops[i];
		for (j = 0 ; j < PTE_ENTRIES_IN_HOP ; j++) {
			clear_pte(ctx, hop3_pte_addr);
			put_pte(ctx, ctx->dram_default_hops[i]);
			hop3_pte_addr += HL_PTE_SIZE;
		}
	}

	hop2_pte_addr = hop2_addr;
	hop2_pte_addr = hop2_addr;
	for (i = 0 ; i < num_of_hop3 ; i++) {
		clear_pte(ctx, hop2_pte_addr);
		put_pte(ctx, hop2_addr);
		hop2_pte_addr += HL_PTE_SIZE;
	}

	clear_pte(ctx, hop1_addr);
	put_pte(ctx, hop1_addr);
	clear_pte(ctx, hop0_addr);

	kfree(ctx->dram_default_hops);

	flush(ctx);
}

/**
 * hl_mmu_v1_init() - initialize the MMU module.
 * @hdev: habanalabs device structure.
 *
 * This function does the following:
 * - Create a pool of pages for pgt_infos.
 * - Create a shadow table for pgt
 *
 * Return: 0 for success, non-zero for failure.
 */
static int hl_mmu_v1_init(struct hl_device *hdev)
{
	struct asic_fixed_properties *prop = &hdev->asic_prop;
	int rc;

	hdev->mmu_priv.dr.mmu_pgt_pool =
			gen_pool_create(__ffs(prop->mmu_hop_table_size), -1);

	if (!hdev->mmu_priv.dr.mmu_pgt_pool) {
		dev_err(hdev->dev, "Failed to create page gen pool\n");
		return -ENOMEM;
	}

	rc = gen_pool_add(hdev->mmu_priv.dr.mmu_pgt_pool, prop->mmu_pgt_addr +
			prop->mmu_hop0_tables_total_size,
			prop->mmu_pgt_size - prop->mmu_hop0_tables_total_size,
			-1);
	if (rc) {
		dev_err(hdev->dev, "Failed to add memory to page gen pool\n");
		goto err_pool_add;
	}

	hdev->mmu_priv.dr.mmu_shadow_hop0 = kvmalloc_array(prop->max_asid,
						prop->mmu_hop_table_size,
						GFP_KERNEL | __GFP_ZERO);
	if (ZERO_OR_NULL_PTR(hdev->mmu_priv.dr.mmu_shadow_hop0)) {
		rc = -ENOMEM;
		goto err_pool_add;
	}

	/* MMU H/W init will be done in device hw_init() */

	return 0;

err_pool_add:
	gen_pool_destroy(hdev->mmu_priv.dr.mmu_pgt_pool);

	return rc;
}

/**
 * hl_mmu_fini() - release the MMU module.
 * @hdev: habanalabs device structure.
 *
 * This function does the following:
 * - Disable MMU in H/W.
 * - Free the pgt_infos pool.
 *
 * All contexts should be freed before calling this function.
 */
static void hl_mmu_v1_fini(struct hl_device *hdev)
{
	/* MMU H/W fini was already done in device hw_fini() */

	if (!ZERO_OR_NULL_PTR(hdev->mmu_priv.hr.mmu_shadow_hop0)) {
		kvfree(hdev->mmu_priv.dr.mmu_shadow_hop0);
		gen_pool_destroy(hdev->mmu_priv.dr.mmu_pgt_pool);
	}

	/* Make sure that if we arrive here again without init was called we
	 * won't cause kernel panic. This can happen for example if we fail
	 * during hard reset code at certain points
	 */
	hdev->mmu_priv.dr.mmu_shadow_hop0 = NULL;
}

/**
 * hl_mmu_ctx_init() - initialize a context for using the MMU module.
 * @ctx: pointer to the context structure to initialize.
 *
 * Initialize a mutex to protect the concurrent mapping flow, a hash to hold all
 * page tables hops related to this context.
 * Return: 0 on success, non-zero otherwise.
 */
static int hl_mmu_v1_ctx_init(struct hl_ctx *ctx)
{
	hash_init(ctx->mmu_shadow_hash);
	return dram_default_mapping_init(ctx);
}

/*
 * hl_mmu_ctx_fini - disable a ctx from using the mmu module
 *
 * @ctx: pointer to the context structure
 *
 * This function does the following:
 * - Free any pgts which were not freed yet
 * - Free the mutex
 * - Free DRAM default page mapping hops
 */
static void hl_mmu_v1_ctx_fini(struct hl_ctx *ctx)
{
	struct hl_device *hdev = ctx->hdev;
	struct pgt_info *pgt_info;
	struct hlist_node *tmp;
	int i;

	dram_default_mapping_fini(ctx);

	if (!hash_empty(ctx->mmu_shadow_hash))
		dev_err(hdev->dev, "ctx %d is freed while it has pgts in use\n",
			ctx->asid);

	hash_for_each_safe(ctx->mmu_shadow_hash, i, tmp, pgt_info, node) {
		dev_err_ratelimited(hdev->dev,
			"pgt_info of addr 0x%llx of asid %d was not destroyed, num_ptes: %d\n",
			pgt_info->phys_addr, ctx->asid, pgt_info->num_of_ptes);
		_free_hop(ctx, pgt_info);
	}
}

static int _hl_mmu_v1_unmap(struct hl_ctx *ctx,
				u64 virt_addr, bool is_dram_addr)
{
	struct hl_device *hdev = ctx->hdev;
	struct asic_fixed_properties *prop = &hdev->asic_prop;
	struct hl_mmu_properties *mmu_prop;
	u64 hop0_addr = 0, hop0_pte_addr = 0,
		hop1_addr = 0, hop1_pte_addr = 0,
		hop2_addr = 0, hop2_pte_addr = 0,
		hop3_addr = 0, hop3_pte_addr = 0,
		hop4_addr = 0, hop4_pte_addr = 0,
		curr_pte;
	bool is_huge, clear_hop3 = true;

	/* shifts and masks are the same in PMMU and HPMMU, use one of them */
	mmu_prop = is_dram_addr ? &prop->dmmu : &prop->pmmu;

	hop0_addr = get_hop0_addr(ctx);
	hop0_pte_addr = get_hop0_pte_addr(ctx, mmu_prop, hop0_addr, virt_addr);

	curr_pte = *(u64 *) (uintptr_t) hop0_pte_addr;

	hop1_addr = get_next_hop_addr(ctx, curr_pte);

	if (hop1_addr == ULLONG_MAX)
		goto not_mapped;

	hop1_pte_addr = get_hop1_pte_addr(ctx, mmu_prop, hop1_addr, virt_addr);

	curr_pte = *(u64 *) (uintptr_t) hop1_pte_addr;

	hop2_addr = get_next_hop_addr(ctx, curr_pte);

	if (hop2_addr == ULLONG_MAX)
		goto not_mapped;

	hop2_pte_addr = get_hop2_pte_addr(ctx, mmu_prop, hop2_addr, virt_addr);

	curr_pte = *(u64 *) (uintptr_t) hop2_pte_addr;

	hop3_addr = get_next_hop_addr(ctx, curr_pte);

	if (hop3_addr == ULLONG_MAX)
		goto not_mapped;

	hop3_pte_addr = get_hop3_pte_addr(ctx, mmu_prop, hop3_addr, virt_addr);

	curr_pte = *(u64 *) (uintptr_t) hop3_pte_addr;

	is_huge = curr_pte & LAST_MASK;

	if (is_dram_addr && !is_huge) {
		dev_err(hdev->dev,
				"DRAM unmapping should use huge pages only\n");
		return -EFAULT;
	}

	if (!is_huge) {
		hop4_addr = get_next_hop_addr(ctx, curr_pte);

		if (hop4_addr == ULLONG_MAX)
			goto not_mapped;

		hop4_pte_addr = get_hop4_pte_addr(ctx, mmu_prop, hop4_addr,
							virt_addr);

		curr_pte = *(u64 *) (uintptr_t) hop4_pte_addr;

		clear_hop3 = false;
	}

	if (hdev->dram_default_page_mapping && is_dram_addr) {
		u64 default_pte = (prop->mmu_dram_default_page_addr &
				HOP_PHYS_ADDR_MASK) | LAST_MASK |
					PAGE_PRESENT_MASK;
		if (curr_pte == default_pte) {
			dev_err(hdev->dev,
				"DRAM: hop3 PTE points to zero page, can't unmap, va: 0x%llx\n",
					virt_addr);
			goto not_mapped;
		}

		if (!(curr_pte & PAGE_PRESENT_MASK)) {
			dev_err(hdev->dev,
				"DRAM: hop3 PTE is cleared! can't unmap, va: 0x%llx\n",
					virt_addr);
			goto not_mapped;
		}

		write_final_pte(ctx, hop3_pte_addr, default_pte);
		put_pte(ctx, hop3_addr);
	} else {
		if (!(curr_pte & PAGE_PRESENT_MASK))
			goto not_mapped;

		if (hop4_addr)
			clear_pte(ctx, hop4_pte_addr);
		else
			clear_pte(ctx, hop3_pte_addr);

		if (hop4_addr && !put_pte(ctx, hop4_addr))
			clear_hop3 = true;

		if (!clear_hop3)
			goto mapped;

		clear_pte(ctx, hop3_pte_addr);

		if (put_pte(ctx, hop3_addr))
			goto mapped;

		clear_pte(ctx, hop2_pte_addr);

		if (put_pte(ctx, hop2_addr))
			goto mapped;

		clear_pte(ctx, hop1_pte_addr);

		if (put_pte(ctx, hop1_addr))
			goto mapped;

		clear_pte(ctx, hop0_pte_addr);
	}

mapped:
	return 0;

not_mapped:
	dev_err(hdev->dev, "virt addr 0x%llx is not mapped to phys addr\n",
		virt_addr);

	return -EINVAL;
}

static int _hl_mmu_v1_map(struct hl_ctx *ctx, u64 virt_addr, u64 phys_addr,
			u32 page_size, bool is_dram_addr)
{
	struct hl_device *hdev = ctx->hdev;
	struct asic_fixed_properties *prop = &hdev->asic_prop;
	struct hl_mmu_properties *mmu_prop;
	u64 hop0_addr = 0, hop0_pte_addr = 0,
		hop1_addr = 0, hop1_pte_addr = 0,
		hop2_addr = 0, hop2_pte_addr = 0,
		hop3_addr = 0, hop3_pte_addr = 0,
		hop4_addr = 0, hop4_pte_addr = 0,
		curr_pte = 0;
	bool hop1_new = false, hop2_new = false, hop3_new = false,
		hop4_new = false, is_huge;
	int rc = -ENOMEM;

	/*
	 * This mapping function can map a page or a huge page. For huge page
	 * there are only 3 hops rather than 4. Currently the DRAM allocation
	 * uses huge pages only but user memory could have been allocated with
	 * one of the two page sizes. Since this is a common code for all the
	 * three cases, we need this hugs page check.
	 */
	if (is_dram_addr) {
		mmu_prop = &prop->dmmu;
		is_huge = true;
	} else if (page_size == prop->pmmu_huge.page_size) {
		mmu_prop = &prop->pmmu_huge;
		is_huge = true;
	} else {
		mmu_prop = &prop->pmmu;
		is_huge = false;
	}

	hop0_addr = get_hop0_addr(ctx);
	hop0_pte_addr = get_hop0_pte_addr(ctx, mmu_prop, hop0_addr, virt_addr);
	curr_pte = *(u64 *) (uintptr_t) hop0_pte_addr;

	hop1_addr = get_alloc_next_hop_addr(ctx, curr_pte, &hop1_new);
	if (hop1_addr == ULLONG_MAX)
		goto err;

	hop1_pte_addr = get_hop1_pte_addr(ctx, mmu_prop, hop1_addr, virt_addr);
	curr_pte = *(u64 *) (uintptr_t) hop1_pte_addr;

	hop2_addr = get_alloc_next_hop_addr(ctx, curr_pte, &hop2_new);
	if (hop2_addr == ULLONG_MAX)
		goto err;

	hop2_pte_addr = get_hop2_pte_addr(ctx, mmu_prop, hop2_addr, virt_addr);
	curr_pte = *(u64 *) (uintptr_t) hop2_pte_addr;

	hop3_addr = get_alloc_next_hop_addr(ctx, curr_pte, &hop3_new);
	if (hop3_addr == ULLONG_MAX)
		goto err;

	hop3_pte_addr = get_hop3_pte_addr(ctx, mmu_prop, hop3_addr, virt_addr);
	curr_pte = *(u64 *) (uintptr_t) hop3_pte_addr;

	if (!is_huge) {
		hop4_addr = get_alloc_next_hop_addr(ctx, curr_pte, &hop4_new);
		if (hop4_addr == ULLONG_MAX)
			goto err;

		hop4_pte_addr = get_hop4_pte_addr(ctx, mmu_prop, hop4_addr,
							virt_addr);
		curr_pte = *(u64 *) (uintptr_t) hop4_pte_addr;
	}

	if (hdev->dram_default_page_mapping && is_dram_addr) {
		u64 default_pte = (prop->mmu_dram_default_page_addr &
					HOP_PHYS_ADDR_MASK) | LAST_MASK |
						PAGE_PRESENT_MASK;

		if (curr_pte != default_pte) {
			dev_err(hdev->dev,
				"DRAM: mapping already exists for virt_addr 0x%llx\n",
					virt_addr);
			rc = -EINVAL;
			goto err;
		}

		if (hop1_new || hop2_new || hop3_new || hop4_new) {
			dev_err(hdev->dev,
				"DRAM mapping should not allocate more hops\n");
			rc = -EFAULT;
			goto err;
		}
	} else if (curr_pte & PAGE_PRESENT_MASK) {
		dev_err(hdev->dev,
			"mapping already exists for virt_addr 0x%llx\n",
				virt_addr);

		dev_dbg(hdev->dev, "hop0 pte: 0x%llx (0x%llx)\n",
			*(u64 *) (uintptr_t) hop0_pte_addr, hop0_pte_addr);
		dev_dbg(hdev->dev, "hop1 pte: 0x%llx (0x%llx)\n",
			*(u64 *) (uintptr_t) hop1_pte_addr, hop1_pte_addr);
		dev_dbg(hdev->dev, "hop2 pte: 0x%llx (0x%llx)\n",
			*(u64 *) (uintptr_t) hop2_pte_addr, hop2_pte_addr);
		dev_dbg(hdev->dev, "hop3 pte: 0x%llx (0x%llx)\n",
			*(u64 *) (uintptr_t) hop3_pte_addr, hop3_pte_addr);

		if (!is_huge)
			dev_dbg(hdev->dev, "hop4 pte: 0x%llx (0x%llx)\n",
				*(u64 *) (uintptr_t) hop4_pte_addr,
				hop4_pte_addr);

		rc = -EINVAL;
		goto err;
	}

	curr_pte = (phys_addr & HOP_PHYS_ADDR_MASK) | LAST_MASK
			| PAGE_PRESENT_MASK;

	if (is_huge)
		write_final_pte(ctx, hop3_pte_addr, curr_pte);
	else
		write_final_pte(ctx, hop4_pte_addr, curr_pte);

	if (hop1_new) {
		curr_pte =
			(hop1_addr & HOP_PHYS_ADDR_MASK) | PAGE_PRESENT_MASK;
		write_pte(ctx, hop0_pte_addr, curr_pte);
	}
	if (hop2_new) {
		curr_pte =
			(hop2_addr & HOP_PHYS_ADDR_MASK) | PAGE_PRESENT_MASK;
		write_pte(ctx, hop1_pte_addr, curr_pte);
		get_pte(ctx, hop1_addr);
	}
	if (hop3_new) {
		curr_pte =
			(hop3_addr & HOP_PHYS_ADDR_MASK) | PAGE_PRESENT_MASK;
		write_pte(ctx, hop2_pte_addr, curr_pte);
		get_pte(ctx, hop2_addr);
	}

	if (!is_huge) {
		if (hop4_new) {
			curr_pte = (hop4_addr & HOP_PHYS_ADDR_MASK) |
					PAGE_PRESENT_MASK;
			write_pte(ctx, hop3_pte_addr, curr_pte);
			get_pte(ctx, hop3_addr);
		}

		get_pte(ctx, hop4_addr);
	} else {
		get_pte(ctx, hop3_addr);
	}

	return 0;

err:
	if (hop4_new)
		free_hop(ctx, hop4_addr);
	if (hop3_new)
		free_hop(ctx, hop3_addr);
	if (hop2_new)
		free_hop(ctx, hop2_addr);
	if (hop1_new)
		free_hop(ctx, hop1_addr);

	return rc;
}

/*
 * hl_mmu_v1_swap_out - marks all mapping of the given ctx as swapped out
 *
 * @ctx: pointer to the context structure
 *
 */
static void hl_mmu_v1_swap_out(struct hl_ctx *ctx)
{

}

/*
 * hl_mmu_v1_swap_in - marks all mapping of the given ctx as swapped in
 *
 * @ctx: pointer to the context structure
 *
 */
static void hl_mmu_v1_swap_in(struct hl_ctx *ctx)
{

}

static inline u64 get_hop_pte_addr(struct hl_ctx *ctx,
				struct hl_mmu_properties *mmu_prop,
				int hop_num, u64 hop_addr, u64 virt_addr)
{
	switch (hop_num) {
	case 0:
		return get_hop0_pte_addr(ctx, mmu_prop, hop_addr, virt_addr);
	case 1:
		return get_hop1_pte_addr(ctx, mmu_prop, hop_addr, virt_addr);
	case 2:
		return get_hop2_pte_addr(ctx, mmu_prop, hop_addr, virt_addr);
	case 3:
		return get_hop3_pte_addr(ctx, mmu_prop, hop_addr, virt_addr);
	case 4:
		return get_hop4_pte_addr(ctx, mmu_prop, hop_addr, virt_addr);
	default:
		break;
	}
	return U64_MAX;
}

static int hl_mmu_v1_get_tlb_info(struct hl_ctx *ctx, u64 virt_addr,
				struct hl_mmu_hop_info *hops)
{
	struct hl_device *hdev = ctx->hdev;
	struct asic_fixed_properties *prop = &hdev->asic_prop;
	struct hl_mmu_properties *mmu_prop;
	bool is_dram_addr, is_pmmu_addr, is_pmmu_h_addr, is_huge;
	int i, used_hops;

	is_dram_addr = hl_mem_area_inside_range(virt_addr, prop->dmmu.page_size,
						prop->dmmu.start_addr,
						prop->dmmu.end_addr);
	is_pmmu_addr = hl_mem_area_inside_range(virt_addr, prop->pmmu.page_size,
						prop->pmmu.start_addr,
						prop->pmmu.end_addr);
	is_pmmu_h_addr = hl_mem_area_inside_range(virt_addr,
						prop->pmmu_huge.page_size,
						prop->pmmu_huge.start_addr,
						prop->pmmu_huge.end_addr);
	if (is_dram_addr) {
		mmu_prop = &prop->dmmu;
		is_huge = true;
	} else if (is_pmmu_addr) {
		mmu_prop = &prop->pmmu;
		is_huge = false;
	} else if (is_pmmu_h_addr) {
		mmu_prop = &prop->pmmu_huge;
		is_huge = true;
	} else {
		return -EINVAL;
	}

	used_hops = mmu_prop->num_hops;

	/* huge pages use lesser hops */
	if (is_huge)
		used_hops--;

	hops->hop_info[0].hop_addr = get_phys_hop0_addr(ctx);
	hops->hop_info[0].hop_pte_addr =
			get_hop_pte_addr(ctx, mmu_prop, 0,
					hops->hop_info[0].hop_addr, virt_addr);
	hops->hop_info[0].hop_pte_val =
			hdev->asic_funcs->read_pte(hdev,
						hops->hop_info[0].hop_pte_addr);

	for (i = 1 ; i < used_hops ; i++) {
		hops->hop_info[i].hop_addr =
			get_next_hop_addr(ctx,
					hops->hop_info[i - 1].hop_pte_val);
		if (hops->hop_info[i].hop_addr == ULLONG_MAX)
			return -EFAULT;

		hops->hop_info[i].hop_pte_addr =
				get_hop_pte_addr(ctx, mmu_prop, i,
						hops->hop_info[i].hop_addr,
						virt_addr);
		hops->hop_info[i].hop_pte_val =
				hdev->asic_funcs->read_pte(hdev,
						hops->hop_info[i].hop_pte_addr);

		if (!(hops->hop_info[i].hop_pte_val & PAGE_PRESENT_MASK))
			return -EFAULT;

		if (hops->hop_info[i].hop_pte_val & LAST_MASK)
			break;
	}

	/* if passed over all hops then no last hop was found */
	if (i == mmu_prop->num_hops)
		return -EFAULT;

	if (!(hops->hop_info[i].hop_pte_val & PAGE_PRESENT_MASK))
		return -EFAULT;

	hops->used_hops = i + 1;

	return 0;
}

/*
 * hl_mmu_v1_prepare - prepare mmu  for working with mmu v1
 *
 * @hdev: pointer to the device structure
 */
void hl_mmu_v1_set_funcs(struct hl_device *hdev, struct hl_mmu_funcs *mmu)
{
	mmu->init = hl_mmu_v1_init;
	mmu->fini = hl_mmu_v1_fini;
	mmu->ctx_init = hl_mmu_v1_ctx_init;
	mmu->ctx_fini = hl_mmu_v1_ctx_fini;
	mmu->map = _hl_mmu_v1_map;
	mmu->unmap = _hl_mmu_v1_unmap;
	mmu->flush = flush;
	mmu->swap_out = hl_mmu_v1_swap_out;
	mmu->swap_in = hl_mmu_v1_swap_in;
	mmu->get_tlb_info = hl_mmu_v1_get_tlb_info;
}
