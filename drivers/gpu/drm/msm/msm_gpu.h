/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 */

#ifndef __MSM_GPU_H__
#define __MSM_GPU_H__

#include <linux/adreno-smmu-priv.h>
#include <linux/clk.h>
#include <linux/interconnect.h>
#include <linux/pm_opp.h>
#include <linux/regulator/consumer.h>

#include "msm_drv.h"
#include "msm_fence.h"
#include "msm_ringbuffer.h"
#include "msm_gem.h"

struct msm_gem_submit;
struct msm_gpu_perfcntr;
struct msm_gpu_state;

struct msm_gpu_config {
	const char *ioname;
	unsigned int nr_rings;
};

/* So far, with hardware that I've seen to date, we can have:
 *  + zero, one, or two z180 2d cores
 *  + a3xx or a2xx 3d core, which share a common CP (the firmware
 *    for the CP seems to implement some different PM4 packet types
 *    but the basics of cmdstream submission are the same)
 *
 * Which means that the eventual complete "class" hierarchy, once
 * support for all past and present hw is in place, becomes:
 *  + msm_gpu
 *    + adreno_gpu
 *      + a3xx_gpu
 *      + a2xx_gpu
 *    + z180_gpu
 */
struct msm_gpu_funcs {
	int (*get_param)(struct msm_gpu *gpu, uint32_t param, uint64_t *value);
	int (*hw_init)(struct msm_gpu *gpu);
	int (*pm_suspend)(struct msm_gpu *gpu);
	int (*pm_resume)(struct msm_gpu *gpu);
	void (*submit)(struct msm_gpu *gpu, struct msm_gem_submit *submit);
	void (*flush)(struct msm_gpu *gpu, struct msm_ringbuffer *ring);
	irqreturn_t (*irq)(struct msm_gpu *irq);
	struct msm_ringbuffer *(*active_ring)(struct msm_gpu *gpu);
	void (*recover)(struct msm_gpu *gpu);
	void (*destroy)(struct msm_gpu *gpu);
#if defined(CONFIG_DEBUG_FS) || defined(CONFIG_DEV_COREDUMP)
	/* show GPU status in debugfs: */
	void (*show)(struct msm_gpu *gpu, struct msm_gpu_state *state,
			struct drm_printer *p);
	/* for generation specific debugfs: */
	void (*debugfs_init)(struct msm_gpu *gpu, struct drm_minor *minor);
#endif
	unsigned long (*gpu_busy)(struct msm_gpu *gpu);
	struct msm_gpu_state *(*gpu_state_get)(struct msm_gpu *gpu);
	int (*gpu_state_put)(struct msm_gpu_state *state);
	unsigned long (*gpu_get_freq)(struct msm_gpu *gpu);
	void (*gpu_set_freq)(struct msm_gpu *gpu, struct dev_pm_opp *opp);
	struct msm_gem_address_space *(*create_address_space)
		(struct msm_gpu *gpu, struct platform_device *pdev);
	struct msm_gem_address_space *(*create_private_address_space)
		(struct msm_gpu *gpu);
	uint32_t (*get_rptr)(struct msm_gpu *gpu, struct msm_ringbuffer *ring);
};

/* Additional state for iommu faults: */
struct msm_gpu_fault_info {
	u64 ttbr0;
	unsigned long iova;
	int flags;
	const char *type;
	const char *block;
};

struct msm_gpu {
	const char *name;
	struct drm_device *dev;
	struct platform_device *pdev;
	const struct msm_gpu_funcs *funcs;

	struct adreno_smmu_priv adreno_smmu;

	/* performance counters (hw & sw): */
	spinlock_t perf_lock;
	bool perfcntr_active;
	struct {
		bool active;
		ktime_t time;
	} last_sample;
	uint32_t totaltime, activetime;    /* sw counters */
	uint32_t last_cntrs[5];            /* hw counters */
	const struct msm_gpu_perfcntr *perfcntrs;
	uint32_t num_perfcntrs;

	struct msm_ringbuffer *rb[MSM_GPU_MAX_RINGS];
	int nr_rings;

	/*
	 * List of GEM active objects on this gpu.  Protected by
	 * msm_drm_private::mm_lock
	 */
	struct list_head active_list;

	/* does gpu need hw_init? */
	bool needs_hw_init;

	/* number of GPU hangs (for all contexts) */
	int global_faults;

	void __iomem *mmio;
	int irq;

	struct msm_gem_address_space *aspace;

	/* Power Control: */
	struct regulator *gpu_reg, *gpu_cx;
	struct clk_bulk_data *grp_clks;
	int nr_clocks;
	struct clk *ebi1_clk, *core_clk, *rbbmtimer_clk;
	uint32_t fast_rate;

	/* Hang and Inactivity Detection:
	 */
#define DRM_MSM_INACTIVE_PERIOD   66 /* in ms (roughly four frames) */

#define DRM_MSM_HANGCHECK_DEFAULT_PERIOD 500 /* in ms */
	struct timer_list hangcheck_timer;

	/* Fault info for most recent iova fault: */
	struct msm_gpu_fault_info fault_info;

	/* work for handling GPU ioval faults: */
	struct kthread_work fault_work;

	/* work for handling GPU recovery: */
	struct kthread_work recover_work;

	/* work for handling active-list retiring: */
	struct kthread_work retire_work;

	/* worker for retire/recover: */
	struct kthread_worker *worker;

	struct drm_gem_object *memptrs_bo;

	struct {
		struct devfreq *devfreq;
		u64 busy_cycles;
		ktime_t time;
	} devfreq;

	uint32_t suspend_count;

	struct msm_gpu_state *crashstate;
	/* True if the hardware supports expanded apriv (a650 and newer) */
	bool hw_apriv;

	struct thermal_cooling_device *cooling;
};

static inline struct msm_gpu *dev_to_gpu(struct device *dev)
{
	struct adreno_smmu_priv *adreno_smmu = dev_get_drvdata(dev);
	return container_of(adreno_smmu, struct msm_gpu, adreno_smmu);
}

/* It turns out that all targets use the same ringbuffer size */
#define MSM_GPU_RINGBUFFER_SZ SZ_32K
#define MSM_GPU_RINGBUFFER_BLKSIZE 32

#define MSM_GPU_RB_CNTL_DEFAULT \
		(AXXX_CP_RB_CNTL_BUFSZ(ilog2(MSM_GPU_RINGBUFFER_SZ / 8)) | \
		AXXX_CP_RB_CNTL_BLKSZ(ilog2(MSM_GPU_RINGBUFFER_BLKSIZE / 8)))

static inline bool msm_gpu_active(struct msm_gpu *gpu)
{
	int i;

	for (i = 0; i < gpu->nr_rings; i++) {
		struct msm_ringbuffer *ring = gpu->rb[i];

		if (ring->seqno > ring->memptrs->fence)
			return true;
	}

	return false;
}

/* Perf-Counters:
 * The select_reg and select_val are just there for the benefit of the child
 * class that actually enables the perf counter..  but msm_gpu base class
 * will handle sampling/displaying the counters.
 */

struct msm_gpu_perfcntr {
	uint32_t select_reg;
	uint32_t sample_reg;
	uint32_t select_val;
	const char *name;
};

struct msm_gpu_submitqueue {
	int id;
	u32 flags;
	u32 prio;
	int faults;
	struct msm_file_private *ctx;
	struct list_head node;
	struct kref ref;
};

struct msm_gpu_state_bo {
	u64 iova;
	size_t size;
	void *data;
	bool encoded;
};

struct msm_gpu_state {
	struct kref ref;
	struct timespec64 time;

	struct {
		u64 iova;
		u32 fence;
		u32 seqno;
		u32 rptr;
		u32 wptr;
		void *data;
		int data_size;
		bool encoded;
	} ring[MSM_GPU_MAX_RINGS];

	int nr_registers;
	u32 *registers;

	u32 rbbm_status;

	char *comm;
	char *cmd;

	struct msm_gpu_fault_info fault_info;

	int nr_bos;
	struct msm_gpu_state_bo *bos;
};

static inline void gpu_write(struct msm_gpu *gpu, u32 reg, u32 data)
{
	msm_writel(data, gpu->mmio + (reg << 2));
}

static inline u32 gpu_read(struct msm_gpu *gpu, u32 reg)
{
	return msm_readl(gpu->mmio + (reg << 2));
}

static inline void gpu_rmw(struct msm_gpu *gpu, u32 reg, u32 mask, u32 or)
{
	msm_rmw(gpu->mmio + (reg << 2), mask, or);
}

static inline u64 gpu_read64(struct msm_gpu *gpu, u32 lo, u32 hi)
{
	u64 val;

	/*
	 * Why not a readq here? Two reasons: 1) many of the LO registers are
	 * not quad word aligned and 2) the GPU hardware designers have a bit
	 * of a history of putting registers where they fit, especially in
	 * spins. The longer a GPU family goes the higher the chance that
	 * we'll get burned.  We could do a series of validity checks if we
	 * wanted to, but really is a readq() that much better? Nah.
	 */

	/*
	 * For some lo/hi registers (like perfcounters), the hi value is latched
	 * when the lo is read, so make sure to read the lo first to trigger
	 * that
	 */
	val = (u64) msm_readl(gpu->mmio + (lo << 2));
	val |= ((u64) msm_readl(gpu->mmio + (hi << 2)) << 32);

	return val;
}

static inline void gpu_write64(struct msm_gpu *gpu, u32 lo, u32 hi, u64 val)
{
	/* Why not a writeq here? Read the screed above */
	msm_writel(lower_32_bits(val), gpu->mmio + (lo << 2));
	msm_writel(upper_32_bits(val), gpu->mmio + (hi << 2));
}

int msm_gpu_pm_suspend(struct msm_gpu *gpu);
int msm_gpu_pm_resume(struct msm_gpu *gpu);
void msm_gpu_resume_devfreq(struct msm_gpu *gpu);

int msm_gpu_hw_init(struct msm_gpu *gpu);

void msm_gpu_perfcntr_start(struct msm_gpu *gpu);
void msm_gpu_perfcntr_stop(struct msm_gpu *gpu);
int msm_gpu_perfcntr_sample(struct msm_gpu *gpu, uint32_t *activetime,
		uint32_t *totaltime, uint32_t ncntrs, uint32_t *cntrs);

void msm_gpu_retire(struct msm_gpu *gpu);
void msm_gpu_submit(struct msm_gpu *gpu, struct msm_gem_submit *submit);

int msm_gpu_init(struct drm_device *drm, struct platform_device *pdev,
		struct msm_gpu *gpu, const struct msm_gpu_funcs *funcs,
		const char *name, struct msm_gpu_config *config);

struct msm_gem_address_space *
msm_gpu_create_private_address_space(struct msm_gpu *gpu, struct task_struct *task);

void msm_gpu_cleanup(struct msm_gpu *gpu);

struct msm_gpu *adreno_load_gpu(struct drm_device *dev);
void __init adreno_register(void);
void __exit adreno_unregister(void);

static inline void msm_submitqueue_put(struct msm_gpu_submitqueue *queue)
{
	if (queue)
		kref_put(&queue->ref, msm_submitqueue_destroy);
}

static inline struct msm_gpu_state *msm_gpu_crashstate_get(struct msm_gpu *gpu)
{
	struct msm_gpu_state *state = NULL;

	mutex_lock(&gpu->dev->struct_mutex);

	if (gpu->crashstate) {
		kref_get(&gpu->crashstate->ref);
		state = gpu->crashstate;
	}

	mutex_unlock(&gpu->dev->struct_mutex);

	return state;
}

static inline void msm_gpu_crashstate_put(struct msm_gpu *gpu)
{
	mutex_lock(&gpu->dev->struct_mutex);

	if (gpu->crashstate) {
		if (gpu->funcs->gpu_state_put(gpu->crashstate))
			gpu->crashstate = NULL;
	}

	mutex_unlock(&gpu->dev->struct_mutex);
}

/*
 * Simple macro to semi-cleanly add the MAP_PRIV flag for targets that can
 * support expanded privileges
 */
#define check_apriv(gpu, flags) \
	(((gpu)->hw_apriv ? MSM_BO_MAP_PRIV : 0) | (flags))


#endif /* __MSM_GPU_H__ */
