// SPDX-License-Identifier: MIT
/*
 * Copyright © 2020 Intel Corporation
 */

#include <linux/log2.h>

#include "gen6_ppgtt.h"
#include "i915_scatterlist.h"
#include "i915_trace.h"
#include "i915_vgpu.h"
#include "intel_gt.h"

/* Write pde (index) from the page directory @pd to the page table @pt */
static void gen6_write_pde(const struct gen6_ppgtt *ppgtt,
			   const unsigned int pde,
			   const struct i915_page_table *pt)
{
	dma_addr_t addr = pt ? px_dma(pt) : px_dma(ppgtt->base.vm.scratch[1]);

	/* Caller needs to make sure the write completes if necessary */
	iowrite32(GEN6_PDE_ADDR_ENCODE(addr) | GEN6_PDE_VALID,
		  ppgtt->pd_addr + pde);
}

void gen7_ppgtt_enable(struct intel_gt *gt)
{
	struct drm_i915_private *i915 = gt->i915;
	struct intel_uncore *uncore = gt->uncore;
	u32 ecochk;

	intel_uncore_rmw(uncore, GAC_ECO_BITS, 0, ECOBITS_PPGTT_CACHE64B);

	ecochk = intel_uncore_read(uncore, GAM_ECOCHK);
	if (IS_HASWELL(i915)) {
		ecochk |= ECOCHK_PPGTT_WB_HSW;
	} else {
		ecochk |= ECOCHK_PPGTT_LLC_IVB;
		ecochk &= ~ECOCHK_PPGTT_GFDT_IVB;
	}
	intel_uncore_write(uncore, GAM_ECOCHK, ecochk);
}

void gen6_ppgtt_enable(struct intel_gt *gt)
{
	struct intel_uncore *uncore = gt->uncore;

	intel_uncore_rmw(uncore,
			 GAC_ECO_BITS,
			 0,
			 ECOBITS_SNB_BIT | ECOBITS_PPGTT_CACHE64B);

	intel_uncore_rmw(uncore,
			 GAB_CTL,
			 0,
			 GAB_CTL_CONT_AFTER_PAGEFAULT);

	intel_uncore_rmw(uncore,
			 GAM_ECOCHK,
			 0,
			 ECOCHK_SNB_BIT | ECOCHK_PPGTT_CACHE64B);

	if (HAS_PPGTT(uncore->i915)) /* may be disabled for VT-d */
		intel_uncore_write(uncore,
				   GFX_MODE,
				   _MASKED_BIT_ENABLE(GFX_PPGTT_ENABLE));
}

/* PPGTT support for Sandybdrige/Gen6 and later */
static void gen6_ppgtt_clear_range(struct i915_address_space *vm,
				   u64 start, u64 length)
{
	struct gen6_ppgtt * const ppgtt = to_gen6_ppgtt(i915_vm_to_ppgtt(vm));
	const unsigned int first_entry = start / I915_GTT_PAGE_SIZE;
	const gen6_pte_t scratch_pte = vm->scratch[0]->encode;
	unsigned int pde = first_entry / GEN6_PTES;
	unsigned int pte = first_entry % GEN6_PTES;
	unsigned int num_entries = length / I915_GTT_PAGE_SIZE;

	while (num_entries) {
		struct i915_page_table * const pt =
			i915_pt_entry(ppgtt->base.pd, pde++);
		const unsigned int count = min(num_entries, GEN6_PTES - pte);
		gen6_pte_t *vaddr;

		num_entries -= count;

		GEM_BUG_ON(count > atomic_read(&pt->used));
		if (!atomic_sub_return(count, &pt->used))
			ppgtt->scan_for_unused_pt = true;

		/*
		 * Note that the hw doesn't support removing PDE on the fly
		 * (they are cached inside the context with no means to
		 * invalidate the cache), so we can only reset the PTE
		 * entries back to scratch.
		 */

		vaddr = px_vaddr(pt);
		memset32(vaddr + pte, scratch_pte, count);

		pte = 0;
	}
}

static void gen6_ppgtt_insert_entries(struct i915_address_space *vm,
				      struct i915_vma *vma,
				      enum i915_cache_level cache_level,
				      u32 flags)
{
	struct i915_ppgtt *ppgtt = i915_vm_to_ppgtt(vm);
	struct i915_page_directory * const pd = ppgtt->pd;
	unsigned int first_entry = vma->node.start / I915_GTT_PAGE_SIZE;
	unsigned int act_pt = first_entry / GEN6_PTES;
	unsigned int act_pte = first_entry % GEN6_PTES;
	const u32 pte_encode = vm->pte_encode(0, cache_level, flags);
	struct sgt_dma iter = sgt_dma(vma);
	gen6_pte_t *vaddr;

	GEM_BUG_ON(!pd->entry[act_pt]);

	vaddr = px_vaddr(i915_pt_entry(pd, act_pt));
	do {
		GEM_BUG_ON(sg_dma_len(iter.sg) < I915_GTT_PAGE_SIZE);
		vaddr[act_pte] = pte_encode | GEN6_PTE_ADDR_ENCODE(iter.dma);

		iter.dma += I915_GTT_PAGE_SIZE;
		if (iter.dma == iter.max) {
			iter.sg = __sg_next(iter.sg);
			if (!iter.sg || sg_dma_len(iter.sg) == 0)
				break;

			iter.dma = sg_dma_address(iter.sg);
			iter.max = iter.dma + sg_dma_len(iter.sg);
		}

		if (++act_pte == GEN6_PTES) {
			vaddr = px_vaddr(i915_pt_entry(pd, ++act_pt));
			act_pte = 0;
		}
	} while (1);

	vma->page_sizes.gtt = I915_GTT_PAGE_SIZE;
}

static void gen6_flush_pd(struct gen6_ppgtt *ppgtt, u64 start, u64 end)
{
	struct i915_page_directory * const pd = ppgtt->base.pd;
	struct i915_page_table *pt;
	unsigned int pde;

	start = round_down(start, SZ_64K);
	end = round_up(end, SZ_64K) - start;

	mutex_lock(&ppgtt->flush);

	gen6_for_each_pde(pt, pd, start, end, pde)
		gen6_write_pde(ppgtt, pde, pt);

	mb();
	ioread32(ppgtt->pd_addr + pde - 1);
	gen6_ggtt_invalidate(ppgtt->base.vm.gt->ggtt);
	mb();

	mutex_unlock(&ppgtt->flush);
}

static void gen6_alloc_va_range(struct i915_address_space *vm,
				struct i915_vm_pt_stash *stash,
				u64 start, u64 length)
{
	struct gen6_ppgtt *ppgtt = to_gen6_ppgtt(i915_vm_to_ppgtt(vm));
	struct i915_page_directory * const pd = ppgtt->base.pd;
	struct i915_page_table *pt;
	bool flush = false;
	u64 from = start;
	unsigned int pde;

	spin_lock(&pd->lock);
	gen6_for_each_pde(pt, pd, start, length, pde) {
		const unsigned int count = gen6_pte_count(start, length);

		if (!pt) {
			spin_unlock(&pd->lock);

			pt = stash->pt[0];
			__i915_gem_object_pin_pages(pt->base);
			i915_gem_object_make_unshrinkable(pt->base);

			fill32_px(pt, vm->scratch[0]->encode);

			spin_lock(&pd->lock);
			if (!pd->entry[pde]) {
				stash->pt[0] = pt->stash;
				atomic_set(&pt->used, 0);
				pd->entry[pde] = pt;
			} else {
				pt = pd->entry[pde];
			}

			flush = true;
		}

		atomic_add(count, &pt->used);
	}
	spin_unlock(&pd->lock);

	if (flush && i915_vma_is_bound(ppgtt->vma, I915_VMA_GLOBAL_BIND)) {
		intel_wakeref_t wakeref;

		with_intel_runtime_pm(&vm->i915->runtime_pm, wakeref)
			gen6_flush_pd(ppgtt, from, start);
	}
}

static int gen6_ppgtt_init_scratch(struct gen6_ppgtt *ppgtt)
{
	struct i915_address_space * const vm = &ppgtt->base.vm;
	int ret;

	ret = setup_scratch_page(vm);
	if (ret)
		return ret;

	vm->scratch[0]->encode =
		vm->pte_encode(px_dma(vm->scratch[0]),
			       I915_CACHE_NONE, PTE_READ_ONLY);

	vm->scratch[1] = vm->alloc_pt_dma(vm, I915_GTT_PAGE_SIZE_4K);
	if (IS_ERR(vm->scratch[1])) {
		ret = PTR_ERR(vm->scratch[1]);
		goto err_scratch0;
	}

	ret = map_pt_dma(vm, vm->scratch[1]);
	if (ret)
		goto err_scratch1;

	fill32_px(vm->scratch[1], vm->scratch[0]->encode);

	return 0;

err_scratch1:
	i915_gem_object_put(vm->scratch[1]);
err_scratch0:
	i915_gem_object_put(vm->scratch[0]);
	return ret;
}

static void gen6_ppgtt_free_pd(struct gen6_ppgtt *ppgtt)
{
	struct i915_page_directory * const pd = ppgtt->base.pd;
	struct i915_page_table *pt;
	u32 pde;

	gen6_for_all_pdes(pt, pd, pde)
		if (pt)
			free_pt(&ppgtt->base.vm, pt);
}

static void gen6_ppgtt_cleanup(struct i915_address_space *vm)
{
	struct gen6_ppgtt *ppgtt = to_gen6_ppgtt(i915_vm_to_ppgtt(vm));

	__i915_vma_put(ppgtt->vma);

	gen6_ppgtt_free_pd(ppgtt);
	free_scratch(vm);

	mutex_destroy(&ppgtt->flush);
	mutex_destroy(&ppgtt->pin_mutex);

	free_pd(&ppgtt->base.vm, ppgtt->base.pd);
}

static int pd_vma_set_pages(struct i915_vma *vma)
{
	vma->pages = ERR_PTR(-ENODEV);
	return 0;
}

static void pd_vma_clear_pages(struct i915_vma *vma)
{
	GEM_BUG_ON(!vma->pages);

	vma->pages = NULL;
}

static void pd_vma_bind(struct i915_address_space *vm,
			struct i915_vm_pt_stash *stash,
			struct i915_vma *vma,
			enum i915_cache_level cache_level,
			u32 unused)
{
	struct i915_ggtt *ggtt = i915_vm_to_ggtt(vm);
	struct gen6_ppgtt *ppgtt = vma->private;
	u32 ggtt_offset = i915_ggtt_offset(vma) / I915_GTT_PAGE_SIZE;

	ppgtt->pp_dir = ggtt_offset * sizeof(gen6_pte_t) << 10;
	ppgtt->pd_addr = (gen6_pte_t __iomem *)ggtt->gsm + ggtt_offset;

	gen6_flush_pd(ppgtt, 0, ppgtt->base.vm.total);
}

static void pd_vma_unbind(struct i915_address_space *vm, struct i915_vma *vma)
{
	struct gen6_ppgtt *ppgtt = vma->private;
	struct i915_page_directory * const pd = ppgtt->base.pd;
	struct i915_page_table *pt;
	unsigned int pde;

	if (!ppgtt->scan_for_unused_pt)
		return;

	/* Free all no longer used page tables */
	gen6_for_all_pdes(pt, ppgtt->base.pd, pde) {
		if (!pt || atomic_read(&pt->used))
			continue;

		free_pt(&ppgtt->base.vm, pt);
		pd->entry[pde] = NULL;
	}

	ppgtt->scan_for_unused_pt = false;
}

static const struct i915_vma_ops pd_vma_ops = {
	.set_pages = pd_vma_set_pages,
	.clear_pages = pd_vma_clear_pages,
	.bind_vma = pd_vma_bind,
	.unbind_vma = pd_vma_unbind,
};

static struct i915_vma *pd_vma_create(struct gen6_ppgtt *ppgtt, int size)
{
	struct i915_ggtt *ggtt = ppgtt->base.vm.gt->ggtt;
	struct i915_vma *vma;

	GEM_BUG_ON(!IS_ALIGNED(size, I915_GTT_PAGE_SIZE));
	GEM_BUG_ON(size > ggtt->vm.total);

	vma = i915_vma_alloc();
	if (!vma)
		return ERR_PTR(-ENOMEM);

	i915_active_init(&vma->active, NULL, NULL, 0);

	kref_init(&vma->ref);
	mutex_init(&vma->pages_mutex);
	vma->vm = i915_vm_get(&ggtt->vm);
	vma->ops = &pd_vma_ops;
	vma->private = ppgtt;

	vma->size = size;
	vma->fence_size = size;
	atomic_set(&vma->flags, I915_VMA_GGTT);
	vma->ggtt_view.type = I915_GGTT_VIEW_ROTATED; /* prevent fencing */

	INIT_LIST_HEAD(&vma->obj_link);
	INIT_LIST_HEAD(&vma->closed_link);

	return vma;
}

int gen6_ppgtt_pin(struct i915_ppgtt *base, struct i915_gem_ww_ctx *ww)
{
	struct gen6_ppgtt *ppgtt = to_gen6_ppgtt(base);
	int err;

	GEM_BUG_ON(!atomic_read(&ppgtt->base.vm.open));

	/*
	 * Workaround the limited maximum vma->pin_count and the aliasing_ppgtt
	 * which will be pinned into every active context.
	 * (When vma->pin_count becomes atomic, I expect we will naturally
	 * need a larger, unpacked, type and kill this redundancy.)
	 */
	if (atomic_add_unless(&ppgtt->pin_count, 1, 0))
		return 0;

	if (mutex_lock_interruptible(&ppgtt->pin_mutex))
		return -EINTR;

	/*
	 * PPGTT PDEs reside in the GGTT and consists of 512 entries. The
	 * allocator works in address space sizes, so it's multiplied by page
	 * size. We allocate at the top of the GTT to avoid fragmentation.
	 */
	err = 0;
	if (!atomic_read(&ppgtt->pin_count))
		err = i915_ggtt_pin(ppgtt->vma, ww, GEN6_PD_ALIGN, PIN_HIGH);
	if (!err)
		atomic_inc(&ppgtt->pin_count);
	mutex_unlock(&ppgtt->pin_mutex);

	return err;
}

void gen6_ppgtt_unpin(struct i915_ppgtt *base)
{
	struct gen6_ppgtt *ppgtt = to_gen6_ppgtt(base);

	GEM_BUG_ON(!atomic_read(&ppgtt->pin_count));
	if (atomic_dec_and_test(&ppgtt->pin_count))
		i915_vma_unpin(ppgtt->vma);
}

void gen6_ppgtt_unpin_all(struct i915_ppgtt *base)
{
	struct gen6_ppgtt *ppgtt = to_gen6_ppgtt(base);

	if (!atomic_read(&ppgtt->pin_count))
		return;

	i915_vma_unpin(ppgtt->vma);
	atomic_set(&ppgtt->pin_count, 0);
}

struct i915_ppgtt *gen6_ppgtt_create(struct intel_gt *gt)
{
	struct i915_ggtt * const ggtt = gt->ggtt;
	struct gen6_ppgtt *ppgtt;
	int err;

	ppgtt = kzalloc(sizeof(*ppgtt), GFP_KERNEL);
	if (!ppgtt)
		return ERR_PTR(-ENOMEM);

	mutex_init(&ppgtt->flush);
	mutex_init(&ppgtt->pin_mutex);

	ppgtt_init(&ppgtt->base, gt);
	ppgtt->base.vm.pd_shift = ilog2(SZ_4K * SZ_4K / sizeof(gen6_pte_t));
	ppgtt->base.vm.top = 1;

	ppgtt->base.vm.bind_async_flags = I915_VMA_LOCAL_BIND;
	ppgtt->base.vm.allocate_va_range = gen6_alloc_va_range;
	ppgtt->base.vm.clear_range = gen6_ppgtt_clear_range;
	ppgtt->base.vm.insert_entries = gen6_ppgtt_insert_entries;
	ppgtt->base.vm.cleanup = gen6_ppgtt_cleanup;

	ppgtt->base.vm.alloc_pt_dma = alloc_pt_dma;
	ppgtt->base.vm.pte_encode = ggtt->vm.pte_encode;

	ppgtt->base.pd = __alloc_pd(I915_PDES);
	if (!ppgtt->base.pd) {
		err = -ENOMEM;
		goto err_free;
	}

	err = gen6_ppgtt_init_scratch(ppgtt);
	if (err)
		goto err_pd;

	ppgtt->vma = pd_vma_create(ppgtt, GEN6_PD_SIZE);
	if (IS_ERR(ppgtt->vma)) {
		err = PTR_ERR(ppgtt->vma);
		goto err_scratch;
	}

	return &ppgtt->base;

err_scratch:
	free_scratch(&ppgtt->base.vm);
err_pd:
	free_pd(&ppgtt->base.vm, ppgtt->base.pd);
err_free:
	mutex_destroy(&ppgtt->pin_mutex);
	kfree(ppgtt);
	return ERR_PTR(err);
}
