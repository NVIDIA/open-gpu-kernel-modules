// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (c) 2017 The Linux Foundation. All rights reserved.
 */

#include "msm_gem.h"
#include "a5xx_gpu.h"

/*
 * Try to transition the preemption state from old to new. Return
 * true on success or false if the original state wasn't 'old'
 */
static inline bool try_preempt_state(struct a5xx_gpu *a5xx_gpu,
		enum preempt_state old, enum preempt_state new)
{
	enum preempt_state cur = atomic_cmpxchg(&a5xx_gpu->preempt_state,
		old, new);

	return (cur == old);
}

/*
 * Force the preemption state to the specified state.  This is used in cases
 * where the current state is known and won't change
 */
static inline void set_preempt_state(struct a5xx_gpu *gpu,
		enum preempt_state new)
{
	/*
	 * preempt_state may be read by other cores trying to trigger a
	 * preemption or in the interrupt handler so barriers are needed
	 * before...
	 */
	smp_mb__before_atomic();
	atomic_set(&gpu->preempt_state, new);
	/* ... and after*/
	smp_mb__after_atomic();
}

/* Write the most recent wptr for the given ring into the hardware */
static inline void update_wptr(struct msm_gpu *gpu, struct msm_ringbuffer *ring)
{
	unsigned long flags;
	uint32_t wptr;

	if (!ring)
		return;

	spin_lock_irqsave(&ring->preempt_lock, flags);
	wptr = get_wptr(ring);
	spin_unlock_irqrestore(&ring->preempt_lock, flags);

	gpu_write(gpu, REG_A5XX_CP_RB_WPTR, wptr);
}

/* Return the highest priority ringbuffer with something in it */
static struct msm_ringbuffer *get_next_ring(struct msm_gpu *gpu)
{
	unsigned long flags;
	int i;

	for (i = 0; i < gpu->nr_rings; i++) {
		bool empty;
		struct msm_ringbuffer *ring = gpu->rb[i];

		spin_lock_irqsave(&ring->preempt_lock, flags);
		empty = (get_wptr(ring) == ring->memptrs->rptr);
		spin_unlock_irqrestore(&ring->preempt_lock, flags);

		if (!empty)
			return ring;
	}

	return NULL;
}

static void a5xx_preempt_timer(struct timer_list *t)
{
	struct a5xx_gpu *a5xx_gpu = from_timer(a5xx_gpu, t, preempt_timer);
	struct msm_gpu *gpu = &a5xx_gpu->base.base;
	struct drm_device *dev = gpu->dev;

	if (!try_preempt_state(a5xx_gpu, PREEMPT_TRIGGERED, PREEMPT_FAULTED))
		return;

	DRM_DEV_ERROR(dev->dev, "%s: preemption timed out\n", gpu->name);
	kthread_queue_work(gpu->worker, &gpu->recover_work);
}

/* Try to trigger a preemption switch */
void a5xx_preempt_trigger(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	struct a5xx_gpu *a5xx_gpu = to_a5xx_gpu(adreno_gpu);
	unsigned long flags;
	struct msm_ringbuffer *ring;

	if (gpu->nr_rings == 1)
		return;

	/*
	 * Try to start preemption by moving from NONE to START. If
	 * unsuccessful, a preemption is already in flight
	 */
	if (!try_preempt_state(a5xx_gpu, PREEMPT_NONE, PREEMPT_START))
		return;

	/* Get the next ring to preempt to */
	ring = get_next_ring(gpu);

	/*
	 * If no ring is populated or the highest priority ring is the current
	 * one do nothing except to update the wptr to the latest and greatest
	 */
	if (!ring || (a5xx_gpu->cur_ring == ring)) {
		/*
		 * Its possible that while a preemption request is in progress
		 * from an irq context, a user context trying to submit might
		 * fail to update the write pointer, because it determines
		 * that the preempt state is not PREEMPT_NONE.
		 *
		 * Close the race by introducing an intermediate
		 * state PREEMPT_ABORT to let the submit path
		 * know that the ringbuffer is not going to change
		 * and can safely update the write pointer.
		 */

		set_preempt_state(a5xx_gpu, PREEMPT_ABORT);
		update_wptr(gpu, a5xx_gpu->cur_ring);
		set_preempt_state(a5xx_gpu, PREEMPT_NONE);
		return;
	}

	/* Make sure the wptr doesn't update while we're in motion */
	spin_lock_irqsave(&ring->preempt_lock, flags);
	a5xx_gpu->preempt[ring->id]->wptr = get_wptr(ring);
	spin_unlock_irqrestore(&ring->preempt_lock, flags);

	/* Set the address of the incoming preemption record */
	gpu_write64(gpu, REG_A5XX_CP_CONTEXT_SWITCH_RESTORE_ADDR_LO,
		REG_A5XX_CP_CONTEXT_SWITCH_RESTORE_ADDR_HI,
		a5xx_gpu->preempt_iova[ring->id]);

	a5xx_gpu->next_ring = ring;

	/* Start a timer to catch a stuck preemption */
	mod_timer(&a5xx_gpu->preempt_timer, jiffies + msecs_to_jiffies(10000));

	/* Set the preemption state to triggered */
	set_preempt_state(a5xx_gpu, PREEMPT_TRIGGERED);

	/* Make sure everything is written before hitting the button */
	wmb();

	/* And actually start the preemption */
	gpu_write(gpu, REG_A5XX_CP_CONTEXT_SWITCH_CNTL, 1);
}

void a5xx_preempt_irq(struct msm_gpu *gpu)
{
	uint32_t status;
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	struct a5xx_gpu *a5xx_gpu = to_a5xx_gpu(adreno_gpu);
	struct drm_device *dev = gpu->dev;

	if (!try_preempt_state(a5xx_gpu, PREEMPT_TRIGGERED, PREEMPT_PENDING))
		return;

	/* Delete the preemption watchdog timer */
	del_timer(&a5xx_gpu->preempt_timer);

	/*
	 * The hardware should be setting CP_CONTEXT_SWITCH_CNTL to zero before
	 * firing the interrupt, but there is a non zero chance of a hardware
	 * condition or a software race that could set it again before we have a
	 * chance to finish. If that happens, log and go for recovery
	 */
	status = gpu_read(gpu, REG_A5XX_CP_CONTEXT_SWITCH_CNTL);
	if (unlikely(status)) {
		set_preempt_state(a5xx_gpu, PREEMPT_FAULTED);
		DRM_DEV_ERROR(dev->dev, "%s: Preemption failed to complete\n",
			gpu->name);
		kthread_queue_work(gpu->worker, &gpu->recover_work);
		return;
	}

	a5xx_gpu->cur_ring = a5xx_gpu->next_ring;
	a5xx_gpu->next_ring = NULL;

	update_wptr(gpu, a5xx_gpu->cur_ring);

	set_preempt_state(a5xx_gpu, PREEMPT_NONE);
}

void a5xx_preempt_hw_init(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	struct a5xx_gpu *a5xx_gpu = to_a5xx_gpu(adreno_gpu);
	int i;

	/* Always come up on rb 0 */
	a5xx_gpu->cur_ring = gpu->rb[0];

	/* No preemption if we only have one ring */
	if (gpu->nr_rings == 1)
		return;

	for (i = 0; i < gpu->nr_rings; i++) {
		a5xx_gpu->preempt[i]->wptr = 0;
		a5xx_gpu->preempt[i]->rptr = 0;
		a5xx_gpu->preempt[i]->rbase = gpu->rb[i]->iova;
	}

	/* Write a 0 to signal that we aren't switching pagetables */
	gpu_write64(gpu, REG_A5XX_CP_CONTEXT_SWITCH_SMMU_INFO_LO,
		REG_A5XX_CP_CONTEXT_SWITCH_SMMU_INFO_HI, 0);

	/* Reset the preemption state */
	set_preempt_state(a5xx_gpu, PREEMPT_NONE);
}

static int preempt_init_ring(struct a5xx_gpu *a5xx_gpu,
		struct msm_ringbuffer *ring)
{
	struct adreno_gpu *adreno_gpu = &a5xx_gpu->base;
	struct msm_gpu *gpu = &adreno_gpu->base;
	struct a5xx_preempt_record *ptr;
	void *counters;
	struct drm_gem_object *bo = NULL, *counters_bo = NULL;
	u64 iova = 0, counters_iova = 0;

	ptr = msm_gem_kernel_new(gpu->dev,
		A5XX_PREEMPT_RECORD_SIZE + A5XX_PREEMPT_COUNTER_SIZE,
		MSM_BO_WC | MSM_BO_MAP_PRIV, gpu->aspace, &bo, &iova);

	if (IS_ERR(ptr))
		return PTR_ERR(ptr);

	/* The buffer to store counters needs to be unprivileged */
	counters = msm_gem_kernel_new(gpu->dev,
		A5XX_PREEMPT_COUNTER_SIZE,
		MSM_BO_WC, gpu->aspace, &counters_bo, &counters_iova);
	if (IS_ERR(counters)) {
		msm_gem_kernel_put(bo, gpu->aspace, true);
		return PTR_ERR(counters);
	}

	msm_gem_object_set_name(bo, "preempt");
	msm_gem_object_set_name(counters_bo, "preempt_counters");

	a5xx_gpu->preempt_bo[ring->id] = bo;
	a5xx_gpu->preempt_counters_bo[ring->id] = counters_bo;
	a5xx_gpu->preempt_iova[ring->id] = iova;
	a5xx_gpu->preempt[ring->id] = ptr;

	/* Set up the defaults on the preemption record */

	ptr->magic = A5XX_PREEMPT_RECORD_MAGIC;
	ptr->info = 0;
	ptr->data = 0;
	ptr->cntl = MSM_GPU_RB_CNTL_DEFAULT | AXXX_CP_RB_CNTL_NO_UPDATE;

	ptr->rptr_addr = shadowptr(a5xx_gpu, ring);
	ptr->counter = counters_iova;

	return 0;
}

void a5xx_preempt_fini(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	struct a5xx_gpu *a5xx_gpu = to_a5xx_gpu(adreno_gpu);
	int i;

	for (i = 0; i < gpu->nr_rings; i++) {
		msm_gem_kernel_put(a5xx_gpu->preempt_bo[i], gpu->aspace, true);
		msm_gem_kernel_put(a5xx_gpu->preempt_counters_bo[i],
			gpu->aspace, true);
	}
}

void a5xx_preempt_init(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	struct a5xx_gpu *a5xx_gpu = to_a5xx_gpu(adreno_gpu);
	int i;

	/* No preemption if we only have one ring */
	if (gpu->nr_rings <= 1)
		return;

	for (i = 0; i < gpu->nr_rings; i++) {
		if (preempt_init_ring(a5xx_gpu, gpu->rb[i])) {
			/*
			 * On any failure our adventure is over. Clean up and
			 * set nr_rings to 1 to force preemption off
			 */
			a5xx_preempt_fini(gpu);
			gpu->nr_rings = 1;

			return;
		}
	}

	timer_setup(&a5xx_gpu->preempt_timer, a5xx_preempt_timer, 0);
}
