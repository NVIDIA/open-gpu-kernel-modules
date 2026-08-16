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
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/stddef.h>
#include <linux/string.h>

#include "hwmon-rm.h"
#include "hwmon-rusd.h"

#define RUSD_READ_ATTEMPTS 10
#define RUSD_POLL_INTERVAL_MS 1000
#define RUSD_CACHE_MS RUSD_POLL_INTERVAL_MS
#define RUSD_ERROR_RETRY_MS 100
#define RUSD_BASE_POLL_MASK \
	(NV00DE_RUSD_POLL_POWER | NV00DE_RUSD_POLL_THERMAL)

enum power_channel {
	NVHWMON_POWER_AVG_GPU,
	NVHWMON_POWER_AVG_MODULE,
	NVHWMON_POWER_AVG_MEMORY,
	NVHWMON_POWER_INST_GPU,
	NVHWMON_POWER_INST_MODULE,
	NVHWMON_POWER_INST_CPU,
	NVHWMON_POWER_POLICY_TOTAL_GPU,
	NVHWMON_POWER_LIMIT_REQUESTED,
	NVHWMON_POWER_LIMIT_ENFORCED,
};

static bool timestamp_valid(NvU64 timestamp)
{
	if (timestamp == RUSD_TIMESTAMP_INVALID ||
	    timestamp == RUSD_TIMESTAMP_WRITE_IN_PROGRESS)
		return false;

	if (timestamp >= RUSD_SEQ_START && (timestamp & 1))
		return false;

	return true;
}

/* Every RUSD record starts with a timestamp used as a lightweight sequence. */
struct field_header {
	NvU64 lastModifiedTimestamp;
};

/*
 * read_field() casts RUSD records to field_header, so every record type used
 * below must keep lastModifiedTimestamp as its first member.
 */
static_assert(offsetof(RUSD_TEMPERATURE, lastModifiedTimestamp) == 0);
static_assert(offsetof(RUSD_AVG_POWER_USAGE, lastModifiedTimestamp) == 0);
static_assert(offsetof(RUSD_INST_POWER_USAGE, lastModifiedTimestamp) == 0);
static_assert(offsetof(RUSD_POWER_POLICY_STATUS, lastModifiedTimestamp) == 0);
static_assert(offsetof(RUSD_POWER_LIMITS, lastModifiedTimestamp) == 0);
static_assert(offsetof(RUSD_FAN_COOLER_STATUS, lastModifiedTimestamp) == 0);

static int read_field(const void *src, void *dst, size_t size)
{
	const struct field_header *header = src;
	u32 attempt;

	for (attempt = 0; attempt < RUSD_READ_ATTEMPTS; attempt++) {
		NvU64 seq1 = READ_ONCE(header->lastModifiedTimestamp);

		if (seq1 == RUSD_TIMESTAMP_INVALID)
			return -ENODATA;

		if (!timestamp_valid(seq1))
			continue;

		/* Pairs with RM/GSP timestamp publication around the payload. */
		smp_rmb();
		memcpy(dst, src, size);
		/* Re-read the timestamp after the payload copy. */
		smp_rmb();

		if (seq1 == READ_ONCE(header->lastModifiedTimestamp)) {
			((struct field_header *)dst)
				->lastModifiedTimestamp = seq1;
			return 0;
		}
	}

	return -EAGAIN;
}

static unsigned long cache_expires(bool retry_soon)
{
	u32 cache_ms = retry_soon ? RUSD_ERROR_RETRY_MS :
				    RUSD_CACHE_MS;

	return jiffies + msecs_to_jiffies(cache_ms);
}

static void refresh_temps(struct nvhwmon_gpu *gpu)
{
	struct nvhwmon_rusd_cache *cache = &gpu->rusd_cache;
	NV00DE_SHARED_DATA *shared = gpu->rusd_map;
	bool retry_soon = false;
	u32 channel;

	if (cache->temp_expires && time_before(jiffies, cache->temp_expires))
		return;

	/* Missing fields are marked invalid; racing writes retry sooner. */
	for (channel = 0; channel < NVHWMON_RUSD_TEMP_CHANNELS; channel++) {
		int ret;

		ret = read_field(&shared->temperatures[channel],
				 &cache->temperatures[channel],
				 sizeof(cache->temperatures[channel]));
		if (!ret)
			cache->temp_valid[channel] = true;
		else if (ret == -ENODATA)
			cache->temp_valid[channel] = false;
		else
			retry_soon = true;
	}

	cache->temp_expires = cache_expires(retry_soon);
}

static void refresh_power(struct nvhwmon_gpu *gpu)
{
	struct nvhwmon_rusd_cache *cache = &gpu->rusd_cache;
	NV00DE_SHARED_DATA *shared = gpu->rusd_map;
	bool retry_soon = false;
	int ret;

	if (cache->power_expires && time_before(jiffies, cache->power_expires))
		return;

	ret = read_field(&shared->avgPowerUsage, &cache->avg_power,
			 sizeof(cache->avg_power));
	if (!ret)
		cache->avg_power_valid = true;
	else if (ret == -ENODATA)
		cache->avg_power_valid = false;
	else
		retry_soon = true;

	ret = read_field(&shared->instPowerUsage, &cache->inst_power,
			 sizeof(cache->inst_power));
	if (!ret)
		cache->inst_power_valid = true;
	else if (ret == -ENODATA)
		cache->inst_power_valid = false;
	else
		retry_soon = true;

	ret = read_field(&shared->powerPolicyStatus, &cache->power_policy,
			 sizeof(cache->power_policy));
	if (!ret)
		cache->power_policy_valid = true;
	else if (ret == -ENODATA)
		cache->power_policy_valid = false;
	else
		retry_soon = true;

	ret = read_field(&shared->powerLimitGpu, &cache->power_limit,
			 sizeof(cache->power_limit));
	if (!ret)
		cache->power_limit_valid = true;
	else if (ret == -ENODATA)
		cache->power_limit_valid = false;
	else
		retry_soon = true;

	cache->power_expires = cache_expires(retry_soon);
}

bool nvhwmon_rusd_power_has_average(u32 channel)
{
	return channel == NVHWMON_POWER_AVG_GPU ||
	       channel == NVHWMON_POWER_AVG_MODULE ||
	       channel == NVHWMON_POWER_AVG_MEMORY;
}

bool nvhwmon_rusd_power_has_cap(u32 channel)
{
	return channel == NVHWMON_POWER_POLICY_TOTAL_GPU;
}

static void refresh_fans(struct nvhwmon_gpu *gpu)
{
	struct nvhwmon_rusd_cache *cache = &gpu->rusd_cache;
	NV00DE_SHARED_DATA *shared = gpu->rusd_map;
	bool retry_soon = false;
	int ret;

	if (cache->fan_expires && time_before(jiffies, cache->fan_expires))
		return;

	ret = read_field(&shared->fanCoolerStatus, &cache->fan_status,
			 sizeof(cache->fan_status));
	if (!ret)
		cache->fan_valid = true;
	else if (ret == -ENODATA)
		cache->fan_valid = false;
	else
		retry_soon = true;

	cache->fan_expires = cache_expires(retry_soon);
}

int nvhwmon_rusd_init(struct nvhwmon_gpu *gpu)
{
	NV00DE_ALLOC_PARAMETERS alloc_params = { 0 };
	NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM interval_params = { 0 };
	NvU64 poll_mask = RUSD_BASE_POLL_MASK;
	NvU32 status;

	/* Fan polling is requested only when native fan coolers were detected. */
	if (gpu->fan_count)
		poll_mask |= NV00DE_RUSD_POLL_FAN;

	gpu->h_rusd = nvhwmon_rm_rusd_handle(gpu);
	alloc_params.polledDataMask = poll_mask;

	status = nvhwmon_rm_alloc(gpu->h_client, gpu->h_subdevice, gpu->h_rusd,
				  RM_USER_SHARED_DATA, &alloc_params,
				  sizeof(alloc_params));
	if (status != NVOS_STATUS_SUCCESS) {
		gpu->h_rusd = NV01_NULL_OBJECT;
		return nvhwmon_rm_status_to_errno(status);
	}

	status = nvhwmon_rm_map_memory(gpu->h_client, gpu->h_device,
				       gpu->h_rusd, 0,
				       sizeof(NV00DE_SHARED_DATA),
				       &gpu->rusd_map,
				       NV04_MAP_MEMORY_FLAGS_NONE);
	if (status != NVOS_STATUS_SUCCESS || !gpu->rusd_map) {
		nvhwmon_rusd_fini(gpu);
		if (status != NVOS_STATUS_SUCCESS)
			return nvhwmon_rm_status_to_errno(status);
		return -EFAULT;
	}

	/* Best effort: default polling still works if RM rejects this request. */
	interval_params.pollingIntervalMs = RUSD_POLL_INTERVAL_MS;
	nvhwmon_rm_control(gpu->h_client, gpu->h_rusd,
			   NV00DE_CTRL_CMD_REQUEST_POLL_INTERVAL,
			   &interval_params, sizeof(interval_params));

	return 0;
}

void nvhwmon_rusd_fini(struct nvhwmon_gpu *gpu)
{
	memset(&gpu->rusd_cache, 0, sizeof(gpu->rusd_cache));

	if (gpu->rusd_map) {
		nvhwmon_rm_unmap_memory(gpu->h_client, gpu->h_device,
					gpu->h_rusd, gpu->rusd_map,
					NV04_MAP_MEMORY_FLAGS_NONE);
		gpu->rusd_map = NULL;
	}

	if (gpu->h_rusd != NV01_NULL_OBJECT) {
		nvhwmon_rm_free(gpu->h_client, gpu->h_subdevice, gpu->h_rusd);
		gpu->h_rusd = NV01_NULL_OBJECT;
	}
}

int nvhwmon_rusd_read_temp(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	NV00DE_SHARED_DATA *shared = gpu->rusd_map;
	RUSD_TEMPERATURE temp;

	if (!shared)
		return -ENODEV;

	if (channel >= NVHWMON_RUSD_TEMP_CHANNELS)
		return -EINVAL;

	refresh_temps(gpu);
	if (!gpu->rusd_cache.temp_valid[channel])
		return -ENODATA;

	temp = gpu->rusd_cache.temperatures[channel];
	*val = div_s64((s64)temp.temperature * 1000, 256);
	return 0;
}

int nvhwmon_rusd_read_power(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	NV00DE_SHARED_DATA *shared = gpu->rusd_map;
	u32 mw;

	if (!shared)
		return -ENODEV;

	if (channel >= NVHWMON_POWER_CHANNELS)
		return -EINVAL;

	refresh_power(gpu);

	switch (channel) {
	case NVHWMON_POWER_AVG_GPU:
		if (!gpu->rusd_cache.avg_power_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.avg_power.info.averageGpuPower;
		break;
	case NVHWMON_POWER_AVG_MODULE:
		if (!gpu->rusd_cache.avg_power_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.avg_power.info.averageModulePower;
		break;
	case NVHWMON_POWER_AVG_MEMORY:
		if (!gpu->rusd_cache.avg_power_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.avg_power.info.averageMemoryPower;
		break;
	case NVHWMON_POWER_INST_GPU:
		if (!gpu->rusd_cache.inst_power_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.inst_power.info.instGpuPower;
		break;
	case NVHWMON_POWER_INST_MODULE:
		if (!gpu->rusd_cache.inst_power_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.inst_power.info.instModulePower;
		break;
	case NVHWMON_POWER_INST_CPU:
		if (!gpu->rusd_cache.inst_power_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.inst_power.info.instCpuPower;
		break;
	case NVHWMON_POWER_POLICY_TOTAL_GPU:
		if (!gpu->rusd_cache.power_policy_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.power_policy.info.tgpmW;
		break;
	case NVHWMON_POWER_LIMIT_REQUESTED:
		if (!gpu->rusd_cache.power_limit_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.power_limit.info.requestedmW;
		break;
	case NVHWMON_POWER_LIMIT_ENFORCED:
		if (!gpu->rusd_cache.power_limit_valid)
			return -ENODATA;
		mw = gpu->rusd_cache.power_limit.info.enforcedmW;
		break;
	default:
		return -EINVAL;
	}

	*val = (long)mw * 1000;
	return 0;
}

int nvhwmon_rusd_read_power_average(struct nvhwmon_gpu *gpu, u32 channel,
				    long *val)
{
	if (!nvhwmon_rusd_power_has_average(channel))
		return -EINVAL;

	return nvhwmon_rusd_read_power(gpu, channel, val);
}

int nvhwmon_rusd_read_power_cap(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	if (!nvhwmon_rusd_power_has_cap(channel))
		return -EINVAL;

	return nvhwmon_rusd_read_power(gpu, NVHWMON_POWER_LIMIT_REQUESTED, val);
}

int nvhwmon_rusd_read_fan(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	NV00DE_SHARED_DATA *shared = gpu->rusd_map;
	u8 rm_index;

	if (!shared)
		return -ENODEV;

	if (channel >= gpu->fan_count)
		return -EINVAL;

	rm_index = gpu->fans[channel].rm_index;
	if (rm_index >= RUSD_FAN_COOLER_MAX_COOLERS)
		return -EINVAL;

	refresh_fans(gpu);
	if (!gpu->rusd_cache.fan_valid)
		return -ENODATA;

	*val = gpu->rusd_cache.fan_status.info.rpmCurr[rm_index];
	return 0;
}

const char *nvhwmon_rusd_temp_label(u32 channel)
{
	static const char *const labels[] = {
		[RUSD_TEMPERATURE_TYPE_GPU] = "gpu",
		[RUSD_TEMPERATURE_TYPE_MEMORY] = "memory",
		[RUSD_TEMPERATURE_TYPE_BOARD] = "board",
		[RUSD_TEMPERATURE_TYPE_POWER_SUPPLY] = "power_supply",
		[RUSD_TEMPERATURE_TYPE_HBM] = "hbm",
	};

	if (channel >= ARRAY_SIZE(labels) || !labels[channel])
		return "unknown";

	return labels[channel];
}

const char *nvhwmon_rusd_power_label(u32 channel)
{
	static const char *const labels[] = {
		[NVHWMON_POWER_AVG_GPU] = "average_gpu",
		[NVHWMON_POWER_AVG_MODULE] = "average_module",
		[NVHWMON_POWER_AVG_MEMORY] = "average_memory",
		[NVHWMON_POWER_INST_GPU] = "instant_gpu",
		[NVHWMON_POWER_INST_MODULE] = "instant_module",
		[NVHWMON_POWER_INST_CPU] = "instant_cpu",
		[NVHWMON_POWER_POLICY_TOTAL_GPU] = "policy_total_gpu",
		[NVHWMON_POWER_LIMIT_REQUESTED] = "limit_requested",
		[NVHWMON_POWER_LIMIT_ENFORCED] = "limit_enforced",
	};

	if (channel >= ARRAY_SIZE(labels) || !labels[channel])
		return "unknown";

	return labels[channel];
}
