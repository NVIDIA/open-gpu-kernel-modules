// SPDX-License-Identifier: GPL-2.0
/*
 * Implementation of the IOMMU SVA API for the ARM SMMUv3
 */

#include <linux/mm.h>
#include <linux/mmu_context.h>
#include <linux/mmu_notifier.h>
#include <linux/slab.h>

#include "arm-smmu-v3.h"
#include "../../iommu-sva-lib.h"
#include "../../io-pgtable-arm.h"

struct arm_smmu_mmu_notifier {
	struct mmu_notifier		mn;
	struct arm_smmu_ctx_desc	*cd;
	bool				cleared;
	refcount_t			refs;
	struct list_head		list;
	struct arm_smmu_domain		*domain;
};

#define mn_to_smmu(mn) container_of(mn, struct arm_smmu_mmu_notifier, mn)

struct arm_smmu_bond {
	struct iommu_sva		sva;
	struct mm_struct		*mm;
	struct arm_smmu_mmu_notifier	*smmu_mn;
	struct list_head		list;
	refcount_t			refs;
};

#define sva_to_bond(handle) \
	container_of(handle, struct arm_smmu_bond, sva)

static DEFINE_MUTEX(sva_lock);

/*
 * Check if the CPU ASID is available on the SMMU side. If a private context
 * descriptor is using it, try to replace it.
 */
static struct arm_smmu_ctx_desc *
arm_smmu_share_asid(struct mm_struct *mm, u16 asid)
{
	int ret;
	u32 new_asid;
	struct arm_smmu_ctx_desc *cd;
	struct arm_smmu_device *smmu;
	struct arm_smmu_domain *smmu_domain;

	cd = xa_load(&arm_smmu_asid_xa, asid);
	if (!cd)
		return NULL;

	if (cd->mm) {
		if (WARN_ON(cd->mm != mm))
			return ERR_PTR(-EINVAL);
		/* All devices bound to this mm use the same cd struct. */
		refcount_inc(&cd->refs);
		return cd;
	}

	smmu_domain = container_of(cd, struct arm_smmu_domain, s1_cfg.cd);
	smmu = smmu_domain->smmu;

	ret = xa_alloc(&arm_smmu_asid_xa, &new_asid, cd,
		       XA_LIMIT(1, (1 << smmu->asid_bits) - 1), GFP_KERNEL);
	if (ret)
		return ERR_PTR(-ENOSPC);
	/*
	 * Race with unmap: TLB invalidations will start targeting the new ASID,
	 * which isn't assigned yet. We'll do an invalidate-all on the old ASID
	 * later, so it doesn't matter.
	 */
	cd->asid = new_asid;
	/*
	 * Update ASID and invalidate CD in all associated masters. There will
	 * be some overlap between use of both ASIDs, until we invalidate the
	 * TLB.
	 */
	arm_smmu_write_ctx_desc(smmu_domain, 0, cd);

	/* Invalidate TLB entries previously associated with that context */
	arm_smmu_tlb_inv_asid(smmu, asid);

	xa_erase(&arm_smmu_asid_xa, asid);
	return NULL;
}

static struct arm_smmu_ctx_desc *arm_smmu_alloc_shared_cd(struct mm_struct *mm)
{
	u16 asid;
	int err = 0;
	u64 tcr, par, reg;
	struct arm_smmu_ctx_desc *cd;
	struct arm_smmu_ctx_desc *ret = NULL;

	asid = arm64_mm_context_get(mm);
	if (!asid)
		return ERR_PTR(-ESRCH);

	cd = kzalloc(sizeof(*cd), GFP_KERNEL);
	if (!cd) {
		err = -ENOMEM;
		goto out_put_context;
	}

	refcount_set(&cd->refs, 1);

	mutex_lock(&arm_smmu_asid_lock);
	ret = arm_smmu_share_asid(mm, asid);
	if (ret) {
		mutex_unlock(&arm_smmu_asid_lock);
		goto out_free_cd;
	}

	err = xa_insert(&arm_smmu_asid_xa, asid, cd, GFP_KERNEL);
	mutex_unlock(&arm_smmu_asid_lock);

	if (err)
		goto out_free_asid;

	tcr = FIELD_PREP(CTXDESC_CD_0_TCR_T0SZ, 64ULL - vabits_actual) |
	      FIELD_PREP(CTXDESC_CD_0_TCR_IRGN0, ARM_LPAE_TCR_RGN_WBWA) |
	      FIELD_PREP(CTXDESC_CD_0_TCR_ORGN0, ARM_LPAE_TCR_RGN_WBWA) |
	      FIELD_PREP(CTXDESC_CD_0_TCR_SH0, ARM_LPAE_TCR_SH_IS) |
	      CTXDESC_CD_0_TCR_EPD1 | CTXDESC_CD_0_AA64;

	switch (PAGE_SIZE) {
	case SZ_4K:
		tcr |= FIELD_PREP(CTXDESC_CD_0_TCR_TG0, ARM_LPAE_TCR_TG0_4K);
		break;
	case SZ_16K:
		tcr |= FIELD_PREP(CTXDESC_CD_0_TCR_TG0, ARM_LPAE_TCR_TG0_16K);
		break;
	case SZ_64K:
		tcr |= FIELD_PREP(CTXDESC_CD_0_TCR_TG0, ARM_LPAE_TCR_TG0_64K);
		break;
	default:
		WARN_ON(1);
		err = -EINVAL;
		goto out_free_asid;
	}

	reg = read_sanitised_ftr_reg(SYS_ID_AA64MMFR0_EL1);
	par = cpuid_feature_extract_unsigned_field(reg, ID_AA64MMFR0_PARANGE_SHIFT);
	tcr |= FIELD_PREP(CTXDESC_CD_0_TCR_IPS, par);

	cd->ttbr = virt_to_phys(mm->pgd);
	cd->tcr = tcr;
	/*
	 * MAIR value is pretty much constant and global, so we can just get it
	 * from the current CPU register
	 */
	cd->mair = read_sysreg(mair_el1);
	cd->asid = asid;
	cd->mm = mm;

	return cd;

out_free_asid:
	arm_smmu_free_asid(cd);
out_free_cd:
	kfree(cd);
out_put_context:
	arm64_mm_context_put(mm);
	return err < 0 ? ERR_PTR(err) : ret;
}

static void arm_smmu_free_shared_cd(struct arm_smmu_ctx_desc *cd)
{
	if (arm_smmu_free_asid(cd)) {
		/* Unpin ASID */
		arm64_mm_context_put(cd->mm);
		kfree(cd);
	}
}

static void arm_smmu_mm_invalidate_range(struct mmu_notifier *mn,
					 struct mm_struct *mm,
					 unsigned long start, unsigned long end)
{
	struct arm_smmu_mmu_notifier *smmu_mn = mn_to_smmu(mn);
	struct arm_smmu_domain *smmu_domain = smmu_mn->domain;
	size_t size = end - start + 1;

	if (!(smmu_domain->smmu->features & ARM_SMMU_FEAT_BTM))
		arm_smmu_tlb_inv_range_asid(start, size, smmu_mn->cd->asid,
					    PAGE_SIZE, false, smmu_domain);
	arm_smmu_atc_inv_domain(smmu_domain, mm->pasid, start, size);
}

static void arm_smmu_mm_release(struct mmu_notifier *mn, struct mm_struct *mm)
{
	struct arm_smmu_mmu_notifier *smmu_mn = mn_to_smmu(mn);
	struct arm_smmu_domain *smmu_domain = smmu_mn->domain;

	mutex_lock(&sva_lock);
	if (smmu_mn->cleared) {
		mutex_unlock(&sva_lock);
		return;
	}

	/*
	 * DMA may still be running. Keep the cd valid to avoid C_BAD_CD events,
	 * but disable translation.
	 */
	arm_smmu_write_ctx_desc(smmu_domain, mm->pasid, &quiet_cd);

	arm_smmu_tlb_inv_asid(smmu_domain->smmu, smmu_mn->cd->asid);
	arm_smmu_atc_inv_domain(smmu_domain, mm->pasid, 0, 0);

	smmu_mn->cleared = true;
	mutex_unlock(&sva_lock);
}

static void arm_smmu_mmu_notifier_free(struct mmu_notifier *mn)
{
	kfree(mn_to_smmu(mn));
}

static struct mmu_notifier_ops arm_smmu_mmu_notifier_ops = {
	.invalidate_range	= arm_smmu_mm_invalidate_range,
	.release		= arm_smmu_mm_release,
	.free_notifier		= arm_smmu_mmu_notifier_free,
};

/* Allocate or get existing MMU notifier for this {domain, mm} pair */
static struct arm_smmu_mmu_notifier *
arm_smmu_mmu_notifier_get(struct arm_smmu_domain *smmu_domain,
			  struct mm_struct *mm)
{
	int ret;
	struct arm_smmu_ctx_desc *cd;
	struct arm_smmu_mmu_notifier *smmu_mn;

	list_for_each_entry(smmu_mn, &smmu_domain->mmu_notifiers, list) {
		if (smmu_mn->mn.mm == mm) {
			refcount_inc(&smmu_mn->refs);
			return smmu_mn;
		}
	}

	cd = arm_smmu_alloc_shared_cd(mm);
	if (IS_ERR(cd))
		return ERR_CAST(cd);

	smmu_mn = kzalloc(sizeof(*smmu_mn), GFP_KERNEL);
	if (!smmu_mn) {
		ret = -ENOMEM;
		goto err_free_cd;
	}

	refcount_set(&smmu_mn->refs, 1);
	smmu_mn->cd = cd;
	smmu_mn->domain = smmu_domain;
	smmu_mn->mn.ops = &arm_smmu_mmu_notifier_ops;

	ret = mmu_notifier_register(&smmu_mn->mn, mm);
	if (ret) {
		kfree(smmu_mn);
		goto err_free_cd;
	}

	ret = arm_smmu_write_ctx_desc(smmu_domain, mm->pasid, cd);
	if (ret)
		goto err_put_notifier;

	list_add(&smmu_mn->list, &smmu_domain->mmu_notifiers);
	return smmu_mn;

err_put_notifier:
	/* Frees smmu_mn */
	mmu_notifier_put(&smmu_mn->mn);
err_free_cd:
	arm_smmu_free_shared_cd(cd);
	return ERR_PTR(ret);
}

static void arm_smmu_mmu_notifier_put(struct arm_smmu_mmu_notifier *smmu_mn)
{
	struct mm_struct *mm = smmu_mn->mn.mm;
	struct arm_smmu_ctx_desc *cd = smmu_mn->cd;
	struct arm_smmu_domain *smmu_domain = smmu_mn->domain;

	if (!refcount_dec_and_test(&smmu_mn->refs))
		return;

	list_del(&smmu_mn->list);
	arm_smmu_write_ctx_desc(smmu_domain, mm->pasid, NULL);

	/*
	 * If we went through clear(), we've already invalidated, and no
	 * new TLB entry can have been formed.
	 */
	if (!smmu_mn->cleared) {
		arm_smmu_tlb_inv_asid(smmu_domain->smmu, cd->asid);
		arm_smmu_atc_inv_domain(smmu_domain, mm->pasid, 0, 0);
	}

	/* Frees smmu_mn */
	mmu_notifier_put(&smmu_mn->mn);
	arm_smmu_free_shared_cd(cd);
}

static struct iommu_sva *
__arm_smmu_sva_bind(struct device *dev, struct mm_struct *mm)
{
	int ret;
	struct arm_smmu_bond *bond;
	struct arm_smmu_master *master = dev_iommu_priv_get(dev);
	struct iommu_domain *domain = iommu_get_domain_for_dev(dev);
	struct arm_smmu_domain *smmu_domain = to_smmu_domain(domain);

	if (!master || !master->sva_enabled)
		return ERR_PTR(-ENODEV);

	/* If bind() was already called for this {dev, mm} pair, reuse it. */
	list_for_each_entry(bond, &master->bonds, list) {
		if (bond->mm == mm) {
			refcount_inc(&bond->refs);
			return &bond->sva;
		}
	}

	bond = kzalloc(sizeof(*bond), GFP_KERNEL);
	if (!bond)
		return ERR_PTR(-ENOMEM);

	/* Allocate a PASID for this mm if necessary */
	ret = iommu_sva_alloc_pasid(mm, 1, (1U << master->ssid_bits) - 1);
	if (ret)
		goto err_free_bond;

	bond->mm = mm;
	bond->sva.dev = dev;
	refcount_set(&bond->refs, 1);

	bond->smmu_mn = arm_smmu_mmu_notifier_get(smmu_domain, mm);
	if (IS_ERR(bond->smmu_mn)) {
		ret = PTR_ERR(bond->smmu_mn);
		goto err_free_pasid;
	}

	list_add(&bond->list, &master->bonds);
	return &bond->sva;

err_free_pasid:
	iommu_sva_free_pasid(mm);
err_free_bond:
	kfree(bond);
	return ERR_PTR(ret);
}

struct iommu_sva *
arm_smmu_sva_bind(struct device *dev, struct mm_struct *mm, void *drvdata)
{
	struct iommu_sva *handle;
	struct iommu_domain *domain = iommu_get_domain_for_dev(dev);
	struct arm_smmu_domain *smmu_domain = to_smmu_domain(domain);

	if (smmu_domain->stage != ARM_SMMU_DOMAIN_S1)
		return ERR_PTR(-EINVAL);

	mutex_lock(&sva_lock);
	handle = __arm_smmu_sva_bind(dev, mm);
	mutex_unlock(&sva_lock);
	return handle;
}

void arm_smmu_sva_unbind(struct iommu_sva *handle)
{
	struct arm_smmu_bond *bond = sva_to_bond(handle);

	mutex_lock(&sva_lock);
	if (refcount_dec_and_test(&bond->refs)) {
		list_del(&bond->list);
		arm_smmu_mmu_notifier_put(bond->smmu_mn);
		iommu_sva_free_pasid(bond->mm);
		kfree(bond);
	}
	mutex_unlock(&sva_lock);
}

u32 arm_smmu_sva_get_pasid(struct iommu_sva *handle)
{
	struct arm_smmu_bond *bond = sva_to_bond(handle);

	return bond->mm->pasid;
}

bool arm_smmu_sva_supported(struct arm_smmu_device *smmu)
{
	unsigned long reg, fld;
	unsigned long oas;
	unsigned long asid_bits;
	u32 feat_mask = ARM_SMMU_FEAT_COHERENCY;

	if (vabits_actual == 52)
		feat_mask |= ARM_SMMU_FEAT_VAX;

	if ((smmu->features & feat_mask) != feat_mask)
		return false;

	if (!(smmu->pgsize_bitmap & PAGE_SIZE))
		return false;

	/*
	 * Get the smallest PA size of all CPUs (sanitized by cpufeature). We're
	 * not even pretending to support AArch32 here. Abort if the MMU outputs
	 * addresses larger than what we support.
	 */
	reg = read_sanitised_ftr_reg(SYS_ID_AA64MMFR0_EL1);
	fld = cpuid_feature_extract_unsigned_field(reg, ID_AA64MMFR0_PARANGE_SHIFT);
	oas = id_aa64mmfr0_parange_to_phys_shift(fld);
	if (smmu->oas < oas)
		return false;

	/* We can support bigger ASIDs than the CPU, but not smaller */
	fld = cpuid_feature_extract_unsigned_field(reg, ID_AA64MMFR0_ASID_SHIFT);
	asid_bits = fld ? 16 : 8;
	if (smmu->asid_bits < asid_bits)
		return false;

	/*
	 * See max_pinned_asids in arch/arm64/mm/context.c. The following is
	 * generally the maximum number of bindable processes.
	 */
	if (arm64_kernel_unmapped_at_el0())
		asid_bits--;
	dev_dbg(smmu->dev, "%d shared contexts\n", (1 << asid_bits) -
		num_possible_cpus() - 2);

	return true;
}

static bool arm_smmu_iopf_supported(struct arm_smmu_master *master)
{
	return false;
}

bool arm_smmu_master_sva_supported(struct arm_smmu_master *master)
{
	if (!(master->smmu->features & ARM_SMMU_FEAT_SVA))
		return false;

	/* SSID and IOPF support are mandatory for the moment */
	return master->ssid_bits && arm_smmu_iopf_supported(master);
}

bool arm_smmu_master_sva_enabled(struct arm_smmu_master *master)
{
	bool enabled;

	mutex_lock(&sva_lock);
	enabled = master->sva_enabled;
	mutex_unlock(&sva_lock);
	return enabled;
}

int arm_smmu_master_enable_sva(struct arm_smmu_master *master)
{
	mutex_lock(&sva_lock);
	master->sva_enabled = true;
	mutex_unlock(&sva_lock);

	return 0;
}

int arm_smmu_master_disable_sva(struct arm_smmu_master *master)
{
	mutex_lock(&sva_lock);
	if (!list_empty(&master->bonds)) {
		dev_err(master->dev, "cannot disable SVA, device is bound\n");
		mutex_unlock(&sva_lock);
		return -EBUSY;
	}
	master->sva_enabled = false;
	mutex_unlock(&sva_lock);

	return 0;
}

void arm_smmu_sva_notifier_synchronize(void)
{
	/*
	 * Some MMU notifiers may still be waiting to be freed, using
	 * arm_smmu_mmu_notifier_free(). Wait for them.
	 */
	mmu_notifier_synchronize();
}
