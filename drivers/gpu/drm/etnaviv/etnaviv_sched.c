// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2017 Etnaviv Project
 */

#include <linux/moduleparam.h>

#include "etnaviv_drv.h"
#include "etnaviv_dump.h"
#include "etnaviv_gem.h"
#include "etnaviv_gpu.h"
#include "etnaviv_sched.h"
#include "state.xml.h"

static int etnaviv_job_hang_limit = 0;
module_param_named(job_hang_limit, etnaviv_job_hang_limit, int , 0444);
static int etnaviv_hw_jobs_limit = 4;
module_param_named(hw_job_limit, etnaviv_hw_jobs_limit, int , 0444);

static struct dma_fence *
etnaviv_sched_dependency(struct drm_sched_job *sched_job,
			 struct drm_sched_entity *entity)
{
	struct etnaviv_gem_submit *submit = to_etnaviv_submit(sched_job);
	struct dma_fence *fence;
	int i;

	if (unlikely(submit->in_fence)) {
		fence = submit->in_fence;
		submit->in_fence = NULL;

		if (!dma_fence_is_signaled(fence))
			return fence;

		dma_fence_put(fence);
	}

	for (i = 0; i < submit->nr_bos; i++) {
		struct etnaviv_gem_submit_bo *bo = &submit->bos[i];
		int j;

		if (bo->excl) {
			fence = bo->excl;
			bo->excl = NULL;

			if (!dma_fence_is_signaled(fence))
				return fence;

			dma_fence_put(fence);
		}

		for (j = 0; j < bo->nr_shared; j++) {
			if (!bo->shared[j])
				continue;

			fence = bo->shared[j];
			bo->shared[j] = NULL;

			if (!dma_fence_is_signaled(fence))
				return fence;

			dma_fence_put(fence);
		}
		kfree(bo->shared);
		bo->nr_shared = 0;
		bo->shared = NULL;
	}

	return NULL;
}

static struct dma_fence *etnaviv_sched_run_job(struct drm_sched_job *sched_job)
{
	struct etnaviv_gem_submit *submit = to_etnaviv_submit(sched_job);
	struct dma_fence *fence = NULL;

	if (likely(!sched_job->s_fence->finished.error))
		fence = etnaviv_gpu_submit(submit);
	else
		dev_dbg(submit->gpu->dev, "skipping bad job\n");

	return fence;
}

static enum drm_gpu_sched_stat etnaviv_sched_timedout_job(struct drm_sched_job
							  *sched_job)
{
	struct etnaviv_gem_submit *submit = to_etnaviv_submit(sched_job);
	struct etnaviv_gpu *gpu = submit->gpu;
	u32 dma_addr;
	int change;

	/* block scheduler */
	drm_sched_stop(&gpu->sched, sched_job);

	/*
	 * If the GPU managed to complete this jobs fence, the timout is
	 * spurious. Bail out.
	 */
	if (dma_fence_is_signaled(submit->out_fence))
		goto out_no_timeout;

	/*
	 * If the GPU is still making forward progress on the front-end (which
	 * should never loop) we shift out the timeout to give it a chance to
	 * finish the job.
	 */
	dma_addr = gpu_read(gpu, VIVS_FE_DMA_ADDRESS);
	change = dma_addr - gpu->hangcheck_dma_addr;
	if (change < 0 || change > 16) {
		gpu->hangcheck_dma_addr = dma_addr;
		goto out_no_timeout;
	}

	if(sched_job)
		drm_sched_increase_karma(sched_job);

	/* get the GPU back into the init state */
	etnaviv_core_dump(submit);
	etnaviv_gpu_recover_hang(gpu);

	drm_sched_resubmit_jobs(&gpu->sched);

	drm_sched_start(&gpu->sched, true);
	return DRM_GPU_SCHED_STAT_NOMINAL;

out_no_timeout:
	/* restart scheduler after GPU is usable again */
	drm_sched_start(&gpu->sched, true);
	return DRM_GPU_SCHED_STAT_NOMINAL;
}

static void etnaviv_sched_free_job(struct drm_sched_job *sched_job)
{
	struct etnaviv_gem_submit *submit = to_etnaviv_submit(sched_job);

	drm_sched_job_cleanup(sched_job);

	etnaviv_submit_put(submit);
}

static const struct drm_sched_backend_ops etnaviv_sched_ops = {
	.dependency = etnaviv_sched_dependency,
	.run_job = etnaviv_sched_run_job,
	.timedout_job = etnaviv_sched_timedout_job,
	.free_job = etnaviv_sched_free_job,
};

int etnaviv_sched_push_job(struct drm_sched_entity *sched_entity,
			   struct etnaviv_gem_submit *submit)
{
	int ret = 0;

	/*
	 * Hold the fence lock across the whole operation to avoid jobs being
	 * pushed out of order with regard to their sched fence seqnos as
	 * allocated in drm_sched_job_init.
	 */
	mutex_lock(&submit->gpu->fence_lock);

	ret = drm_sched_job_init(&submit->sched_job, sched_entity,
				 submit->ctx);
	if (ret)
		goto out_unlock;

	submit->out_fence = dma_fence_get(&submit->sched_job.s_fence->finished);
	submit->out_fence_id = idr_alloc_cyclic(&submit->gpu->fence_idr,
						submit->out_fence, 0,
						INT_MAX, GFP_KERNEL);
	if (submit->out_fence_id < 0) {
		drm_sched_job_cleanup(&submit->sched_job);
		ret = -ENOMEM;
		goto out_unlock;
	}

	/* the scheduler holds on to the job now */
	kref_get(&submit->refcount);

	drm_sched_entity_push_job(&submit->sched_job, sched_entity);

out_unlock:
	mutex_unlock(&submit->gpu->fence_lock);

	return ret;
}

int etnaviv_sched_init(struct etnaviv_gpu *gpu)
{
	int ret;

	ret = drm_sched_init(&gpu->sched, &etnaviv_sched_ops,
			     etnaviv_hw_jobs_limit, etnaviv_job_hang_limit,
			     msecs_to_jiffies(500), NULL, NULL,
			     dev_name(gpu->dev));
	if (ret)
		return ret;

	return 0;
}

void etnaviv_sched_fini(struct etnaviv_gpu *gpu)
{
	drm_sched_fini(&gpu->sched);
}
