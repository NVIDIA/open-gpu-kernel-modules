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
#include <linux/err.h>
#include <linux/hwmon.h>
#include <linux/kernel.h>
#include <linux/string.h>

#include "hwmon-fan.h"
#include "hwmon-device.h"
#include "hwmon-rusd.h"
#include "hwmon-temp-limit.h"
#include "hwmon-thermal.h"

#define TEMP_CONFIG                                           \
	(HWMON_T_INPUT | HWMON_T_LABEL | HWMON_T_MAX | HWMON_T_CRIT | \
	 HWMON_T_EMERGENCY | HWMON_T_RATED_MIN | HWMON_T_RATED_MAX)
#define POWER_CONFIG \
	(HWMON_P_INPUT | HWMON_P_LABEL | HWMON_P_AVERAGE | HWMON_P_CAP)
#define FAN_CONFIG (HWMON_F_INPUT | HWMON_F_LABEL | HWMON_F_MAX)
#define PWM_CONFIG (HWMON_PWM_INPUT | HWMON_PWM_ENABLE | HWMON_PWM_MODE)

/*
 * hwmon needs static channel tables. Per-GPU detection is enforced later by
 * is_visible(), so unsupported files never appear in sysfs.
 */
static const u32 temp_config[NVHWMON_TEMP_CHANNELS + 1] = {
	[0 ... NVHWMON_TEMP_CHANNELS - 1] = TEMP_CONFIG,
};

static const u32 power_config[NVHWMON_POWER_CHANNELS + 1] = {
	[0 ... NVHWMON_POWER_CHANNELS - 1] = POWER_CONFIG,
};

static const u32 fan_config[NVHWMON_MAX_FANS + 1] = {
	[0 ... NVHWMON_MAX_FANS - 1] = FAN_CONFIG,
};

static const u32 pwm_config[NVHWMON_MAX_FANS + 1] = {
	[0 ... NVHWMON_MAX_FANS - 1] = PWM_CONFIG,
};

static const struct hwmon_channel_info temp_channel_info = {
	.type = hwmon_temp,
	.config = temp_config,
};

static const struct hwmon_channel_info power_channel_info = {
	.type = hwmon_power,
	.config = power_config,
};

static const struct hwmon_channel_info fan_channel_info = {
	.type = hwmon_fan,
	.config = fan_config,
};

static const struct hwmon_channel_info pwm_channel_info = {
	.type = hwmon_pwm,
	.config = pwm_config,
};

static const struct hwmon_channel_info *const channel_info[] = {
	&temp_channel_info, &power_channel_info, &fan_channel_info,
	&pwm_channel_info, NULL
};

static umode_t is_visible(const void *drvdata, enum hwmon_sensor_types type,
			  u32 attr, int channel)
{
	const struct nvhwmon_gpu *gpu = drvdata;
	u32 temp_channel;

	if (channel < 0)
		return 0;

	switch (type) {
	case hwmon_temp:
		if (channel < NVHWMON_RUSD_TEMP_CHANNELS) {
			if (attr == hwmon_temp_input ||
			    attr == hwmon_temp_label)
				return 0444;
			if (nvhwmon_temp_limit_has(gpu, channel, attr))
				return 0444;
			return 0;
		}
		channel -= NVHWMON_RUSD_TEMP_CHANNELS;
		if (!nvhwmon_thermal_has_sensor(gpu, channel))
			return 0;
		if (attr == hwmon_temp_input || attr == hwmon_temp_label)
			return 0444;
		if (attr == hwmon_temp_rated_min &&
		    nvhwmon_thermal_has_rated_min(gpu, channel))
			return 0444;
		if (attr == hwmon_temp_rated_max &&
		    nvhwmon_thermal_has_rated_max(gpu, channel))
			return 0444;
		temp_channel = NVHWMON_RUSD_TEMP_CHANNELS + channel;
		if (nvhwmon_temp_limit_has(gpu, temp_channel, attr))
			return 0444;
		return 0;
	case hwmon_power:
		if (channel >= NVHWMON_POWER_CHANNELS)
			return 0;
		if (attr == hwmon_power_input || attr == hwmon_power_label)
			return 0444;
		if (attr == hwmon_power_average &&
		    nvhwmon_rusd_power_has_average(channel))
			return 0444;
		if (attr == hwmon_power_cap &&
		    nvhwmon_rusd_power_has_cap(channel))
			return 0444;
		return 0;
	case hwmon_fan:
		if (channel >= gpu->fan_count)
			return 0;
		if (attr == hwmon_fan_input || attr == hwmon_fan_label)
			return 0444;
		if (attr == hwmon_fan_max &&
		    nvhwmon_fan_has_max_rpm(gpu, channel))
			return 0444;
		return 0;
	case hwmon_pwm:
		if (!nvhwmon_fan_has_pwm(gpu, channel))
			return 0;
		if (attr == hwmon_pwm_input || attr == hwmon_pwm_enable ||
		    attr == hwmon_pwm_mode)
			return 0644;
		return 0;
	default:
		return 0;
	}
}

/*
 * Attributes backed by probe-time state avoid taking gpu->lock. Return -EAGAIN
 * for dynamic RM/RUSD reads so the common read path can lock once.
 */
static int read_static(struct nvhwmon_gpu *gpu, enum hwmon_sensor_types type,
		       u32 attr, int channel, long *val)
{
	switch (type) {
	case hwmon_temp:
		if (channel < 0 || channel >= NVHWMON_TEMP_CHANNELS)
			return -EINVAL;
		if (attr == hwmon_temp_max || attr == hwmon_temp_crit ||
		    attr == hwmon_temp_emergency)
			return nvhwmon_temp_limit_read(gpu, channel, attr, val);
		if (channel < NVHWMON_RUSD_TEMP_CHANNELS)
			return -EAGAIN;

		channel -= NVHWMON_RUSD_TEMP_CHANNELS;
		if (attr == hwmon_temp_rated_min)
			return nvhwmon_thermal_read_rated_min(gpu, channel,
							      val);
		if (attr == hwmon_temp_rated_max)
			return nvhwmon_thermal_read_rated_max(gpu, channel,
							      val);
		return -EAGAIN;
	case hwmon_fan:
		if (attr == hwmon_fan_max)
			return nvhwmon_fan_read_max_rpm(gpu, channel, val);
		return -EAGAIN;
	default:
		return -EAGAIN;
	}
}

static int read_temp(struct nvhwmon_gpu *gpu, u32 attr, int channel, long *val)
{
	int thermal_channel;

	if (channel < 0 || channel >= NVHWMON_TEMP_CHANNELS)
		return -EINVAL;

	if (attr == hwmon_temp_max || attr == hwmon_temp_crit ||
	    attr == hwmon_temp_emergency)
		return nvhwmon_temp_limit_read(gpu, channel, attr, val);

	if (channel < NVHWMON_RUSD_TEMP_CHANNELS)
		return attr == hwmon_temp_input ?
			       nvhwmon_rusd_read_temp(gpu, channel, val) :
			       -EOPNOTSUPP;

	thermal_channel = channel - NVHWMON_RUSD_TEMP_CHANNELS;
	switch (attr) {
	case hwmon_temp_input:
		return nvhwmon_thermal_read_temp(gpu, thermal_channel, val);
	case hwmon_temp_rated_min:
		return nvhwmon_thermal_read_rated_min(gpu, thermal_channel,
							      val);
	case hwmon_temp_rated_max:
		return nvhwmon_thermal_read_rated_max(gpu, thermal_channel,
							      val);
	default:
		return -EOPNOTSUPP;
	}
}

static int read(struct device *dev, enum hwmon_sensor_types type, u32 attr,
		int channel, long *val)
{
	struct nvhwmon_gpu *gpu = dev_get_drvdata(dev);
	int ret;

	ret = read_static(gpu, type, attr, channel, val);
	if (ret != -EAGAIN)
		return ret;

	mutex_lock(&gpu->lock);

	switch (type) {
	case hwmon_temp:
		ret = read_temp(gpu, attr, channel, val);
		break;
	case hwmon_power:
		if (attr == hwmon_power_input)
			ret = nvhwmon_rusd_read_power(gpu, channel, val);
		else if (attr == hwmon_power_average)
			ret = nvhwmon_rusd_read_power_average(gpu, channel, val);
		else if (attr == hwmon_power_cap)
			ret = nvhwmon_rusd_read_power_cap(gpu, channel, val);
		else
			ret = -EOPNOTSUPP;
		break;
	case hwmon_fan:
		if (attr == hwmon_fan_input)
			ret = nvhwmon_rusd_read_fan(gpu, channel, val);
		else if (attr == hwmon_fan_max)
			ret = nvhwmon_fan_read_max_rpm(gpu, channel, val);
		else
			ret = -EOPNOTSUPP;
		break;
	case hwmon_pwm:
		if (attr == hwmon_pwm_input)
			ret = nvhwmon_fan_read_pwm(gpu, channel, val);
		else if (attr == hwmon_pwm_enable)
			ret = nvhwmon_fan_read_pwm_enable(gpu, channel, val);
		else if (attr == hwmon_pwm_mode)
			ret = nvhwmon_fan_read_pwm_mode(gpu, channel, val);
		else
			ret = -EOPNOTSUPP;
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	mutex_unlock(&gpu->lock);
	return ret;
}

/* Writes use the same per-GPU RM lock as dynamic reads. */
static int write(struct device *dev, enum hwmon_sensor_types type, u32 attr,
		 int channel, long val)
{
	struct nvhwmon_gpu *gpu = dev_get_drvdata(dev);
	int ret;

	if (channel < 0)
		return -EINVAL;

	mutex_lock(&gpu->lock);

	if (type == hwmon_pwm && attr == hwmon_pwm_input)
		ret = nvhwmon_fan_write_pwm(gpu, channel, val);
	else if (type == hwmon_pwm && attr == hwmon_pwm_enable)
		ret = nvhwmon_fan_write_pwm_enable(gpu, channel, val);
	else if (type == hwmon_pwm && attr == hwmon_pwm_mode)
		ret = nvhwmon_fan_write_pwm_mode(gpu, channel, val);
	else
		ret = -EOPNOTSUPP;

	mutex_unlock(&gpu->lock);
	return ret;
}

static int read_string(struct device *dev, enum hwmon_sensor_types type,
		       u32 attr, int channel, const char **str)
{
	struct nvhwmon_gpu *gpu = dev_get_drvdata(dev);
	int thermal_channel;

	if (channel < 0)
		return -EINVAL;

	switch (type) {
	case hwmon_temp:
		if (attr != hwmon_temp_label)
			return -EOPNOTSUPP;
		if (channel < NVHWMON_RUSD_TEMP_CHANNELS) {
			*str = nvhwmon_rusd_temp_label(channel);
		} else {
			thermal_channel = channel - NVHWMON_RUSD_TEMP_CHANNELS;
			*str = nvhwmon_thermal_label(gpu, thermal_channel);
		}
		return 0;
	case hwmon_power:
		if (attr != hwmon_power_label)
			return -EOPNOTSUPP;
		*str = nvhwmon_rusd_power_label(channel);
		return 0;
	case hwmon_fan:
		if (attr != hwmon_fan_label || channel >= gpu->fan_count)
			return -EOPNOTSUPP;
		*str = gpu->fans[channel].label;
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static const struct hwmon_ops ops = {
	.is_visible = is_visible,
	.read = read,
	.write = write,
	.read_string = read_string,
};

static const struct hwmon_chip_info chip_info = {
	.ops = &ops,
	.info = channel_info,
};

int nvhwmon_register_device(struct nvhwmon_gpu *gpu)
{
	const struct hwmon_chip_info *chip = &chip_info;
	struct device *dev = gpu->parent;
	struct device *nvhwmon_dev;

	BUILD_BUG_ON(ARRAY_SIZE(temp_config) != NVHWMON_TEMP_CHANNELS + 1);
	BUILD_BUG_ON(ARRAY_SIZE(power_config) != NVHWMON_POWER_CHANNELS + 1);
	BUILD_BUG_ON(ARRAY_SIZE(fan_config) != NVHWMON_MAX_FANS + 1);
	BUILD_BUG_ON(ARRAY_SIZE(pwm_config) != NVHWMON_MAX_FANS + 1);

	nvhwmon_dev = hwmon_device_register_with_info(dev, NVHWMON_HWMON_NAME, gpu, chip, NULL);
	if (IS_ERR(nvhwmon_dev))
		return PTR_ERR(nvhwmon_dev);

	gpu->nvhwmon_dev = nvhwmon_dev;
	return 0;
}

void nvhwmon_unregister_device(struct nvhwmon_gpu *gpu)
{
	if (IS_ERR_OR_NULL(gpu->nvhwmon_dev))
		return;

	hwmon_device_unregister(gpu->nvhwmon_dev);
	gpu->nvhwmon_dev = NULL;
}
