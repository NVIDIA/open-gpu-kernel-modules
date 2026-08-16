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

#ifndef NVHWMON_MAIN_H
#define NVHWMON_MAIN_H

#include <linux/device.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/types.h>

#include "hwmon-entry.h"
#include "hwmon-nvidia.h"

#define NVHWMON_DRIVER_NAME "nvidia-hwmon"
#define NVHWMON_HWMON_NAME "nvidia_hwmon"
/* Static hwmon config sizes; is_visible() hides channels not present on a GPU. */
#define NVHWMON_MAX_FANS RUSD_FAN_COOLER_MAX_COOLERS
#define NVHWMON_FAN_LABEL_LEN 8
#define NVHWMON_RUSD_TEMP_CHANNELS RUSD_TEMPERATURE_TYPE_MAX
#define NVHWMON_THERMAL_MAX_SENSORS 16
#define NVHWMON_THERMAL_LABEL_LEN 32
#define NVHWMON_TEMP_CHANNELS \
	(NVHWMON_RUSD_TEMP_CHANNELS + NVHWMON_THERMAL_MAX_SENSORS)
#define NVHWMON_POWER_CHANNELS 9

struct nvhwmon_fan {
	bool pwm_native;
	bool pwm_manual;
	/* Tracks only writes made through hwmon so teardown can restore them. */
	bool manual_touched;
	u8 rm_index;
	char label[NVHWMON_FAN_LABEL_LEN];
	u32 max_rpm;
	u32 min_percent;
	u32 max_percent;
	u32 target_percent;
};

struct nvhwmon_rusd_cache {
	unsigned long temp_expires;
	unsigned long power_expires;
	unsigned long fan_expires;

	bool temp_valid[NVHWMON_RUSD_TEMP_CHANNELS];
	bool avg_power_valid;
	bool inst_power_valid;
	bool power_policy_valid;
	bool power_limit_valid;
	bool fan_valid;

	RUSD_TEMPERATURE temperatures[NVHWMON_RUSD_TEMP_CHANNELS];
	RUSD_AVG_POWER_USAGE avg_power;
	RUSD_INST_POWER_USAGE inst_power;
	RUSD_POWER_POLICY_STATUS power_policy;
	RUSD_POWER_LIMITS power_limit;
	RUSD_FAN_COOLER_STATUS fan_status;
};

struct nvhwmon_thermal_sensor {
	bool input_valid;
	bool min_valid;
	bool max_valid;
	u32 sensor_index;
	u32 target_type;
	s32 input_c;
	s32 min_c;
	s32 max_c;
	char label[NVHWMON_THERMAL_LABEL_LEN];
};

struct nvhwmon_temp_limits {
	bool max_valid;
	bool crit_valid;
	bool emergency_valid;
	s32 max_mc;
	s32 crit_mc;
	s32 emergency_mc;
};

struct nvhwmon_gpu {
	struct list_head node;
	/* Compact local slot used to derive non-overlapping RM object handles. */
	u32 index;
	/* Stable RM GPU id passed through nvidia_modeset_probe/remove. */
	u32 gpu_id;
	struct device *parent;
	struct device *nvhwmon_dev;
	/* Serializes RM-backed telemetry and fan control access. */
	struct mutex lock;
	/* True while nvidia_dev_get() holds a device usage reference. */
	bool opened;

	NvHandle h_client;
	NvHandle h_device;
	NvHandle h_subdevice;
	NvHandle h_rusd;
	void *rusd_map;
	/* RUSD is mmap-backed telemetry; thermal and fan controls use RM calls. */
	struct nvhwmon_rusd_cache rusd_cache;
	unsigned long thermal_expires;
	u8 thermal_count;
	struct nvhwmon_thermal_sensor
		thermal_sensors[NVHWMON_THERMAL_MAX_SENSORS];
	struct nvhwmon_temp_limits temp_limits[NVHWMON_TEMP_CHANNELS];
	unsigned long fan_control_expires;
	bool fan_control_valid;

	u8 fan_count;
	struct nvhwmon_fan fans[NVHWMON_MAX_FANS];
};

#endif
