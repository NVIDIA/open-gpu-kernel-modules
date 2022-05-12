/*
 * Copyright 2019 Advanced Micro Devices, Inc.
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

#include "amdgpu_vm.h"
#include "amdgpu_job.h"
#include "amdgpu_object.h"
#include "amdgpu_trace.h"

#define AMDGPU_VM_SDMA_MIN_NUM_DW	256u
#define AMDGPU_VM_SDMA_MAX_NUM_DW	(16u * 1024u)

/**
 * amdgpu_vm_sdma_map_table - make sure new PDs/PTs are GTT mapped
 *
 * @table: newly allocated or validated PD/PT
 */
static int amdgpu_vm_sdma_map_table(struct amdgpu_bo_vm *table)
{
	int r;

	r = amdgpu_ttm_alloc_gart(&table->bo.tbo);
	if (r)
		return r;

	if (table->shadow)
		r = amdgpu_ttm_alloc_gart(&table->shadow->tbo);

	return r;
}

/**
 * amdgpu_vm_sdma_prepare - prepare SDMA command submission
 *
 * @p: see amdgpu_vm_update_params definition
 * @resv: reservation object with embedded fence
 * @sync_mode: synchronization mode
 *
 * Returns:
 * Negativ errno, 0 for success.
 */
static int amdgpu_vm_sdma_prepare(struct amdgpu_vm_update_params *p,
				  struct dma_resv *resv,
				  enum amdgpu_sync_mode sync_mode)
{
	enum amdgpu_ib_pool_type pool = p->immediate ? AMDGPU_IB_POOL_IMMEDIATE
		: AMDGPU_IB_POOL_DELAYED;
	unsigned int ndw = AMDGPU_VM_SDMA_MIN_NUM_DW;
	int r;

	r = amdgpu_job_alloc_with_ib(p->adev, ndw * 4, pool, &p->job);
	if (r)
		return r;

	p->num_dw_left = ndw;

	if (!resv)
		return 0;

	return amdgpu_sync_resv(p->adev, &p->job->sync, resv, sync_mode, p->vm);
}

/**
 * amdgpu_vm_sdma_commit - commit SDMA command submission
 *
 * @p: see amdgpu_vm_update_params definition
 * @fence: resulting fence
 *
 * Returns:
 * Negativ errno, 0 for success.
 */
static int amdgpu_vm_sdma_commit(struct amdgpu_vm_update_params *p,
				 struct dma_fence **fence)
{
	struct amdgpu_ib *ib = p->job->ibs;
	struct drm_sched_entity *entity;
	struct amdgpu_ring *ring;
	struct dma_fence *f;
	int r;

	entity = p->immediate ? &p->vm->immediate : &p->vm->delayed;
	ring = container_of(entity->rq->sched, struct amdgpu_ring, sched);

	WARN_ON(ib->length_dw == 0);
	amdgpu_ring_pad_ib(ring, ib);
	WARN_ON(ib->length_dw > p->num_dw_left);
	r = amdgpu_job_submit(p->job, entity, AMDGPU_FENCE_OWNER_VM, &f);
	if (r)
		goto error;

	if (p->unlocked) {
		struct dma_fence *tmp = dma_fence_get(f);

		swap(p->vm->last_unlocked, f);
		dma_fence_put(tmp);
	} else {
		amdgpu_bo_fence(p->vm->root.bo, f, true);
	}

	if (fence && !p->immediate)
		swap(*fence, f);
	dma_fence_put(f);
	return 0;

error:
	amdgpu_job_free(p->job);
	return r;
}

/**
 * amdgpu_vm_sdma_copy_ptes - copy the PTEs from mapping
 *
 * @p: see amdgpu_vm_update_params definition
 * @bo: PD/PT to update
 * @pe: addr of the page entry
 * @count: number of page entries to copy
 *
 * Traces the parameters and calls the DMA function to copy the PTEs.
 */
static void amdgpu_vm_sdma_copy_ptes(struct amdgpu_vm_update_params *p,
				     struct amdgpu_bo *bo, uint64_t pe,
				     unsigned count)
{
	struct amdgpu_ib *ib = p->job->ibs;
	uint64_t src = ib->gpu_addr;

	src += p->num_dw_left * 4;

	pe += amdgpu_gmc_sign_extend(amdgpu_bo_gpu_offset_no_check(bo));
	trace_amdgpu_vm_copy_ptes(pe, src, count, p->immediate);

	amdgpu_vm_copy_pte(p->adev, ib, pe, src, count);
}

/**
 * amdgpu_vm_sdma_set_ptes - helper to call the right asic function
 *
 * @p: see amdgpu_vm_update_params definition
 * @bo: PD/PT to update
 * @pe: byte offset of the PDE/PTE, relative to start of PDB/PTB
 * @addr: dst addr to write into pe
 * @count: number of page entries to update
 * @incr: increase next addr by incr bytes
 * @flags: hw access flags
 *
 * Traces the parameters and calls the right asic functions
 * to setup the page table using the DMA.
 */
static void amdgpu_vm_sdma_set_ptes(struct amdgpu_vm_update_params *p,
				    struct amdgpu_bo *bo, uint64_t pe,
				    uint64_t addr, unsigned count,
				    uint32_t incr, uint64_t flags)
{
	struct amdgpu_ib *ib = p->job->ibs;

	pe += amdgpu_gmc_sign_extend(amdgpu_bo_gpu_offset_no_check(bo));
	trace_amdgpu_vm_set_ptes(pe, addr, count, incr, flags, p->immediate);
	if (count < 3) {
		amdgpu_vm_write_pte(p->adev, ib, pe, addr | flags,
				    count, incr);
	} else {
		amdgpu_vm_set_pte_pde(p->adev, ib, pe, addr,
				      count, incr, flags);
	}
}

/**
 * amdgpu_vm_sdma_update - execute VM update
 *
 * @p: see amdgpu_vm_update_params definition
 * @vmbo: PD/PT to update
 * @pe: byte offset of the PDE/PTE, relative to start of PDB/PTB
 * @addr: dst addr to write into pe
 * @count: number of page entries to update
 * @incr: increase next addr by incr bytes
 * @flags: hw access flags
 *
 * Reserve space in the IB, setup mapping buffer on demand and write commands to
 * the IB.
 */
static int amdgpu_vm_sdma_update(struct amdgpu_vm_update_params *p,
				 struct amdgpu_bo_vm *vmbo, uint64_t pe,
				 uint64_t addr, unsigned count, uint32_t incr,
				 uint64_t flags)
{
	struct amdgpu_bo *bo = &vmbo->bo;
	enum amdgpu_ib_pool_type pool = p->immediate ? AMDGPU_IB_POOL_IMMEDIATE
		: AMDGPU_IB_POOL_DELAYED;
	unsigned int i, ndw, nptes;
	uint64_t *pte;
	int r;

	/* Wait for PD/PT moves to be completed */
	r = amdgpu_sync_fence(&p->job->sync, bo->tbo.moving);
	if (r)
		return r;

	do {
		ndw = p->num_dw_left;
		ndw -= p->job->ibs->length_dw;

		if (ndw < 32) {
			r = amdgpu_vm_sdma_commit(p, NULL);
			if (r)
				return r;

			/* estimate how many dw we need */
			ndw = 32;
			if (p->pages_addr)
				ndw += count * 2;
			ndw = max(ndw, AMDGPU_VM_SDMA_MIN_NUM_DW);
			ndw = min(ndw, AMDGPU_VM_SDMA_MAX_NUM_DW);

			r = amdgpu_job_alloc_with_ib(p->adev, ndw * 4, pool,
						     &p->job);
			if (r)
				return r;

			p->num_dw_left = ndw;
		}

		if (!p->pages_addr) {
			/* set page commands needed */
			if (vmbo->shadow)
				amdgpu_vm_sdma_set_ptes(p, vmbo->shadow, pe, addr,
							count, incr, flags);
			amdgpu_vm_sdma_set_ptes(p, bo, pe, addr, count,
						incr, flags);
			return 0;
		}

		/* copy commands needed */
		ndw -= p->adev->vm_manager.vm_pte_funcs->copy_pte_num_dw *
			(vmbo->shadow ? 2 : 1);

		/* for padding */
		ndw -= 7;

		nptes = min(count, ndw / 2);

		/* Put the PTEs at the end of the IB. */
		p->num_dw_left -= nptes * 2;
		pte = (uint64_t *)&(p->job->ibs->ptr[p->num_dw_left]);
		for (i = 0; i < nptes; ++i, addr += incr) {
			pte[i] = amdgpu_vm_map_gart(p->pages_addr, addr);
			pte[i] |= flags;
		}

		if (vmbo->shadow)
			amdgpu_vm_sdma_copy_ptes(p, vmbo->shadow, pe, nptes);
		amdgpu_vm_sdma_copy_ptes(p, bo, pe, nptes);

		pe += nptes * 8;
		count -= nptes;
	} while (count);

	return 0;
}

const struct amdgpu_vm_update_funcs amdgpu_vm_sdma_funcs = {
	.map_table = amdgpu_vm_sdma_map_table,
	.prepare = amdgpu_vm_sdma_prepare,
	.update = amdgpu_vm_sdma_update,
	.commit = amdgpu_vm_sdma_commit
};
