// SPDX-License-Identifier:	GPL-2.0
/* Copyright 2019 Linaro, Ltd, Rob Herring <robh@kernel.org> */
#include <linux/atomic.h>
#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/io-pgtable.h>
#include <linux/iommu.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/shmem_fs.h>
#include <linux/sizes.h>

#include "panfrost_device.h"
#include "panfrost_mmu.h"
#include "panfrost_gem.h"
#include "panfrost_features.h"
#include "panfrost_regs.h"

#define mmu_write(dev, reg, data) writel(data, dev->iomem + reg)
#define mmu_read(dev, reg) readl(dev->iomem + reg)

static int wait_ready(struct panfrost_device *pfdev, u32 as_nr)
{
	int ret;
	u32 val;

	/* Wait for the MMU status to indicate there is no active command, in
	 * case one is pending. */
	ret = readl_relaxed_poll_timeout_atomic(pfdev->iomem + AS_STATUS(as_nr),
		val, !(val & AS_STATUS_AS_ACTIVE), 10, 1000);

	if (ret)
		dev_err(pfdev->dev, "AS_ACTIVE bit stuck\n");

	return ret;
}

static int write_cmd(struct panfrost_device *pfdev, u32 as_nr, u32 cmd)
{
	int status;

	/* write AS_COMMAND when MMU is ready to accept another command */
	status = wait_ready(pfdev, as_nr);
	if (!status)
		mmu_write(pfdev, AS_COMMAND(as_nr), cmd);

	return status;
}

static void lock_region(struct panfrost_device *pfdev, u32 as_nr,
			u64 iova, size_t size)
{
	u8 region_width;
	u64 region = iova & PAGE_MASK;
	/*
	 * fls returns:
	 * 1 .. 32
	 *
	 * 10 + fls(num_pages)
	 * results in the range (11 .. 42)
	 */

	size = round_up(size, PAGE_SIZE);

	region_width = 10 + fls(size >> PAGE_SHIFT);
	if ((size >> PAGE_SHIFT) != (1ul << (region_width - 11))) {
		/* not pow2, so must go up to the next pow2 */
		region_width += 1;
	}
	region |= region_width;

	/* Lock the region that needs to be updated */
	mmu_write(pfdev, AS_LOCKADDR_LO(as_nr), region & 0xFFFFFFFFUL);
	mmu_write(pfdev, AS_LOCKADDR_HI(as_nr), (region >> 32) & 0xFFFFFFFFUL);
	write_cmd(pfdev, as_nr, AS_COMMAND_LOCK);
}


static int mmu_hw_do_operation_locked(struct panfrost_device *pfdev, int as_nr,
				      u64 iova, size_t size, u32 op)
{
	if (as_nr < 0)
		return 0;

	if (op != AS_COMMAND_UNLOCK)
		lock_region(pfdev, as_nr, iova, size);

	/* Run the MMU operation */
	write_cmd(pfdev, as_nr, op);

	/* Wait for the flush to complete */
	return wait_ready(pfdev, as_nr);
}

static int mmu_hw_do_operation(struct panfrost_device *pfdev,
			       struct panfrost_mmu *mmu,
			       u64 iova, size_t size, u32 op)
{
	int ret;

	spin_lock(&pfdev->as_lock);
	ret = mmu_hw_do_operation_locked(pfdev, mmu->as, iova, size, op);
	spin_unlock(&pfdev->as_lock);
	return ret;
}

static void panfrost_mmu_enable(struct panfrost_device *pfdev, struct panfrost_mmu *mmu)
{
	int as_nr = mmu->as;
	struct io_pgtable_cfg *cfg = &mmu->pgtbl_cfg;
	u64 transtab = cfg->arm_mali_lpae_cfg.transtab;
	u64 memattr = cfg->arm_mali_lpae_cfg.memattr;

	mmu_hw_do_operation_locked(pfdev, as_nr, 0, ~0UL, AS_COMMAND_FLUSH_MEM);

	mmu_write(pfdev, AS_TRANSTAB_LO(as_nr), transtab & 0xffffffffUL);
	mmu_write(pfdev, AS_TRANSTAB_HI(as_nr), transtab >> 32);

	/* Need to revisit mem attrs.
	 * NC is the default, Mali driver is inner WT.
	 */
	mmu_write(pfdev, AS_MEMATTR_LO(as_nr), memattr & 0xffffffffUL);
	mmu_write(pfdev, AS_MEMATTR_HI(as_nr), memattr >> 32);

	write_cmd(pfdev, as_nr, AS_COMMAND_UPDATE);
}

static void panfrost_mmu_disable(struct panfrost_device *pfdev, u32 as_nr)
{
	mmu_hw_do_operation_locked(pfdev, as_nr, 0, ~0UL, AS_COMMAND_FLUSH_MEM);

	mmu_write(pfdev, AS_TRANSTAB_LO(as_nr), 0);
	mmu_write(pfdev, AS_TRANSTAB_HI(as_nr), 0);

	mmu_write(pfdev, AS_MEMATTR_LO(as_nr), 0);
	mmu_write(pfdev, AS_MEMATTR_HI(as_nr), 0);

	write_cmd(pfdev, as_nr, AS_COMMAND_UPDATE);
}

u32 panfrost_mmu_as_get(struct panfrost_device *pfdev, struct panfrost_mmu *mmu)
{
	int as;

	spin_lock(&pfdev->as_lock);

	as = mmu->as;
	if (as >= 0) {
		int en = atomic_inc_return(&mmu->as_count);

		/*
		 * AS can be retained by active jobs or a perfcnt context,
		 * hence the '+ 1' here.
		 */
		WARN_ON(en >= (NUM_JOB_SLOTS + 1));

		list_move(&mmu->list, &pfdev->as_lru_list);
		goto out;
	}

	/* Check for a free AS */
	as = ffz(pfdev->as_alloc_mask);
	if (!(BIT(as) & pfdev->features.as_present)) {
		struct panfrost_mmu *lru_mmu;

		list_for_each_entry_reverse(lru_mmu, &pfdev->as_lru_list, list) {
			if (!atomic_read(&lru_mmu->as_count))
				break;
		}
		WARN_ON(&lru_mmu->list == &pfdev->as_lru_list);

		list_del_init(&lru_mmu->list);
		as = lru_mmu->as;

		WARN_ON(as < 0);
		lru_mmu->as = -1;
	}

	/* Assign the free or reclaimed AS to the FD */
	mmu->as = as;
	set_bit(as, &pfdev->as_alloc_mask);
	atomic_set(&mmu->as_count, 1);
	list_add(&mmu->list, &pfdev->as_lru_list);

	dev_dbg(pfdev->dev, "Assigned AS%d to mmu %p, alloc_mask=%lx", as, mmu, pfdev->as_alloc_mask);

	panfrost_mmu_enable(pfdev, mmu);

out:
	spin_unlock(&pfdev->as_lock);
	return as;
}

void panfrost_mmu_as_put(struct panfrost_device *pfdev, struct panfrost_mmu *mmu)
{
	atomic_dec(&mmu->as_count);
	WARN_ON(atomic_read(&mmu->as_count) < 0);
}

void panfrost_mmu_reset(struct panfrost_device *pfdev)
{
	struct panfrost_mmu *mmu, *mmu_tmp;

	spin_lock(&pfdev->as_lock);

	pfdev->as_alloc_mask = 0;

	list_for_each_entry_safe(mmu, mmu_tmp, &pfdev->as_lru_list, list) {
		mmu->as = -1;
		atomic_set(&mmu->as_count, 0);
		list_del_init(&mmu->list);
	}

	spin_unlock(&pfdev->as_lock);

	mmu_write(pfdev, MMU_INT_CLEAR, ~0);
	mmu_write(pfdev, MMU_INT_MASK, ~0);
}

static size_t get_pgsize(u64 addr, size_t size)
{
	if (addr & (SZ_2M - 1) || size < SZ_2M)
		return SZ_4K;

	return SZ_2M;
}

static void panfrost_mmu_flush_range(struct panfrost_device *pfdev,
				     struct panfrost_mmu *mmu,
				     u64 iova, size_t size)
{
	if (mmu->as < 0)
		return;

	pm_runtime_get_noresume(pfdev->dev);

	/* Flush the PTs only if we're already awake */
	if (pm_runtime_active(pfdev->dev))
		mmu_hw_do_operation(pfdev, mmu, iova, size, AS_COMMAND_FLUSH_PT);

	pm_runtime_put_sync_autosuspend(pfdev->dev);
}

static int mmu_map_sg(struct panfrost_device *pfdev, struct panfrost_mmu *mmu,
		      u64 iova, int prot, struct sg_table *sgt)
{
	unsigned int count;
	struct scatterlist *sgl;
	struct io_pgtable_ops *ops = mmu->pgtbl_ops;
	u64 start_iova = iova;

	for_each_sgtable_dma_sg(sgt, sgl, count) {
		unsigned long paddr = sg_dma_address(sgl);
		size_t len = sg_dma_len(sgl);

		dev_dbg(pfdev->dev, "map: as=%d, iova=%llx, paddr=%lx, len=%zx", mmu->as, iova, paddr, len);

		while (len) {
			size_t pgsize = get_pgsize(iova | paddr, len);

			ops->map(ops, iova, paddr, pgsize, prot, GFP_KERNEL);
			iova += pgsize;
			paddr += pgsize;
			len -= pgsize;
		}
	}

	panfrost_mmu_flush_range(pfdev, mmu, start_iova, iova - start_iova);

	return 0;
}

int panfrost_mmu_map(struct panfrost_gem_mapping *mapping)
{
	struct panfrost_gem_object *bo = mapping->obj;
	struct drm_gem_object *obj = &bo->base.base;
	struct panfrost_device *pfdev = to_panfrost_device(obj->dev);
	struct sg_table *sgt;
	int prot = IOMMU_READ | IOMMU_WRITE;

	if (WARN_ON(mapping->active))
		return 0;

	if (bo->noexec)
		prot |= IOMMU_NOEXEC;

	sgt = drm_gem_shmem_get_pages_sgt(obj);
	if (WARN_ON(IS_ERR(sgt)))
		return PTR_ERR(sgt);

	mmu_map_sg(pfdev, mapping->mmu, mapping->mmnode.start << PAGE_SHIFT,
		   prot, sgt);
	mapping->active = true;

	return 0;
}

void panfrost_mmu_unmap(struct panfrost_gem_mapping *mapping)
{
	struct panfrost_gem_object *bo = mapping->obj;
	struct drm_gem_object *obj = &bo->base.base;
	struct panfrost_device *pfdev = to_panfrost_device(obj->dev);
	struct io_pgtable_ops *ops = mapping->mmu->pgtbl_ops;
	u64 iova = mapping->mmnode.start << PAGE_SHIFT;
	size_t len = mapping->mmnode.size << PAGE_SHIFT;
	size_t unmapped_len = 0;

	if (WARN_ON(!mapping->active))
		return;

	dev_dbg(pfdev->dev, "unmap: as=%d, iova=%llx, len=%zx",
		mapping->mmu->as, iova, len);

	while (unmapped_len < len) {
		size_t unmapped_page;
		size_t pgsize = get_pgsize(iova, len - unmapped_len);

		if (ops->iova_to_phys(ops, iova)) {
			unmapped_page = ops->unmap(ops, iova, pgsize, NULL);
			WARN_ON(unmapped_page != pgsize);
		}
		iova += pgsize;
		unmapped_len += pgsize;
	}

	panfrost_mmu_flush_range(pfdev, mapping->mmu,
				 mapping->mmnode.start << PAGE_SHIFT, len);
	mapping->active = false;
}

static void mmu_tlb_inv_context_s1(void *cookie)
{}

static void mmu_tlb_sync_context(void *cookie)
{
	//struct panfrost_device *pfdev = cookie;
	// TODO: Wait 1000 GPU cycles for HW_ISSUE_6367/T60X
}

static void mmu_tlb_flush_walk(unsigned long iova, size_t size, size_t granule,
			       void *cookie)
{
	mmu_tlb_sync_context(cookie);
}

static const struct iommu_flush_ops mmu_tlb_ops = {
	.tlb_flush_all	= mmu_tlb_inv_context_s1,
	.tlb_flush_walk = mmu_tlb_flush_walk,
};

int panfrost_mmu_pgtable_alloc(struct panfrost_file_priv *priv)
{
	struct panfrost_mmu *mmu = &priv->mmu;
	struct panfrost_device *pfdev = priv->pfdev;

	INIT_LIST_HEAD(&mmu->list);
	mmu->as = -1;

	mmu->pgtbl_cfg = (struct io_pgtable_cfg) {
		.pgsize_bitmap	= SZ_4K | SZ_2M,
		.ias		= FIELD_GET(0xff, pfdev->features.mmu_features),
		.oas		= FIELD_GET(0xff00, pfdev->features.mmu_features),
		.coherent_walk	= pfdev->coherent,
		.tlb		= &mmu_tlb_ops,
		.iommu_dev	= pfdev->dev,
	};

	mmu->pgtbl_ops = alloc_io_pgtable_ops(ARM_MALI_LPAE, &mmu->pgtbl_cfg,
					      priv);
	if (!mmu->pgtbl_ops)
		return -EINVAL;

	return 0;
}

void panfrost_mmu_pgtable_free(struct panfrost_file_priv *priv)
{
	struct panfrost_device *pfdev = priv->pfdev;
	struct panfrost_mmu *mmu = &priv->mmu;

	spin_lock(&pfdev->as_lock);
	if (mmu->as >= 0) {
		pm_runtime_get_noresume(pfdev->dev);
		if (pm_runtime_active(pfdev->dev))
			panfrost_mmu_disable(pfdev, mmu->as);
		pm_runtime_put_autosuspend(pfdev->dev);

		clear_bit(mmu->as, &pfdev->as_alloc_mask);
		clear_bit(mmu->as, &pfdev->as_in_use_mask);
		list_del(&mmu->list);
	}
	spin_unlock(&pfdev->as_lock);

	free_io_pgtable_ops(mmu->pgtbl_ops);
}

static struct panfrost_gem_mapping *
addr_to_mapping(struct panfrost_device *pfdev, int as, u64 addr)
{
	struct panfrost_gem_mapping *mapping = NULL;
	struct panfrost_file_priv *priv;
	struct drm_mm_node *node;
	u64 offset = addr >> PAGE_SHIFT;
	struct panfrost_mmu *mmu;

	spin_lock(&pfdev->as_lock);
	list_for_each_entry(mmu, &pfdev->as_lru_list, list) {
		if (as == mmu->as)
			goto found_mmu;
	}
	goto out;

found_mmu:
	priv = container_of(mmu, struct panfrost_file_priv, mmu);

	spin_lock(&priv->mm_lock);

	drm_mm_for_each_node(node, &priv->mm) {
		if (offset >= node->start &&
		    offset < (node->start + node->size)) {
			mapping = drm_mm_node_to_panfrost_mapping(node);

			kref_get(&mapping->refcount);
			break;
		}
	}

	spin_unlock(&priv->mm_lock);
out:
	spin_unlock(&pfdev->as_lock);
	return mapping;
}

#define NUM_FAULT_PAGES (SZ_2M / PAGE_SIZE)

static int panfrost_mmu_map_fault_addr(struct panfrost_device *pfdev, int as,
				       u64 addr)
{
	int ret, i;
	struct panfrost_gem_mapping *bomapping;
	struct panfrost_gem_object *bo;
	struct address_space *mapping;
	pgoff_t page_offset;
	struct sg_table *sgt;
	struct page **pages;

	bomapping = addr_to_mapping(pfdev, as, addr);
	if (!bomapping)
		return -ENOENT;

	bo = bomapping->obj;
	if (!bo->is_heap) {
		dev_WARN(pfdev->dev, "matching BO is not heap type (GPU VA = %llx)",
			 bomapping->mmnode.start << PAGE_SHIFT);
		ret = -EINVAL;
		goto err_bo;
	}
	WARN_ON(bomapping->mmu->as != as);

	/* Assume 2MB alignment and size multiple */
	addr &= ~((u64)SZ_2M - 1);
	page_offset = addr >> PAGE_SHIFT;
	page_offset -= bomapping->mmnode.start;

	mutex_lock(&bo->base.pages_lock);

	if (!bo->base.pages) {
		bo->sgts = kvmalloc_array(bo->base.base.size / SZ_2M,
				     sizeof(struct sg_table), GFP_KERNEL | __GFP_ZERO);
		if (!bo->sgts) {
			mutex_unlock(&bo->base.pages_lock);
			ret = -ENOMEM;
			goto err_bo;
		}

		pages = kvmalloc_array(bo->base.base.size >> PAGE_SHIFT,
				       sizeof(struct page *), GFP_KERNEL | __GFP_ZERO);
		if (!pages) {
			kvfree(bo->sgts);
			bo->sgts = NULL;
			mutex_unlock(&bo->base.pages_lock);
			ret = -ENOMEM;
			goto err_bo;
		}
		bo->base.pages = pages;
		bo->base.pages_use_count = 1;
	} else {
		pages = bo->base.pages;
		if (pages[page_offset]) {
			/* Pages are already mapped, bail out. */
			mutex_unlock(&bo->base.pages_lock);
			goto out;
		}
	}

	mapping = bo->base.base.filp->f_mapping;
	mapping_set_unevictable(mapping);

	for (i = page_offset; i < page_offset + NUM_FAULT_PAGES; i++) {
		pages[i] = shmem_read_mapping_page(mapping, i);
		if (IS_ERR(pages[i])) {
			mutex_unlock(&bo->base.pages_lock);
			ret = PTR_ERR(pages[i]);
			goto err_pages;
		}
	}

	mutex_unlock(&bo->base.pages_lock);

	sgt = &bo->sgts[page_offset / (SZ_2M / PAGE_SIZE)];
	ret = sg_alloc_table_from_pages(sgt, pages + page_offset,
					NUM_FAULT_PAGES, 0, SZ_2M, GFP_KERNEL);
	if (ret)
		goto err_pages;

	ret = dma_map_sgtable(pfdev->dev, sgt, DMA_BIDIRECTIONAL, 0);
	if (ret)
		goto err_map;

	mmu_map_sg(pfdev, bomapping->mmu, addr,
		   IOMMU_WRITE | IOMMU_READ | IOMMU_NOEXEC, sgt);

	bomapping->active = true;

	dev_dbg(pfdev->dev, "mapped page fault @ AS%d %llx", as, addr);

out:
	panfrost_gem_mapping_put(bomapping);

	return 0;

err_map:
	sg_free_table(sgt);
err_pages:
	drm_gem_shmem_put_pages(&bo->base);
err_bo:
	drm_gem_object_put(&bo->base.base);
	return ret;
}

static const char *access_type_name(struct panfrost_device *pfdev,
		u32 fault_status)
{
	switch (fault_status & AS_FAULTSTATUS_ACCESS_TYPE_MASK) {
	case AS_FAULTSTATUS_ACCESS_TYPE_ATOMIC:
		if (panfrost_has_hw_feature(pfdev, HW_FEATURE_AARCH64_MMU))
			return "ATOMIC";
		else
			return "UNKNOWN";
	case AS_FAULTSTATUS_ACCESS_TYPE_READ:
		return "READ";
	case AS_FAULTSTATUS_ACCESS_TYPE_WRITE:
		return "WRITE";
	case AS_FAULTSTATUS_ACCESS_TYPE_EX:
		return "EXECUTE";
	default:
		WARN_ON(1);
		return NULL;
	}
}

static irqreturn_t panfrost_mmu_irq_handler(int irq, void *data)
{
	struct panfrost_device *pfdev = data;

	if (!mmu_read(pfdev, MMU_INT_STAT))
		return IRQ_NONE;

	mmu_write(pfdev, MMU_INT_MASK, 0);
	return IRQ_WAKE_THREAD;
}

static irqreturn_t panfrost_mmu_irq_handler_thread(int irq, void *data)
{
	struct panfrost_device *pfdev = data;
	u32 status = mmu_read(pfdev, MMU_INT_RAWSTAT);
	int ret;

	while (status) {
		u32 as = ffs(status | (status >> 16)) - 1;
		u32 mask = BIT(as) | BIT(as + 16);
		u64 addr;
		u32 fault_status;
		u32 exception_type;
		u32 access_type;
		u32 source_id;

		fault_status = mmu_read(pfdev, AS_FAULTSTATUS(as));
		addr = mmu_read(pfdev, AS_FAULTADDRESS_LO(as));
		addr |= (u64)mmu_read(pfdev, AS_FAULTADDRESS_HI(as)) << 32;

		/* decode the fault status */
		exception_type = fault_status & 0xFF;
		access_type = (fault_status >> 8) & 0x3;
		source_id = (fault_status >> 16);

		mmu_write(pfdev, MMU_INT_CLEAR, mask);

		/* Page fault only */
		ret = -1;
		if ((status & mask) == BIT(as) && (exception_type & 0xF8) == 0xC0)
			ret = panfrost_mmu_map_fault_addr(pfdev, as, addr);

		if (ret)
			/* terminal fault, print info about the fault */
			dev_err(pfdev->dev,
				"Unhandled Page fault in AS%d at VA 0x%016llX\n"
				"Reason: %s\n"
				"raw fault status: 0x%X\n"
				"decoded fault status: %s\n"
				"exception type 0x%X: %s\n"
				"access type 0x%X: %s\n"
				"source id 0x%X\n",
				as, addr,
				"TODO",
				fault_status,
				(fault_status & (1 << 10) ? "DECODER FAULT" : "SLAVE FAULT"),
				exception_type, panfrost_exception_name(pfdev, exception_type),
				access_type, access_type_name(pfdev, fault_status),
				source_id);

		status &= ~mask;

		/* If we received new MMU interrupts, process them before returning. */
		if (!status)
			status = mmu_read(pfdev, MMU_INT_RAWSTAT);
	}

	mmu_write(pfdev, MMU_INT_MASK, ~0);
	return IRQ_HANDLED;
};

int panfrost_mmu_init(struct panfrost_device *pfdev)
{
	int err, irq;

	irq = platform_get_irq_byname(to_platform_device(pfdev->dev), "mmu");
	if (irq <= 0)
		return -ENODEV;

	err = devm_request_threaded_irq(pfdev->dev, irq,
					panfrost_mmu_irq_handler,
					panfrost_mmu_irq_handler_thread,
					IRQF_SHARED, KBUILD_MODNAME "-mmu",
					pfdev);

	if (err) {
		dev_err(pfdev->dev, "failed to request mmu irq");
		return err;
	}

	return 0;
}

void panfrost_mmu_fini(struct panfrost_device *pfdev)
{
	mmu_write(pfdev, MMU_INT_MASK, 0);
}
