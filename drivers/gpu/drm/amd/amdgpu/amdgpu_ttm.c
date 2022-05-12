/*
 * Copyright 2009 Jerome Glisse.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 */
/*
 * Authors:
 *    Jerome Glisse <glisse@freedesktop.org>
 *    Thomas Hellstrom <thomas-at-tungstengraphics-dot-com>
 *    Dave Airlie
 */

#include <linux/dma-mapping.h>
#include <linux/iommu.h>
#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
#include <linux/hmm.h>
#endif
#include <linux/pagemap.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include <linux/swiotlb.h>
#include <linux/dma-buf.h>
#include <linux/sizes.h>

#include <drm/drm_drv.h>
#include <drm/ttm/ttm_bo_api.h>
#include <drm/ttm/ttm_bo_driver.h>
#include <drm/ttm/ttm_placement.h>
#include <drm/ttm/ttm_range_manager.h>

#include <drm/amdgpu_drm.h>
#include <drm/drm_drv.h>

#include "amdgpu.h"
#include "amdgpu_object.h"
#include "amdgpu_trace.h"
#include "amdgpu_amdkfd.h"
#include "amdgpu_sdma.h"
#include "amdgpu_ras.h"
#include "amdgpu_atomfirmware.h"
#include "amdgpu_res_cursor.h"
#include "bif/bif_4_1_d.h"

#define AMDGPU_TTM_VRAM_MAX_DW_READ	(size_t)128

struct amdgpu_dgma_node {
	struct ttm_buffer_object *tbo;
	struct ttm_range_mgr_node base;
};

static int amdgpu_ttm_backend_bind(struct ttm_device *bdev,
				   struct ttm_tt *ttm,
				   struct ttm_resource *bo_mem);
static void amdgpu_ttm_backend_unbind(struct ttm_device *bdev,
				      struct ttm_tt *ttm);

static int amdgpu_ttm_init_on_chip(struct amdgpu_device *adev,
				    unsigned int type,
				    uint64_t size_in_page)
{
	return ttm_range_man_init(&adev->mman.bdev, type,
				  false, size_in_page);
}

/**
 * amdgpu_evict_flags - Compute placement flags
 *
 * @bo: The buffer object to evict
 * @placement: Possible destination(s) for evicted BO
 *
 * Fill in placement data when ttm_bo_evict() is called
 */
static void amdgpu_evict_flags(struct ttm_buffer_object *bo,
				struct ttm_placement *placement)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->bdev);
	struct amdgpu_bo *abo;
	static const struct ttm_place placements = {
		.fpfn = 0,
		.lpfn = 0,
		.mem_type = TTM_PL_SYSTEM,
		.flags = 0
	};

	/* Don't handle scatter gather BOs */
	if (bo->type == ttm_bo_type_sg) {
		placement->num_placement = 0;
		placement->num_busy_placement = 0;
		return;
	}

	/* Object isn't an AMDGPU object so ignore */
	if (!amdgpu_bo_is_amdgpu_bo(bo)) {
		placement->placement = &placements;
		placement->busy_placement = &placements;
		placement->num_placement = 1;
		placement->num_busy_placement = 1;
		return;
	}

	abo = ttm_to_amdgpu_bo(bo);
	if (abo->flags & AMDGPU_AMDKFD_CREATE_SVM_BO) {
		struct dma_fence *fence;
		struct dma_resv *resv = &(amdkcl_ttm_resv(bo));

		rcu_read_lock();
		fence = rcu_dereference(resv->fence_excl);
		if (fence && !fence->ops->signaled)
			dma_fence_enable_sw_signaling(fence);

		placement->num_placement = 0;
		placement->num_busy_placement = 0;
		rcu_read_unlock();
		return;
	}

	switch (bo->resource->mem_type) {
	case AMDGPU_PL_GDS:
	case AMDGPU_PL_GWS:
	case AMDGPU_PL_OA:
		placement->num_placement = 0;
		placement->num_busy_placement = 0;
		return;

	case TTM_PL_VRAM:
	case AMDGPU_PL_DGMA:
		if (!adev->mman.buffer_funcs_enabled) {
			/* Move to system memory */
			amdgpu_bo_placement_from_domain(abo, AMDGPU_GEM_DOMAIN_CPU);
		} else if (!amdgpu_gmc_vram_full_visible(&adev->gmc) &&
			   !(abo->flags & AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED) &&
			   amdgpu_bo_in_cpu_visible_vram(abo)) {

			/* Try evicting to the CPU inaccessible part of VRAM
			 * first, but only set GTT as busy placement, so this
			 * BO will be evicted to GTT rather than causing other
			 * BOs to be evicted from VRAM
			 */
			amdgpu_bo_placement_from_domain(abo, AMDGPU_GEM_DOMAIN_VRAM |
							 AMDGPU_GEM_DOMAIN_GTT);
			abo->placements[0].fpfn = adev->gmc.visible_vram_size >> PAGE_SHIFT;
			abo->placements[0].lpfn = 0;
			abo->placement.busy_placement = &abo->placements[1];
			abo->placement.num_busy_placement = 1;
		} else {
			/* Move to GTT memory */
			amdgpu_bo_placement_from_domain(abo, AMDGPU_GEM_DOMAIN_GTT);
		}
		break;
	case TTM_PL_TT:
	case AMDGPU_PL_DGMA_IMPORT:
	case AMDGPU_PL_PREEMPT:
	default:
		amdgpu_bo_placement_from_domain(abo, AMDGPU_GEM_DOMAIN_CPU);
		break;
	}
	*placement = abo->placement;
}

#ifdef HAVE_STRUCT_DRM_DRV_GEM_OPEN_OBJECT_CALLBACK
/**
 * amdgpu_verify_access - Verify access for a mmap call
 *
 * @bo: The buffer object to map
 * @filp: The file pointer from the process performing the mmap
 *
 * This is called by ttm_bo_mmap() to verify whether a process
 * has the right to mmap a BO to their process space.
 */
static int amdgpu_verify_access(struct ttm_buffer_object *bo, struct file *filp)
{
        struct amdgpu_bo *abo = ttm_to_amdgpu_bo(bo);

        if (amdgpu_ttm_tt_get_usermm(bo->ttm))
                return -EPERM;
        return drm_vma_node_verify_access(&abo->tbo.base.vma_node,
                                          filp->private_data);
}
#endif

/**
 * amdgpu_ttm_map_buffer - Map memory into the GART windows
 * @bo: buffer object to map
 * @mem: memory object to map
 * @mm_cur: range to map
 * @window: which GART window to use
 * @ring: DMA ring to use for the copy
 * @tmz: if we should setup a TMZ enabled mapping
 * @size: in number of bytes to map, out number of bytes mapped
 * @addr: resulting address inside the MC address space
 *
 * Setup one of the GART windows to access a specific piece of memory or return
 * the physical address for local memory.
 */
static int amdgpu_ttm_map_buffer(struct ttm_buffer_object *bo,
				 struct ttm_resource *mem,
				 struct amdgpu_res_cursor *mm_cur,
				 unsigned window, struct amdgpu_ring *ring,
				 bool tmz, uint64_t *size, uint64_t *addr)
{
	struct amdgpu_device *adev = ring->adev;
	unsigned offset, num_pages, num_dw, num_bytes;
	uint64_t src_addr, dst_addr;
	struct dma_fence *fence;
	struct amdgpu_job *job;
	void *cpu_addr;
	uint64_t flags;
	unsigned int i;
	int r;

	BUG_ON(adev->mman.buffer_funcs->copy_max_bytes <
	       AMDGPU_GTT_MAX_TRANSFER_SIZE * 8);

	if (WARN_ON(mem->mem_type == AMDGPU_PL_PREEMPT))
		return -EINVAL;

	/* Map only what can't be accessed directly */
	if (!tmz && mem->start != AMDGPU_BO_INVALID_OFFSET) {
		*addr = amdgpu_ttm_domain_start(adev, mem->mem_type) +
			mm_cur->start;
		return 0;
	}


	/*
	 * If start begins at an offset inside the page, then adjust the size
	 * and addr accordingly
	 */
	offset = mm_cur->start & ~PAGE_MASK;

	num_pages = PFN_UP(*size + offset);
	num_pages = min_t(uint32_t, num_pages, AMDGPU_GTT_MAX_TRANSFER_SIZE);

	*size = min(*size, (uint64_t)num_pages * PAGE_SIZE - offset);

	*addr = adev->gmc.gart_start;
	*addr += (u64)window * AMDGPU_GTT_MAX_TRANSFER_SIZE *
		AMDGPU_GPU_PAGE_SIZE;
	*addr += offset;

	num_dw = ALIGN(adev->mman.buffer_funcs->copy_num_dw, 8);
	num_bytes = num_pages * 8 * AMDGPU_GPU_PAGES_IN_CPU_PAGE;

	r = amdgpu_job_alloc_with_ib(adev, num_dw * 4 + num_bytes,
				     AMDGPU_IB_POOL_DELAYED, &job);
	if (r)
		return r;

	src_addr = num_dw * 4;
	src_addr += job->ibs[0].gpu_addr;

	dst_addr = amdgpu_bo_gpu_offset(adev->gart.bo);
	dst_addr += window * AMDGPU_GTT_MAX_TRANSFER_SIZE * 8;
	amdgpu_emit_copy_buffer(adev, &job->ibs[0], src_addr,
				dst_addr, num_bytes, false);

	amdgpu_ring_pad_ib(ring, &job->ibs[0]);
	WARN_ON(job->ibs[0].length_dw > num_dw);

	flags = amdgpu_ttm_tt_pte_flags(adev, bo->ttm, mem);
	if (tmz)
		flags |= AMDGPU_PTE_TMZ;

	cpu_addr = &job->ibs[0].ptr[num_dw];

	if (mem->mem_type == TTM_PL_TT) {
		dma_addr_t *dma_addr;

		dma_addr = &bo->ttm->dma_address[mm_cur->start >> PAGE_SHIFT];
		amdgpu_gart_map(adev, 0, num_pages, dma_addr, flags, cpu_addr);
	} else {
		dma_addr_t dma_address;

		dma_address = mm_cur->start;
		dma_address += adev->vm_manager.vram_base_offset;

		for (i = 0; i < num_pages; ++i) {
			amdgpu_gart_map(adev, i << PAGE_SHIFT, 1, &dma_address,
					flags, cpu_addr);
			dma_address += PAGE_SIZE;
		}
	}

	r = amdgpu_job_submit(job, &adev->mman.entity,
			      AMDGPU_FENCE_OWNER_UNDEFINED, &fence);
	if (r)
		goto error_free;

	dma_fence_put(fence);

	return r;

error_free:
	amdgpu_job_free(job);
	return r;
}

/**
 * amdgpu_ttm_copy_mem_to_mem - Helper function for copy
 * @adev: amdgpu device
 * @src: buffer/address where to read from
 * @dst: buffer/address where to write to
 * @size: number of bytes to copy
 * @tmz: if a secure copy should be used
 * @resv: resv object to sync to
 * @f: Returns the last fence if multiple jobs are submitted.
 *
 * The function copies @size bytes from {src->mem + src->offset} to
 * {dst->mem + dst->offset}. src->bo and dst->bo could be same BO for a
 * move and different for a BO to BO copy.
 *
 */
int amdgpu_ttm_copy_mem_to_mem(struct amdgpu_device *adev,
			       const struct amdgpu_copy_mem *src,
			       const struct amdgpu_copy_mem *dst,
			       uint64_t size, bool tmz,
			       struct dma_resv *resv,
			       struct dma_fence **f)
{
	struct amdgpu_ring *ring = adev->mman.buffer_funcs_ring;
	struct amdgpu_res_cursor src_mm, dst_mm;
	struct dma_fence *fence = NULL;
	int r = 0;

	if (!adev->mman.buffer_funcs_enabled) {
		DRM_ERROR("Trying to move memory with ring turned off.\n");
		return -EINVAL;
	}

	amdgpu_res_first(src->mem, src->offset, size, &src_mm);
	amdgpu_res_first(dst->mem, dst->offset, size, &dst_mm);

	mutex_lock(&adev->mman.gtt_window_lock);
	while (src_mm.remaining) {
		uint64_t from, to, cur_size;
		struct dma_fence *next;

		/* Never copy more than 256MiB at once to avoid a timeout */
		cur_size = min3(src_mm.size, dst_mm.size, 256ULL << 20);

		/* Map src to window 0 and dst to window 1. */
		r = amdgpu_ttm_map_buffer(src->bo, src->mem, &src_mm,
					  0, ring, tmz, &cur_size, &from);
		if (r)
			goto error;

		r = amdgpu_ttm_map_buffer(dst->bo, dst->mem, &dst_mm,
					  1, ring, tmz, &cur_size, &to);
		if (r)
			goto error;

		r = amdgpu_copy_buffer(ring, from, to, cur_size,
				       resv, &next, false, true, tmz);
		if (r)
			goto error;

		dma_fence_put(fence);
		fence = next;

		amdgpu_res_next(&src_mm, cur_size);
		amdgpu_res_next(&dst_mm, cur_size);
	}
error:
	mutex_unlock(&adev->mman.gtt_window_lock);
	if (f)
		*f = dma_fence_get(fence);
	dma_fence_put(fence);
	return r;
}

/*
 * amdgpu_move_blit - Copy an entire buffer to another buffer
 *
 * This is a helper called by amdgpu_bo_move() and amdgpu_move_vram_ram() to
 * help move buffers to and from VRAM.
 */
static int amdgpu_move_blit(struct ttm_buffer_object *bo,
			    bool evict,
			    struct ttm_resource *new_mem,
			    struct ttm_resource *old_mem)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->bdev);
	struct amdgpu_bo *abo = ttm_to_amdgpu_bo(bo);
	struct amdgpu_copy_mem src, dst;
	struct dma_fence *fence = NULL;
	int r;

	src.bo = bo;
	dst.bo = bo;
	src.mem = old_mem;
	dst.mem = new_mem;
	src.offset = 0;
	dst.offset = 0;

	r = amdgpu_ttm_copy_mem_to_mem(adev, &src, &dst,
				       new_mem->num_pages << PAGE_SHIFT,
				       amdgpu_bo_encrypted(abo),
				       amdkcl_ttm_resvp(bo), &fence);
	if (r)
		goto error;

	/* clear the space being freed */
	if (old_mem->mem_type == TTM_PL_VRAM &&
	    (abo->flags & AMDGPU_GEM_CREATE_VRAM_WIPE_ON_RELEASE)) {
		struct dma_fence *wipe_fence = NULL;

		r = amdgpu_fill_buffer(abo, AMDGPU_POISON, NULL, &wipe_fence);
		if (r) {
			goto error;
		} else if (wipe_fence) {
			dma_fence_put(fence);
			fence = wipe_fence;
		}
	}

	/* Always block for VM page tables before committing the new location */
	if (bo->type == ttm_bo_type_kernel)
		r = ttm_bo_move_accel_cleanup(bo, fence, true, false, new_mem);
	else
		r = ttm_bo_move_accel_cleanup(bo, fence, evict, true, new_mem);
	dma_fence_put(fence);
	return r;

error:
	if (fence)
		dma_fence_wait(fence, false);
	dma_fence_put(fence);
	return r;
}

/*
 * amdgpu_mem_visible - Check that memory can be accessed by ttm_bo_move_memcpy
 *
 * Called by amdgpu_bo_move()
 */
static bool amdgpu_mem_visible(struct amdgpu_device *adev,
			       struct ttm_resource *mem)
{
	uint64_t mem_size = (u64)mem->num_pages << PAGE_SHIFT;
	struct amdgpu_res_cursor cursor;

	if (mem->mem_type == TTM_PL_SYSTEM ||
	    mem->mem_type == TTM_PL_TT)
		return true;
	if (mem->mem_type != TTM_PL_VRAM)
		return false;

	amdgpu_res_first(mem, 0, mem_size, &cursor);

	/* ttm_resource_ioremap only supports contiguous memory */
	if (cursor.size != mem_size)
		return false;

	return cursor.start + cursor.size <= adev->gmc.visible_vram_size;
}

/*
 * amdgpu_bo_move - Move a buffer object to a new memory location
 *
 * Called by ttm_bo_handle_move_mem()
 */
static int amdgpu_bo_move(struct ttm_buffer_object *bo, bool evict,
			  struct ttm_operation_ctx *ctx,
			  struct ttm_resource *new_mem,
			  struct ttm_place *hop)
{
	struct amdgpu_device *adev;
	struct amdgpu_bo *abo;
	struct ttm_resource *old_mem = bo->resource;
	int r;

	if (new_mem->mem_type == TTM_PL_TT ||
	    new_mem->mem_type == AMDGPU_PL_PREEMPT) {
		r = amdgpu_ttm_backend_bind(bo->bdev, bo->ttm, new_mem);
		if (r)
			return r;
	}

	/* Can't move a pinned BO */
	abo = ttm_to_amdgpu_bo(bo);
	if (WARN_ON_ONCE(abo->tbo.pin_count > 0))
		return -EINVAL;

	if (old_mem->mem_type == AMDGPU_GEM_DOMAIN_DGMA ||
	    old_mem->mem_type == AMDGPU_GEM_DOMAIN_DGMA_IMPORT)
		return -EINVAL;

	adev = amdgpu_ttm_adev(bo->bdev);

	if (old_mem->mem_type == TTM_PL_SYSTEM && bo->ttm == NULL) {
		ttm_bo_move_null(bo, new_mem);
		goto out;
	}
	if (old_mem->mem_type == TTM_PL_SYSTEM &&
	    (new_mem->mem_type == TTM_PL_TT ||
	     new_mem->mem_type == AMDGPU_PL_PREEMPT)) {
		ttm_bo_move_null(bo, new_mem);
		goto out;
	}
	if ((old_mem->mem_type == TTM_PL_TT ||
	     old_mem->mem_type == AMDGPU_PL_PREEMPT) &&
	    new_mem->mem_type == TTM_PL_SYSTEM) {
		r = ttm_bo_wait_ctx(bo, ctx);
		if (r)
			return r;

		amdgpu_ttm_backend_unbind(bo->bdev, bo->ttm);
		ttm_resource_free(bo, &bo->resource);
		ttm_bo_assign_mem(bo, new_mem);
		goto out;
	}

	if (old_mem->mem_type == AMDGPU_PL_GDS ||
	    old_mem->mem_type == AMDGPU_PL_GWS ||
	    old_mem->mem_type == AMDGPU_PL_OA ||
	    new_mem->mem_type == AMDGPU_PL_GDS ||
	    new_mem->mem_type == AMDGPU_PL_GWS ||
	    new_mem->mem_type == AMDGPU_PL_OA) {
		/* Nothing to save here */
		ttm_bo_move_null(bo, new_mem);
		goto out;
	}

	if (bo->type == ttm_bo_type_device &&
	    new_mem->mem_type == TTM_PL_VRAM &&
	    old_mem->mem_type != TTM_PL_VRAM) {
		/* amdgpu_bo_fault_reserve_notify will re-set this if the CPU
		 * accesses the BO after it's moved.
		 */
		abo->flags &= ~AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED;
	}

	if (adev->mman.buffer_funcs_enabled) {
		if (((old_mem->mem_type == TTM_PL_SYSTEM &&
		      new_mem->mem_type == TTM_PL_VRAM) ||
		     (old_mem->mem_type == TTM_PL_VRAM &&
		      new_mem->mem_type == TTM_PL_SYSTEM))) {
			hop->fpfn = 0;
			hop->lpfn = 0;
			hop->mem_type = TTM_PL_TT;
			hop->flags = 0;
			return -EMULTIHOP;
		}

		r = amdgpu_move_blit(bo, evict, new_mem, old_mem);
	} else {
		r = -ENODEV;
	}

	if (r) {
		/* Check that all memory is CPU accessible */
		if (!amdgpu_mem_visible(adev, old_mem) ||
		    !amdgpu_mem_visible(adev, new_mem)) {
			pr_err("Move buffer fallback to memcpy unavailable\n");
			return r;
		}

		r = ttm_bo_move_memcpy(bo, ctx, new_mem);
		if (r)
			return r;
	}

out:
	/* update statistics */
	atomic64_add(bo->base.size, &adev->num_bytes_moved);
	amdgpu_bo_move_notify(bo, evict, new_mem);
	return 0;
}

/*
 * amdgpu_ttm_io_mem_reserve - Reserve a block of memory during a fault
 *
 * Called by ttm_mem_io_reserve() ultimately via ttm_bo_vm_fault()
 */
static int amdgpu_ttm_io_mem_reserve(struct ttm_device *bdev,
				     struct ttm_resource *mem)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bdev);
	size_t bus_size = (size_t)mem->num_pages << PAGE_SHIFT;

	switch (mem->mem_type) {
	case TTM_PL_SYSTEM:
		/* system memory */
		return 0;
	case TTM_PL_TT:
	case AMDGPU_PL_PREEMPT:
		break;
	case TTM_PL_VRAM:
	case AMDGPU_PL_DGMA:
		mem->bus.offset = (mem->start << PAGE_SHIFT) +
				amdgpu_ttm_domain_start(adev, mem->mem_type) -
				amdgpu_ttm_domain_start(adev, TTM_PL_VRAM);
		/* check if it's visible */
		if ((mem->bus.offset + bus_size) > adev->gmc.visible_vram_size)
			return -EINVAL;

		if (adev->mman.aper_base_kaddr &&
		    mem->placement & TTM_PL_FLAG_CONTIGUOUS)
			mem->bus.addr = (u8 *)adev->mman.aper_base_kaddr +
					mem->bus.offset;

		mem->bus.offset += adev->gmc.aper_base;
		mem->bus.is_iomem = true;
		break;
	case AMDGPU_PL_DGMA_IMPORT:
	{
		struct amdgpu_dgma_node *node;
		struct amdgpu_bo *abo;

		node = container_of(mem, struct amdgpu_dgma_node, base.base);
		abo = ttm_to_amdgpu_bo(node->tbo);
		mem->bus.addr = abo->dgma_addr;
		mem->bus.offset = abo->dgma_import_base;
		mem->bus.is_iomem = true;
		mem->bus.caching = ttm_write_combined;
		break;
	}
	default:
		return -EINVAL;
	}
	return 0;
}

static unsigned long amdgpu_ttm_io_mem_pfn(struct ttm_buffer_object *bo,
					   unsigned long page_offset)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->bdev);
	struct amdgpu_res_cursor cursor;

	if (bo->resource->mem_type == AMDGPU_PL_DGMA ||
			bo->resource->mem_type == AMDGPU_PL_DGMA_IMPORT) {
		return (bo->resource->bus.offset >> PAGE_SHIFT) + page_offset;
	} else {
                amdgpu_res_first(bo->resource, (u64)page_offset << PAGE_SHIFT, 0, &cursor);
                return (adev->gmc.aper_base + cursor.start) >> PAGE_SHIFT;
	}
}

/**
 * amdgpu_ttm_domain_start - Returns GPU start address
 * @adev: amdgpu device object
 * @type: type of the memory
 *
 * Returns:
 * GPU start address of a memory domain
 */

uint64_t amdgpu_ttm_domain_start(struct amdgpu_device *adev, uint32_t type)
{
	switch (type) {
	case TTM_PL_TT:
		return adev->gmc.gart_start;
	case TTM_PL_VRAM:
		return adev->gmc.vram_start;
	case AMDGPU_PL_DGMA:
		if (adev->direct_gma.dgma_bo)
			return amdgpu_bo_gpu_offset(adev->direct_gma.dgma_bo);
		fallthrough;
	case AMDGPU_PL_DGMA_IMPORT:
		return 0;
	}

	return 0;
}

/*
 * TTM backend functions.
 */
#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
struct amdgpu_ttm_gup_task_list {
	struct list_head	list;
	struct task_struct	*task;
};
#endif

struct amdgpu_ttm_tt {
	struct ttm_tt	ttm;
	struct drm_gem_object	*gobj;
	u64			offset;
	uint64_t		userptr;
	struct task_struct	*usertask;
	uint32_t		userflags;
	bool			bound;
#if IS_ENABLED(CONFIG_DRM_AMDGPU_USERPTR)
#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	struct hmm_range	*range;
#endif
#endif

#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	spinlock_t		guptasklock;
	struct list_head	guptasks;
	atomic_t		mmu_invalidations;
	uint32_t		last_set_pages;
#endif /* HAVE_AMDKCL_HMM_MIRROR_ENABLED */
};

#ifdef CONFIG_DRM_AMDGPU_USERPTR
#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
/*
 * amdgpu_ttm_tt_get_user_pages - get device accessible pages that back user
 * memory and start HMM tracking CPU page table update
 *
 * Calling function must call amdgpu_ttm_tt_userptr_range_done() once and only
 * once afterwards to stop HMM tracking
 */
int amdgpu_ttm_tt_get_user_pages(struct amdgpu_bo *bo, struct page **pages)
{
	struct ttm_tt *ttm = bo->tbo.ttm;
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	unsigned long start = gtt->userptr;
	struct vm_area_struct *vma;
	struct mm_struct *mm;
	bool readonly;
	int r = 0;

	mm = bo->notifier.mm;
	if (unlikely(!mm)) {
		DRM_DEBUG_DRIVER("BO is not registered?\n");
		return -EFAULT;
	}

	/* Another get_user_pages is running at the same time?? */
	if (WARN_ON(gtt->range))
		return -EFAULT;

	if (!mmget_not_zero(mm)) /* Happens during process shutdown */
		return -ESRCH;

	mmap_read_lock(mm);
	vma = find_vma(mm, start);
	mmap_read_unlock(mm);
	if (unlikely(!vma || start < vma->vm_start)) {
		r = -EFAULT;
		goto out_putmm;
	}
	if (unlikely((gtt->userflags & AMDGPU_GEM_USERPTR_ANONONLY) &&
		vma->vm_file)) {
		r = -EPERM;
		goto out_putmm;
	}

	readonly = amdgpu_ttm_tt_is_readonly(ttm);
	r = amdgpu_hmm_range_get_pages(&bo->notifier, mm, pages, start,
				       ttm->num_pages, &gtt->range, readonly,
				       false, NULL);
	if (r)
		pr_debug("failed %d to get user pages 0x%lx\n", r, start);

out_putmm:
	mmput(mm);

	return r;
}

/*
 * amdgpu_ttm_tt_userptr_range_done - stop HMM track the CPU page table change
 * Check if the pages backing this ttm range have been invalidated
 *
 * Returns: true if pages are still valid
 */
bool amdgpu_ttm_tt_get_user_pages_done(struct ttm_tt *ttm)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	bool r = false;

	if (!gtt || !gtt->userptr)
		return false;

	DRM_DEBUG_DRIVER("user_pages_done 0x%llx pages 0x%x\n",
		gtt->userptr, ttm->num_pages);

#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
	WARN_ONCE(!gtt->range || !gtt->range->pfns,
#else
	WARN_ONCE(!gtt->range || !gtt->range->hmm_pfns,
#endif
		"No user pages to check\n");

	if (gtt->range) {
		/*
		 * FIXME: Must always hold notifier_lock for this, and must
		 * not ignore the return code.
		 */
		r = amdgpu_hmm_range_get_pages_done(gtt->range);
		gtt->range = NULL;
	}

	return !r;
}

#else
/*
 * amdgpu_ttm_tt_get_user_pages - Pin pages of memory pointed to by a USERPTR
 * pointer to memory
 *
 * Called by amdgpu_gem_userptr_ioctl() and amdgpu_cs_parser_bos().
 * This provides a wrapper around the get_user_pages() call to provide
 * device accessible pages that back user memory.
 */
int amdgpu_ttm_tt_get_user_pages(struct amdgpu_bo *bo, struct page **pages)
{
	struct ttm_tt *ttm = bo->tbo.ttm;
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	struct mm_struct *mm = gtt->usertask->mm;
	unsigned int flags = 0;
	unsigned pinned = 0;
	int r;

	if (!mm) /* Happens during process shutdown */
		return -ESRCH;

	if (!(gtt->userflags & AMDGPU_GEM_USERPTR_READONLY))
		flags |= FOLL_WRITE;

	down_read(&mm->mmap_sem);

	if (gtt->userflags & AMDGPU_GEM_USERPTR_ANONONLY) {
		/*
		 * check that we only use anonymous memory to prevent problems
		 * with writeback
		 */
		unsigned long end = gtt->userptr + ttm->num_pages * PAGE_SIZE;
		struct vm_area_struct *vma;

		vma = find_vma(mm, gtt->userptr);
		if (!vma || vma->vm_file || vma->vm_end < end) {
			up_read(&mm->mmap_sem);
			return -EPERM;
		}
	}

	/* loop enough times using contiguous pages of memory */
	do {
		unsigned num_pages = ttm->num_pages - pinned;
		uint64_t userptr = gtt->userptr + pinned * PAGE_SIZE;
		struct page **p = pages + pinned;
		struct amdgpu_ttm_gup_task_list guptask;

		guptask.task = current;
		spin_lock(&gtt->guptasklock);
		list_add(&guptask.list, &gtt->guptasks);
		spin_unlock(&gtt->guptasklock);

		if (mm == current->mm)
			r = get_user_pages(userptr, num_pages, flags, p, NULL);
		else
			r = kcl_get_user_pages_remote(gtt->usertask,
					mm, userptr, num_pages,
					flags, p, NULL, NULL);

		spin_lock(&gtt->guptasklock);
		list_del(&guptask.list);
		spin_unlock(&gtt->guptasklock);

		if (r < 0)
			goto release_pages;

		pinned += r;

	} while (pinned < ttm->num_pages);

	up_read(&mm->mmap_sem);
	return 0;

release_pages:
	release_pages(pages, pinned);
	up_read(&mm->mmap_sem);
	return r;
}
#endif /* HAVE_AMDKCL_HMM_MIRROR_ENABLED */
#endif /* CONFIG_DRM_AMDGPU_USERPTR */

#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
/*
 * amdgpu_ttm_tt_set_user_pages - Copy pages in, putting old pages as necessary.
 *
 * Called by amdgpu_cs_list_validate(). This creates the page list
 * that backs user memory and will ultimately be mapped into the device
 * address space.
 */
void amdgpu_ttm_tt_set_user_pages(struct ttm_tt *ttm, struct page **pages)
{
	unsigned long i;

	for (i = 0; i < ttm->num_pages; ++i)
		ttm->pages[i] = pages ? pages[i] : NULL;
}

#else
/**
 * amdgpu_ttm_tt_set_user_pages - Copy pages in, putting old pages as necessary.
 *
 * Called by amdgpu_cs_list_validate(). This creates the page list
 * that backs user memory and will ultimately be mapped into the device
 * address space.
 */
void amdgpu_ttm_tt_set_user_pages(struct ttm_tt *ttm, struct page **pages)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	unsigned i;

	gtt->last_set_pages = atomic_read(&gtt->mmu_invalidations);
	for (i = 0; i < ttm->num_pages; ++i) {
		if (ttm->pages[i])
			put_page(ttm->pages[i]);

		ttm->pages[i] = pages ? pages[i] : NULL;
	}
}

/**
 * amdgpu_ttm_tt_mark_user_page - Mark pages as dirty
 *
 * Called while unpinning userptr pages
 */
void amdgpu_ttm_tt_mark_user_pages(struct ttm_tt *ttm)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	unsigned i;

	for (i = 0; i < ttm->num_pages; ++i) {
		struct page *page = ttm->pages[i];

		if (!page)
			continue;

		if (!(gtt->userflags & AMDGPU_GEM_USERPTR_READONLY))
			set_page_dirty(page);

		mark_page_accessed(page);
	}
}
#endif

/*
 * amdgpu_ttm_tt_pin_userptr - prepare the sg table with the user pages
 *
 * Called by amdgpu_ttm_backend_bind()
 **/
static int amdgpu_ttm_tt_pin_userptr(struct ttm_device *bdev,
				     struct ttm_tt *ttm)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bdev);
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	int write = !(gtt->userflags & AMDGPU_GEM_USERPTR_READONLY);
	enum dma_data_direction direction = write ?
		DMA_BIDIRECTIONAL : DMA_TO_DEVICE;
	int r;

	/* Allocate an SG array and squash pages into it */
	r = sg_alloc_table_from_pages(ttm->sg, ttm->pages, ttm->num_pages, 0,
				      (u64)ttm->num_pages << PAGE_SHIFT,
				      GFP_KERNEL);
	if (r)
		goto release_sg;

	/* Map SG to device */
	r = dma_map_sgtable(adev->dev, ttm->sg, direction, 0);
	if (r)
		goto release_sg;

	/* convert SG to linear array of pages and dma addresses */
	drm_prime_sg_to_dma_addr_array(ttm->sg, gtt->ttm.dma_address,
				       ttm->num_pages);

	return 0;

release_sg:
	kfree(ttm->sg);
	ttm->sg = NULL;
	return r;
}

/*
 * amdgpu_ttm_tt_unpin_userptr - Unpin and unmap userptr pages
 */
static void amdgpu_ttm_tt_unpin_userptr(struct ttm_device *bdev,
					struct ttm_tt *ttm)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bdev);
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	int write = !(gtt->userflags & AMDGPU_GEM_USERPTR_READONLY);
	enum dma_data_direction direction = write ?
		DMA_BIDIRECTIONAL : DMA_TO_DEVICE;

	/* double check that we don't free the table twice */
	if (!ttm->sg || !ttm->sg->sgl)
		return;

	/* unmap the pages mapped to the device */
	dma_unmap_sgtable(adev->dev, ttm->sg, direction, 0);

#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	sg_free_table(ttm->sg);

#if IS_ENABLED(CONFIG_DRM_AMDGPU_USERPTR)
	if (gtt->range) {
		unsigned long i;

		for (i = 0; i < ttm->num_pages; i++) {
			if (ttm->pages[i] !=
#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
				hmm_device_entry_to_page(gtt->range,
					      gtt->range->pfns[i]))
#else
			    hmm_pfn_to_page(gtt->range->hmm_pfns[i]))
#endif
				break;
		}

		WARN((i == ttm->num_pages), "Missing get_user_page_done\n");
	}
#endif
#else
	/* mark the pages as dirty */
	amdgpu_ttm_tt_mark_user_pages(ttm);

	sg_free_table(ttm->sg);
#endif
}

static void amdgpu_ttm_gart_bind(struct amdgpu_device *adev,
				 struct ttm_buffer_object *tbo,
				 uint64_t flags)
{
	struct amdgpu_bo *abo = ttm_to_amdgpu_bo(tbo);
	struct ttm_tt *ttm = tbo->ttm;
	struct amdgpu_ttm_tt *gtt = (void *)ttm;

	if (amdgpu_bo_encrypted(abo))
		flags |= AMDGPU_PTE_TMZ;

	if (abo->flags & AMDGPU_GEM_CREATE_CP_MQD_GFX9) {
		uint64_t page_idx = 1;

		amdgpu_gart_bind(adev, gtt->offset, page_idx,
				 gtt->ttm.dma_address, flags);

		/* The memory type of the first page defaults to UC. Now
		 * modify the memory type to NC from the second page of
		 * the BO onward.
		 */
		flags &= ~AMDGPU_PTE_MTYPE_VG10_MASK;
		flags |= AMDGPU_PTE_MTYPE_VG10(AMDGPU_MTYPE_NC);

		amdgpu_gart_bind(adev, gtt->offset + (page_idx << PAGE_SHIFT),
				 ttm->num_pages - page_idx,
				 &(gtt->ttm.dma_address[page_idx]), flags);
	} else {
		amdgpu_gart_bind(adev, gtt->offset, ttm->num_pages,
				 gtt->ttm.dma_address, flags);
	}
}

/*
 * amdgpu_ttm_backend_bind - Bind GTT memory
 *
 * Called by ttm_tt_bind() on behalf of ttm_bo_handle_move_mem().
 * This handles binding GTT memory to the device address space.
 */
static int amdgpu_ttm_backend_bind(struct ttm_device *bdev,
				   struct ttm_tt *ttm,
				   struct ttm_resource *bo_mem)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bdev);
	struct amdgpu_ttm_tt *gtt = (void*)ttm;
	uint64_t flags;
	int r;

	if (!bo_mem)
		return -EINVAL;

	if (gtt->bound)
		return 0;

	if (gtt->userptr) {
		r = amdgpu_ttm_tt_pin_userptr(bdev, ttm);
		if (r) {
			DRM_ERROR("failed to pin userptr\n");
			return r;
		}
	} else if (ttm->page_flags & TTM_PAGE_FLAG_SG) {
#if defined(HAVE_DMA_BUF_OPS_DYNAMIC_MAPPING) || \
	defined(HAVE_STRUCT_DMA_BUF_OPS_PIN)
		if (!ttm->sg) {
			struct dma_buf_attachment *attach;
			struct sg_table *sgt;

			attach = gtt->gobj->import_attach;
			sgt = dma_buf_map_attachment(attach, DMA_BIDIRECTIONAL);
			if (IS_ERR(sgt))
				return PTR_ERR(sgt);

			ttm->sg = sgt;
		}
#endif

		drm_prime_sg_to_dma_addr_array(ttm->sg, gtt->ttm.dma_address,
					       ttm->num_pages);
	}

	if (!ttm->num_pages) {
		WARN(1, "nothing to bind %u pages for mreg %p back %p!\n",
		     ttm->num_pages, bo_mem, ttm);
	}

	if (bo_mem->mem_type != TTM_PL_TT ||
	    !amdgpu_gtt_mgr_has_gart_addr(bo_mem)) {
		gtt->offset = AMDGPU_BO_INVALID_OFFSET;
		return 0;
	}

	/* compute PTE flags relevant to this BO memory */
	flags = amdgpu_ttm_tt_pte_flags(adev, ttm, bo_mem);

	/* bind pages into GART page tables */
	gtt->offset = (u64)bo_mem->start << PAGE_SHIFT;
	amdgpu_gart_bind(adev, gtt->offset, ttm->num_pages,
			 gtt->ttm.dma_address, flags);
	gtt->bound = true;
	return 0;
}

/*
 * amdgpu_ttm_alloc_gart - Make sure buffer object is accessible either
 * through AGP or GART aperture.
 *
 * If bo is accessible through AGP aperture, then use AGP aperture
 * to access bo; otherwise allocate logical space in GART aperture
 * and map bo to GART aperture.
 */
int amdgpu_ttm_alloc_gart(struct ttm_buffer_object *bo)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->bdev);
	struct ttm_operation_ctx ctx = { false, false };
	struct amdgpu_ttm_tt *gtt = (void *)bo->ttm;
	struct ttm_placement placement;
	struct ttm_place placements;
	struct ttm_resource *tmp;
	uint64_t addr, flags;
	int r;

	if (bo->resource->start != AMDGPU_BO_INVALID_OFFSET)
		return 0;

	addr = amdgpu_gmc_agp_addr(bo);
	if (addr != AMDGPU_BO_INVALID_OFFSET) {
		bo->resource->start = addr >> PAGE_SHIFT;
		return 0;
	}

	/* allocate GART space */
	placement.num_placement = 1;
	placement.placement = &placements;
	placement.num_busy_placement = 1;
	placement.busy_placement = &placements;
	placements.fpfn = 0;
	placements.lpfn = adev->gmc.gart_size >> PAGE_SHIFT;
	placements.mem_type = TTM_PL_TT;
	placements.flags = bo->resource->placement;

	r = ttm_bo_mem_space(bo, &placement, &tmp, &ctx);
	if (unlikely(r))
		return r;

	/* compute PTE flags for this buffer object */
	flags = amdgpu_ttm_tt_pte_flags(adev, bo->ttm, tmp);

	/* Bind pages */
	gtt->offset = (u64)tmp->start << PAGE_SHIFT;
	amdgpu_ttm_gart_bind(adev, bo, flags);
	amdgpu_gart_invalidate_tlb(adev);
	ttm_resource_free(bo, &bo->resource);
	ttm_bo_assign_mem(bo, tmp);

	return 0;
}

/*
 * amdgpu_ttm_recover_gart - Rebind GTT pages
 *
 * Called by amdgpu_gtt_mgr_recover() from amdgpu_device_reset() to
 * rebind GTT pages during a GPU reset.
 */
void amdgpu_ttm_recover_gart(struct ttm_buffer_object *tbo)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(tbo->bdev);
	uint64_t flags;

	if (!tbo->ttm)
		return;

	flags = amdgpu_ttm_tt_pte_flags(adev, tbo->ttm, tbo->resource);
	amdgpu_ttm_gart_bind(adev, tbo, flags);
}

/*
 * amdgpu_ttm_backend_unbind - Unbind GTT mapped pages
 *
 * Called by ttm_tt_unbind() on behalf of ttm_bo_move_ttm() and
 * ttm_tt_destroy().
 */
static void amdgpu_ttm_backend_unbind(struct ttm_device *bdev,
				      struct ttm_tt *ttm)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bdev);
	struct amdgpu_ttm_tt *gtt = (void *)ttm;

	/* if the pages have userptr pinning then clear that first */
	if (gtt->userptr) {
		amdgpu_ttm_tt_unpin_userptr(bdev, ttm);
	} else if (ttm->sg && gtt->gobj->import_attach) {
#if defined(HAVE_DMA_BUF_OPS_DYNAMIC_MAPPING) || \
	defined(HAVE_STRUCT_DMA_BUF_OPS_PIN)
		struct dma_buf_attachment *attach;

		attach = gtt->gobj->import_attach;
		dma_buf_unmap_attachment(attach, ttm->sg, DMA_BIDIRECTIONAL);
		ttm->sg = NULL;
#endif
	}

	if (!gtt->bound)
		return;

	if (gtt->offset == AMDGPU_BO_INVALID_OFFSET)
		return;

	/* unbind shouldn't be done for GDS/GWS/OA in ttm_bo_clean_mm */
	amdgpu_gart_unbind(adev, gtt->offset, ttm->num_pages);
	gtt->bound = false;
}

static void amdgpu_ttm_backend_destroy(struct ttm_device *bdev,
				       struct ttm_tt *ttm)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;

	amdgpu_ttm_backend_unbind(bdev, ttm);
	ttm_tt_destroy_common(bdev, ttm);
	if (gtt->usertask)
		put_task_struct(gtt->usertask);

	ttm_tt_fini(&gtt->ttm);
	kfree(gtt);
}

/**
 * amdgpu_ttm_tt_create - Create a ttm_tt object for a given BO
 *
 * @bo: The buffer object to create a GTT ttm_tt object around
 * @page_flags: Page flags to be added to the ttm_tt object
 *
 * Called by ttm_tt_create().
 */
static struct ttm_tt *amdgpu_ttm_tt_create(struct ttm_buffer_object *bo,
					   uint32_t page_flags)
{
	struct amdgpu_bo *abo = ttm_to_amdgpu_bo(bo);
	struct amdgpu_ttm_tt *gtt;
	enum ttm_caching caching;

	gtt = kzalloc(sizeof(struct amdgpu_ttm_tt), GFP_KERNEL);
	if (gtt == NULL) {
		return NULL;
	}
	gtt->gobj = &bo->base;

	if (abo->flags & AMDGPU_GEM_CREATE_CPU_GTT_USWC)
		caching = ttm_write_combined;
	else
		caching = ttm_cached;

	/* allocate space for the uninitialized page entries */
	if (ttm_sg_tt_init(&gtt->ttm, bo, page_flags, caching)) {
		kfree(gtt);
		return NULL;
	}
	return &gtt->ttm;
}

/*
 * amdgpu_ttm_tt_populate - Map GTT pages visible to the device
 *
 * Map the pages of a ttm_tt object to an address space visible
 * to the underlying device.
 */
static int amdgpu_ttm_tt_populate(struct ttm_device *bdev,
				  struct ttm_tt *ttm,
				  struct ttm_operation_ctx *ctx)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bdev);
	struct amdgpu_ttm_tt *gtt = (void *)ttm;

	/* user pages are bound by amdgpu_ttm_tt_pin_userptr() */
	if (gtt->userptr) {
		ttm->sg = kzalloc(sizeof(struct sg_table), GFP_KERNEL);
		if (!ttm->sg)
			return -ENOMEM;
		return 0;
	}

	if (ttm->page_flags & TTM_PAGE_FLAG_SG)
		return 0;

	return ttm_pool_alloc(&adev->mman.bdev.pool, ttm, ctx);
}

/*
 * amdgpu_ttm_tt_unpopulate - unmap GTT pages and unpopulate page arrays
 *
 * Unmaps pages of a ttm_tt object from the device address space and
 * unpopulates the page array backing it.
 */
static void amdgpu_ttm_tt_unpopulate(struct ttm_device *bdev,
				     struct ttm_tt *ttm)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	struct amdgpu_device *adev;

	if (gtt->userptr) {
		amdgpu_ttm_tt_set_user_pages(ttm, NULL);
		kfree(ttm->sg);
		ttm->sg = NULL;
		return;
	}

	if (ttm->page_flags & TTM_PAGE_FLAG_SG)
		return;

	adev = amdgpu_ttm_adev(bdev);
	return ttm_pool_free(&adev->mman.bdev.pool, ttm);
}

/**
 * amdgpu_ttm_tt_get_userptr - Return the userptr GTT ttm_tt for the current
 * task
 *
 * @tbo: The ttm_buffer_object that contains the userptr
 * @user_addr:  The returned value
 */
int amdgpu_ttm_tt_get_userptr(const struct ttm_buffer_object *tbo,
			      uint64_t *user_addr)
{
	struct amdgpu_ttm_tt *gtt;

	if (!tbo->ttm)
		return -EINVAL;

	gtt = (void *)tbo->ttm;
	*user_addr = gtt->userptr;
	return 0;
}

/**
 * amdgpu_ttm_tt_set_userptr - Initialize userptr GTT ttm_tt for the current
 * task
 *
 * @bo: The ttm_buffer_object to bind this userptr to
 * @addr:  The address in the current tasks VM space to use
 * @flags: Requirements of userptr object.
 *
 * Called by amdgpu_gem_userptr_ioctl() to bind userptr pages
 * to current task
 */
int amdgpu_ttm_tt_set_userptr(struct ttm_buffer_object *bo,
			      uint64_t addr, uint32_t flags)
{
	struct amdgpu_ttm_tt *gtt;

	if (!bo->ttm) {
		/* TODO: We want a separate TTM object type for userptrs */
		bo->ttm = amdgpu_ttm_tt_create(bo, 0);
		if (bo->ttm == NULL)
			return -ENOMEM;
	}

	/* Set TTM_PAGE_FLAG_SG before populate but after create. */
	bo->ttm->page_flags |= TTM_PAGE_FLAG_SG;

	gtt = (void *)bo->ttm;
	gtt->userptr = addr;
	gtt->userflags = flags;

	if (gtt->usertask)
		put_task_struct(gtt->usertask);
	gtt->usertask = current->group_leader;
	get_task_struct(gtt->usertask);

#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
	spin_lock_init(&gtt->guptasklock);
	INIT_LIST_HEAD(&gtt->guptasks);
	atomic_set(&gtt->mmu_invalidations, 0);
	gtt->last_set_pages = 0;
#endif

	return 0;
}

/*
 * amdgpu_ttm_tt_get_usermm - Return memory manager for ttm_tt object
 */
struct mm_struct *amdgpu_ttm_tt_get_usermm(struct ttm_tt *ttm)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;

	if (gtt == NULL)
		return NULL;

	if (gtt->usertask == NULL)
		return NULL;

	return gtt->usertask->mm;
}

#ifdef HAVE_AMDKCL_HMM_MIRROR_ENABLED
/*
 * amdgpu_ttm_tt_affect_userptr - Determine if a ttm_tt object lays inside an
 * address range for the current task.
 *
 */
bool amdgpu_ttm_tt_affect_userptr(struct ttm_tt *ttm, unsigned long start,
				  unsigned long end, unsigned long *userptr)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	unsigned long size;

	if (gtt == NULL || !gtt->userptr)
		return false;

	/* Return false if no part of the ttm_tt object lies within
	 * the range
	 */
	size = (unsigned long)gtt->ttm.num_pages * PAGE_SIZE;
	if (gtt->userptr > end || gtt->userptr + size <= start)
		return false;

	if (userptr)
		*userptr = gtt->userptr;
	return true;
}

/*
 * amdgpu_ttm_tt_is_userptr - Have the pages backing by userptr?
 */
bool amdgpu_ttm_tt_is_userptr(struct ttm_tt *ttm)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;

	if (gtt == NULL || !gtt->userptr)
		return false;

	return true;
}

#else
/*
 * amdgpu_ttm_tt_affect_userptr - Determine if a ttm_tt object lays inside an
 * address range for the current task.
 *
 */
bool amdgpu_ttm_tt_affect_userptr(struct ttm_tt *ttm, unsigned long start,
				  unsigned long end, unsigned long *userptr)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	struct amdgpu_ttm_gup_task_list *entry;
	unsigned long size;

	if (gtt == NULL || !gtt->userptr)
		return false;

	/* Return false if no part of the ttm_tt object lies within
	 * the range
	 */
	size = (unsigned long)gtt->ttm.num_pages * PAGE_SIZE;
	if (gtt->userptr > end || gtt->userptr + size <= start)
		return false;

	/* Search the lists of tasks that hold this mapping and see
	 * if current is one of them.  If it is return false.
	 */
	spin_lock(&gtt->guptasklock);
	list_for_each_entry(entry, &gtt->guptasks, list) {
		if (entry->task == current) {
			spin_unlock(&gtt->guptasklock);
			return false;
		}
	}
	spin_unlock(&gtt->guptasklock);

	atomic_inc(&gtt->mmu_invalidations);

	if (userptr)
		*userptr = gtt->userptr;

	return true;
}

/**
 * amdgpu_ttm_tt_userptr_invalidated - Has the ttm_tt object been invalidated?
 */
bool amdgpu_ttm_tt_userptr_invalidated(struct ttm_tt *ttm,
				       int *last_invalidated)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;
	int prev_invalidated = *last_invalidated;

	*last_invalidated = atomic_read(&gtt->mmu_invalidations);
	return prev_invalidated != *last_invalidated;
}

/**
 * amdgpu_ttm_tt_userptr_needs_pages - Have the pages backing this ttm_tt object
 * been invalidated since the last time they've been set?
 */
bool amdgpu_ttm_tt_userptr_needs_pages(struct ttm_tt *ttm)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;

	if (gtt == NULL || !gtt->userptr)
		return false;

	return atomic_read(&gtt->mmu_invalidations) != gtt->last_set_pages;
}
#endif /* HAVE_AMDKCL_HMM_MIRROR_ENABLED */

/*
 * amdgpu_ttm_tt_is_readonly - Is the ttm_tt object read only?
 */
bool amdgpu_ttm_tt_is_readonly(struct ttm_tt *ttm)
{
	struct amdgpu_ttm_tt *gtt = (void *)ttm;

	if (gtt == NULL)
		return false;

	return !!(gtt->userflags & AMDGPU_GEM_USERPTR_READONLY);
}

/**
 * amdgpu_ttm_tt_pde_flags - Compute PDE flags for ttm_tt object
 *
 * @ttm: The ttm_tt object to compute the flags for
 * @mem: The memory registry backing this ttm_tt object
 *
 * Figure out the flags to use for a VM PDE (Page Directory Entry).
 */
uint64_t amdgpu_ttm_tt_pde_flags(struct ttm_tt *ttm, struct ttm_resource *mem)
{
	uint64_t flags = 0;

	if (mem && mem->mem_type != TTM_PL_SYSTEM)
		flags |= AMDGPU_PTE_VALID;

	if (mem && (mem->mem_type == TTM_PL_TT ||
		    mem->mem_type == AMDGPU_PL_PREEMPT)) {
		flags |= AMDGPU_PTE_SYSTEM;

		if (ttm->caching == ttm_cached)
			flags |= AMDGPU_PTE_SNOOPED;
	}

	if (mem && mem->mem_type == TTM_PL_VRAM &&
			mem->bus.caching == ttm_cached)
		flags |= AMDGPU_PTE_SNOOPED;

	return flags;
}

/**
 * amdgpu_ttm_tt_pte_flags - Compute PTE flags for ttm_tt object
 *
 * @adev: amdgpu_device pointer
 * @ttm: The ttm_tt object to compute the flags for
 * @mem: The memory registry backing this ttm_tt object
 *
 * Figure out the flags to use for a VM PTE (Page Table Entry).
 */
uint64_t amdgpu_ttm_tt_pte_flags(struct amdgpu_device *adev, struct ttm_tt *ttm,
				 struct ttm_resource *mem)
{
	uint64_t flags = amdgpu_ttm_tt_pde_flags(ttm, mem);

	if (mem && mem->mem_type == AMDGPU_PL_DGMA_IMPORT)
		flags |= AMDGPU_PTE_SYSTEM;

	flags |= adev->gart.gart_pte_flags;
	flags |= AMDGPU_PTE_READABLE;

	if (!amdgpu_ttm_tt_is_readonly(ttm))
		flags |= AMDGPU_PTE_WRITEABLE;

	return flags;
}

/*
 * amdgpu_ttm_bo_eviction_valuable - Check to see if we can evict a buffer
 * object.
 *
 * Return true if eviction is sensible. Called by ttm_mem_evict_first() on
 * behalf of ttm_bo_mem_force_space() which tries to evict buffer objects until
 * it can find space for a new object and by ttm_bo_force_list_clean() which is
 * used to clean out a memory space.
 */
static bool amdgpu_ttm_bo_eviction_valuable(struct ttm_buffer_object *bo,
					    const struct ttm_place *place)
{
	unsigned long num_pages = bo->resource->num_pages;
	struct amdgpu_res_cursor cursor;
	struct dma_resv_list *flist;
	struct dma_fence *f;
	int i;

	/* Swapout? */
	if (bo->resource->mem_type == TTM_PL_SYSTEM)
		return true;

	if (bo->type == ttm_bo_type_kernel &&
	    !amdgpu_vm_evictable(ttm_to_amdgpu_bo(bo)))
		return false;

	/* If bo is a KFD BO, check if the bo belongs to the current process.
	 * If true, then return false as any KFD process needs all its BOs to
	 * be resident to run successfully
	 */
	flist = dma_resv_shared_list(amdkcl_ttm_resvp(bo));
	if (flist) {
		for (i = 0; i < flist->shared_count; ++i) {
			f = rcu_dereference_protected(flist->shared[i],
				dma_resv_held(amdkcl_ttm_resvp(bo)));
			if (amdkfd_fence_check_mm(f, current->mm))
				return false;
		}
	}

	switch (bo->resource->mem_type) {
	case AMDGPU_PL_PREEMPT:
		/* Preemptible BOs don't own system resources managed by the
		 * driver (pages, VRAM, GART space). They point to resources
		 * owned by someone else (e.g. pageable memory in user mode
		 * or a DMABuf). They are used in a preemptible context so we
		 * can guarantee no deadlocks and good QoS in case of MMU
		 * notifiers or DMABuf move notifiers from the resource owner.
		 */
		return false;
	case TTM_PL_TT:
		if (amdgpu_bo_is_amdgpu_bo(bo) &&
		    amdgpu_bo_encrypted(ttm_to_amdgpu_bo(bo)))
			return false;
		return true;

	case TTM_PL_VRAM:
		/* Check each drm MM node individually */
		amdgpu_res_first(bo->resource, 0, (u64)num_pages << PAGE_SHIFT,
				 &cursor);
		while (cursor.remaining) {
			if (place->fpfn < PFN_DOWN(cursor.start + cursor.size)
			    && !(place->lpfn &&
				 place->lpfn <= PFN_DOWN(cursor.start)))
				return true;

			amdgpu_res_next(&cursor, cursor.size);
		}
		return false;

	default:
		break;
	}

	return ttm_bo_eviction_valuable(bo, place);
}

static void amdgpu_ttm_vram_mm_access(struct amdgpu_device *adev, loff_t pos,
				      void *buf, size_t size, bool write)
{
	while (size) {
		uint64_t aligned_pos = ALIGN_DOWN(pos, 4);
		uint64_t bytes = 4 - (pos & 0x3);
		uint32_t shift = (pos & 0x3) * 8;
		uint32_t mask = 0xffffffff << shift;
		uint32_t value = 0;

		if (size < bytes) {
			mask &= 0xffffffff >> (bytes - size) * 8;
			bytes = size;
		}

		if (mask != 0xffffffff) {
			amdgpu_device_mm_access(adev, aligned_pos, &value, 4, false);
			if (write) {
				value &= ~mask;
				value |= (*(uint32_t *)buf << shift) & mask;
				amdgpu_device_mm_access(adev, aligned_pos, &value, 4, true);
			} else {
				value = (value & mask) >> shift;
				memcpy(buf, &value, bytes);
			}
		} else {
			amdgpu_device_mm_access(adev, aligned_pos, buf, 4, write);
		}

		pos += bytes;
		buf += bytes;
		size -= bytes;
	}
}

static int amdgpu_ttm_access_memory_sdma(struct ttm_buffer_object *bo,
					unsigned long offset, void *buf, int len, int write)
{
	struct amdgpu_bo *abo = ttm_to_amdgpu_bo(bo);
	struct amdgpu_device *adev = amdgpu_ttm_adev(abo->tbo.bdev);
	struct amdgpu_res_cursor src_mm;
	struct amdgpu_job *job;
	struct dma_fence *fence;
	uint64_t src_addr, dst_addr;
	unsigned int num_dw;
	int r, idx;

	if (len != PAGE_SIZE)
		return -EINVAL;

	if (!adev->mman.sdma_access_ptr)
		return -EACCES;

	if (!drm_dev_enter(adev_to_drm(adev), &idx))
		return -ENODEV;

	if (write)
		memcpy(adev->mman.sdma_access_ptr, buf, len);

	num_dw = ALIGN(adev->mman.buffer_funcs->copy_num_dw, 8);
	r = amdgpu_job_alloc_with_ib(adev, num_dw * 4, AMDGPU_IB_POOL_DELAYED, &job);
	if (r)
		goto out;

	amdgpu_res_first(abo->tbo.resource, offset, len, &src_mm);
	src_addr = amdgpu_ttm_domain_start(adev, bo->resource->mem_type) + src_mm.start;
	dst_addr = amdgpu_bo_gpu_offset(adev->mman.sdma_access_bo);
	if (write)
		swap(src_addr, dst_addr);

	amdgpu_emit_copy_buffer(adev, &job->ibs[0], src_addr, dst_addr, PAGE_SIZE, false);

	amdgpu_ring_pad_ib(adev->mman.buffer_funcs_ring, &job->ibs[0]);
	WARN_ON(job->ibs[0].length_dw > num_dw);

	r = amdgpu_job_submit(job, &adev->mman.entity, AMDGPU_FENCE_OWNER_UNDEFINED, &fence);
	if (r) {
		amdgpu_job_free(job);
		goto out;
	}

	if (!dma_fence_wait_timeout(fence, false, adev->sdma_timeout))
		r = -ETIMEDOUT;
	dma_fence_put(fence);

	if (!(r || write))
		memcpy(buf, adev->mman.sdma_access_ptr, len);
out:
	drm_dev_exit(idx);
	return r;
}

/**
 * amdgpu_ttm_access_memory - Read or Write memory that backs a buffer object.
 *
 * @bo:  The buffer object to read/write
 * @offset:  Offset into buffer object
 * @buf:  Secondary buffer to write/read from
 * @len: Length in bytes of access
 * @write:  true if writing
 *
 * This is used to access VRAM that backs a buffer object via MMIO
 * access for debugging purposes.
 */
static int amdgpu_ttm_access_memory(struct ttm_buffer_object *bo,
				    unsigned long offset, void *buf, int len,
				    int write)
{
	struct amdgpu_bo *abo = ttm_to_amdgpu_bo(bo);
	struct amdgpu_device *adev = amdgpu_ttm_adev(abo->tbo.bdev);
	struct amdgpu_res_cursor cursor;
	int ret = 0;

	if (bo->resource->mem_type != TTM_PL_VRAM)
		return -EIO;

	if (amdgpu_device_has_timeouts_enabled(adev) &&
			!amdgpu_ttm_access_memory_sdma(bo, offset, buf, len, write))
		return len;

	amdgpu_res_first(bo->resource, offset, len, &cursor);
	while (cursor.remaining) {
		size_t count, size = cursor.size;
		loff_t pos = cursor.start;

		count = amdgpu_device_aper_access(adev, pos, buf, size, write);
		size -= count;
		if (size) {
			/* using MM to access rest vram and handle un-aligned address */
			pos += count;
			buf += count;
			amdgpu_ttm_vram_mm_access(adev, pos, buf, size, write);
		}

		ret += cursor.size;
		buf += cursor.size;
		amdgpu_res_next(&cursor, cursor.size);
	}

	return ret;
}

static void
amdgpu_bo_delete_mem_notify(struct ttm_buffer_object *bo)
{
	amdgpu_bo_move_notify(bo, false, NULL);
}

static struct ttm_device_funcs amdgpu_bo_driver = {
	.ttm_tt_create = &amdgpu_ttm_tt_create,
	.ttm_tt_populate = &amdgpu_ttm_tt_populate,
	.ttm_tt_unpopulate = &amdgpu_ttm_tt_unpopulate,
	.ttm_tt_destroy = &amdgpu_ttm_backend_destroy,
	.eviction_valuable = amdgpu_ttm_bo_eviction_valuable,
	.evict_flags = &amdgpu_evict_flags,
	.move = &amdgpu_bo_move,
#ifdef HAVE_STRUCT_DRM_DRV_GEM_OPEN_OBJECT_CALLBACK
	.verify_access = &amdgpu_verify_access,
#endif
	.delete_mem_notify = &amdgpu_bo_delete_mem_notify,
	.release_notify = &amdgpu_bo_release_notify,
	.io_mem_reserve = &amdgpu_ttm_io_mem_reserve,
	.io_mem_pfn = amdgpu_ttm_io_mem_pfn,
	.access_memory = &amdgpu_ttm_access_memory,
	.del_from_lru_notify = &amdgpu_vm_del_from_lru_notify
};

/*
 * Firmware Reservation functions
 */
/**
 * amdgpu_ttm_fw_reserve_vram_fini - free fw reserved vram
 *
 * @adev: amdgpu_device pointer
 *
 * free fw reserved vram if it has been reserved.
 */
static void amdgpu_ttm_fw_reserve_vram_fini(struct amdgpu_device *adev)
{
	amdgpu_bo_free_kernel(&adev->mman.fw_vram_usage_reserved_bo,
		NULL, &adev->mman.fw_vram_usage_va);
}

/**
 * amdgpu_ttm_fw_reserve_vram_init - create bo vram reservation from fw
 *
 * @adev: amdgpu_device pointer
 *
 * create bo vram reservation from fw.
 */
static int amdgpu_ttm_fw_reserve_vram_init(struct amdgpu_device *adev)
{
	uint64_t vram_size = adev->gmc.visible_vram_size;

	adev->mman.fw_vram_usage_va = NULL;
	adev->mman.fw_vram_usage_reserved_bo = NULL;

	if (adev->mman.fw_vram_usage_size == 0 ||
	    adev->mman.fw_vram_usage_size > vram_size)
		return 0;

	return amdgpu_bo_create_kernel_at(adev,
					  adev->mman.fw_vram_usage_start_offset,
					  adev->mman.fw_vram_usage_size,
					  AMDGPU_GEM_DOMAIN_VRAM,
					  &adev->mman.fw_vram_usage_reserved_bo,
					  &adev->mman.fw_vram_usage_va);
}

/*
 * Memoy training reservation functions
 */

/**
 * amdgpu_ttm_training_reserve_vram_fini - free memory training reserved vram
 *
 * @adev: amdgpu_device pointer
 *
 * free memory training reserved vram if it has been reserved.
 */
static int amdgpu_ttm_training_reserve_vram_fini(struct amdgpu_device *adev)
{
	struct psp_memory_training_context *ctx = &adev->psp.mem_train_ctx;

	ctx->init = PSP_MEM_TRAIN_NOT_SUPPORT;
	amdgpu_bo_free_kernel(&ctx->c2p_bo, NULL, NULL);
	ctx->c2p_bo = NULL;

	return 0;
}

static void amdgpu_ttm_training_data_block_init(struct amdgpu_device *adev)
{
	struct psp_memory_training_context *ctx = &adev->psp.mem_train_ctx;

	memset(ctx, 0, sizeof(*ctx));

	ctx->c2p_train_data_offset =
		ALIGN((adev->gmc.mc_vram_size - adev->mman.discovery_tmr_size - SZ_1M), SZ_1M);
	ctx->p2c_train_data_offset =
		(adev->gmc.mc_vram_size - GDDR6_MEM_TRAINING_OFFSET);
	ctx->train_data_size =
		GDDR6_MEM_TRAINING_DATA_SIZE_IN_BYTES;

	DRM_DEBUG("train_data_size:%llx,p2c_train_data_offset:%llx,c2p_train_data_offset:%llx.\n",
			ctx->train_data_size,
			ctx->p2c_train_data_offset,
			ctx->c2p_train_data_offset);
}

/*
 * reserve TMR memory at the top of VRAM which holds
 * IP Discovery data and is protected by PSP.
 */
static int amdgpu_ttm_reserve_tmr(struct amdgpu_device *adev)
{
	int ret;
	struct psp_memory_training_context *ctx = &adev->psp.mem_train_ctx;
	bool mem_train_support = false;

	if (!amdgpu_sriov_vf(adev)) {
		if (amdgpu_atomfirmware_mem_training_supported(adev))
			mem_train_support = true;
		else
			DRM_DEBUG("memory training does not support!\n");
	}

	/*
	 * Query reserved tmr size through atom firmwareinfo for Sienna_Cichlid and onwards for all
	 * the use cases (IP discovery/G6 memory training/profiling/diagnostic data.etc)
	 *
	 * Otherwise, fallback to legacy approach to check and reserve tmr block for ip
	 * discovery data and G6 memory training data respectively
	 */
	adev->mman.discovery_tmr_size =
		amdgpu_atomfirmware_get_fw_reserved_fb_size(adev);
	if (!adev->mman.discovery_tmr_size)
		adev->mman.discovery_tmr_size = DISCOVERY_TMR_OFFSET;

	if (mem_train_support) {
		/* reserve vram for mem train according to TMR location */
		amdgpu_ttm_training_data_block_init(adev);
		ret = amdgpu_bo_create_kernel_at(adev,
					 ctx->c2p_train_data_offset,
					 ctx->train_data_size,
					 AMDGPU_GEM_DOMAIN_VRAM,
					 &ctx->c2p_bo,
					 NULL);
		if (ret) {
			DRM_ERROR("alloc c2p_bo failed(%d)!\n", ret);
			amdgpu_ttm_training_reserve_vram_fini(adev);
			return ret;
		}
		ctx->init = PSP_MEM_TRAIN_RESERVE_SUCCESS;
	}

	ret = amdgpu_bo_create_kernel_at(adev,
				adev->gmc.real_vram_size - adev->mman.discovery_tmr_size,
				adev->mman.discovery_tmr_size,
				AMDGPU_GEM_DOMAIN_VRAM,
				&adev->mman.discovery_memory,
				NULL);
	if (ret) {
		DRM_ERROR("alloc tmr failed(%d)!\n", ret);
		amdgpu_bo_free_kernel(&adev->mman.discovery_memory, NULL, NULL);
		return ret;
	}

	return 0;
}

static inline struct amdgpu_dgma_import_mgr *to_dgma_import_mgr(struct ttm_resource_manager *man)
{
	return container_of(man, struct amdgpu_dgma_import_mgr, manager);
}

static const struct ttm_resource_manager_func amdgpu_dgma_import_mgr_func;
/**
 * amdgpu_dgma_import_mgr_init - init DGMA_import manager and DRM MM
 *
 * @adev: amdgpu_device pointer
 * @dgma_size: maximum size of DGMA
 *
 * Allocate and initialize the DGMA manager.
 */
static int amdgpu_dgma_import_mgr_init(struct amdgpu_device *adev, uint64_t p_size)
{
	struct amdgpu_dgma_import_mgr *mgr = &adev->mman.dgma_import_mgr;
	struct ttm_resource_manager *man = &mgr->manager;

	man->func = &amdgpu_dgma_import_mgr_func;

	ttm_resource_manager_init(man, p_size);
	drm_mm_init(&mgr->mm, 0, p_size);
	spin_lock_init(&mgr->lock);
	atomic64_set(&mgr->available, p_size);

	BUG_ON(AMDGPU_PL_DGMA_IMPORT >= TTM_NUM_MEM_TYPES);
	ttm_set_driver_manager(&adev->mman.bdev, AMDGPU_PL_DGMA_IMPORT, man);
	ttm_resource_manager_set_used(man, true);
	return 0;
}

/**
 * amdgpu_dgma_import_mgr_fini - free and destroy DGMA import manager
 *
 * @adev: amdgpu_device pointer
 *
 * Destroy and free the DGMA import manager, returns -EBUSY if ranges are still
 * allocated inside it.
 */
static void amdgpu_dgma_import_mgr_fini(struct amdgpu_device *adev)
{
	struct amdgpu_dgma_import_mgr *mgr = &adev->mman.dgma_import_mgr;
	struct ttm_resource_manager *man = &mgr->manager;
	int ret;

	ttm_resource_manager_set_used(man, false);

	ret = ttm_resource_manager_evict_all(&adev->mman.bdev, man);
	if (ret)
		return;

	spin_lock(&mgr->lock);
	drm_mm_takedown(&mgr->mm);
	spin_unlock(&mgr->lock);
	ttm_resource_manager_cleanup(man);
	ttm_set_driver_manager(&adev->mman.bdev, TTM_PL_TT, NULL);
}

/**
 * amdgpu_dgma_import_mgr_new - allocate a new node
 *
 * @man: TTM memory type manager
 * @tbo: TTM BO we need this range for
 * @place: placement flags and restrictions
 * @mem: the resulting mem object
 */
static int amdgpu_dgma_import_mgr_new(struct ttm_resource_manager *man,
			      struct ttm_buffer_object *tbo,
			      const struct ttm_place *place,
			      struct ttm_resource **res)
{
	struct amdgpu_dgma_import_mgr *mgr = to_dgma_import_mgr(man);
	uint32_t num_pages = PFN_UP(tbo->base.size);
	struct amdgpu_dgma_node *node;
	unsigned long lpfn;
	int r;

	spin_lock(&mgr->lock);
	if (atomic64_read(&mgr->available) < num_pages) {
		spin_unlock(&mgr->lock);
		return -ENOSPC;
	}
	atomic64_sub(num_pages, &mgr->available);
	spin_unlock(&mgr->lock);

	lpfn = place->lpfn;
	if (!lpfn)
		lpfn = man->size;

	node = kzalloc(struct_size(node, base.mm_nodes, 1), GFP_KERNEL);
	if (!node) {
		r = -ENOMEM;
		goto err_out;
	}

	node->tbo = tbo;
	ttm_resource_init(tbo, place, &node->base.base);

	spin_lock(&mgr->lock);
	r = drm_mm_insert_node_in_range(&mgr->mm, &node->base.mm_nodes[0], num_pages,
					tbo->page_alignment, 0, place->fpfn,
					lpfn, DRM_MM_INSERT_BEST);
	spin_unlock(&mgr->lock);

	if (unlikely(r))
		goto err_free;

	*res = &node->base.base;
	(*res)->start = node->base.mm_nodes[0].start;

	return 0;

err_free:
	kfree(node);

err_out:
	atomic64_add(num_pages, &mgr->available);

	return r;
}

/**
 * amdgpu_dgma_import_mgr_del - free ranges
 *
 * @man: TTM memory type manager
 * @mem: TTM memory object
 *
 * Free the allocated node.
 */
static void amdgpu_dgma_import_mgr_del(struct ttm_resource_manager *man,
			       struct ttm_resource *mem)
{
	struct amdgpu_dgma_import_mgr *mgr = to_dgma_import_mgr(man);
	struct amdgpu_dgma_node *node = container_of(mem, struct amdgpu_dgma_node, base.base);

	if (node) {
		spin_lock(&mgr->lock);
		drm_mm_remove_node(&node->base.mm_nodes[0]);
		spin_unlock(&mgr->lock);
		kfree(node);
	}

	atomic64_add(mem->num_pages, &mgr->available);
}

static void amdgpu_dgma_import_mgr_debug(struct ttm_resource_manager *man,
			     struct drm_printer *printer)
{
	struct amdgpu_dgma_import_mgr *rman = to_dgma_import_mgr(man);

	spin_lock(&rman->lock);
	drm_mm_print(&rman->mm, printer);
	spin_unlock(&rman->lock);
}

static const struct ttm_resource_manager_func amdgpu_dgma_import_mgr_func = {
	.alloc = amdgpu_dgma_import_mgr_new,
	.free = amdgpu_dgma_import_mgr_del,
	.debug = amdgpu_dgma_import_mgr_debug
};

static int amdgpu_direct_gma_init(struct amdgpu_device *adev)
{
	struct amdgpu_bo *abo;
	struct amdgpu_bo_param bp;
	unsigned long size;
	int r;

	if (amdgpu_direct_gma_size == 0)
		return 0;

	size = (unsigned long)amdgpu_direct_gma_size << 20;

	memset(&bp, 0, sizeof(bp));
	bp.size = size;
	bp.byte_align = PAGE_SIZE;
	bp.domain = AMDGPU_GEM_DOMAIN_VRAM;
	bp.flags = AMDGPU_GEM_CREATE_CPU_ACCESS_REQUIRED | AMDGPU_GEM_CREATE_VRAM_CONTIGUOUS |
						AMDGPU_GEM_CREATE_TOP_DOWN;
	bp.type = ttm_bo_type_kernel;
	bp.resv = NULL;
	bp.bo_ptr_size = sizeof(struct amdgpu_bo);

	/* reserve in visible vram */
	r = amdgpu_bo_create(adev, &bp, &abo);
	if (unlikely(r))
		goto error_out;

	r = amdgpu_bo_reserve(abo, false);
	if (unlikely(r))
		goto error_free;

	r = amdgpu_bo_pin(abo, AMDGPU_GEM_DOMAIN_VRAM);
	amdgpu_bo_unreserve(abo);
	if (unlikely(r))
		goto error_free;

	adev->direct_gma.dgma_bo = abo;

	/* reserve in gtt */
	atomic64_add(size, &adev->gart_pin_size);
	r = ttm_range_man_init(&adev->mman.bdev, AMDGPU_PL_DGMA,
					  false, size >> PAGE_SHIFT);
	if (unlikely(r))
		goto error_put_node;

	r = amdgpu_dgma_import_mgr_init(adev, size >> PAGE_SHIFT);
	if (unlikely(r))
		goto error_release_mm;

	DRM_INFO("%dMB VRAM/GTT reserved for Direct GMA\n", amdgpu_direct_gma_size);
	return 0;

error_release_mm:
	ttm_range_man_fini(&adev->mman.bdev, AMDGPU_PL_DGMA);

error_put_node:
	atomic64_sub(size, &adev->gart_pin_size);
error_free:
	amdgpu_bo_unref(&abo);

error_out:
	amdgpu_direct_gma_size = 0;
	memset(&adev->direct_gma, 0, sizeof(adev->direct_gma));
	DRM_ERROR("Fail to enable Direct GMA\n");
	return r;
}

static void amdgpu_direct_gma_fini(struct amdgpu_device *adev)
{
	int r;

	if (amdgpu_direct_gma_size == 0)
		return;

	ttm_range_man_fini(&adev->mman.bdev, AMDGPU_PL_DGMA);
	amdgpu_dgma_import_mgr_fini(adev);

	r = amdgpu_bo_reserve(adev->direct_gma.dgma_bo, false);
	if (r == 0) {
		amdgpu_bo_unpin(adev->direct_gma.dgma_bo);
		amdgpu_bo_unreserve(adev->direct_gma.dgma_bo);
	}
	amdgpu_bo_unref(&adev->direct_gma.dgma_bo);
	atomic64_sub((u64)amdgpu_direct_gma_size << 20, &adev->gart_pin_size);
}

#ifdef CONFIG_ENABLE_SSG
#include <linux/memremap.h>

static struct amdgpu_ssg *to_amdgpu_ssg(struct percpu_ref *ref)
{
	return container_of(ref, struct amdgpu_ssg, ref);
}

static void amdgpu_ssg_percpu_release(struct percpu_ref *ref)
{
	struct amdgpu_ssg *ssg = to_amdgpu_ssg(ref);

	complete(&ssg->cmp);
}

static int amdgpu_ssg_init(struct amdgpu_device *adev)
{
	struct resource res;
	void *addr;
	int rc;

	adev->ssg.enabled = false;

	if (!amdgpu_ssg_enabled)
		return 0;

	if (amdgpu_direct_gma_size == 0) {
		DRM_INFO("SSG: not enabled due to DirectGMA is disabled\n");
		return 0;
	}

	init_completion(&adev->ssg.cmp);

	res.start = adev->gmc.aper_base +
		(amdgpu_bo_gpu_offset(adev->direct_gma.dgma_bo) -
		 adev->gmc.vram_start);
	res.end = res.start + amdgpu_bo_size(adev->direct_gma.dgma_bo) - 1;
	res.name = "DirectGMA";

	rc = percpu_ref_init(&adev->ssg.ref, amdgpu_ssg_percpu_release,
			     0, GFP_KERNEL);
	if (rc)
		return rc;

#if defined(HAVE_DEVM_MEMREMAP_PAGES_DEV_PAGEMAP)
#ifdef HAVE_DEV_PAGEMAP_RANGE
	adev->ssg.pgmap.nr_range = 1;
	adev->ssg.pgmap.range.start = res.start;
	adev->ssg.pgmap.range.end = res.end;
#else
	adev->ssg.pgmap.res.start = res.start;
	adev->ssg.pgmap.res.end = res.end;
	adev->ssg.pgmap.res.name = res.name;
#endif
	adev->ssg.pgmap.ref = &adev->ssg.ref;
	addr = devm_memremap_pages(adev->dev, &adev->ssg.pgmap);
#else
	addr = devm_memremap_pages(adev->dev, &res, &adev->ssg.ref, NULL);
#endif
	if (IS_ERR(addr)) {
		percpu_ref_exit(&adev->ssg.ref);
		return PTR_ERR(addr);
	}

	adev->ssg.enabled = true;
	DRM_INFO("SSG: remap %llx-%llx to %p\n", res.start, res.end, addr);
	return 0;
}

static void amdgpu_ssg_fini(struct amdgpu_device *adev)
{
	if (!adev->ssg.enabled)
		return;

	percpu_ref_kill(&adev->ssg.ref);
	wait_for_completion(&adev->ssg.cmp);
	percpu_ref_exit(&adev->ssg.ref);
}
#else
static int amdgpu_ssg_init(struct amdgpu_device *adev)
{
	adev->ssg.enabled = false;
	return 0;
}

static void amdgpu_ssg_fini(struct amdgpu_device *adev)
{

}
#endif

/*
 * amdgpu_ttm_init - Init the memory management (ttm) as well as various
 * gtt/vram related fields.
 *
 * This initializes all of the memory space pools that the TTM layer
 * will need such as the GTT space (system memory mapped to the device),
 * VRAM (on-board memory), and on-chip memories (GDS, GWS, OA) which
 * can be mapped per VMID.
 */
int amdgpu_ttm_init(struct amdgpu_device *adev)
{
	uint64_t gtt_size;
	int r;
	u64 vis_vram_limit;
	bool need_dma32;

#ifdef AMDKCL_DMA_ADDRESSING_LIMITED_WORKAROUND
	/*
	 * set DMA mask + need_dma32 flags.
	 * PCIE - can handle 44-bits.
	 * IGP - can handle 44-bits
	 * PCI - dma32 for legacy pci gart
	 */
	need_dma32 = !!pci_set_dma_mask(adev->pdev, DMA_BIT_MASK(44));
#else
	need_dma32 = dma_addressing_limited(adev->dev);
#endif

	mutex_init(&adev->mman.gtt_window_lock);

	/* No others user of address space so set it to 0 */
	r = ttm_device_init(&adev->mman.bdev, &amdgpu_bo_driver, adev->dev,
			       adev_to_drm(adev)->anon_inode->i_mapping,
			       adev_to_drm(adev)->vma_offset_manager,
			       adev->need_swiotlb,
			       need_dma32);
	if (r) {
		DRM_ERROR("failed initializing buffer object driver(%d).\n", r);
		return r;
	}
	adev->mman.initialized = true;

	/* Initialize VRAM pool with all of VRAM divided into pages */
	r = amdgpu_vram_mgr_init(adev);
	if (r) {
		DRM_ERROR("Failed initializing VRAM heap.\n");
		return r;
	}

	/* Reduce size of CPU-visible VRAM if requested */
	vis_vram_limit = (u64)amdgpu_vis_vram_limit * 1024 * 1024;
	if (amdgpu_vis_vram_limit > 0 &&
	    vis_vram_limit <= adev->gmc.visible_vram_size)
		adev->gmc.visible_vram_size = vis_vram_limit;

	/* Change the size here instead of the init above so only lpfn is affected */
	amdgpu_ttm_set_buffer_funcs_status(adev, false);
#ifdef CONFIG_64BIT
#ifdef CONFIG_X86
	if (adev->gmc.xgmi.connected_to_cpu)
		adev->mman.aper_base_kaddr = ioremap_cache(adev->gmc.aper_base,
				adev->gmc.visible_vram_size);

	else
#endif
		adev->mman.aper_base_kaddr = ioremap_wc(adev->gmc.aper_base,
				adev->gmc.visible_vram_size);
#endif

	/*
	 *The reserved vram for firmware must be pinned to the specified
	 *place on the VRAM, so reserve it early.
	 */
	r = amdgpu_ttm_fw_reserve_vram_init(adev);
	if (r) {
		return r;
	}

	/*
	 * only NAVI10 and onwards ASIC support for IP discovery.
	 * If IP discovery enabled, a block of memory should be
	 * reserved for IP discovey.
	 */
	if (adev->mman.discovery_bin) {
		r = amdgpu_ttm_reserve_tmr(adev);
		if (r)
			return r;
	}

	/* allocate memory as required for VGA
	 * This is used for VGA emulation and pre-OS scanout buffers to
	 * avoid display artifacts while transitioning between pre-OS
	 * and driver.  */
	r = amdgpu_bo_create_kernel_at(adev, 0, adev->mman.stolen_vga_size,
				       AMDGPU_GEM_DOMAIN_VRAM,
				       &adev->mman.stolen_vga_memory,
				       NULL);
	if (r)
		return r;
	r = amdgpu_bo_create_kernel_at(adev, adev->mman.stolen_vga_size,
				       adev->mman.stolen_extended_size,
				       AMDGPU_GEM_DOMAIN_VRAM,
				       &adev->mman.stolen_extended_memory,
				       NULL);
	if (r)
		return r;
	r = amdgpu_bo_create_kernel_at(adev, adev->mman.stolen_reserved_offset,
				       adev->mman.stolen_reserved_size,
				       AMDGPU_GEM_DOMAIN_VRAM,
				       &adev->mman.stolen_reserved_memory,
				       NULL);
	if (r)
		return r;

	DRM_INFO("amdgpu: %uM of VRAM memory ready\n",
		 (unsigned) (adev->gmc.real_vram_size / (1024 * 1024)));

	/* Compute GTT size, either bsaed on 3/4th the size of RAM size
	 * or whatever the user passed on module init */
	if (amdgpu_gtt_size == -1) {
		struct sysinfo si;

		si_meminfo(&si);
		gtt_size = max3((AMDGPU_DEFAULT_GTT_SIZE_MB << 20),
				adev->gmc.mc_vram_size,
				((uint64_t)si.totalram * si.mem_unit));
	} else
		gtt_size = (uint64_t)amdgpu_gtt_size << 20;

	/* reserve for DGMA import domain */
	gtt_size -= (uint64_t)amdgpu_direct_gma_size << 20;

	/* Initialize GTT memory pool */
	r = amdgpu_gtt_mgr_init(adev, gtt_size);
	if (r) {
		DRM_ERROR("Failed initializing GTT heap.\n");
		return r;
	}
	DRM_INFO("amdgpu: %uM of GTT memory ready.\n",
		 (unsigned)(gtt_size / (1024 * 1024)));

	amdgpu_direct_gma_init(adev);
	amdgpu_ssg_init(adev);

	/* Initialize preemptible memory pool */
	r = amdgpu_preempt_mgr_init(adev);
	if (r) {
		DRM_ERROR("Failed initializing PREEMPT heap.\n");
		return r;
	}

	/* Initialize various on-chip memory pools */
	r = amdgpu_ttm_init_on_chip(adev, AMDGPU_PL_GDS, adev->gds.gds_size);
	if (r) {
		DRM_ERROR("Failed initializing GDS heap.\n");
		return r;
	}

	r = amdgpu_ttm_init_on_chip(adev, AMDGPU_PL_GWS, adev->gds.gws_size);
	if (r) {
		DRM_ERROR("Failed initializing gws heap.\n");
		return r;
	}

	r = amdgpu_ttm_init_on_chip(adev, AMDGPU_PL_OA, adev->gds.oa_size);
	if (r) {
		DRM_ERROR("Failed initializing oa heap.\n");
		return r;
	}

	if (amdgpu_bo_create_kernel(adev, PAGE_SIZE, PAGE_SIZE,
				AMDGPU_GEM_DOMAIN_GTT,
				&adev->mman.sdma_access_bo, NULL,
				&adev->mman.sdma_access_ptr))
		DRM_WARN("Debug VRAM access will use slowpath MM access\n");

	return 0;
}

/*
 * amdgpu_ttm_fini - De-initialize the TTM memory pools
 */
void amdgpu_ttm_fini(struct amdgpu_device *adev)
{
	int idx;
	if (!adev->mman.initialized)
		return;

	amdgpu_ttm_training_reserve_vram_fini(adev);
	/* return the stolen vga memory back to VRAM */
	amdgpu_bo_free_kernel(&adev->mman.stolen_vga_memory, NULL, NULL);
	amdgpu_bo_free_kernel(&adev->mman.stolen_extended_memory, NULL, NULL);
	/* return the IP Discovery TMR memory back to VRAM */
	amdgpu_bo_free_kernel(&adev->mman.discovery_memory, NULL, NULL);
	if (adev->mman.stolen_reserved_size)
		amdgpu_bo_free_kernel(&adev->mman.stolen_reserved_memory,
				      NULL, NULL);
	amdgpu_bo_free_kernel(&adev->mman.sdma_access_bo, NULL,
					&adev->mman.sdma_access_ptr);
	amdgpu_ttm_fw_reserve_vram_fini(adev);

	if (drm_dev_enter(adev_to_drm(adev), &idx)) {

		if (adev->mman.aper_base_kaddr)
			iounmap(adev->mman.aper_base_kaddr);
		adev->mman.aper_base_kaddr = NULL;

		drm_dev_exit(idx);
	}

	amdgpu_ssg_fini(adev);
	amdgpu_direct_gma_fini(adev);

	amdgpu_vram_mgr_fini(adev);
	amdgpu_gtt_mgr_fini(adev);
	amdgpu_preempt_mgr_fini(adev);
	ttm_range_man_fini(&adev->mman.bdev, AMDGPU_PL_GDS);
	ttm_range_man_fini(&adev->mman.bdev, AMDGPU_PL_GWS);
	ttm_range_man_fini(&adev->mman.bdev, AMDGPU_PL_OA);
	ttm_device_fini(&adev->mman.bdev);
	adev->mman.initialized = false;
	DRM_INFO("amdgpu: ttm finalized\n");
}

/**
 * amdgpu_ttm_set_buffer_funcs_status - enable/disable use of buffer functions
 *
 * @adev: amdgpu_device pointer
 * @enable: true when we can use buffer functions.
 *
 * Enable/disable use of buffer functions during suspend/resume. This should
 * only be called at bootup or when userspace isn't running.
 */
void amdgpu_ttm_set_buffer_funcs_status(struct amdgpu_device *adev, bool enable)
{
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev, TTM_PL_VRAM);
	uint64_t size;
	int r;

	if (!adev->mman.initialized || amdgpu_in_reset(adev) ||
	    adev->mman.buffer_funcs_enabled == enable)
		return;

	if (enable) {
		struct amdgpu_ring *ring;
		struct drm_gpu_scheduler *sched;

		ring = adev->mman.buffer_funcs_ring;
		sched = &ring->sched;
		r = drm_sched_entity_init(&adev->mman.entity,
					  DRM_SCHED_PRIORITY_KERNEL, &sched,
					  1, NULL);
		if (r) {
			DRM_ERROR("Failed setting up TTM BO move entity (%d)\n",
				  r);
			return;
		}
	} else {
		drm_sched_entity_destroy(&adev->mman.entity);
		dma_fence_put(man->move);
		man->move = NULL;
	}

	/* this just adjusts TTM size idea, which sets lpfn to the correct value */
	if (enable)
		size = adev->gmc.real_vram_size;
	else
		size = adev->gmc.visible_vram_size;
	man->size = size >> PAGE_SHIFT;
	adev->mman.buffer_funcs_enabled = enable;
}

#ifdef HAVE_STRUCT_DRM_DRV_GEM_OPEN_OBJECT_CALLBACK
#ifndef HAVE_VM_OPERATIONS_STRUCT_FAULT_1ARG
static vm_fault_t amdgpu_ttm_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
        struct ttm_buffer_object *bo = vma->vm_private_data;
#else
static vm_fault_t amdgpu_ttm_fault(struct vm_fault *vmf)
{
        struct ttm_buffer_object *bo = vmf->vma->vm_private_data;
#endif
        vm_fault_t ret;

#ifndef HAVE_VM_OPERATIONS_STRUCT_FAULT_1ARG
        ret = ttm_bo_vm_reserve(bo, vmf, vma);
#else
        ret = ttm_bo_vm_reserve(bo, vmf);
#endif
        if (ret)
                return ret;

        ret = amdgpu_bo_fault_reserve_notify(bo);
        if (ret)
                goto unlock;

#ifndef HAVE_VM_OPERATIONS_STRUCT_FAULT_1ARG
        ret = ttm_bo_vm_fault_reserved(vmf, vma, vma->vm_page_prot, TTM_BO_VM_NUM_PREFAULT, 1);
#else
        ret = ttm_bo_vm_fault_reserved(vmf, vmf->vma->vm_page_prot,
                                       TTM_BO_VM_NUM_PREFAULT, 1);
#endif
        if (ret == VM_FAULT_RETRY && !(vmf->flags & FAULT_FLAG_RETRY_NOWAIT))
                return ret;

unlock:
        dma_resv_unlock(amdkcl_ttm_resvp(bo));
        return ret;
}

static struct vm_operations_struct amdgpu_ttm_vm_ops = {
        .fault = amdgpu_ttm_fault,
        .open = ttm_bo_vm_open,
        .close = ttm_bo_vm_close,
        .access = ttm_bo_vm_access
};

int amdgpu_mmap(struct file *filp, struct vm_area_struct *vma)
{
        struct drm_file *file_priv = filp->private_data;
        struct amdgpu_device *adev = drm_to_adev(file_priv->minor->dev);
        int r;

        r = ttm_bo_mmap(filp, vma, &adev->mman.bdev);
        if (unlikely(r != 0))
                return r;

        vma->vm_ops = &amdgpu_ttm_vm_ops;
        return 0;
}
#endif /* HAVE_STRUCT_DRM_DRV_GEM_OPEN_OBJECT_CALLBACK */

static int amdgpu_ttm_prepare_job(struct amdgpu_device *adev,
				  bool direct_submit,
				  unsigned int num_dw,
				  struct dma_resv *resv,
				  bool vm_needs_flush,
				  struct amdgpu_job **job)
{
	enum amdgpu_ib_pool_type pool = direct_submit ?
		AMDGPU_IB_POOL_DIRECT :
		AMDGPU_IB_POOL_DELAYED;
	int r;

	r = amdgpu_job_alloc_with_ib(adev, num_dw * 4, pool, job);
	if (r)
		return r;

	if (vm_needs_flush) {
		(*job)->vm_pd_addr = amdgpu_gmc_pd_addr(adev->gmc.pdb0_bo ?
							adev->gmc.pdb0_bo :
							adev->gart.bo);
		(*job)->vm_needs_flush = true;
	}
	if (resv) {
		r = amdgpu_sync_resv(adev, &(*job)->sync, resv,
				     AMDGPU_SYNC_ALWAYS,
				     AMDGPU_FENCE_OWNER_UNDEFINED);
		if (r) {
			DRM_ERROR("sync failed (%d).\n", r);
			amdgpu_job_free(*job);
			return r;
		}
	}
	return 0;
}

int amdgpu_copy_buffer(struct amdgpu_ring *ring, uint64_t src_offset,
		       uint64_t dst_offset, uint32_t byte_count,
		       struct dma_resv *resv,
		       struct dma_fence **fence, bool direct_submit,
		       bool vm_needs_flush, bool tmz)
{
	struct amdgpu_device *adev = ring->adev;
	unsigned num_loops, num_dw;
	struct amdgpu_job *job;
	uint32_t max_bytes;
	unsigned i;
	int r;

	if (!direct_submit && !ring->sched.ready) {
		DRM_ERROR("Trying to move memory with ring turned off.\n");
		return -EINVAL;
	}

	max_bytes = adev->mman.buffer_funcs->copy_max_bytes;
	num_loops = DIV_ROUND_UP(byte_count, max_bytes);
	num_dw = ALIGN(num_loops * adev->mman.buffer_funcs->copy_num_dw, 8);
	r = amdgpu_ttm_prepare_job(adev, direct_submit, num_dw,
				   resv, vm_needs_flush, &job);
	if (r)
		return r;

	for (i = 0; i < num_loops; i++) {
		uint32_t cur_size_in_bytes = min(byte_count, max_bytes);

		amdgpu_emit_copy_buffer(adev, &job->ibs[0], src_offset,
					dst_offset, cur_size_in_bytes, tmz);

		src_offset += cur_size_in_bytes;
		dst_offset += cur_size_in_bytes;
		byte_count -= cur_size_in_bytes;
	}

	amdgpu_ring_pad_ib(ring, &job->ibs[0]);
	WARN_ON(job->ibs[0].length_dw > num_dw);
	if (direct_submit)
		r = amdgpu_job_submit_direct(job, ring, fence);
	else
		r = amdgpu_job_submit(job, &adev->mman.entity,
				      AMDGPU_FENCE_OWNER_UNDEFINED, fence);
	if (r)
		goto error_free;

	return r;

error_free:
	amdgpu_job_free(job);
	DRM_ERROR("Error scheduling IBs (%d)\n", r);
	return r;
}

static int amdgpu_ttm_fill_mem(struct amdgpu_ring *ring, uint32_t src_data,
			       uint64_t dst_addr, uint32_t byte_count,
			       struct dma_resv *resv,
			       struct dma_fence **fence,
			       bool vm_needs_flush)
{
	struct amdgpu_device *adev = ring->adev;
	unsigned int num_loops, num_dw;
	struct amdgpu_job *job;
	uint32_t max_bytes;
	unsigned int i;
	int r;

	max_bytes = adev->mman.buffer_funcs->fill_max_bytes;
	num_loops = DIV_ROUND_UP_ULL(byte_count, max_bytes);
	num_dw = ALIGN(num_loops * adev->mman.buffer_funcs->fill_num_dw, 8);
	r = amdgpu_ttm_prepare_job(adev, false, num_dw, resv, vm_needs_flush,
				   &job);
	if (r)
		return r;

	for (i = 0; i < num_loops; i++) {
		uint32_t cur_size = min(byte_count, max_bytes);

		amdgpu_emit_fill_buffer(adev, &job->ibs[0], src_data, dst_addr,
					cur_size);

		dst_addr += cur_size;
		byte_count -= cur_size;
	}

	amdgpu_ring_pad_ib(ring, &job->ibs[0]);
	WARN_ON(job->ibs[0].length_dw > num_dw);
	r = amdgpu_job_submit(job, &adev->mman.entity,
			      AMDGPU_FENCE_OWNER_UNDEFINED, fence);
	if (r)
		goto error_free;

	return 0;

error_free:
	amdgpu_job_free(job);
	return r;
}

int amdgpu_fill_buffer(struct amdgpu_bo *bo,
			uint32_t src_data,
			struct dma_resv *resv,
			struct dma_fence **f)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->tbo.bdev);
	struct amdgpu_ring *ring = adev->mman.buffer_funcs_ring;
	struct dma_fence *fence = NULL;
	struct amdgpu_res_cursor dst;
	int r;

	if (!adev->mman.buffer_funcs_enabled) {
		DRM_ERROR("Trying to clear memory with ring turned off.\n");
		return -EINVAL;
	}

	amdgpu_res_first(bo->tbo.resource, 0, amdgpu_bo_size(bo), &dst);

	mutex_lock(&adev->mman.gtt_window_lock);
	while (dst.remaining) {
		struct dma_fence *next;
		uint64_t cur_size, to;

		/* Never fill more than 256MiB at once to avoid timeouts */
		cur_size = min(dst.size, 256ULL << 20);

		r = amdgpu_ttm_map_buffer(&bo->tbo, bo->tbo.resource, &dst,
					  1, ring, false, &cur_size, &to);
		if (r)
			goto error;

		r = amdgpu_ttm_fill_mem(ring, src_data, to, cur_size, resv,
					&next, true);
		if (r)
			goto error;

		dma_fence_put(fence);
		fence = next;

		amdgpu_res_next(&dst, cur_size);
	}
error:
	mutex_unlock(&adev->mman.gtt_window_lock);
	if (f)
		*f = dma_fence_get(fence);
	dma_fence_put(fence);
	return r;
}

/**
 * amdgpu_ttm_evict_resources - evict memory buffers
 * @adev: amdgpu device object
 * @mem_type: evicted BO's memory type
 *
 * Evicts all @mem_type buffers on the lru list of the memory type.
 *
 * Returns:
 * 0 for success or a negative error code on failure.
 */
int amdgpu_ttm_evict_resources(struct amdgpu_device *adev, int mem_type)
{
	struct ttm_resource_manager *man;

	switch (mem_type) {
	case TTM_PL_VRAM:
	case TTM_PL_TT:
	case AMDGPU_PL_GWS:
	case AMDGPU_PL_GDS:
	case AMDGPU_PL_OA:
		man = ttm_manager_type(&adev->mman.bdev, mem_type);
		break;
	default:
		DRM_ERROR("Trying to evict invalid memory type\n");
		return -EINVAL;
	}

	return ttm_resource_manager_evict_all(&adev->mman.bdev, man);
}

#if defined(CONFIG_DEBUG_FS)
static int amdgpu_mm_vram_table_show(struct seq_file *m, void *unused)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)m->private;
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev,
							    TTM_PL_VRAM);
	struct drm_printer p = drm_seq_file_printer(m);
	man->func->debug(man, &p);
	return 0;
}

static int amdgpu_ttm_page_pool_show(struct seq_file *m, void *unused)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)m->private;

	return ttm_pool_debugfs(&adev->mman.bdev.pool, m);
}

static int amdgpu_mm_tt_table_show(struct seq_file *m, void *unused)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)m->private;
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev,
							    TTM_PL_TT);
	struct drm_printer p = drm_seq_file_printer(m);
	man->func->debug(man, &p);
	return 0;
}

static int amdgpu_mm_gds_table_show(struct seq_file *m, void *unused)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)m->private;
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev,
							    AMDGPU_PL_GDS);
	struct drm_printer p = drm_seq_file_printer(m);
	man->func->debug(man, &p);
	return 0;
}

static int amdgpu_mm_gws_table_show(struct seq_file *m, void *unused)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)m->private;
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev,
							    AMDGPU_PL_GWS);
	struct drm_printer p = drm_seq_file_printer(m);
	man->func->debug(man, &p);
	return 0;
}

static int amdgpu_mm_oa_table_show(struct seq_file *m, void *unused)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)m->private;
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev,
							    AMDGPU_PL_OA);
	struct drm_printer p = drm_seq_file_printer(m);
	man->func->debug(man, &p);
	return 0;
}

static int amdgpu_mm_dgma_table_show(struct seq_file *m, void *unused)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)m->private;
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev,
							    AMDGPU_PL_DGMA);
	struct drm_printer p = drm_seq_file_printer(m);

	man->func->debug(man, &p);
	return 0;
}

static int amdgpu_mm_dgma_import_table_show(struct seq_file *m, void *unused)
{
	struct amdgpu_device *adev = (struct amdgpu_device *)m->private;
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev,
							    AMDGPU_PL_DGMA_IMPORT);
	struct drm_printer p = drm_seq_file_printer(m);

	man->func->debug(man, &p);
	return 0;
}

DEFINE_SHOW_ATTRIBUTE(amdgpu_mm_vram_table);
DEFINE_SHOW_ATTRIBUTE(amdgpu_mm_tt_table);
DEFINE_SHOW_ATTRIBUTE(amdgpu_mm_gds_table);
DEFINE_SHOW_ATTRIBUTE(amdgpu_mm_gws_table);
DEFINE_SHOW_ATTRIBUTE(amdgpu_mm_oa_table);
DEFINE_SHOW_ATTRIBUTE(amdgpu_ttm_page_pool);
DEFINE_SHOW_ATTRIBUTE(amdgpu_mm_dgma_table);
DEFINE_SHOW_ATTRIBUTE(amdgpu_mm_dgma_import_table);

/*
 * amdgpu_ttm_vram_read - Linear read access to VRAM
 *
 * Accesses VRAM via MMIO for debugging purposes.
 */
static ssize_t amdgpu_ttm_vram_read(struct file *f, char __user *buf,
				    size_t size, loff_t *pos)
{
	struct amdgpu_device *adev = file_inode(f)->i_private;
	ssize_t result = 0;

	if (size & 0x3 || *pos & 0x3)
		return -EINVAL;

	if (*pos >= adev->gmc.mc_vram_size)
		return -ENXIO;

	size = min(size, (size_t)(adev->gmc.mc_vram_size - *pos));
	while (size) {
		size_t bytes = min(size, AMDGPU_TTM_VRAM_MAX_DW_READ * 4);
		uint32_t value[AMDGPU_TTM_VRAM_MAX_DW_READ];

		amdgpu_device_vram_access(adev, *pos, value, bytes, false);
		if (copy_to_user(buf, value, bytes))
			return -EFAULT;

		result += bytes;
		buf += bytes;
		*pos += bytes;
		size -= bytes;
	}

	return result;
}

/*
 * amdgpu_ttm_vram_write - Linear write access to VRAM
 *
 * Accesses VRAM via MMIO for debugging purposes.
 */
static ssize_t amdgpu_ttm_vram_write(struct file *f, const char __user *buf,
				    size_t size, loff_t *pos)
{
	struct amdgpu_device *adev = file_inode(f)->i_private;
	ssize_t result = 0;
	int r;

	if (size & 0x3 || *pos & 0x3)
		return -EINVAL;

	if (*pos >= adev->gmc.mc_vram_size)
		return -ENXIO;

	while (size) {
		uint32_t value;

		if (*pos >= adev->gmc.mc_vram_size)
			return result;

		r = get_user(value, (uint32_t *)buf);
		if (r)
			return r;

		amdgpu_device_mm_access(adev, *pos, &value, 4, true);

		result += 4;
		buf += 4;
		*pos += 4;
		size -= 4;
	}

	return result;
}

static const struct file_operations amdgpu_ttm_vram_fops = {
	.owner = THIS_MODULE,
	.read = amdgpu_ttm_vram_read,
	.write = amdgpu_ttm_vram_write,
	.llseek = default_llseek,
};

/*
 * amdgpu_iomem_read - Virtual read access to GPU mapped memory
 *
 * This function is used to read memory that has been mapped to the
 * GPU and the known addresses are not physical addresses but instead
 * bus addresses (e.g., what you'd put in an IB or ring buffer).
 */
static ssize_t amdgpu_iomem_read(struct file *f, char __user *buf,
				 size_t size, loff_t *pos)
{
	struct amdgpu_device *adev = file_inode(f)->i_private;
	struct iommu_domain *dom;
	ssize_t result = 0;
	int r;

	/* retrieve the IOMMU domain if any for this device */
	dom = iommu_get_domain_for_dev(adev->dev);

	while (size) {
		phys_addr_t addr = *pos & PAGE_MASK;
		loff_t off = *pos & ~PAGE_MASK;
		size_t bytes = PAGE_SIZE - off;
		unsigned long pfn;
		struct page *p;
		void *ptr;

		bytes = bytes < size ? bytes : size;

		/* Translate the bus address to a physical address.  If
		 * the domain is NULL it means there is no IOMMU active
		 * and the address translation is the identity
		 */
		addr = dom ? iommu_iova_to_phys(dom, addr) : addr;

		pfn = addr >> PAGE_SHIFT;
		if (!pfn_valid(pfn))
			return -EPERM;

		p = pfn_to_page(pfn);
		if (p->mapping != adev->mman.bdev.dev_mapping)
			return -EPERM;

		ptr = kmap(p);
		r = copy_to_user(buf, ptr + off, bytes);
		kunmap(p);
		if (r)
			return -EFAULT;

		size -= bytes;
		*pos += bytes;
		result += bytes;
	}

	return result;
}

/*
 * amdgpu_iomem_write - Virtual write access to GPU mapped memory
 *
 * This function is used to write memory that has been mapped to the
 * GPU and the known addresses are not physical addresses but instead
 * bus addresses (e.g., what you'd put in an IB or ring buffer).
 */
static ssize_t amdgpu_iomem_write(struct file *f, const char __user *buf,
				 size_t size, loff_t *pos)
{
	struct amdgpu_device *adev = file_inode(f)->i_private;
	struct iommu_domain *dom;
	ssize_t result = 0;
	int r;

	dom = iommu_get_domain_for_dev(adev->dev);

	while (size) {
		phys_addr_t addr = *pos & PAGE_MASK;
		loff_t off = *pos & ~PAGE_MASK;
		size_t bytes = PAGE_SIZE - off;
		unsigned long pfn;
		struct page *p;
		void *ptr;

		bytes = bytes < size ? bytes : size;

		addr = dom ? iommu_iova_to_phys(dom, addr) : addr;

		pfn = addr >> PAGE_SHIFT;
		if (!pfn_valid(pfn))
			return -EPERM;

		p = pfn_to_page(pfn);
		if (p->mapping != adev->mman.bdev.dev_mapping)
			return -EPERM;

		ptr = kmap(p);
		r = copy_from_user(ptr + off, buf, bytes);
		kunmap(p);
		if (r)
			return -EFAULT;

		size -= bytes;
		*pos += bytes;
		result += bytes;
	}

	return result;
}

static const struct file_operations amdgpu_ttm_iomem_fops = {
	.owner = THIS_MODULE,
	.read = amdgpu_iomem_read,
	.write = amdgpu_iomem_write,
	.llseek = default_llseek
};

#endif

void amdgpu_ttm_debugfs_init(struct amdgpu_device *adev)
{
#if defined(CONFIG_DEBUG_FS)
	struct drm_minor *minor = adev_to_drm(adev)->primary;
	struct dentry *root = minor->debugfs_root;

	debugfs_create_file_size("amdgpu_vram", 0444, root, adev,
				 &amdgpu_ttm_vram_fops, adev->gmc.mc_vram_size);
	debugfs_create_file("amdgpu_iomem", 0444, root, adev,
			    &amdgpu_ttm_iomem_fops);
	debugfs_create_file("amdgpu_vram_mm", 0444, root, adev,
			    &amdgpu_mm_vram_table_fops);
	debugfs_create_file("amdgpu_gtt_mm", 0444, root, adev,
			    &amdgpu_mm_tt_table_fops);
	debugfs_create_file("amdgpu_gds_mm", 0444, root, adev,
			    &amdgpu_mm_gds_table_fops);
	debugfs_create_file("amdgpu_gws_mm", 0444, root, adev,
			    &amdgpu_mm_gws_table_fops);
	debugfs_create_file("amdgpu_oa_mm", 0444, root, adev,
			    &amdgpu_mm_oa_table_fops);
	debugfs_create_file("ttm_page_pool", 0444, root, adev,
			    &amdgpu_ttm_page_pool_fops);

	if (amdgpu_direct_gma_size) {
		debugfs_create_file("amdgpu_dgma_mm", 0444, root, adev,
			    &amdgpu_mm_dgma_table_fops);
		debugfs_create_file("amdgpu_dgma_import_mm", 0444, root, adev,
			    &amdgpu_mm_dgma_import_table_fops);
	}
#endif
}
