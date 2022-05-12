/*
 * Copyright 2014-2018 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include <linux/dma-buf.h>
#include <linux/list.h>
#include <linux/pagemap.h>
#include <linux/sched/mm.h>
#include <linux/sched/task.h>

#include "amdgpu_object.h"
#include "amdgpu_gem.h"
#include "amdgpu_vm.h"
#include "amdgpu_amdkfd.h"
#include "amdgpu_dma_buf.h"
#include "kfd_ipc.h"
#include <uapi/linux/kfd_ioctl.h>
#include "amdgpu_xgmi.h"

/* Userptr restore delay, just long enough to allow consecutive VM
 * changes to accumulate
 */
#define AMDGPU_USERPTR_RESTORE_DELAY_MS 1

/* Impose limit on how much memory KFD can use */
static struct {
	uint64_t max_system_mem_limit;
	uint64_t max_ttm_mem_limit;
	int64_t system_mem_used;
	int64_t ttm_mem_used;
	spinlock_t mem_limit_lock;
} kfd_mem_limit;

static const char * const domain_bit_to_string[] = {
		"CPU",
		"GTT",
		"VRAM",
		"GDS",
		"GWS",
		"OA"
};

#define domain_string(domain) domain_bit_to_string[ffs(domain)-1]

static void amdgpu_amdkfd_restore_userptr_worker(struct work_struct *work);

static bool kfd_mem_is_attached(struct amdgpu_vm *avm,
		struct kgd_mem *mem)
{
	struct kfd_mem_attachment *entry;

	list_for_each_entry(entry, &mem->attachments, list)
		if (entry->bo_va->base.vm == avm)
			return true;

	return false;
}

/* Set memory usage limits. Current, limits are
 *  System (TTM + userptr) memory - 15/16th System RAM
 *  TTM memory - 3/8th System RAM
 */
void amdgpu_amdkfd_gpuvm_init_mem_limits(void)
{
	struct sysinfo si;
	uint64_t mem;

	si_meminfo(&si);
	mem = si.freeram - si.freehigh;
	mem *= si.mem_unit;

	spin_lock_init(&kfd_mem_limit.mem_limit_lock);
	kfd_mem_limit.max_system_mem_limit = mem - (mem >> 4);
	kfd_mem_limit.max_ttm_mem_limit = (mem >> 1) - (mem >> 3);
	pr_debug("Kernel memory limit %lluM, TTM limit %lluM\n",
		(kfd_mem_limit.max_system_mem_limit >> 20),
		(kfd_mem_limit.max_ttm_mem_limit >> 20));
}

void amdgpu_amdkfd_reserve_system_mem(uint64_t size)
{
	kfd_mem_limit.system_mem_used += size;
}

/* Estimate page table size needed to represent a given memory size
 *
 * With 4KB pages, we need one 8 byte PTE for each 4KB of memory
 * (factor 512, >> 9). With 2MB pages, we need one 8 byte PTE for 2MB
 * of memory (factor 256K, >> 18). ROCm user mode tries to optimize
 * for 2MB pages for TLB efficiency. However, small allocations and
 * fragmented system memory still need some 4KB pages. We choose a
 * compromise that should work in most cases without reserving too
 * much memory for page tables unnecessarily (factor 16K, >> 14).
 */
#define ESTIMATE_PT_SIZE(mem_size) ((mem_size) >> 14)

static size_t amdgpu_amdkfd_acc_size(uint64_t size)
{
	size >>= PAGE_SHIFT;
	size *= sizeof(dma_addr_t) + sizeof(void *);

	return __roundup_pow_of_two(sizeof(struct amdgpu_bo)) +
		__roundup_pow_of_two(sizeof(struct ttm_tt)) +
		PAGE_ALIGN(size);
}

/**
 * amdgpu_amdkfd_reserve_mem_limit() - Decrease available memory by size
 * of buffer including any reserved for control structures
 *
 * @adev: Device to which allocated BO belongs to
 * @size: Size of buffer, in bytes, encapsulated by B0. This should be
 * equivalent to amdgpu_bo_size(BO)
 * @alloc_flag: Flag used in allocating a BO as noted above
 *
 * Return: returns -ENOMEM in case of error, ZERO otherwise
 */
static int amdgpu_amdkfd_reserve_mem_limit(struct amdgpu_device *adev,
		uint64_t size, u32 alloc_flag)
{
	uint64_t reserved_for_pt =
		ESTIMATE_PT_SIZE(amdgpu_amdkfd_total_mem_size);
	size_t acc_size, system_mem_needed, ttm_mem_needed, vram_needed;
	int ret = 0;

	acc_size = amdgpu_amdkfd_acc_size(size);

	vram_needed = 0;
	if (alloc_flag & KFD_IOC_ALLOC_MEM_FLAGS_GTT) {
		system_mem_needed = acc_size + size;
		ttm_mem_needed = acc_size + size;
	} else if (alloc_flag & KFD_IOC_ALLOC_MEM_FLAGS_VRAM) {
		system_mem_needed = acc_size;
		ttm_mem_needed = acc_size;
		vram_needed = size;
	} else if (alloc_flag & KFD_IOC_ALLOC_MEM_FLAGS_USERPTR) {
		system_mem_needed = acc_size + size;
		ttm_mem_needed = acc_size;
	} else if (alloc_flag &
		   (KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL |
		    KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP)) {
		system_mem_needed = acc_size;
		ttm_mem_needed = acc_size;
	} else {
		pr_err("%s: Invalid BO type %#x\n", __func__, alloc_flag);
		return -ENOMEM;
	}

	spin_lock(&kfd_mem_limit.mem_limit_lock);

	if (kfd_mem_limit.system_mem_used + system_mem_needed >
	    kfd_mem_limit.max_system_mem_limit)
		pr_debug("Set no_system_mem_limit=1 if using shared memory\n");

	if ((kfd_mem_limit.system_mem_used + system_mem_needed >
	     kfd_mem_limit.max_system_mem_limit && !no_system_mem_limit) ||
	    (kfd_mem_limit.ttm_mem_used + ttm_mem_needed >
	     kfd_mem_limit.max_ttm_mem_limit) ||
	    (adev->kfd.vram_used + vram_needed >
	     adev->gmc.real_vram_size - reserved_for_pt)) {
		ret = -ENOMEM;
		goto release;
	}

	/* Update memory accounting by decreasing available system
	 * memory, TTM memory and GPU memory as computed above
	 */
	adev->kfd.vram_used += vram_needed;
	kfd_mem_limit.system_mem_used += system_mem_needed;
	kfd_mem_limit.ttm_mem_used += ttm_mem_needed;

release:
	spin_unlock(&kfd_mem_limit.mem_limit_lock);
	return ret;
}

static void unreserve_mem_limit(struct amdgpu_device *adev,
		uint64_t size, u32 alloc_flag)
{
	size_t acc_size;

	acc_size = amdgpu_amdkfd_acc_size(size);

	spin_lock(&kfd_mem_limit.mem_limit_lock);

	if (alloc_flag & KFD_IOC_ALLOC_MEM_FLAGS_GTT) {
		kfd_mem_limit.system_mem_used -= (acc_size + size);
		kfd_mem_limit.ttm_mem_used -= (acc_size + size);
	} else if (alloc_flag & KFD_IOC_ALLOC_MEM_FLAGS_VRAM) {
		kfd_mem_limit.system_mem_used -= acc_size;
		kfd_mem_limit.ttm_mem_used -= acc_size;
		adev->kfd.vram_used -= size;
	} else if (alloc_flag & KFD_IOC_ALLOC_MEM_FLAGS_USERPTR) {
		kfd_mem_limit.system_mem_used -= (acc_size + size);
		kfd_mem_limit.ttm_mem_used -= acc_size;
	} else if (alloc_flag &
		   (KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL |
		    KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP)) {
		kfd_mem_limit.system_mem_used -= acc_size;
		kfd_mem_limit.ttm_mem_used -= acc_size;
	} else {
		pr_err("%s: Invalid BO type %#x\n", __func__, alloc_flag);
		goto release;
	}

	WARN_ONCE(adev->kfd.vram_used < 0,
		  "KFD VRAM memory accounting unbalanced");
	WARN_ONCE(kfd_mem_limit.ttm_mem_used < 0,
		  "KFD TTM memory accounting unbalanced");
	WARN_ONCE(kfd_mem_limit.system_mem_used < 0,
		  "KFD system memory accounting unbalanced");

release:
	spin_unlock(&kfd_mem_limit.mem_limit_lock);
}

void amdgpu_amdkfd_release_notify(struct amdgpu_bo *bo)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->tbo.bdev);
	u32 alloc_flags = bo->kfd_bo->alloc_flags;
	u64 size = amdgpu_bo_size(bo);

	unreserve_mem_limit(adev, size, alloc_flags);

	kfree(bo->kfd_bo);
}

/* amdgpu_amdkfd_remove_eviction_fence - Removes eviction fence from BO's
 *  reservation object.
 *
 * @bo: [IN] Remove eviction fence(s) from this BO
 * @ef: [IN] This eviction fence is removed if it
 *  is present in the shared list.
 *
 * NOTE: Must be called with BO reserved i.e. bo->tbo.resv->lock held.
 */
static int amdgpu_amdkfd_remove_eviction_fence(struct amdgpu_bo *bo,
					struct amdgpu_amdkfd_fence *ef)
{
	struct dma_resv *resv = amdkcl_ttm_resvp(&bo->tbo);
	struct dma_resv_list *old, *new;
	unsigned int i, j, k;

	if (!ef)
		return -EINVAL;

	old = dma_resv_shared_list(resv);
	if (!old)
		return 0;

	new = kmalloc(struct_size(new, shared, old->shared_max), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	/* Go through all the shared fences in the resevation object and sort
	 * the interesting ones to the end of the list.
	 */
	for (i = 0, j = old->shared_count, k = 0; i < old->shared_count; ++i) {
		struct dma_fence *f;

		f = rcu_dereference_protected(old->shared[i],
					      dma_resv_held(resv));

		if (f->context == ef->base.context)
			RCU_INIT_POINTER(new->shared[--j], f);
		else
			RCU_INIT_POINTER(new->shared[k++], f);
	}
	new->shared_max = old->shared_max;
	new->shared_count = k;

	/* Install the new fence list, seqcount provides the barriers */
	write_seqcount_begin(&resv->seq);
	RCU_INIT_POINTER(resv->fence, new);
	write_seqcount_end(&resv->seq);

	/* Drop the references to the removed fences or move them to ef_list */
	for (i = j; i < old->shared_count; ++i) {
		struct dma_fence *f;

		f = rcu_dereference_protected(new->shared[i],
					      dma_resv_held(resv));
		dma_fence_put(f);
	}
	kfree_rcu(old, rcu);

	return 0;
}

int amdgpu_amdkfd_remove_fence_on_pt_pd_bos(struct amdgpu_bo *bo)
{
	struct amdgpu_bo *root = bo;
	struct amdgpu_vm_bo_base *vm_bo;
	struct amdgpu_vm *vm;
	struct amdkfd_process_info *info;
	struct amdgpu_amdkfd_fence *ef;
	int ret;

	/* we can always get vm_bo from root PD bo.*/
	while (root->parent)
		root = root->parent;

	vm_bo = root->vm_bo;
	if (!vm_bo)
		return 0;

	vm = vm_bo->vm;
	if (!vm)
		return 0;

	info = vm->process_info;
	if (!info || !info->eviction_fence)
		return 0;

	ef = container_of(dma_fence_get(&info->eviction_fence->base),
			struct amdgpu_amdkfd_fence, base);

	BUG_ON(!dma_resv_trylock(amdkcl_ttm_resvp(&bo->tbo)));
	ret = amdgpu_amdkfd_remove_eviction_fence(bo, ef);
	dma_resv_unlock(amdkcl_ttm_resvp(&bo->tbo));

	dma_fence_put(&ef->base);
	return ret;
}

static int amdgpu_amdkfd_bo_validate(struct amdgpu_bo *bo, uint32_t domain,
				     bool wait)
{
	struct ttm_operation_ctx ctx = { false, false };
	int ret;

	if (WARN(amdgpu_ttm_tt_get_usermm(bo->tbo.ttm),
		 "Called with userptr BO"))
		return -EINVAL;

	amdgpu_bo_placement_from_domain(bo, domain);

	ret = ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);
	if (ret)
		goto validate_fail;
	if (wait)
		amdgpu_bo_sync_wait(bo, AMDGPU_FENCE_OWNER_KFD, false);

validate_fail:
	return ret;
}

static int amdgpu_amdkfd_validate_vm_bo(void *_unused, struct amdgpu_bo *bo)
{
	return amdgpu_amdkfd_bo_validate(bo, bo->allowed_domains, false);
}

/* vm_validate_pt_pd_bos - Validate page table and directory BOs
 *
 * Page directories are not updated here because huge page handling
 * during page table updates can invalidate page directory entries
 * again. Page directories are only updated after updating page
 * tables.
 */
static int vm_validate_pt_pd_bos(struct amdgpu_vm *vm)
{
	struct amdgpu_bo *pd = vm->root.bo;
	struct amdgpu_device *adev = amdgpu_ttm_adev(pd->tbo.bdev);
	int ret;

	ret = amdgpu_vm_validate_pt_bos(adev, vm, amdgpu_amdkfd_validate_vm_bo, NULL);
	if (ret) {
		pr_err("failed to validate PT BOs\n");
		return ret;
	}

	ret = amdgpu_amdkfd_validate_vm_bo(NULL, pd);
	if (ret) {
		pr_err("failed to validate PD\n");
		return ret;
	}

	vm->pd_phys_addr = amdgpu_gmc_pd_addr(vm->root.bo);

	if (vm->use_cpu_for_update) {
		ret = amdgpu_bo_kmap(pd, NULL);
		if (ret) {
			pr_err("failed to kmap PD, ret=%d\n", ret);
			return ret;
		}
	}

	return 0;
}

static int vm_update_pds(struct amdgpu_vm *vm, struct amdgpu_sync *sync)
{
	struct amdgpu_bo *pd = vm->root.bo;
	struct amdgpu_device *adev = amdgpu_ttm_adev(pd->tbo.bdev);
	int ret;

	ret = amdgpu_vm_update_pdes(adev, vm, false);
	if (ret)
		return ret;

	return amdgpu_sync_fence(sync, vm->last_update);
}

static uint64_t get_pte_flags(struct amdgpu_device *adev, struct kgd_mem *mem)
{
	struct amdgpu_device *bo_adev = amdgpu_ttm_adev(mem->bo->tbo.bdev);
	bool coherent = mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_COHERENT;
	bool uncached = mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_UNCACHED;
	uint32_t mapping_flags;
	uint64_t pte_flags;
	bool snoop = false;

	mapping_flags = AMDGPU_VM_PAGE_READABLE;
	if (mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE)
		mapping_flags |= AMDGPU_VM_PAGE_WRITEABLE;
	if (mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_EXECUTABLE)
		mapping_flags |= AMDGPU_VM_PAGE_EXECUTABLE;

	switch (adev->asic_type) {
	case CHIP_ARCTURUS:
		if (mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_VRAM) {
			if (bo_adev == adev)
				mapping_flags |= coherent ?
					AMDGPU_VM_MTYPE_CC : AMDGPU_VM_MTYPE_RW;
			else
				mapping_flags |= coherent ?
					AMDGPU_VM_MTYPE_UC : AMDGPU_VM_MTYPE_NC;
		} else {
			mapping_flags |= coherent ?
				AMDGPU_VM_MTYPE_UC : AMDGPU_VM_MTYPE_NC;
		}
		break;
	case CHIP_ALDEBARAN:
		if (coherent && uncached) {
			if (adev->gmc.xgmi.connected_to_cpu ||
				!(mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_VRAM))
				snoop = true;
			mapping_flags |= AMDGPU_VM_MTYPE_UC;
		} else if (mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_VRAM) {
			if (bo_adev == adev) {
				mapping_flags |= coherent ?
					AMDGPU_VM_MTYPE_CC : AMDGPU_VM_MTYPE_RW;
				if (adev->gmc.xgmi.connected_to_cpu)
					snoop = true;
			} else {
				mapping_flags |= coherent ?
					AMDGPU_VM_MTYPE_UC : AMDGPU_VM_MTYPE_NC;
				if (amdgpu_xgmi_same_hive(adev, bo_adev))
					snoop = true;
			}
		} else {
			snoop = true;
			mapping_flags |= coherent ?
				AMDGPU_VM_MTYPE_UC : AMDGPU_VM_MTYPE_NC;
		}
		break;
	default:
		mapping_flags |= coherent ?
			AMDGPU_VM_MTYPE_UC : AMDGPU_VM_MTYPE_NC;
	}

	pte_flags = amdgpu_gem_va_map_flags(adev, mapping_flags);
	pte_flags |= snoop ? AMDGPU_PTE_SNOOPED : 0;

	return pte_flags;
}

static int
kfd_mem_dmamap_userptr(struct kgd_mem *mem,
		       struct kfd_mem_attachment *attachment)
{
	enum dma_data_direction direction =
		mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE ?
		DMA_BIDIRECTIONAL : DMA_TO_DEVICE;
	struct ttm_operation_ctx ctx = {.interruptible = true};
	struct amdgpu_bo *bo = attachment->bo_va->base.bo;
	struct amdgpu_device *adev = attachment->adev;
	struct ttm_tt *src_ttm = mem->bo->tbo.ttm;
	struct ttm_tt *ttm = bo->tbo.ttm;
	int ret;

	ttm->sg = kmalloc(sizeof(*ttm->sg), GFP_KERNEL);
	if (unlikely(!ttm->sg))
		return -ENOMEM;

	if (WARN_ON(ttm->num_pages != src_ttm->num_pages))
		return -EINVAL;

	/* Same sequence as in amdgpu_ttm_tt_pin_userptr */
	ret = sg_alloc_table_from_pages(ttm->sg, src_ttm->pages,
					ttm->num_pages, 0,
					(u64)ttm->num_pages << PAGE_SHIFT,
					GFP_KERNEL);
	if (unlikely(ret))
		goto free_sg;

	ret = dma_map_sgtable(adev->dev, ttm->sg, direction, 0);
	if (unlikely(ret))
		goto release_sg;

	drm_prime_sg_to_dma_addr_array(ttm->sg, ttm->dma_address,
				       ttm->num_pages);

	amdgpu_bo_placement_from_domain(bo, AMDGPU_GEM_DOMAIN_GTT);
	ret = ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);
	if (ret)
		goto unmap_sg;

	return 0;

unmap_sg:
	dma_unmap_sgtable(adev->dev, ttm->sg, direction, 0);
release_sg:
	pr_err("DMA map userptr failed: %d\n", ret);
	sg_free_table(ttm->sg);
free_sg:
	kfree(ttm->sg);
	ttm->sg = NULL;
	return ret;
}

static int
kfd_mem_dmamap_dmabuf(struct kfd_mem_attachment *attachment)
{
	struct ttm_operation_ctx ctx = {.interruptible = true};
	struct amdgpu_bo *bo = attachment->bo_va->base.bo;

	amdgpu_bo_placement_from_domain(bo, AMDGPU_GEM_DOMAIN_GTT);
	return ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);
}

static int
kfd_mem_dmamap_attachment(struct kgd_mem *mem,
			  struct kfd_mem_attachment *attachment)
{
	switch (attachment->type) {
	case KFD_MEM_ATT_SHARED:
		return 0;
	case KFD_MEM_ATT_USERPTR:
		return kfd_mem_dmamap_userptr(mem, attachment);
	case KFD_MEM_ATT_DMABUF:
		return kfd_mem_dmamap_dmabuf(attachment);
	default:
		WARN_ON_ONCE(1);
	}
	return -EINVAL;
}

static void
kfd_mem_dmaunmap_userptr(struct kgd_mem *mem,
			 struct kfd_mem_attachment *attachment)
{
	enum dma_data_direction direction =
		mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE ?
		DMA_BIDIRECTIONAL : DMA_TO_DEVICE;
	struct ttm_operation_ctx ctx = {.interruptible = false};
	struct amdgpu_bo *bo = attachment->bo_va->base.bo;
	struct amdgpu_device *adev = attachment->adev;
	struct ttm_tt *ttm = bo->tbo.ttm;

	if (unlikely(!ttm->sg))
		return;

	amdgpu_bo_placement_from_domain(bo, AMDGPU_GEM_DOMAIN_CPU);
	ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);

	dma_unmap_sgtable(adev->dev, ttm->sg, direction, 0);
	sg_free_table(ttm->sg);
	kfree(ttm->sg);
	ttm->sg = NULL;
}

static void
kfd_mem_dmaunmap_dmabuf(struct kfd_mem_attachment *attachment)
{
	struct ttm_operation_ctx ctx = {.interruptible = true};
	struct amdgpu_bo *bo = attachment->bo_va->base.bo;

	amdgpu_bo_placement_from_domain(bo, AMDGPU_GEM_DOMAIN_CPU);
	ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);
}

static void
kfd_mem_dmaunmap_attachment(struct kgd_mem *mem,
			    struct kfd_mem_attachment *attachment)
{
	switch (attachment->type) {
	case KFD_MEM_ATT_SHARED:
		break;
	case KFD_MEM_ATT_USERPTR:
		kfd_mem_dmaunmap_userptr(mem, attachment);
		break;
	case KFD_MEM_ATT_DMABUF:
		kfd_mem_dmaunmap_dmabuf(attachment);
		break;
	default:
		WARN_ON_ONCE(1);
	}
}

static int
kfd_mem_attach_userptr(struct amdgpu_device *adev, struct kgd_mem *mem,
		       struct amdgpu_bo **bo)
{
	unsigned long bo_size = mem->bo->tbo.base.size;
	struct drm_gem_object *gobj;
	int ret;

	ret = amdgpu_bo_reserve(mem->bo, false);
	if (ret)
		return ret;

	ret = amdgpu_gem_object_create(adev, bo_size, 1,
				       AMDGPU_GEM_DOMAIN_CPU,
				       AMDGPU_GEM_CREATE_PREEMPTIBLE,
				       ttm_bo_type_sg, amdkcl_ttm_resvp(&mem->bo->tbo),
				       &gobj);
	amdgpu_bo_unreserve(mem->bo);
	if (ret)
		return ret;

	*bo = gem_to_amdgpu_bo(gobj);
	(*bo)->parent = amdgpu_bo_ref(mem->bo);

	return 0;
}

#ifdef AMDKCL_AMDGPU_DMABUF_OPS
static int
kfd_mem_attach_dmabuf(struct amdgpu_device *adev, struct kgd_mem *mem,
		      struct amdgpu_bo **bo)
{
	struct drm_gem_object *gobj;
	int ret;

	if (!mem->dmabuf) {
#ifdef HAVE_DRM_DRV_GEM_PRIME_EXPORT_PI
		mem->dmabuf = amdgpu_gem_prime_export(&mem->bo->tbo.base,
#else
		struct amdgpu_device *bo_adev;

		bo_adev = amdgpu_ttm_adev(mem->bo->tbo.bdev);
		mem->dmabuf = amdgpu_gem_prime_export(adev_to_drm(bo_adev),
						&mem->bo->tbo.base,
#endif
			mem->alloc_flags & KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE ?
				DRM_RDWR : 0);
		if (IS_ERR(mem->dmabuf)) {
			ret = PTR_ERR(mem->dmabuf);
			mem->dmabuf = NULL;
			return ret;
		}
	}

	gobj = amdgpu_gem_prime_import(adev_to_drm(adev), mem->dmabuf);
	if (IS_ERR(gobj))
		return PTR_ERR(gobj);

	*bo = gem_to_amdgpu_bo(gobj);
	(*bo)->flags |= AMDGPU_GEM_CREATE_PREEMPTIBLE;
	(*bo)->parent = amdgpu_bo_ref(mem->bo);

	return 0;
}
#endif

/**
 * @kfd_mem_attach_vram_bo: Acquires the handle of a VRAM BO that could
 * be used to enable a peer GPU access it
 *
 * Implementation determines if access to VRAM BO would employ DMABUF
 * or Shared BO mechanism. Employ DMABUF mechanism if kernel has config
 * option DMABUF_MOVE_NOTIFY enabled. Employ Shared BO mechanism if above
 * config option is not set. It is important to note that a Shared BO
 * cannot be used to enable peer acces if system has IOMMU enabled
 *
 * @TODO: ADD Check to ensure IOMMU is not enabled. Should this check
 * be somewhere as this is information could be useful in other places
 */
static int kfd_mem_attach_vram_bo(struct amdgpu_device *adev,
			struct kgd_mem *mem, struct amdgpu_bo **bo,
			struct kfd_mem_attachment *attachment)
{
	int ret =  0;

#if defined(CONFIG_DMABUF_MOVE_NOTIFY) && defined(CONFIG_PCI_P2PDMA)
	attachment->type = KFD_MEM_ATT_DMABUF;
	ret = kfd_mem_attach_dmabuf(adev, mem, bo);
	pr_debug("Employ DMABUF mechanim to enable peer GPU access\n");
#else
	*bo = mem->bo;
	attachment->type = KFD_MEM_ATT_SHARED;
	drm_gem_object_get(&(*bo)->tbo.base);
	pr_debug("Employ Shared BO mechanim to enable peer GPU access\n");
#endif
	return ret;
}

/* kfd_mem_attach - Add a BO to a VM
 *
 * Everything that needs to bo done only once when a BO is first added
 * to a VM. It can later be mapped and unmapped many times without
 * repeating these steps.
 *
 * 0. Create BO for DMA mapping, if needed
 * 1. Allocate and initialize BO VA entry data structure
 * 2. Add BO to the VM
 * 3. Determine ASIC-specific PTE flags
 * 4. Alloc page tables and directories if needed
 * 4a.  Validate new page tables and directories
 */
static int kfd_mem_attach(struct amdgpu_device *adev, struct kgd_mem *mem,
		struct amdgpu_vm *vm, bool is_aql)
{
	struct amdgpu_device *bo_adev = amdgpu_ttm_adev(mem->bo->tbo.bdev);
	unsigned long bo_size = mem->bo->tbo.base.size;
	uint64_t va = mem->va;
	struct kfd_mem_attachment *attachment[2] = {NULL, NULL};
	struct amdgpu_bo *bo[2] = {NULL, NULL};
	bool same_hive = false;
	int i, ret;

	if (!va) {
		pr_err("Invalid VA when adding BO to VM\n");
		return -EINVAL;
	}

	/* Determine if the mapping of VRAM BO to a peer device is valid
	 * It is possible that the peer device is connected via PCIe or
	 * xGMI link. Access over PCIe is allowed if device owning VRAM BO
	 * has large BAR. In contrast, access over xGMI is allowed for both
	 * small and large BAR configurations of device owning the VRAM BO
	 */
	if (adev != bo_adev && mem->domain == AMDGPU_GEM_DOMAIN_VRAM) {
		same_hive = amdgpu_xgmi_same_hive(adev, bo_adev);
		if (!same_hive &&
		    !amdgpu_device_is_peer_accessible(bo_adev, adev))
				return -EINVAL;
	}

	for (i = 0; i <= is_aql; i++) {
		attachment[i] = kzalloc(sizeof(*attachment[i]), GFP_KERNEL);
		if (unlikely(!attachment[i])) {
			ret = -ENOMEM;
			goto unwind;
		}

		pr_debug("\t add VA 0x%llx - 0x%llx to vm %p\n", va,
			 va + bo_size, vm);

		if (adev == bo_adev ||
		   (amdgpu_ttm_tt_get_usermm(mem->bo->tbo.ttm) && adev->ram_is_direct_mapped) ||
		   (mem->domain == AMDGPU_GEM_DOMAIN_VRAM && amdgpu_xgmi_same_hive(adev, bo_adev))) {
			/* Mappings on the local GPU, or VRAM mappings in the
			 * local hive, or userptr mapping IOMMU direct map mode
			 * share the original BO
			 */
			attachment[i]->type = KFD_MEM_ATT_SHARED;
			bo[i] = mem->bo;
			drm_gem_object_get(&bo[i]->tbo.base);
		} else if (i > 0) {
			/* Multiple mappings on the same GPU share the BO */
			attachment[i]->type = KFD_MEM_ATT_SHARED;
			bo[i] = bo[0];
			drm_gem_object_get(&bo[i]->tbo.base);
		} else if (amdgpu_ttm_tt_get_usermm(mem->bo->tbo.ttm)) {
			/* Create an SG BO to DMA-map userptrs on other GPUs */
			attachment[i]->type = KFD_MEM_ATT_USERPTR;
			ret = kfd_mem_attach_userptr(adev, mem, &bo[i]);
			if (ret)
				goto unwind;
#ifdef AMDKCL_AMDGPU_DMABUF_OPS
		} else if (mem->domain == AMDGPU_GEM_DOMAIN_GTT &&
			   mem->bo->tbo.type != ttm_bo_type_sg) {
			/* GTT BOs use DMA-mapping ability of dynamic-attach
			 * DMA bufs. TODO: The same should work for VRAM on
			 * large-BAR GPUs.
			 */
			attachment[i]->type = KFD_MEM_ATT_DMABUF;
			ret = kfd_mem_attach_dmabuf(adev, mem, &bo[i]);
			if (ret)
				goto unwind;
#endif
		/* Enable peer acces to VRAM BO's */
		} else if (mem->domain == AMDGPU_GEM_DOMAIN_VRAM &&
			   mem->bo->tbo.type == ttm_bo_type_device) {
			ret = kfd_mem_attach_vram_bo(adev, mem,
						&bo[i], attachment[i]);
			if (ret)
				goto unwind;
		} else {
			/* FIXME: Need to DMA-map other BO types:
			 * large-BAR VRAM, doorbells, MMIO remap
			 */
			attachment[i]->type = KFD_MEM_ATT_SHARED;
			bo[i] = mem->bo;
			drm_gem_object_get(&bo[i]->tbo.base);
		}

		/* Add BO to VM internal data structures */
		ret = amdgpu_bo_reserve(bo[i], false);
		if (ret) {
			pr_debug("Unable to reserve BO during memory attach");
			goto unwind;
		}
		attachment[i]->bo_va = amdgpu_vm_bo_add(adev, vm, bo[i]);
		amdgpu_bo_unreserve(bo[i]);
		if (unlikely(!attachment[i]->bo_va)) {
			ret = -ENOMEM;
			pr_err("Failed to add BO object to VM. ret == %d\n",
			       ret);
			goto unwind;
		}
		attachment[i]->va = va;
		attachment[i]->pte_flags = get_pte_flags(adev, mem);
		attachment[i]->adev = adev;
		list_add(&attachment[i]->list, &mem->attachments);

		va += bo_size;
	}

	return 0;

unwind:
	for (; i >= 0; i--) {
		if (!attachment[i])
			continue;
		if (attachment[i]->bo_va) {
			amdgpu_bo_reserve(bo[i], true);
			amdgpu_vm_bo_del(adev, attachment[i]->bo_va);
			amdgpu_bo_unreserve(bo[i]);
			list_del(&attachment[i]->list);
		}
		if (bo[i])
			drm_gem_object_put(&bo[i]->tbo.base);
		kfree(attachment[i]);
	}
	return ret;
}

static void kfd_mem_detach(struct kfd_mem_attachment *attachment)
{
	struct amdgpu_bo *bo = attachment->bo_va->base.bo;

	pr_debug("\t remove VA 0x%llx in entry %p\n",
			attachment->va, attachment);
	amdgpu_vm_bo_del(attachment->adev, attachment->bo_va);
	drm_gem_object_put(&bo->tbo.base);
	list_del(&attachment->list);
	kfree(attachment);
}

static void add_kgd_mem_to_kfd_bo_list(struct kgd_mem *mem,
				struct amdkfd_process_info *process_info,
				bool userptr)
{
	struct ttm_validate_buffer *entry = &mem->validate_list;
	struct amdgpu_bo *bo = mem->bo;

	INIT_LIST_HEAD(&entry->head);
	entry->num_shared = 1;
	entry->bo = &bo->tbo;
	mutex_lock(&process_info->lock);
	if (userptr)
		list_add_tail(&entry->head, &process_info->userptr_valid_list);
	else
		list_add_tail(&entry->head, &process_info->kfd_bo_list);
	mutex_unlock(&process_info->lock);
}

static void remove_kgd_mem_from_kfd_bo_list(struct kgd_mem *mem,
		struct amdkfd_process_info *process_info)
{
	struct ttm_validate_buffer *bo_list_entry;

	bo_list_entry = &mem->validate_list;
	mutex_lock(&process_info->lock);
	list_del(&bo_list_entry->head);
	mutex_unlock(&process_info->lock);
}

/* Initializes user pages. It registers the MMU notifier and validates
 * the userptr BO in the GTT domain.
 *
 * The BO must already be on the userptr_valid_list. Otherwise an
 * eviction and restore may happen that leaves the new BO unmapped
 * with the user mode queues running.
 *
 * Takes the process_info->lock to protect against concurrent restore
 * workers.
 *
 * Returns 0 for success, negative errno for errors.
 */
static int init_user_pages(struct kgd_mem *mem, uint64_t user_addr,
			   bool criu_resume)
{
	struct amdkfd_process_info *process_info = mem->process_info;
	struct amdgpu_bo *bo = mem->bo;
	struct ttm_operation_ctx ctx = { true, false };
	int ret = 0;

	mutex_lock(&process_info->lock);

	ret = amdgpu_ttm_tt_set_userptr(&bo->tbo, user_addr, 0);
	if (ret) {
		pr_err("%s: Failed to set userptr: %d\n", __func__, ret);
		goto out;
	}

	ret = amdgpu_mn_register(bo, user_addr);
	if (ret) {
		pr_err("%s: Failed to register MMU notifier: %d\n",
		       __func__, ret);
		goto out;
	}

	if (criu_resume) {
		/*
		 * During a CRIU restore operation, the userptr buffer objects
		 * will be validated in the restore_userptr_work worker at a
		 * later stage when it is scheduled by another ioctl called by
		 * CRIU master process for the target pid for restore.
		 */
		atomic_inc(&mem->invalid);
		mutex_unlock(&process_info->lock);
		return 0;
	}

#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	ret = amdgpu_ttm_tt_get_user_pages(bo, bo->tbo.ttm->pages);
	if (ret) {
		pr_err("%s: Failed to get user pages: %d\n", __func__, ret);
		goto unregister_out;
	}
#else
	/* If no restore worker is running concurrently, user_pages
	 * should not be allocated
	 */
	WARN(mem->user_pages, "Leaking user_pages array");

	mem->user_pages = kvmalloc_array(bo->tbo.ttm->num_pages,
					   sizeof(struct page *),
					   GFP_KERNEL | __GFP_ZERO);
	if (!mem->user_pages) {
		pr_err("%s: Failed to allocate pages array\n", __func__);
		ret = -ENOMEM;
		goto unregister_out;
	}

	ret = amdgpu_ttm_tt_get_user_pages(bo, mem->user_pages);
	if (ret) {
		pr_err("%s: Failed to get user pages: %d\n", __func__, ret);
		goto free_out;
	}

	amdgpu_ttm_tt_set_user_pages(bo->tbo.ttm, mem->user_pages);
#endif

	ret = amdgpu_bo_reserve(bo, true);
	if (ret) {
		pr_err("%s: Failed to reserve BO\n", __func__);
		goto release_out;
	}
	amdgpu_bo_placement_from_domain(bo, mem->domain);
	ret = ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);
	if (ret)
		pr_err("%s: failed to validate BO\n", __func__);
	amdgpu_bo_unreserve(bo);

release_out:
#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	amdgpu_ttm_tt_get_user_pages_done(bo->tbo.ttm);
#else
	if (ret)
		amdgpu_ttm_tt_set_user_pages(bo->tbo.ttm, NULL);
free_out:
	kvfree(mem->user_pages);
	mem->user_pages = NULL;
#endif
unregister_out:
	if (ret)
		amdgpu_mn_unregister(bo);
out:
	mutex_unlock(&process_info->lock);
	return ret;
}

/* Reserving a BO and its page table BOs must happen atomically to
 * avoid deadlocks. Some operations update multiple VMs at once. Track
 * all the reservation info in a context structure. Optionally a sync
 * object can track VM updates.
 */
struct bo_vm_reservation_context {
	struct amdgpu_bo_list_entry kfd_bo; /* BO list entry for the KFD BO */
	unsigned int n_vms;		    /* Number of VMs reserved	    */
	struct amdgpu_bo_list_entry *vm_pd; /* Array of VM BO list entries  */
	struct ww_acquire_ctx ticket;	    /* Reservation ticket	    */
	struct list_head list, duplicates;  /* BO lists			    */
	struct amdgpu_sync *sync;	    /* Pointer to sync object	    */
	bool reserved;			    /* Whether BOs are reserved	    */
};

enum bo_vm_match {
	BO_VM_NOT_MAPPED = 0,	/* Match VMs where a BO is not mapped */
	BO_VM_MAPPED,		/* Match VMs where a BO is mapped     */
	BO_VM_ALL,		/* Match all VMs a BO was added to    */
};

/**
 * reserve_bo_and_vm - reserve a BO and a VM unconditionally.
 * @mem: KFD BO structure.
 * @vm: the VM to reserve.
 * @ctx: the struct that will be used in unreserve_bo_and_vms().
 */
static int reserve_bo_and_vm(struct kgd_mem *mem,
			      struct amdgpu_vm *vm,
			      struct bo_vm_reservation_context *ctx)
{
	struct amdgpu_bo *bo = mem->bo;
	int ret;

	WARN_ON(!vm);

	ctx->reserved = false;
	ctx->n_vms = 1;
	ctx->sync = &mem->sync;

	INIT_LIST_HEAD(&ctx->list);
	INIT_LIST_HEAD(&ctx->duplicates);

	ctx->vm_pd = kcalloc(ctx->n_vms, sizeof(*ctx->vm_pd), GFP_KERNEL);
	if (!ctx->vm_pd)
		return -ENOMEM;

	ctx->kfd_bo.priority = 0;
	ctx->kfd_bo.tv.bo = &bo->tbo;
	ctx->kfd_bo.tv.num_shared = 1;
#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	ctx->kfd_bo.user_pages = NULL;
#endif
	list_add(&ctx->kfd_bo.tv.head, &ctx->list);

	amdgpu_vm_get_pd_bo(vm, &ctx->list, &ctx->vm_pd[0]);

	ret = ttm_eu_reserve_buffers(&ctx->ticket, &ctx->list,
				     false, &ctx->duplicates);
	if (ret) {
		pr_err("Failed to reserve buffers in ttm.\n");
		kfree(ctx->vm_pd);
		ctx->vm_pd = NULL;
		return ret;
	}

	ctx->reserved = true;
	return 0;
}

/**
 * reserve_bo_and_cond_vms - reserve a BO and some VMs conditionally
 * @mem: KFD BO structure.
 * @vm: the VM to reserve. If NULL, then all VMs associated with the BO
 * is used. Otherwise, a single VM associated with the BO.
 * @map_type: the mapping status that will be used to filter the VMs.
 * @ctx: the struct that will be used in unreserve_bo_and_vms().
 *
 * Returns 0 for success, negative for failure.
 */
static int reserve_bo_and_cond_vms(struct kgd_mem *mem,
				struct amdgpu_vm *vm, enum bo_vm_match map_type,
				struct bo_vm_reservation_context *ctx)
{
	struct amdgpu_bo *bo = mem->bo;
	struct kfd_mem_attachment *entry;
	unsigned int i;
	int ret;

	ctx->reserved = false;
	ctx->n_vms = 0;
	ctx->vm_pd = NULL;
	ctx->sync = &mem->sync;

	INIT_LIST_HEAD(&ctx->list);
	INIT_LIST_HEAD(&ctx->duplicates);

	list_for_each_entry(entry, &mem->attachments, list) {
		if ((vm && vm != entry->bo_va->base.vm) ||
			(entry->is_mapped != map_type
			&& map_type != BO_VM_ALL))
			continue;

		ctx->n_vms++;
	}

	if (ctx->n_vms != 0) {
		ctx->vm_pd = kcalloc(ctx->n_vms, sizeof(*ctx->vm_pd),
				     GFP_KERNEL);
		if (!ctx->vm_pd)
			return -ENOMEM;
	}

	ctx->kfd_bo.priority = 0;
	ctx->kfd_bo.tv.bo = &bo->tbo;
	ctx->kfd_bo.tv.num_shared = 1;
#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	ctx->kfd_bo.user_pages = NULL;
#endif
	list_add(&ctx->kfd_bo.tv.head, &ctx->list);

	i = 0;
	list_for_each_entry(entry, &mem->attachments, list) {
		if ((vm && vm != entry->bo_va->base.vm) ||
			(entry->is_mapped != map_type
			&& map_type != BO_VM_ALL))
			continue;

		amdgpu_vm_get_pd_bo(entry->bo_va->base.vm, &ctx->list,
				&ctx->vm_pd[i]);
		i++;
	}

	ret = ttm_eu_reserve_buffers(&ctx->ticket, &ctx->list,
				     false, &ctx->duplicates);
	if (ret) {
		pr_err("Failed to reserve buffers in ttm.\n");
		kfree(ctx->vm_pd);
		ctx->vm_pd = NULL;
		return ret;
	}

	ctx->reserved = true;
	return 0;
}

/**
 * unreserve_bo_and_vms - Unreserve BO and VMs from a reservation context
 * @ctx: Reservation context to unreserve
 * @wait: Optionally wait for a sync object representing pending VM updates
 * @intr: Whether the wait is interruptible
 *
 * Also frees any resources allocated in
 * reserve_bo_and_(cond_)vm(s). Returns the status from
 * amdgpu_sync_wait.
 */
static int unreserve_bo_and_vms(struct bo_vm_reservation_context *ctx,
				 bool wait, bool intr)
{
	int ret = 0;

	if (wait)
		ret = amdgpu_sync_wait(ctx->sync, intr);

	if (ctx->reserved)
		ttm_eu_backoff_reservation(&ctx->ticket, &ctx->list);
	kfree(ctx->vm_pd);

	ctx->sync = NULL;

	ctx->reserved = false;
	ctx->vm_pd = NULL;

	return ret;
}

static void unmap_bo_from_gpuvm(struct kgd_mem *mem,
				struct kfd_mem_attachment *entry,
				struct amdgpu_sync *sync)
{
	struct amdgpu_bo_va *bo_va = entry->bo_va;
	struct amdgpu_device *adev = entry->adev;
	struct amdgpu_vm *vm = bo_va->base.vm;

	amdgpu_vm_bo_unmap(adev, bo_va, entry->va);

	amdgpu_vm_clear_freed(adev, vm, &bo_va->last_pt_update);

	amdgpu_sync_fence(sync, bo_va->last_pt_update);

	kfd_mem_dmaunmap_attachment(mem, entry);
}

static int update_gpuvm_pte(struct kgd_mem *mem,
			    struct kfd_mem_attachment *entry,
			    struct amdgpu_sync *sync,
			    bool *table_freed)
{
	struct amdgpu_bo_va *bo_va = entry->bo_va;
	struct amdgpu_device *adev = entry->adev;
	int ret;

	ret = kfd_mem_dmamap_attachment(mem, entry);
	if (ret)
		return ret;

	/* Update the page tables  */
	ret = amdgpu_vm_bo_update(adev, bo_va, false, table_freed);
	if (ret) {
		pr_err("amdgpu_vm_bo_update failed\n");
		return ret;
	}

	return amdgpu_sync_fence(sync, bo_va->last_pt_update);
}

static int map_bo_to_gpuvm(struct kgd_mem *mem,
			   struct kfd_mem_attachment *entry,
			   struct amdgpu_sync *sync,
			   bool no_update_pte,
			   bool *table_freed)
{
	int ret;

	/* Set virtual address for the allocation */
	ret = amdgpu_vm_bo_map(entry->adev, entry->bo_va, entry->va, 0,
			       amdgpu_bo_size(entry->bo_va->base.bo),
			       entry->pte_flags);
	if (ret) {
		pr_err("Failed to map VA 0x%llx in vm. ret %d\n",
				entry->va, ret);
		return ret;
	}

	if (no_update_pte)
		return 0;

	ret = update_gpuvm_pte(mem, entry, sync, table_freed);
	if (ret) {
		pr_err("update_gpuvm_pte() failed\n");
		goto update_gpuvm_pte_failed;
	}

	return 0;

update_gpuvm_pte_failed:
	unmap_bo_from_gpuvm(mem, entry, sync);
	return ret;
}

static struct sg_table *create_doorbell_sg(uint64_t addr, uint32_t size)
{
	struct sg_table *sg = kmalloc(sizeof(*sg), GFP_KERNEL);

	if (!sg)
		return NULL;
	if (sg_alloc_table(sg, 1, GFP_KERNEL)) {
		kfree(sg);
		return NULL;
	}
	sg->sgl->dma_address = addr;
	sg->sgl->length = size;
#ifdef CONFIG_NEED_SG_DMA_LENGTH
	sg->sgl->dma_length = size;
#endif
	return sg;
}

static bool check_sg_size(struct sg_table *sgt, uint64_t size)
{
	unsigned int count;
	struct scatterlist *sg;

	for_each_sg(sgt->sgl, sg, sgt->nents, count)
		size -= sg->length;

	return (size == 0);
}

static int process_validate_vms(struct amdkfd_process_info *process_info)
{
	struct amdgpu_vm *peer_vm;
	int ret;

	list_for_each_entry(peer_vm, &process_info->vm_list_head,
			    vm_list_node) {
		ret = vm_validate_pt_pd_bos(peer_vm);
		if (ret)
			return ret;
	}

	return 0;
}

static int process_sync_pds_resv(struct amdkfd_process_info *process_info,
				 struct amdgpu_sync *sync)
{
	struct amdgpu_vm *peer_vm;
	int ret;

	list_for_each_entry(peer_vm, &process_info->vm_list_head,
			    vm_list_node) {
		struct amdgpu_bo *pd = peer_vm->root.bo;

		ret = amdgpu_sync_resv(NULL, sync, amdkcl_ttm_resvp(&pd->tbo),
				       AMDGPU_SYNC_NE_OWNER,
				       AMDGPU_FENCE_OWNER_KFD);
		if (ret)
			return ret;
	}

	return 0;
}

static int process_update_pds(struct amdkfd_process_info *process_info,
			      struct amdgpu_sync *sync)
{
	struct amdgpu_vm *peer_vm;
	int ret;

	list_for_each_entry(peer_vm, &process_info->vm_list_head,
			    vm_list_node) {
		ret = vm_update_pds(peer_vm, sync);
		if (ret)
			return ret;
	}

	return 0;
}

static int init_kfd_vm(struct amdgpu_vm *vm, void **process_info,
		       struct dma_fence **ef)
{
	struct amdkfd_process_info *info = NULL;
	int ret;

	if (!*process_info) {
		info = kzalloc(sizeof(*info), GFP_KERNEL);
		if (!info)
			return -ENOMEM;

		mutex_init(&info->lock);
		INIT_LIST_HEAD(&info->vm_list_head);
		INIT_LIST_HEAD(&info->kfd_bo_list);
		INIT_LIST_HEAD(&info->userptr_valid_list);
		INIT_LIST_HEAD(&info->userptr_inval_list);

		info->eviction_fence =
			amdgpu_amdkfd_fence_create(dma_fence_context_alloc(1),
						   current->mm,
						   NULL);
		if (!info->eviction_fence) {
			pr_err("Failed to create eviction fence\n");
			ret = -ENOMEM;
			goto create_evict_fence_fail;
		}

		info->pid = get_task_pid(current->group_leader, PIDTYPE_PID);
		atomic_set(&info->evicted_bos, 0);
		INIT_DELAYED_WORK(&info->restore_userptr_work,
				  amdgpu_amdkfd_restore_userptr_worker);

		*process_info = info;
		*ef = dma_fence_get(&info->eviction_fence->base);
	}

	vm->process_info = *process_info;

	/* Validate page directory and attach eviction fence */
	ret = amdgpu_bo_reserve(vm->root.bo, true);
	if (ret)
		goto reserve_pd_fail;
	ret = vm_validate_pt_pd_bos(vm);
	if (ret) {
		pr_err("validate_pt_pd_bos() failed\n");
		goto validate_pd_fail;
	}
	ret = amdgpu_bo_sync_wait(vm->root.bo,
				  AMDGPU_FENCE_OWNER_KFD, false);
	if (ret)
		goto wait_pd_fail;
	ret = dma_resv_reserve_shared(amdkcl_ttm_resvp(&vm->root.bo->tbo), 1);
	if (ret)
		goto reserve_shared_fail;
	amdgpu_bo_fence(vm->root.bo,
			&vm->process_info->eviction_fence->base, true);
	amdgpu_bo_unreserve(vm->root.bo);

	/* Update process info */
	mutex_lock(&vm->process_info->lock);
	list_add_tail(&vm->vm_list_node,
			&(vm->process_info->vm_list_head));
	vm->process_info->n_vms++;
	mutex_unlock(&vm->process_info->lock);

	return 0;

reserve_shared_fail:
wait_pd_fail:
validate_pd_fail:
	amdgpu_bo_unreserve(vm->root.bo);
reserve_pd_fail:
	vm->process_info = NULL;
	if (info) {
		/* Two fence references: one in info and one in *ef */
		dma_fence_put(&info->eviction_fence->base);
		dma_fence_put(*ef);
		*ef = NULL;
		*process_info = NULL;
		put_pid(info->pid);
create_evict_fence_fail:
		mutex_destroy(&info->lock);
		kfree(info);
	}
	return ret;
}

int amdgpu_amdkfd_gpuvm_acquire_process_vm(struct amdgpu_device *adev,
					   struct file *filp, u32 pasid,
					   void **process_info,
					   struct dma_fence **ef)
{
	struct amdgpu_fpriv *drv_priv;
	struct amdgpu_vm *avm;
	int ret;

	ret = amdgpu_file_to_fpriv(filp, &drv_priv);
	if (ret)
		return ret;
	avm = &drv_priv->vm;

	/* Already a compute VM? */
	if (avm->process_info)
		return -EINVAL;

	/* Free the original amdgpu allocated pasid,
	 * will be replaced with kfd allocated pasid.
	 */
	if (avm->pasid) {
		amdgpu_pasid_free(avm->pasid);
		amdgpu_vm_set_pasid(adev, avm, 0);
	}

	/* Convert VM into a compute VM */
	ret = amdgpu_vm_make_compute(adev, avm);
	if (ret)
		return ret;

	ret = amdgpu_vm_set_pasid(adev, avm, pasid);
	if (ret)
		return ret;
	/* Initialize KFD part of the VM and process info */
	ret = init_kfd_vm(avm, process_info, ef);
	if (ret)
		return ret;

	amdgpu_vm_set_task_info(avm);

	return 0;
}

void amdgpu_amdkfd_gpuvm_destroy_cb(struct amdgpu_device *adev,
				    struct amdgpu_vm *vm)
{
	struct amdkfd_process_info *process_info = vm->process_info;
	struct amdgpu_bo *pd = vm->root.bo;

	if (!process_info)
		return;

	/* Release eviction fence from PD */
	amdgpu_bo_reserve(pd, false);
	amdgpu_bo_fence(pd, NULL, false);
	amdgpu_bo_unreserve(pd);

	/* Update process info */
	mutex_lock(&process_info->lock);
	process_info->n_vms--;
	list_del(&vm->vm_list_node);
	mutex_unlock(&process_info->lock);

	vm->process_info = NULL;

	/* Release per-process resources when last compute VM is destroyed */
	if (!process_info->n_vms) {
		WARN_ON(!list_empty(&process_info->kfd_bo_list));
		WARN_ON(!list_empty(&process_info->userptr_valid_list));
		WARN_ON(!list_empty(&process_info->userptr_inval_list));

		dma_fence_put(&process_info->eviction_fence->base);
		cancel_delayed_work_sync(&process_info->restore_userptr_work);
		put_pid(process_info->pid);
		mutex_destroy(&process_info->lock);
		kfree(process_info);
	}
}

void amdgpu_amdkfd_gpuvm_release_process_vm(struct amdgpu_device *adev,
					    void *drm_priv)
{
	struct amdgpu_vm *avm;

	if (WARN_ON(!adev || !drm_priv))
		return;

	avm = drm_priv_to_vm(drm_priv);

	pr_debug("Releasing process vm %p\n", avm);

	/* The original pasid of amdgpu vm has already been
	 * released during making a amdgpu vm to a compute vm
	 * The current pasid is managed by kfd and will be
	 * released on kfd process destroy. Set amdgpu pasid
	 * to 0 to avoid duplicate release.
	 */
	amdgpu_vm_release_compute(adev, avm);
}

uint64_t amdgpu_amdkfd_gpuvm_get_process_page_dir(void *drm_priv)
{
	struct amdgpu_vm *avm = drm_priv_to_vm(drm_priv);
	struct amdgpu_bo *pd = avm->root.bo;
	struct amdgpu_device *adev = amdgpu_ttm_adev(pd->tbo.bdev);

	if (adev->asic_type < CHIP_VEGA10)
		return avm->pd_phys_addr >> AMDGPU_GPU_PAGE_SHIFT;
	return avm->pd_phys_addr;
}

void amdgpu_amdkfd_block_mmu_notifications(void *p)
{
	struct amdkfd_process_info *pinfo = (struct amdkfd_process_info *)p;

	mutex_lock(&pinfo->lock);
	WRITE_ONCE(pinfo->block_mmu_notifications, true);
	mutex_unlock(&pinfo->lock);
}

int amdgpu_amdkfd_criu_resume(void *p)
{
	int ret = 0;
	struct amdkfd_process_info *pinfo = (struct amdkfd_process_info *)p;

	mutex_lock(&pinfo->lock);
	pr_debug("scheduling work\n");
	atomic_inc(&pinfo->evicted_bos);
	if (!READ_ONCE(pinfo->block_mmu_notifications)) {
		ret = -EINVAL;
		goto out_unlock;
	}
	WRITE_ONCE(pinfo->block_mmu_notifications, false);
	schedule_delayed_work(&pinfo->restore_userptr_work, 0);

out_unlock:
	mutex_unlock(&pinfo->lock);
	return ret;
}

int amdgpu_amdkfd_gpuvm_alloc_memory_of_gpu(
		struct amdgpu_device *adev, uint64_t va, uint64_t size,
		void *drm_priv, struct sg_table *sg, struct kgd_mem **mem,
		uint64_t *offset, uint32_t flags, bool criu_resume)
{
	struct amdgpu_vm *avm = drm_priv_to_vm(drm_priv);
	enum ttm_bo_type bo_type = ttm_bo_type_device;
	uint64_t user_addr = 0;
	struct amdgpu_bo *bo;
	struct drm_gem_object *gobj = NULL;
	u32 domain, alloc_domain;
	u64 alloc_flags;
	int ret;

	/*
	 * Check on which domain to allocate BO
	 */
	if (flags & KFD_IOC_ALLOC_MEM_FLAGS_VRAM) {
		domain = alloc_domain = AMDGPU_GEM_DOMAIN_VRAM;
		alloc_flags = AMDGPU_GEM_CREATE_VRAM_WIPE_ON_RELEASE;
		alloc_flags |= (flags & KFD_IOC_ALLOC_MEM_FLAGS_PUBLIC) ?
			AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED : 0;
	} else if (flags & KFD_IOC_ALLOC_MEM_FLAGS_GTT) {
		domain = alloc_domain = AMDGPU_GEM_DOMAIN_GTT;
		alloc_flags = 0;
		if (sg && !check_sg_size(sg, size))
			return -EINVAL;
	} else if (flags & KFD_IOC_ALLOC_MEM_FLAGS_USERPTR) {
		domain = AMDGPU_GEM_DOMAIN_GTT;
		alloc_domain = AMDGPU_GEM_DOMAIN_CPU;
		alloc_flags = AMDGPU_GEM_CREATE_PREEMPTIBLE;
		if (!offset || !*offset)
			return -EINVAL;
		user_addr = untagged_addr(*offset);
	} else if (flags & (KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL |
			KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP)) {
		domain = AMDGPU_GEM_DOMAIN_GTT;
		alloc_domain = AMDGPU_GEM_DOMAIN_CPU;
		bo_type = ttm_bo_type_sg;
		alloc_flags = 0;
		if (size > UINT_MAX)
			return -EINVAL;
		WARN_ON(sg);
		sg = create_doorbell_sg(*offset, size);
		if (!sg)
			return -ENOMEM;
	} else {
		return -EINVAL;
	}

	if (sg) {
		alloc_domain = AMDGPU_GEM_DOMAIN_CPU;
		bo_type = ttm_bo_type_sg;
	}
	*mem = kzalloc(sizeof(struct kgd_mem), GFP_KERNEL);
	if (!*mem) {
		ret = -ENOMEM;
		goto err;
	}
	INIT_LIST_HEAD(&(*mem)->attachments);
	mutex_init(&(*mem)->lock);
	(*mem)->aql_queue = !!(flags & KFD_IOC_ALLOC_MEM_FLAGS_AQL_QUEUE_MEM);

	/* Workaround for AQL queue wraparound bug. Map the same
	 * memory twice. That means we only actually allocate half
	 * the memory.
	 */
	if ((*mem)->aql_queue)
		size = size >> 1;

	(*mem)->alloc_flags = flags;

	amdgpu_sync_create(&(*mem)->sync);

	ret = amdgpu_amdkfd_reserve_mem_limit(adev, size, flags);
	if (ret) {
		pr_debug("Insufficient memory\n");
		goto err_reserve_limit;
	}

	pr_debug("\tcreate BO VA 0x%llx size 0x%llx domain %s\n",
			va, size, domain_string(alloc_domain));

	ret = amdgpu_gem_object_create(adev, size, 1, alloc_domain, alloc_flags,
				       bo_type, NULL, &gobj);
	if (ret) {
		pr_debug("Failed to create BO on domain %s. ret %d\n",
			 domain_string(alloc_domain), ret);
		goto err_bo_create;
	}
	ret = drm_vma_node_allow(&gobj->vma_node, drm_priv);
	if (ret) {
		pr_debug("Failed to allow vma node access. ret %d\n", ret);
		goto err_node_allow;
	}
	bo = gem_to_amdgpu_bo(gobj);
	if (bo_type == ttm_bo_type_sg) {
		bo->tbo.sg = sg;
		bo->tbo.ttm->sg = sg;
	}
	bo->kfd_bo = *mem;
	(*mem)->bo = bo;
	if (user_addr)
		bo->flags |= AMDGPU_AMDKFD_CREATE_USERPTR_BO;

	(*mem)->va = va;
	(*mem)->domain = domain;
	(*mem)->mapped_to_gpu_memory = 0;
	(*mem)->process_info = avm->process_info;
	add_kgd_mem_to_kfd_bo_list(*mem, avm->process_info, user_addr);

	if (user_addr) {
		pr_debug("creating userptr BO for user_addr = %llu\n", user_addr);
		ret = init_user_pages(*mem, user_addr, criu_resume);
		if (ret)
			goto allocate_init_user_pages_failed;
	} else  if (flags & (KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL |
				KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP)) {
		ret = amdgpu_amdkfd_gpuvm_pin_bo(bo, AMDGPU_GEM_DOMAIN_GTT);
		if (ret) {
			pr_err("Pinning MMIO/DOORBELL BO during ALLOC FAILED\n");
			goto err_pin_bo;
		}
		bo->allowed_domains = AMDGPU_GEM_DOMAIN_GTT;
		bo->preferred_domains = AMDGPU_GEM_DOMAIN_GTT;
	}

	if (offset)
		*offset = amdgpu_bo_mmap_offset(bo);

	return 0;

allocate_init_user_pages_failed:
err_pin_bo:
	remove_kgd_mem_from_kfd_bo_list(*mem, avm->process_info);
	drm_vma_node_revoke(&gobj->vma_node, drm_priv);
err_node_allow:
	/* Don't unreserve system mem limit twice */
	goto err_reserve_limit;
err_bo_create:
	unreserve_mem_limit(adev, size, flags);
err_reserve_limit:
	mutex_destroy(&(*mem)->lock);
	if (gobj)
		drm_gem_object_put(gobj);
	else
		kfree(*mem);
err:
	if (sg) {
		sg_free_table(sg);
		kfree(sg);
	}
	return ret;
}

int amdgpu_amdkfd_gpuvm_free_memory_of_gpu(
		struct amdgpu_device *adev, struct kgd_mem *mem, void *drm_priv,
		uint64_t *size)
{
	struct amdkfd_process_info *process_info = mem->process_info;
	unsigned long bo_size = mem->bo->tbo.base.size;
	struct kfd_mem_attachment *entry, *tmp;
	struct bo_vm_reservation_context ctx;
	struct ttm_validate_buffer *bo_list_entry;
	unsigned int mapped_to_gpu_memory;
	int ret;
	bool is_imported = false;

	mutex_lock(&mem->lock);

	/* Unpin MMIO/DOORBELL BO's that were pinnned during allocation */
	if (mem->alloc_flags &
	    (KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL |
	     KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP)) {
		amdgpu_amdkfd_gpuvm_unpin_bo(mem->bo);
	}

	mapped_to_gpu_memory = mem->mapped_to_gpu_memory;
	is_imported = mem->is_imported;
	mutex_unlock(&mem->lock);
	/* lock is not needed after this, since mem is unused and will
	 * be freed anyway
	 */

	if (mapped_to_gpu_memory > 0) {
		pr_debug("BO VA 0x%llx size 0x%lx is still mapped.\n",
				mem->va, bo_size);
		return -EBUSY;
	}

	/* Make sure restore workers don't access the BO any more */
	bo_list_entry = &mem->validate_list;
	mutex_lock(&process_info->lock);
	list_del(&bo_list_entry->head);
	mutex_unlock(&process_info->lock);

	/* No more MMU notifiers */
	amdgpu_mn_unregister(mem->bo);

#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	/* Free user pages if necessary */
	if (mem->user_pages) {
		pr_debug("%s: Freeing user_pages array\n", __func__);
		if (mem->user_pages[0])
			release_pages(mem->user_pages,
					mem->bo->tbo.ttm->num_pages);
		kvfree(mem->user_pages);
	}
#endif

	ret = reserve_bo_and_cond_vms(mem, NULL, BO_VM_ALL, &ctx);
	if (unlikely(ret))
		return ret;

	/* The eviction fence should be removed by the last unmap.
	 * TODO: Log an error condition if the bo still has the eviction fence
	 * attached
	 */
	amdgpu_amdkfd_remove_eviction_fence(mem->bo,
					process_info->eviction_fence);
	pr_debug("Release VA 0x%llx - 0x%llx\n", mem->va,
		mem->va + bo_size * (1 + mem->aql_queue));

	/* Remove from VM internal data structures */
	list_for_each_entry_safe(entry, tmp, &mem->attachments, list)
		kfd_mem_detach(entry);

	ret = unreserve_bo_and_vms(&ctx, false, false);

	/* Free the sync object */
	amdgpu_sync_free(&mem->sync);

	/* If the SG is not NULL, it's one we created for a doorbell or mmio
	 * remap BO. We need to free it.
	 */
	if (mem->bo->tbo.sg) {
		sg_free_table(mem->bo->tbo.sg);
		kfree(mem->bo->tbo.sg);
	}

	/* Update the size of the BO being freed if it was allocated from
	 * VRAM and is not imported.
	 */
	if (size) {
		if ((mem->bo->preferred_domains == AMDGPU_GEM_DOMAIN_VRAM) &&
		    (!is_imported))
			*size = bo_size;
		else
			*size = 0;
	}

	/* Unreference the ipc_obj if applicable */
	kfd_ipc_obj_put(&mem->ipc_obj);

	/* Free the BO*/
	drm_vma_node_revoke(&mem->bo->tbo.base.vma_node, drm_priv);
	if (mem->dmabuf)
		dma_buf_put(mem->dmabuf);
	mutex_destroy(&mem->lock);

	/* If this releases the last reference, it will end up calling
	 * amdgpu_amdkfd_release_notify and kfree the mem struct. That's why
	 * this needs to be the last call here.
	 */
	drm_gem_object_put(&mem->bo->tbo.base);

	return ret;
}

int amdgpu_amdkfd_gpuvm_map_memory_to_gpu(
		struct amdgpu_device *adev, struct kgd_mem *mem,
		void *drm_priv, bool *table_freed)
{
	struct amdgpu_vm *avm = drm_priv_to_vm(drm_priv);
	int ret;
	struct amdgpu_bo *bo;
	uint32_t domain;
	struct kfd_mem_attachment *entry;
	struct bo_vm_reservation_context ctx;
	unsigned long bo_size;
	bool is_invalid_userptr = false;

	bo = mem->bo;
	if (!bo) {
		pr_err("Invalid BO when mapping memory to GPU\n");
		return -EINVAL;
	}

	/* Make sure restore is not running concurrently. Since we
	 * don't map invalid userptr BOs, we rely on the next restore
	 * worker to do the mapping
	 */
	mutex_lock(&mem->process_info->lock);

	/* Lock mmap-sem. If we find an invalid userptr BO, we can be
	 * sure that the MMU notifier is no longer running
	 * concurrently and the queues are actually stopped
	 */
	if (amdgpu_ttm_tt_get_usermm(bo->tbo.ttm)) {
		mmap_write_lock(current->mm);
		is_invalid_userptr = atomic_read(&mem->invalid);
		mmap_write_unlock(current->mm);
	}

	mutex_lock(&mem->lock);

	domain = mem->domain;
	bo_size = bo->tbo.base.size;

	pr_debug("Map VA 0x%llx - 0x%llx to vm %p domain %s\n",
			mem->va,
			mem->va + bo_size * (1 + mem->aql_queue),
			avm, domain_string(domain));

	if (!kfd_mem_is_attached(avm, mem)) {
		ret = kfd_mem_attach(adev, mem, avm, mem->aql_queue);
		if (ret)
			goto out;
	}

	ret = reserve_bo_and_vm(mem, avm, &ctx);
	if (unlikely(ret))
		goto out;

	/* Userptr can be marked as "not invalid", but not actually be
	 * validated yet (still in the system domain). In that case
	 * the queues are still stopped and we can leave mapping for
	 * the next restore worker
	 */
	if (amdgpu_ttm_tt_get_usermm(bo->tbo.ttm) &&
	    bo->tbo.resource->mem_type == TTM_PL_SYSTEM)
		is_invalid_userptr = true;

	ret = vm_validate_pt_pd_bos(avm);
	if (unlikely(ret))
		goto out_unreserve;

	if (mem->mapped_to_gpu_memory == 0 &&
	    !amdgpu_ttm_tt_get_usermm(bo->tbo.ttm)) {
		/* Validate BO only once. The eviction fence gets added to BO
		 * the first time it is mapped. Validate will wait for all
		 * background evictions to complete.
		 */
		ret = amdgpu_amdkfd_bo_validate(bo, domain, true);
		if (ret) {
			pr_debug("Validate failed\n");
			goto out_unreserve;
		}
	}

	list_for_each_entry(entry, &mem->attachments, list) {
		if (entry->bo_va->base.vm != avm || entry->is_mapped)
			continue;

		pr_debug("\t map VA 0x%llx - 0x%llx in entry %p\n",
			 entry->va, entry->va + bo_size, entry);

		ret = map_bo_to_gpuvm(mem, entry, ctx.sync,
				      is_invalid_userptr, table_freed);
		if (ret) {
			pr_err("Failed to map bo to gpuvm\n");
			goto out_unreserve;
		}

		ret = vm_update_pds(avm, ctx.sync);
		if (ret) {
			pr_err("Failed to update page directories\n");
			goto out_unreserve;
		}

		entry->is_mapped = true;
		mem->mapped_to_gpu_memory++;
		pr_debug("\t INC mapping count %d\n",
			 mem->mapped_to_gpu_memory);
	}

	if (!amdgpu_ttm_tt_get_usermm(bo->tbo.ttm) && !bo->tbo.pin_count)
		amdgpu_bo_fence(bo,
				&avm->process_info->eviction_fence->base,
				true);
	ret = unreserve_bo_and_vms(&ctx, false, false);

	goto out;

out_unreserve:
	unreserve_bo_and_vms(&ctx, false, false);
out:
	mutex_unlock(&mem->process_info->lock);
	mutex_unlock(&mem->lock);
	return ret;
}

int amdgpu_amdkfd_gpuvm_unmap_memory_from_gpu(
		struct amdgpu_device *adev, struct kgd_mem *mem, void *drm_priv)
{
	struct amdgpu_vm *avm = drm_priv_to_vm(drm_priv);
	struct amdkfd_process_info *process_info = avm->process_info;
	unsigned long bo_size = mem->bo->tbo.base.size;
	struct kfd_mem_attachment *entry;
	struct bo_vm_reservation_context ctx;
	int ret;

	mutex_lock(&mem->lock);

	ret = reserve_bo_and_cond_vms(mem, avm, BO_VM_MAPPED, &ctx);
	if (unlikely(ret))
		goto out;
	/* If no VMs were reserved, it means the BO wasn't actually mapped */
	if (ctx.n_vms == 0) {
		ret = -EINVAL;
		goto unreserve_out;
	}

	ret = vm_validate_pt_pd_bos(avm);
	if (unlikely(ret))
		goto unreserve_out;

	pr_debug("Unmap VA 0x%llx - 0x%llx from vm %p\n",
		mem->va,
		mem->va + bo_size * (1 + mem->aql_queue),
		avm);

	list_for_each_entry(entry, &mem->attachments, list) {
		if (entry->bo_va->base.vm != avm || !entry->is_mapped)
			continue;

		pr_debug("\t unmap VA 0x%llx - 0x%llx from entry %p\n",
			 entry->va, entry->va + bo_size, entry);

		unmap_bo_from_gpuvm(mem, entry, ctx.sync);
		entry->is_mapped = false;

		mem->mapped_to_gpu_memory--;
		pr_debug("\t DEC mapping count %d\n",
			 mem->mapped_to_gpu_memory);
	}

	/* If BO is unmapped from all VMs, unfence it. It can be evicted if
	 * required.
	 */
	if (mem->mapped_to_gpu_memory == 0 &&
	    !amdgpu_ttm_tt_get_usermm(mem->bo->tbo.ttm) &&
	    !mem->bo->tbo.pin_count)
		amdgpu_amdkfd_remove_eviction_fence(mem->bo,
						process_info->eviction_fence);

unreserve_out:
	unreserve_bo_and_vms(&ctx, false, false);
out:
	mutex_unlock(&mem->lock);
	return ret;
}

int amdgpu_amdkfd_gpuvm_sync_memory(
		struct amdgpu_device *adev, struct kgd_mem *mem, bool intr)
{
	struct amdgpu_sync sync;
	int ret;

	amdgpu_sync_create(&sync);

	mutex_lock(&mem->lock);
	amdgpu_sync_clone(&mem->sync, &sync);
	mutex_unlock(&mem->lock);

	ret = amdgpu_sync_wait(&sync, intr);
	amdgpu_sync_free(&sync);
	return ret;
}

int amdgpu_amdkfd_gpuvm_map_gtt_bo_to_kernel(struct amdgpu_device *adev,
		struct kgd_mem *mem, void **kptr, uint64_t *size)
{
	int ret;
	struct amdgpu_bo *bo = mem->bo;

	if (amdgpu_ttm_tt_get_usermm(bo->tbo.ttm)) {
		pr_err("userptr can't be mapped to kernel\n");
		return -EINVAL;
	}

	/* delete kgd_mem from kfd_bo_list to avoid re-validating
	 * this BO in BO's restoring after eviction.
	 */
	mutex_lock(&mem->process_info->lock);

	ret = amdgpu_bo_reserve(bo, true);
	if (ret) {
		pr_err("Failed to reserve bo. ret %d\n", ret);
		goto bo_reserve_failed;
	}

	ret = amdgpu_bo_pin(bo, AMDGPU_GEM_DOMAIN_GTT);
	if (ret) {
		pr_err("Failed to pin bo. ret %d\n", ret);
		goto pin_failed;
	}

	ret = amdgpu_bo_kmap(bo, kptr);
	if (ret) {
		pr_err("Failed to map bo to kernel. ret %d\n", ret);
		goto kmap_failed;
	}

	amdgpu_amdkfd_remove_eviction_fence(
		bo, mem->process_info->eviction_fence);
	list_del_init(&mem->validate_list.head);

	if (size)
		*size = amdgpu_bo_size(bo);

	amdgpu_bo_unreserve(bo);

	mutex_unlock(&mem->process_info->lock);
	return 0;

kmap_failed:
	amdgpu_bo_unpin(bo);
pin_failed:
	amdgpu_bo_unreserve(bo);
bo_reserve_failed:
	mutex_unlock(&mem->process_info->lock);

	return ret;
}

void amdgpu_amdkfd_gpuvm_unmap_gtt_bo_from_kernel(struct amdgpu_device *adev,
						  struct kgd_mem *mem)
{
	struct amdgpu_bo *bo = mem->bo;

	amdgpu_bo_reserve(bo, true);
	amdgpu_bo_kunmap(bo);
	amdgpu_bo_unpin(bo);
	amdgpu_bo_unreserve(bo);
}

int amdgpu_amdkfd_gpuvm_get_vm_fault_info(struct amdgpu_device *adev,
					  struct kfd_vm_fault_info *mem)
{
	if (atomic_read(&adev->gmc.vm_fault_info_updated) == 1) {
		*mem = *adev->gmc.vm_fault_info;
		mb();
		atomic_set(&adev->gmc.vm_fault_info_updated, 0);
	}
	return 0;
}

struct amdgpu_bo *amdgpu_amdkfd_gpuvm_get_bo_ref(struct kgd_mem *mem,
						 uint32_t *flags)
{
	struct amdgpu_bo *bo = mem->bo;

	if (flags)
		*flags = mem->alloc_flags;
	drm_gem_object_get(&bo->tbo.base);
	return bo;
}

void amdgpu_amdkfd_gpuvm_put_bo_ref(struct amdgpu_bo *bo)
{
	drm_gem_object_put(&bo->tbo.base);
}

int amdgpu_amdkfd_gpuvm_pin_bo(struct amdgpu_bo *bo, u32 domain)
{
	int ret = 0;

	ret = amdgpu_bo_reserve(bo, false);
	if (unlikely(ret))
		return ret;

	ret = amdgpu_bo_pin_restricted(bo, domain, 0, 0);
	if (ret)
		pr_err("Error in Pinning BO to domain: %d\n", domain);

	amdgpu_bo_sync_wait(bo, AMDGPU_FENCE_OWNER_KFD, false);
	amdgpu_bo_unreserve(bo);

	return ret;
}

void amdgpu_amdkfd_gpuvm_unpin_bo(struct amdgpu_bo *bo)
{
	int ret = 0;

	ret = amdgpu_bo_reserve(bo, false);
	if (unlikely(ret))
		return;

	amdgpu_bo_unpin(bo);
	amdgpu_bo_unreserve(bo);
}

#define AMD_GPU_PAGE_SHIFT	PAGE_SHIFT
#define AMD_GPU_PAGE_SIZE (_AC(1, UL) << AMD_GPU_PAGE_SHIFT)

/**
 * @get_sg_table_of_mmio_or_doorbel_bo - Builds and returns an instance
 * of scatter gather table (sg_table) for BO's that represent MMIO or
 * DOORBELL memory. An example of this is the MMIO BO that is used to
 * surface HDP registers.
 *
 * @note: Per current design and implementation MMIO or DOORBELL BO's
 * use only one scatterlist node in their sg_table. This is because
 * the size of backing memory is relatively small (e.g. 4096 bytes
 * for MMIO BO surfacing HDP registers). Implementation of this method
 * relies on this design choice.
 *
 * The method does the following:
 *	Acquire address to use in building scatterlist nodes
 *	Acquire size of memory to use in building scatterlist nodes
 *	Invoke DMA Map service to obtain DMA'able address
 *	Access sg_table construction service with above parameters
 *	Return the handle of scatter gather table
 *
 * @adev: GPU device whose MMIO address needs to be exported
 * @bo: Buffer object representing MMIO/DOORBELL memory e.g. HDP registers
 * @dma_dev: Handle of peer PCIe device that wishes to access BO's memory
 * @dir: Direction of data movement from peer PCIe devices perspective
 *
 * @sgt: Output parameter that is built and returned
 *
 * Return: zero if successful, non-zero otherwise
 *
 * @FIXME: This will only work as long as bo->tbo.sg->sgl->dma_address
 * is not a DMA address but a physical BAR address. This will be reworked
 * later when we add DMA mapping support for doorbell and MMIO BOs
 */
static int get_sg_table_of_mmio_or_doorbel_bo(struct amdgpu_bo *bo,
		struct device *dma_dev, enum dma_data_direction dir,
		struct sg_table **sgt)
{
	dma_addr_t dma_addr;
	s32 size, ret;
	u64 addr;

	/* Acquire the address of MMIO or DOORBELL BO being
	 * exported. By policy the entire backing memory is
	 * encapsulated in one scatterlist node
	 */
	size = bo->tbo.sg->sgl->length;
	addr = bo->tbo.sg->sgl->dma_address;
	pr_debug("MMIO/Doorbell address being exported: %llx\n", addr);

	/* DMA map the acquired address - MMIO or DOORBELL */
	dma_addr = dma_map_resource(dma_dev, addr, size,
				    dir, DMA_ATTR_SKIP_CPU_SYNC);
	ret = dma_mapping_error(dma_dev, dma_addr);
	if (ret)
		return ret;

	/* Update output parameter with a new sg_table */
	pr_debug("MMIO/Doorbell BO size: %d\n", size);
	pr_debug("MMIO/Doorbell's DMA Address: %llx\n", dma_addr);
	*sgt = create_doorbell_sg(dma_addr, size);
	return 0;
}

int amdgpu_amdkfd_gpuvm_get_sg_table(struct amdgpu_device *adev,
		struct amdgpu_bo *bo, uint32_t flags,
		uint64_t offset, uint64_t size,
		struct device *dma_dev, enum dma_data_direction dir,
		struct sg_table **ret_sg)
{
	struct sg_table *sg = NULL;
	struct scatterlist *s;
	struct page **pages;
	uint64_t offset_in_page;
	unsigned int page_size;
	unsigned int cur_page;
	unsigned int chunks;
	unsigned int idx;
	int ret;

	/* Determine access does not cross memory boundary */
	if (size + offset > amdgpu_bo_size(bo))
		return -EFAULT;

	/* For GPU memory use VRAM Mgr to build SG Table */
	if (bo->preferred_domains == AMDGPU_GEM_DOMAIN_VRAM) {
		ret = amdgpu_vram_mgr_alloc_sgt(adev, bo->tbo.resource, offset,
						size, dma_dev, dir, &sg);
		*ret_sg = (ret == 0) ?  sg : NULL;
		return ret;
	}

	/* Handle BO (type: ttm_bo_type_sg) that is used to surface
	 * resources from MMIO address space. The allocation flag of
	 * BO fall in MMIO_REMAP / DOORBELL domain
	 */
	if (bo->tbo.type == ttm_bo_type_sg &&
	    ((flags & KFD_IOC_ALLOC_MEM_FLAGS_DOORBELL) ||
	     (flags & KFD_IOC_ALLOC_MEM_FLAGS_MMIO_REMAP))) {
		ret = get_sg_table_of_mmio_or_doorbel_bo(bo, dma_dev, dir, &sg);
		*ret_sg = (ret == 0) ?  sg : NULL;
		return ret;
	}

	/* Handle BO (type: ttm_bo_type_device) that is used to surface
	 * memory resources from GPU's GART aperture. The allocation flag
	 * of BO falls in GTT domain i.e. the physical backing memory is
	 * part of system memory. Construction of SG Table proceeds
	 * as follows:
	 *
	 *    Allocate memory for SG Table
	 *    Determine number of Scatterlist node in table
	 *       Logic uses one Scatterlist node per PAGE_SIZE
	 *    Allocate memory for Scatterlist nodes
	 *    Initialize Scatterlist nodes to zero length
	 *    Walk down system memory pointed by BO while
	 *       Updating Scatterlist nodes with system memory info
	 */
	sg = kmalloc(sizeof(*sg), GFP_KERNEL);
	if (!sg) {
		ret = -ENOMEM;
		goto out;
	}

	page_size = PAGE_SIZE;
	offset_in_page = offset & (page_size - 1);
	chunks = (size  + offset_in_page + page_size - 1)
			/ page_size;

	ret = sg_alloc_table(sg, chunks, GFP_KERNEL);
	if (unlikely(ret))
		goto out;

	for_each_sgtable_sg(sg, s, idx)
		s->length = 0;

	pages = bo->tbo.ttm->pages;
	cur_page = offset / page_size;
	for_each_sg(sg->sgl, s, sg->orig_nents, idx) {
		uint64_t chunk_size, length;

		chunk_size = page_size - offset_in_page;
		length = min(size, chunk_size);

		sg_set_page(s, pages[cur_page], length, offset_in_page);
		s->dma_address = page_to_phys(pages[cur_page]);
		s->dma_length = length;

		size -= length;
		offset_in_page = 0;
		cur_page++;
	}

	ret = dma_map_sgtable(dma_dev, sg, dir, DMA_ATTR_SKIP_CPU_SYNC);
	if (ret)
		goto out_of_range;

	*ret_sg = sg;
	return 0;

out_of_range:
	sg_free_table(sg);
out:
	kfree(sg);
	*ret_sg = NULL;
	return ret;
}

void amdgpu_amdkfd_gpuvm_put_sg_table(struct amdgpu_bo *bo,
		struct device *dma_dev, enum dma_data_direction dir,
		struct sg_table *sgt)
{
	/* Unmap GPU device memory */
	if (bo->preferred_domains == AMDGPU_GEM_DOMAIN_VRAM) {
		amdgpu_vram_mgr_free_sgt(dma_dev, dir, sgt);
		return;
	}

	/* Unmap system memory */
	dma_unmap_sgtable(dma_dev, sgt, dir, DMA_ATTR_SKIP_CPU_SYNC);
	sg_free_table(sgt);
	kfree(sgt);
}

int amdgpu_amdkfd_gpuvm_import_dmabuf(struct amdgpu_device *adev,
				      struct dma_buf *dma_buf,
				      struct kfd_ipc_obj *ipc_obj,
				      uint64_t va, void *drm_priv,
				      struct kgd_mem **mem, uint64_t *size,
				      uint64_t *mmap_offset)
{
	struct amdgpu_vm *avm = drm_priv_to_vm(drm_priv);
	struct drm_gem_object *obj;
	struct amdgpu_bo *bo;
	int ret;

#if defined(AMDKCL_AMDGPU_DMABUF_OPS)
	if (dma_buf->ops != &amdgpu_dmabuf_ops)
		/* Can't handle non-graphics buffers */
		return -EINVAL;
#endif

	obj = dma_buf->priv;
	if (drm_to_adev(obj->dev) != adev)
		/* Can't handle buffers from other devices */
		return -EINVAL;

	bo = gem_to_amdgpu_bo(obj);
	if (!(bo->preferred_domains & (AMDGPU_GEM_DOMAIN_VRAM |
				    AMDGPU_GEM_DOMAIN_GTT)))
		/* Only VRAM and GTT BOs are supported */
		return -EINVAL;

	*mem = kzalloc(sizeof(struct kgd_mem), GFP_KERNEL);
	if (!*mem)
		return -ENOMEM;

	ret = drm_vma_node_allow(&obj->vma_node, drm_priv);
	if (ret) {
		kfree(mem);
		return ret;
	}

	if (size)
		*size = amdgpu_bo_size(bo);

	if (mmap_offset)
		*mmap_offset = amdgpu_bo_mmap_offset(bo);

	INIT_LIST_HEAD(&(*mem)->attachments);
	mutex_init(&(*mem)->lock);
	if (bo->kfd_bo)
		(*mem)->alloc_flags = bo->kfd_bo->alloc_flags;
	else
		(*mem)->alloc_flags =
			((bo->preferred_domains & AMDGPU_GEM_DOMAIN_VRAM) ?
			KFD_IOC_ALLOC_MEM_FLAGS_VRAM : KFD_IOC_ALLOC_MEM_FLAGS_GTT)
			| KFD_IOC_ALLOC_MEM_FLAGS_WRITABLE
			| KFD_IOC_ALLOC_MEM_FLAGS_EXECUTABLE;

	drm_gem_object_get(&bo->tbo.base);
	(*mem)->bo = bo;
	(*mem)->ipc_obj = ipc_obj;
	(*mem)->va = va;
	(*mem)->domain = (bo->preferred_domains & AMDGPU_GEM_DOMAIN_VRAM) ?
		AMDGPU_GEM_DOMAIN_VRAM : AMDGPU_GEM_DOMAIN_GTT;
	(*mem)->mapped_to_gpu_memory = 0;
	(*mem)->process_info = avm->process_info;
	add_kgd_mem_to_kfd_bo_list(*mem, avm->process_info, false);
	amdgpu_sync_create(&(*mem)->sync);
	(*mem)->is_imported = true;

	return 0;
}

int amdgpu_amdkfd_gpuvm_export_ipc_obj(struct amdgpu_device *adev, void *vm,
				       struct kgd_mem *mem,
				       struct kfd_ipc_obj **ipc_obj,
				       uint32_t flags)
{
	struct dma_buf *dmabuf;
	int r = 0;

	if (!adev|| !vm || !mem)
		return -EINVAL;

	mutex_lock(&mem->lock);

	if (mem->ipc_obj) {
		*ipc_obj = mem->ipc_obj;
		goto unlock_out;
	}

#ifdef HAVE_DRM_DRV_GEM_PRIME_EXPORT_PI
	dmabuf = amdgpu_gem_prime_export(&mem->bo->tbo.base, 0);
#else
	dmabuf = amdgpu_gem_prime_export(adev_to_drm(adev), &mem->bo->tbo.base, 0);
#endif
	if (IS_ERR(dmabuf)) {
		r = PTR_ERR(dmabuf);
		goto unlock_out;
	}

	r = kfd_ipc_store_insert(dmabuf, &mem->ipc_obj, flags);
	if (r)
		dma_buf_put(dmabuf);
	else
		*ipc_obj = mem->ipc_obj;

unlock_out:
	mutex_unlock(&mem->lock);
	return r;
}

/* Evict a userptr BO by stopping the queues if necessary
 *
 * Runs in MMU notifier, may be in RECLAIM_FS context. This means it
 * cannot do any memory allocations, and cannot take any locks that
 * are held elsewhere while allocating memory. Therefore this is as
 * simple as possible, using atomic counters.
 *
 * It doesn't do anything to the BO itself. The real work happens in
 * restore, where we get updated page addresses. This function only
 * ensures that GPU access to the BO is stopped.
 */
int amdgpu_amdkfd_evict_userptr(struct kgd_mem *mem,
				struct mm_struct *mm)
{
	struct amdkfd_process_info *process_info = mem->process_info;
	int evicted_bos;
	int r = 0;

	/* Do not process MMU notifications until stage-4 IOCTL is received */
	if (READ_ONCE(process_info->block_mmu_notifications))
		return 0;

	atomic_inc(&mem->invalid);
	evicted_bos = atomic_inc_return(&process_info->evicted_bos);
	if (evicted_bos == 1) {
		/* First eviction, stop the queues */
		r = kgd2kfd_quiesce_mm(mm);
		if (r)
			pr_err("Failed to quiesce KFD\n");
		schedule_delayed_work(&process_info->restore_userptr_work,
			msecs_to_jiffies(AMDGPU_USERPTR_RESTORE_DELAY_MS));
	}

	return r;
}

/* Update invalid userptr BOs
 *
 * Moves invalidated (evicted) userptr BOs from userptr_valid_list to
 * userptr_inval_list and updates user pages for all BOs that have
 * been invalidated since their last update.
 */
static int update_invalid_user_pages(struct amdkfd_process_info *process_info,
				     struct mm_struct *mm)
{
	struct kgd_mem *mem, *tmp_mem;
	struct amdgpu_bo *bo;
	struct ttm_operation_ctx ctx = { false, false };
	int invalid, ret;

	/* Move all invalidated BOs to the userptr_inval_list and
	 * release their user pages by migration to the CPU domain
	 */
	list_for_each_entry_safe(mem, tmp_mem,
				 &process_info->userptr_valid_list,
				 validate_list.head) {
		if (!atomic_read(&mem->invalid))
			continue; /* BO is still valid */

		bo = mem->bo;

		if (amdgpu_bo_reserve(bo, true))
			return -EAGAIN;
		amdgpu_bo_placement_from_domain(bo, AMDGPU_GEM_DOMAIN_CPU);
		ret = ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);
		amdgpu_bo_unreserve(bo);
		if (ret) {
			pr_err("%s: Failed to invalidate userptr BO\n",
			       __func__);
			return -EAGAIN;
		}

		list_move_tail(&mem->validate_list.head,
			       &process_info->userptr_inval_list);
	}

	if (list_empty(&process_info->userptr_inval_list))
		return 0; /* All evicted userptr BOs were freed */

	/* Go through userptr_inval_list and update any invalid user_pages */
	list_for_each_entry(mem, &process_info->userptr_inval_list,
			    validate_list.head) {
		invalid = atomic_read(&mem->invalid);
		if (!invalid)
			/* BO hasn't been invalidated since the last
			 * revalidation attempt. Keep its BO list.
			 */
			continue;

		bo = mem->bo;

#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
		/* Get updated user pages */
		ret = amdgpu_ttm_tt_get_user_pages(bo, bo->tbo.ttm->pages);
		if (ret) {
			pr_debug("Failed %d to get user pages\n", ret);

			/* Return -EFAULT bad address error as success. It will
			 * fail later with a VM fault if the GPU tries to access
			 * it. Better than hanging indefinitely with stalled
			 * user mode queues.
			 *
			 * Return other error -EBUSY or -ENOMEM to retry restore
			 */
			if (ret != -EFAULT)
				return ret;
		} else {

			/*
			 * FIXME: Cannot ignore the return code, must hold
			 * notifier_lock
			 */
			amdgpu_ttm_tt_get_user_pages_done(bo->tbo.ttm);
		}
#else
		if (!mem->user_pages) {
			mem->user_pages =
				kvmalloc_array(bo->tbo.ttm->num_pages,
						 sizeof(struct page *),
						 GFP_KERNEL | __GFP_ZERO);
			if (!mem->user_pages) {
				pr_err("%s: Failed to allocate pages array\n",
				       __func__);
				return -ENOMEM;
			}
		} else if (mem->user_pages[0]) {
			release_pages(mem->user_pages, bo->tbo.ttm->num_pages);
		}

		/* Get updated user pages */
		ret = amdgpu_ttm_tt_get_user_pages(bo, mem->user_pages);
		if (ret) {
			mem->user_pages[0] = NULL;
			pr_info("%s: Failed to get user pages: %d\n",
				__func__, ret);
			/* Pretend it succeeded. It will fail later
			 * with a VM fault if the GPU tries to access
			 * it. Better than hanging indefinitely with
			 * stalled user mode queues.
			 */
		}
#endif
		/* Mark the BO as valid unless it was invalidated
		 * again concurrently.
		 */
		if (atomic_cmpxchg(&mem->invalid, invalid, 0) != invalid)
			return -EAGAIN;
	}

	return 0;
}

/* Validate invalid userptr BOs
 *
 * Validates BOs on the userptr_inval_list, and moves them back to the
 * userptr_valid_list. Also updates GPUVM page tables with new page
 * addresses and waits for the page table updates to complete.
 */
static int validate_invalid_user_pages(struct amdkfd_process_info *process_info)
{
	struct amdgpu_bo_list_entry *pd_bo_list_entries;
	struct list_head resv_list, duplicates;
	struct ww_acquire_ctx ticket;
	struct amdgpu_sync sync;

	struct amdgpu_vm *peer_vm;
	struct kgd_mem *mem, *tmp_mem;
	struct amdgpu_bo *bo;
	struct ttm_operation_ctx ctx = { false, false };
	int i, ret;

	pd_bo_list_entries = kcalloc(process_info->n_vms,
				     sizeof(struct amdgpu_bo_list_entry),
				     GFP_KERNEL);
	if (!pd_bo_list_entries) {
		pr_err("%s: Failed to allocate PD BO list entries\n", __func__);
		ret = -ENOMEM;
		goto out_no_mem;
	}

	INIT_LIST_HEAD(&resv_list);
	INIT_LIST_HEAD(&duplicates);

	/* Get all the page directory BOs that need to be reserved */
	i = 0;
	list_for_each_entry(peer_vm, &process_info->vm_list_head,
			    vm_list_node)
		amdgpu_vm_get_pd_bo(peer_vm, &resv_list,
				    &pd_bo_list_entries[i++]);
	/* Add the userptr_inval_list entries to resv_list */
	list_for_each_entry(mem, &process_info->userptr_inval_list,
			    validate_list.head) {
		list_add_tail(&mem->resv_list.head, &resv_list);
		mem->resv_list.bo = mem->validate_list.bo;
		mem->resv_list.num_shared = mem->validate_list.num_shared;
	}

	/* Reserve all BOs and page tables for validation */
	ret = ttm_eu_reserve_buffers(&ticket, &resv_list, false, &duplicates);
	WARN(!list_empty(&duplicates), "Duplicates should be empty");
	if (ret)
		goto out_free;

	amdgpu_sync_create(&sync);

	ret = process_validate_vms(process_info);
	if (ret)
		goto unreserve_out;

	/* Validate BOs and update GPUVM page tables */
	list_for_each_entry_safe(mem, tmp_mem,
				 &process_info->userptr_inval_list,
				 validate_list.head) {
		struct kfd_mem_attachment *attachment;

		bo = mem->bo;

#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
		/* Validate the BO if we got user pages */
		if (bo->tbo.ttm->pages[0]) {
			amdgpu_bo_placement_from_domain(bo, mem->domain);
			ret = ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);
			if (ret) {
				pr_err("%s: failed to validate BO\n", __func__);
				goto unreserve_out;
			}
		}

#else
		/* Copy pages array and validate the BO if we got user pages */
		if (mem->user_pages[0]) {
			amdgpu_ttm_tt_set_user_pages(bo->tbo.ttm,
						     mem->user_pages);
			amdgpu_bo_placement_from_domain(bo, mem->domain);
			ret = ttm_bo_validate(&bo->tbo, &bo->placement, &ctx);
			if (ret) {
				pr_err("%s: failed to validate BO\n", __func__);
				goto unreserve_out;
			}
		}

		/* Validate succeeded, now the BO owns the pages, free
		 * our copy of the pointer array. Put this BO back on
		 * the userptr_valid_list. If we need to revalidate
		 * it, we need to start from scratch.
		 */
		kvfree(mem->user_pages);
		mem->user_pages = NULL;
#endif

		list_move_tail(&mem->validate_list.head,
			       &process_info->userptr_valid_list);

		/* Update mapping. If the BO was not validated
		 * (because we couldn't get user pages), this will
		 * clear the page table entries, which will result in
		 * VM faults if the GPU tries to access the invalid
		 * memory.
		 */
		list_for_each_entry(attachment, &mem->attachments, list) {
			if (!attachment->is_mapped)
				continue;

			kfd_mem_dmaunmap_attachment(mem, attachment);
			ret = update_gpuvm_pte(mem, attachment, &sync, NULL);
			if (ret) {
				pr_err("%s: update PTE failed\n", __func__);
				/* make sure this gets validated again */
				atomic_inc(&mem->invalid);
				goto unreserve_out;
			}
		}
	}

	/* Update page directories */
	ret = process_update_pds(process_info, &sync);

unreserve_out:
	ttm_eu_backoff_reservation(&ticket, &resv_list);
	amdgpu_sync_wait(&sync, false);
	amdgpu_sync_free(&sync);
out_free:
	kfree(pd_bo_list_entries);
out_no_mem:

	return ret;
}

/* Worker callback to restore evicted userptr BOs
 *
 * Tries to update and validate all userptr BOs. If successful and no
 * concurrent evictions happened, the queues are restarted. Otherwise,
 * reschedule for another attempt later.
 */
static void amdgpu_amdkfd_restore_userptr_worker(struct work_struct *work)
{
	struct delayed_work *dwork = to_delayed_work(work);
	struct amdkfd_process_info *process_info =
		container_of(dwork, struct amdkfd_process_info,
			     restore_userptr_work);
	struct task_struct *usertask;
	struct mm_struct *mm;
	int evicted_bos;

	evicted_bos = atomic_read(&process_info->evicted_bos);
	if (!evicted_bos)
		return;

	/* Reference task and mm in case of concurrent process termination */
	usertask = get_pid_task(process_info->pid, PIDTYPE_PID);
	if (!usertask)
		return;
	mm = get_task_mm(usertask);
	if (!mm) {
		put_task_struct(usertask);
		return;
	}

	mutex_lock(&process_info->lock);

	if (update_invalid_user_pages(process_info, mm))
		goto unlock_out;
	/* userptr_inval_list can be empty if all evicted userptr BOs
	 * have been freed. In that case there is nothing to validate
	 * and we can just restart the queues.
	 */
	if (!list_empty(&process_info->userptr_inval_list)) {
		if (atomic_read(&process_info->evicted_bos) != evicted_bos)
			goto unlock_out; /* Concurrent eviction, try again */

		if (validate_invalid_user_pages(process_info))
			goto unlock_out;
	}
	/* Final check for concurrent evicton and atomic update. If
	 * another eviction happens after successful update, it will
	 * be a first eviction that calls quiesce_mm. The eviction
	 * reference counting inside KFD will handle this case.
	 */
	if (atomic_cmpxchg(&process_info->evicted_bos, evicted_bos, 0) !=
	    evicted_bos)
		goto unlock_out;
	evicted_bos = 0;
	if (kgd2kfd_resume_mm(mm)) {
		pr_err("%s: Failed to resume KFD\n", __func__);
		/* No recovery from this failure. Probably the CP is
		 * hanging. No point trying again.
		 */
	}

unlock_out:
	mutex_unlock(&process_info->lock);
	mmput(mm);
	put_task_struct(usertask);

	/* If validation failed, reschedule another attempt */
	if (evicted_bos)
		schedule_delayed_work(&process_info->restore_userptr_work,
			msecs_to_jiffies(AMDGPU_USERPTR_RESTORE_DELAY_MS));
}

/** amdgpu_amdkfd_gpuvm_restore_process_bos - Restore all BOs for the given
 *   KFD process identified by process_info
 *
 * @process_info: amdkfd_process_info of the KFD process
 *
 * After memory eviction, restore thread calls this function. The function
 * should be called when the Process is still valid. BO restore involves -
 *
 * 1.  Release old eviction fence and create new one
 * 2.  Get two copies of PD BO list from all the VMs. Keep one copy as pd_list.
 * 3   Use the second PD list and kfd_bo_list to create a list (ctx.list) of
 *     BOs that need to be reserved.
 * 4.  Reserve all the BOs
 * 5.  Validate of PD and PT BOs.
 * 6.  Validate all KFD BOs using kfd_bo_list and Map them and add new fence
 * 7.  Add fence to all PD and PT BOs.
 * 8.  Unreserve all BOs
 */
int amdgpu_amdkfd_gpuvm_restore_process_bos(void *info, struct dma_fence **ef)
{
	struct amdgpu_bo_list_entry *pd_bo_list;
	struct amdkfd_process_info *process_info = info;
	struct amdgpu_vm *peer_vm;
	struct kgd_mem *mem;
	struct bo_vm_reservation_context ctx;
	struct amdgpu_amdkfd_fence *new_fence;
	int ret = 0, i;
	struct list_head duplicate_save;
	struct amdgpu_sync sync_obj;
	unsigned long failed_size = 0;
	unsigned long total_size = 0;

	INIT_LIST_HEAD(&duplicate_save);
	INIT_LIST_HEAD(&ctx.list);
	INIT_LIST_HEAD(&ctx.duplicates);

	pd_bo_list = kcalloc(process_info->n_vms,
			     sizeof(struct amdgpu_bo_list_entry),
			     GFP_KERNEL);
	if (!pd_bo_list)
		return -ENOMEM;

	i = 0;
	mutex_lock(&process_info->lock);
	list_for_each_entry(peer_vm, &process_info->vm_list_head,
			vm_list_node)
		amdgpu_vm_get_pd_bo(peer_vm, &ctx.list, &pd_bo_list[i++]);

	/* Reserve all BOs and page tables/directory. Add all BOs from
	 * kfd_bo_list to ctx.list
	 */
	list_for_each_entry(mem, &process_info->kfd_bo_list,
			    validate_list.head) {

		list_add_tail(&mem->resv_list.head, &ctx.list);
		mem->resv_list.bo = mem->validate_list.bo;
		mem->resv_list.num_shared = mem->validate_list.num_shared;
	}

	ret = ttm_eu_reserve_buffers(&ctx.ticket, &ctx.list,
				     false, &duplicate_save);
	if (ret) {
		pr_debug("Memory eviction: TTM Reserve Failed. Try again\n");
		goto ttm_reserve_fail;
	}

	amdgpu_sync_create(&sync_obj);

	/* Validate PDs and PTs */
	ret = process_validate_vms(process_info);
	if (ret)
		goto validate_map_fail;

	ret = process_sync_pds_resv(process_info, &sync_obj);
	if (ret) {
		pr_debug("Memory eviction: Failed to sync to PD BO moving fence. Try again\n");
		goto validate_map_fail;
	}

	/* Validate BOs and map them to GPUVM (update VM page tables). */
	list_for_each_entry(mem, &process_info->kfd_bo_list,
			    validate_list.head) {

		struct amdgpu_bo *bo = mem->bo;
		uint32_t domain = mem->domain;
		struct kfd_mem_attachment *attachment;

		total_size += amdgpu_bo_size(bo);

		ret = amdgpu_amdkfd_bo_validate(bo, domain, false);
		if (ret) {
			pr_debug("Memory eviction: Validate BOs failed\n");
			failed_size += amdgpu_bo_size(bo);
			ret = amdgpu_amdkfd_bo_validate(bo,
						AMDGPU_GEM_DOMAIN_GTT, false);
			if (ret) {
				pr_debug("Memory eviction: Try again\n");
				goto validate_map_fail;
			}
		}
		ret = amdgpu_sync_fence(&sync_obj, bo->tbo.moving);
		if (ret) {
			pr_debug("Memory eviction: Sync BO fence failed. Try again\n");
			goto validate_map_fail;
		}
		list_for_each_entry(attachment, &mem->attachments, list) {
			if (!attachment->is_mapped)
				continue;

			kfd_mem_dmaunmap_attachment(mem, attachment);
			ret = update_gpuvm_pte(mem, attachment, &sync_obj, NULL);
			if (ret) {
				pr_debug("Memory eviction: update PTE failed. Try again\n");
				goto validate_map_fail;
			}
		}
	}

	if (failed_size)
		pr_debug("0x%lx/0x%lx in system\n", failed_size, total_size);

	/* Update page directories */
	ret = process_update_pds(process_info, &sync_obj);
	if (ret) {
		pr_debug("Memory eviction: update PDs failed. Try again\n");
		goto validate_map_fail;
	}

	/* Wait for validate and PT updates to finish */
	amdgpu_sync_wait(&sync_obj, false);

	/* Release old eviction fence and create new one, because fence only
	 * goes from unsignaled to signaled, fence cannot be reused.
	 * Use context and mm from the old fence.
	 */
	new_fence = amdgpu_amdkfd_fence_create(
				process_info->eviction_fence->base.context,
				process_info->eviction_fence->mm,
				NULL);
	if (!new_fence) {
		pr_err("Failed to create eviction fence\n");
		ret = -ENOMEM;
		goto validate_map_fail;
	}
	dma_fence_put(&process_info->eviction_fence->base);
	process_info->eviction_fence = new_fence;
	*ef = dma_fence_get(&new_fence->base);

	/* Attach new eviction fence to all BOs */
	list_for_each_entry(mem, &process_info->kfd_bo_list,
		validate_list.head)
		amdgpu_bo_fence(mem->bo,
			&process_info->eviction_fence->base, true);

	/* Attach eviction fence to PD / PT BOs */
	list_for_each_entry(peer_vm, &process_info->vm_list_head,
			    vm_list_node) {
		struct amdgpu_bo *bo = peer_vm->root.bo;

		amdgpu_bo_fence(bo, &process_info->eviction_fence->base, true);
	}

validate_map_fail:
	ttm_eu_backoff_reservation(&ctx.ticket, &ctx.list);
	amdgpu_sync_free(&sync_obj);
ttm_reserve_fail:
	mutex_unlock(&process_info->lock);
	kfree(pd_bo_list);
	return ret;
}

int amdgpu_amdkfd_add_gws_to_process(void *info, void *gws, struct kgd_mem **mem)
{
	struct amdkfd_process_info *process_info = (struct amdkfd_process_info *)info;
	struct amdgpu_bo *gws_bo = (struct amdgpu_bo *)gws;
	int ret;

	if (!info || !gws)
		return -EINVAL;

	*mem = kzalloc(sizeof(struct kgd_mem), GFP_KERNEL);
	if (!*mem)
		return -ENOMEM;

	mutex_init(&(*mem)->lock);
	INIT_LIST_HEAD(&(*mem)->attachments);
	(*mem)->bo = amdgpu_bo_ref(gws_bo);
	(*mem)->domain = AMDGPU_GEM_DOMAIN_GWS;
	(*mem)->process_info = process_info;
	add_kgd_mem_to_kfd_bo_list(*mem, process_info, false);
	amdgpu_sync_create(&(*mem)->sync);


	/* Validate gws bo the first time it is added to process */
	mutex_lock(&(*mem)->process_info->lock);
	ret = amdgpu_bo_reserve(gws_bo, false);
	if (unlikely(ret)) {
		pr_err("Reserve gws bo failed %d\n", ret);
		goto bo_reservation_failure;
	}

	ret = amdgpu_amdkfd_bo_validate(gws_bo, AMDGPU_GEM_DOMAIN_GWS, true);
	if (ret) {
		pr_err("GWS BO validate failed %d\n", ret);
		goto bo_validation_failure;
	}
	/* GWS resource is shared b/t amdgpu and amdkfd
	 * Add process eviction fence to bo so they can
	 * evict each other.
	 */
	ret = dma_resv_reserve_shared(amdkcl_ttm_resvp(&gws_bo->tbo), 1);
	if (ret)
		goto reserve_shared_fail;
	amdgpu_bo_fence(gws_bo, &process_info->eviction_fence->base, true);
	amdgpu_bo_unreserve(gws_bo);
	mutex_unlock(&(*mem)->process_info->lock);

	return ret;

reserve_shared_fail:
bo_validation_failure:
	amdgpu_bo_unreserve(gws_bo);
bo_reservation_failure:
	mutex_unlock(&(*mem)->process_info->lock);
	amdgpu_sync_free(&(*mem)->sync);
	remove_kgd_mem_from_kfd_bo_list(*mem, process_info);
	amdgpu_bo_unref(&gws_bo);
	mutex_destroy(&(*mem)->lock);
	kfree(*mem);
	*mem = NULL;
	return ret;
}

int amdgpu_amdkfd_remove_gws_from_process(void *info, void *mem)
{
	int ret;
	struct amdkfd_process_info *process_info = (struct amdkfd_process_info *)info;
	struct kgd_mem *kgd_mem = (struct kgd_mem *)mem;
	struct amdgpu_bo *gws_bo = kgd_mem->bo;

	/* Remove BO from process's validate list so restore worker won't touch
	 * it anymore
	 */
	remove_kgd_mem_from_kfd_bo_list(kgd_mem, process_info);

	ret = amdgpu_bo_reserve(gws_bo, false);
	if (unlikely(ret)) {
		pr_err("Reserve gws bo failed %d\n", ret);
		//TODO add BO back to validate_list?
		return ret;
	}
	amdgpu_amdkfd_remove_eviction_fence(gws_bo,
			process_info->eviction_fence);
	amdgpu_bo_unreserve(gws_bo);
	amdgpu_sync_free(&kgd_mem->sync);
	amdgpu_bo_unref(&gws_bo);
	mutex_destroy(&kgd_mem->lock);
	kfree(mem);
	return 0;
}

int amdgpu_amdkfd_copy_mem_to_mem(struct amdgpu_device *adev, struct kgd_mem *src_mem,
				  uint64_t src_offset, struct kgd_mem *dst_mem,
				  uint64_t dst_offset, uint64_t size,
				  struct dma_fence **f, uint64_t *actual_size)
{
	struct amdgpu_copy_mem src, dst;
	struct ww_acquire_ctx ticket;
	struct list_head list, duplicates;
	struct ttm_validate_buffer resv_list[2];
	struct dma_fence *fence = NULL;
	int i, r;

	if (!adev|| !src_mem || !dst_mem || !actual_size)
		return -EINVAL;

	*actual_size = 0;

	INIT_LIST_HEAD(&list);
	INIT_LIST_HEAD(&duplicates);

	src.bo = &src_mem->bo->tbo;
	dst.bo = &dst_mem->bo->tbo;
	src.mem = src.bo->resource;
	dst.mem = dst.bo->resource;
	src.offset = src_offset;
	dst.offset = dst_offset;

	resv_list[0].bo = src.bo;
	resv_list[1].bo = dst.bo;

	for (i = 0; i < 2; i++) {
		resv_list[i].num_shared = 1;
		list_add_tail(&resv_list[i].head, &list);
	}

	r = ttm_eu_reserve_buffers(&ticket, &list, false, &duplicates);
	if (r) {
		pr_err("Copy buffer failed. Unable to reserve bo (%d)\n", r);
		return r;
	}

	/* The process to which the Source and Dest BOs belong to could be
	 * evicted and the BOs invalidated. So validate BOs before use
	 */
	r = amdgpu_amdkfd_bo_validate(src_mem->bo, src_mem->domain, false);
	if (r) {
		pr_err("CMA fail: SRC BO validate failed %d\n", r);
		goto validate_fail;
	}


	r = amdgpu_amdkfd_bo_validate(dst_mem->bo, dst_mem->domain, false);
	if (r) {
		pr_err("CMA fail: DST BO validate failed %d\n", r);
		goto validate_fail;
	}


	r = amdgpu_ttm_copy_mem_to_mem(adev, &src, &dst, size, false, NULL,
				       &fence);
	if (r)
		pr_err("Copy buffer failed %d\n", r);
	else
		*actual_size = size;
	if (fence) {
		amdgpu_bo_fence(src_mem->bo, fence, true);
		amdgpu_bo_fence(dst_mem->bo, fence, true);
	}
	if (f)
		*f = dma_fence_get(fence);
	dma_fence_put(fence);

validate_fail:
	ttm_eu_backoff_reservation(&ticket, &list);
	return r;
}

/* Returns GPU-specific tiling mode information */
int amdgpu_amdkfd_get_tile_config(struct amdgpu_device *adev,
				struct tile_config *config)
{
	config->gb_addr_config = adev->gfx.config.gb_addr_config;
	config->tile_config_ptr = adev->gfx.config.tile_mode_array;
	config->num_tile_configs =
			ARRAY_SIZE(adev->gfx.config.tile_mode_array);
	config->macro_tile_config_ptr =
			adev->gfx.config.macrotile_mode_array;
	config->num_macro_tile_configs =
			ARRAY_SIZE(adev->gfx.config.macrotile_mode_array);

	/* Those values are not set from GFX9 onwards */
	config->num_banks = adev->gfx.config.num_banks;
	config->num_ranks = adev->gfx.config.num_ranks;

	return 0;
}

bool amdgpu_amdkfd_bo_mapped_to_dev(struct amdgpu_device *adev, struct kgd_mem *mem)
{
	struct kfd_mem_attachment *entry;

	list_for_each_entry(entry, &mem->attachments, list) {
		if (entry->is_mapped && entry->adev == adev)
			return true;
	}
	return false;
}
