/* SPDX-License-Identifier: GPL-2.0-only */
/* Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
 */

#ifndef _DPU_CORE_PERF_H_
#define _DPU_CORE_PERF_H_

#include <linux/types.h>
#include <linux/dcache.h>
#include <linux/mutex.h>
#include <drm/drm_crtc.h>

#include "dpu_hw_catalog.h"

#define	DPU_PERF_DEFAULT_MAX_CORE_CLK_RATE	412500000

/**
 * enum dpu_core_perf_data_bus_id - data bus identifier
 * @DPU_CORE_PERF_DATA_BUS_ID_MNOC: DPU/MNOC data bus
 * @DPU_CORE_PERF_DATA_BUS_ID_LLCC: MNOC/LLCC data bus
 * @DPU_CORE_PERF_DATA_BUS_ID_EBI: LLCC/EBI data bus
 */
enum dpu_core_perf_data_bus_id {
	DPU_CORE_PERF_DATA_BUS_ID_MNOC,
	DPU_CORE_PERF_DATA_BUS_ID_LLCC,
	DPU_CORE_PERF_DATA_BUS_ID_EBI,
	DPU_CORE_PERF_DATA_BUS_ID_MAX,
};

/**
 * struct dpu_core_perf_params - definition of performance parameters
 * @max_per_pipe_ib: maximum instantaneous bandwidth request
 * @bw_ctl: arbitrated bandwidth request
 * @core_clk_rate: core clock rate request
 */
struct dpu_core_perf_params {
	u64 max_per_pipe_ib;
	u64 bw_ctl;
	u64 core_clk_rate;
};

/**
 * struct dpu_core_perf_tune - definition of performance tuning control
 * @mode: performance mode
 * @min_core_clk: minimum core clock
 * @min_bus_vote: minimum bus vote
 */
struct dpu_core_perf_tune {
	u32 mode;
	u64 min_core_clk;
	u64 min_bus_vote;
};

/**
 * struct dpu_core_perf - definition of core performance context
 * @dev: Pointer to drm device
 * @debugfs_root: top level debug folder
 * @catalog: Pointer to catalog configuration
 * @core_clk: Pointer to core clock structure
 * @core_clk_rate: current core clock rate
 * @max_core_clk_rate: maximum allowable core clock rate
 * @perf_tune: debug control for performance tuning
 * @enable_bw_release: debug control for bandwidth release
 * @fix_core_clk_rate: fixed core clock request in Hz used in mode 2
 * @fix_core_ib_vote: fixed core ib vote in bps used in mode 2
 * @fix_core_ab_vote: fixed core ab vote in bps used in mode 2
 */
struct dpu_core_perf {
	struct drm_device *dev;
	struct dentry *debugfs_root;
	struct dpu_mdss_cfg *catalog;
	struct dss_clk *core_clk;
	u64 core_clk_rate;
	u64 max_core_clk_rate;
	struct dpu_core_perf_tune perf_tune;
	u32 enable_bw_release;
	u64 fix_core_clk_rate;
	u64 fix_core_ib_vote;
	u64 fix_core_ab_vote;
};

/**
 * dpu_core_perf_crtc_check - validate performance of the given crtc state
 * @crtc: Pointer to crtc
 * @state: Pointer to new crtc state
 * return: zero if success, or error code otherwise
 */
int dpu_core_perf_crtc_check(struct drm_crtc *crtc,
		struct drm_crtc_state *state);

/**
 * dpu_core_perf_crtc_update - update performance of the given crtc
 * @crtc: Pointer to crtc
 * @params_changed: true if crtc parameters are modified
 * @stop_req: true if this is a stop request
 * return: zero if success, or error code otherwise
 */
int dpu_core_perf_crtc_update(struct drm_crtc *crtc,
		int params_changed, bool stop_req);

/**
 * dpu_core_perf_crtc_release_bw - release bandwidth of the given crtc
 * @crtc: Pointer to crtc
 */
void dpu_core_perf_crtc_release_bw(struct drm_crtc *crtc);

/**
 * dpu_core_perf_destroy - destroy the given core performance context
 * @perf: Pointer to core performance context
 */
void dpu_core_perf_destroy(struct dpu_core_perf *perf);

/**
 * dpu_core_perf_init - initialize the given core performance context
 * @perf: Pointer to core performance context
 * @dev: Pointer to drm device
 * @catalog: Pointer to catalog
 * @core_clk: pointer to core clock
 */
int dpu_core_perf_init(struct dpu_core_perf *perf,
		struct drm_device *dev,
		struct dpu_mdss_cfg *catalog,
		struct dss_clk *core_clk);

struct dpu_kms;

/**
 * dpu_core_perf_debugfs_init - initialize debugfs for core performance context
 * @dpu_kms: Pointer to the dpu_kms struct
 * @debugfs_parent: Pointer to parent debugfs
 */
int dpu_core_perf_debugfs_init(struct dpu_kms *dpu_kms, struct dentry *parent);

#endif /* _DPU_CORE_PERF_H_ */
