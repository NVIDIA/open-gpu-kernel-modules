/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 Jihong Min. All rights reserved.
 * SPDX-License-Identifier: MIT
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <linux/build_bug.h>
#include <linux/errno.h>
#include <linux/hwmon.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/unaligned.h>

#include "hwmon-rm.h"
#include "hwmon-temp-limit.h"

#define CTRL_CMD_THERMAL_GET_THRESHOLD 0x20808546U
#define THERMAL_THRESHOLD_SLOWDOWN_FIRST 0x00000009U
#define THERMAL_THRESHOLD_SLOWDOWN_LAST 0x00000014U
#define THERMAL_THRESHOLD_SHUTDOWN 0x00000015U

#define CTRL_CMD_PWR_POLICY_GET_INFO 0x2080852aU
#define CTRL_CMD_PWR_POLICY_GET_STATUS 0x2080852cU
#define PWR_POLICY_INFO_SIZE 1784U
#define PWR_POLICY_STATUS_SIZE 1440U
#define PWR_POLICY_THERM_POLICY_OFFSET 0x08U
#define PWR_POLICY_GPU_MAX_TEMP_OFFSET 0x24U
#define TEMP_LIMIT_MAX_NV_TEMP (200 * 256)

struct threshold_params {
	u32 threshold_id;
	s32 threshold;
	u32 reserved;
	u32 flags;
	u32 policy_mask;
	u32 target_mask;
};

static s32 nvtemp_to_mc(s32 nvtemp)
{
	return div_s64((s64)nvtemp * 1000, 256);
}

static bool valid_nvtemp(s32 nvtemp)
{
	return nvtemp > 0 && nvtemp <= TEMP_LIMIT_MAX_NV_TEMP;
}

static int get_threshold(struct nvhwmon_gpu *gpu, u32 threshold_id, s32 *mc)
{
	struct threshold_params params = {
		.threshold_id = threshold_id,
	};
	u32 status;

	BUILD_BUG_ON(sizeof(params) != 24);

	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    CTRL_CMD_THERMAL_GET_THRESHOLD,
				    &params, sizeof(params));
	if (status != NVOS_STATUS_SUCCESS)
		return nvhwmon_rm_status_to_errno(status);

	if (!valid_nvtemp(params.threshold))
		return -ENODATA;

	*mc = nvtemp_to_mc(params.threshold);
	return 0;
}

static int get_slowdown(struct nvhwmon_gpu *gpu, s32 *mc)
{
	bool found = false;
	s32 min_mc = S32_MAX;
	u32 threshold_id;

	/* Multiple slowdown thresholds may exist; expose the most conservative. */
	for (threshold_id = THERMAL_THRESHOLD_SLOWDOWN_FIRST;
	     threshold_id <= THERMAL_THRESHOLD_SLOWDOWN_LAST;
	     threshold_id++) {
		s32 candidate_mc;
		int ret;

		ret = get_threshold(gpu, threshold_id, &candidate_mc);
		if (ret)
			continue;

		found = true;
		min_mc = min(min_mc, candidate_mc);
	}

	if (!found)
		return -ENODATA;

	*mc = min_mc;
	return 0;
}

/*
 * The power policy status blob is keyed by policy id. Read the id from the
 * info blob first, then request the matching status blob.
 */
static int get_gpu_max(struct nvhwmon_gpu *gpu, s32 *mc)
{
	u8 *params;
	s32 nvtemp;
	u32 policy_id;
	u32 status;
	int ret = 0;

	BUILD_BUG_ON(PWR_POLICY_THERM_POLICY_OFFSET + sizeof(u32) >
		     PWR_POLICY_INFO_SIZE);
	BUILD_BUG_ON(PWR_POLICY_THERM_POLICY_OFFSET + sizeof(u32) >
		     PWR_POLICY_STATUS_SIZE);
	BUILD_BUG_ON(PWR_POLICY_GPU_MAX_TEMP_OFFSET + sizeof(u32) >
		     PWR_POLICY_STATUS_SIZE);

	params = kzalloc(max(PWR_POLICY_INFO_SIZE,
			     PWR_POLICY_STATUS_SIZE),
			 GFP_KERNEL);
	if (!params)
		return -ENOMEM;

	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    CTRL_CMD_PWR_POLICY_GET_INFO,
				    params, PWR_POLICY_INFO_SIZE);
	if (status != NVOS_STATUS_SUCCESS) {
		ret = nvhwmon_rm_status_to_errno(status);
		goto out;
	}

	policy_id = get_unaligned_le32(params +
				       PWR_POLICY_THERM_POLICY_OFFSET);
	memset(params, 0, PWR_POLICY_STATUS_SIZE);
	put_unaligned_le32(policy_id,
			   params + PWR_POLICY_THERM_POLICY_OFFSET);

	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    CTRL_CMD_PWR_POLICY_GET_STATUS,
				    params, PWR_POLICY_STATUS_SIZE);
	if (status != NVOS_STATUS_SUCCESS) {
		ret = nvhwmon_rm_status_to_errno(status);
		goto out;
	}

	nvtemp = (s32)get_unaligned_le32(params +
					 PWR_POLICY_GPU_MAX_TEMP_OFFSET);
	if (!valid_nvtemp(nvtemp)) {
		ret = -ENODATA;
		goto out;
	}

	*mc = nvtemp_to_mc(nvtemp);

out:
	kfree(params);
	return ret;
}

static void apply_gpu_limits(struct nvhwmon_gpu *gpu, u32 channel,
			     const struct nvhwmon_temp_limits *limits)
{
	if (channel >= NVHWMON_TEMP_CHANNELS)
		return;

	gpu->temp_limits[channel] = *limits;
}

void nvhwmon_temp_limit_probe(struct nvhwmon_gpu *gpu)
{
	struct nvhwmon_temp_limits limits = { 0 };
	u32 channel;
	int ret;

	ret = get_gpu_max(gpu, &limits.max_mc);
	limits.max_valid = !ret;

	ret = get_slowdown(gpu, &limits.crit_mc);
	limits.crit_valid = !ret;

	ret = get_threshold(gpu, THERMAL_THRESHOLD_SHUTDOWN,
			    &limits.emergency_mc);
	limits.emergency_valid = !ret;

	if (!limits.max_valid && !limits.crit_valid && !limits.emergency_valid)
		return;

	/* Apply GPU limits to both the RUSD GPU channel and GPU target sensors. */
	apply_gpu_limits(gpu, RUSD_TEMPERATURE_TYPE_GPU, &limits);

	for (channel = 0; channel < gpu->thermal_count; channel++) {
		if (gpu->thermal_sensors[channel].target_type !=
		    NV2080_CTRL_THERMAL_SYSTEM_TARGET_GPU)
			continue;

		apply_gpu_limits(gpu, NVHWMON_RUSD_TEMP_CHANNELS + channel,
				 &limits);
	}
}

bool nvhwmon_temp_limit_has(const struct nvhwmon_gpu *gpu, u32 channel, u32 attr)
{
	const struct nvhwmon_temp_limits *limits;

	if (channel >= NVHWMON_TEMP_CHANNELS)
		return false;

	limits = &gpu->temp_limits[channel];

	switch (attr) {
	case hwmon_temp_max:
		return limits->max_valid;
	case hwmon_temp_crit:
		return limits->crit_valid;
	case hwmon_temp_emergency:
		return limits->emergency_valid;
	default:
		return false;
	}
}

int nvhwmon_temp_limit_read(const struct nvhwmon_gpu *gpu, u32 channel, u32 attr,
			    long *val)
{
	const struct nvhwmon_temp_limits *limits;

	if (!nvhwmon_temp_limit_has(gpu, channel, attr))
		return -EINVAL;

	limits = &gpu->temp_limits[channel];

	switch (attr) {
	case hwmon_temp_max:
		*val = limits->max_mc;
		return 0;
	case hwmon_temp_crit:
		*val = limits->crit_mc;
		return 0;
	case hwmon_temp_emergency:
		*val = limits->emergency_mc;
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}
