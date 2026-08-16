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
#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/math.h>
#include <linux/string.h>

#include "hwmon-fan.h"
#include "hwmon-rm.h"

#define CTRL_CMD_CLIENT_FAN_COOLERS_GET_INFO	0x2080852eU
#define CTRL_CMD_CLIENT_FAN_COOLERS_GET_STATUS	0x2080852fU
#define CTRL_CMD_CLIENT_FAN_COOLERS_GET_CONTROL	0x20808530U
#define CTRL_CMD_CLIENT_FAN_COOLERS_SET_CONTROL	0x2080c531U

#define FAN_INFO_PARAMS_SIZE			596
#define FAN_STATUS_PARAMS_SIZE			844
#define FAN_CONTROL_PARAMS_SIZE			980
#define FAN_INFO_ENTRY_COUNT			NVHWMON_MAX_FANS
#define FAN_STATUS_ENTRY_COUNT			NVHWMON_MAX_FANS
#define FAN_CONTROL_ENTRY_COUNT			NVHWMON_MAX_FANS
#define FAN_CONTROL_CACHE_MS			1000
#define FAN_CONTROL_GET_ALL			3
#define FAN_CONTROL_RESTORE_DEFAULT		1
#define FAN_CONTROL_PWM_MODE			1
#define FAN_CONTROL_AUTO			2
#define FAN_CONTROL_MANUAL			1

/*
 * These payloads model only fields used by hwmon. BUILD_BUG_ON checks keep the
 * total sizes aligned with the RM control ABI.
 */
struct fan_info_entry {
	u32 unknown0;
	u32 unknown1;
	u32 unknown2;
	u32 unknown3;
	u32 unknown4;
	u32 unknown5;
	u32 unknown6;
	u32 unknown7;
	u32 max_rpm;
};

struct fan_info_params {
	u32 version;
	u32 unknown0;
	u32 unknown1;
	u32 unknown2;
	u32 unknown3;
	struct fan_info_entry entries[FAN_INFO_ENTRY_COUNT];
};

struct fan_status_entry {
	u32 flags;
	u32 unknown0;
	u32 unknown1;
	u32 min_percent_q16;
	u32 max_percent_q16;
	u32 unknown2;
	u32 unknown3;
	u32 unknown4;
	u32 unknown5;
	u32 unknown6;
	u32 unknown7;
	u32 unknown8;
	u32 unknown9;
};

struct fan_status_params {
	u32 version;
	u32 selector;
	u32 unknown0;
	struct fan_status_entry entries[FAN_STATUS_ENTRY_COUNT];
};

struct fan_control_entry {
	u32 flags;
	u32 unknown0;
	u32 unknown1;
	u32 unknown2;
	u32 manual;
	u32 target_percent_q16;
	u32 unknown3;
	u32 unknown4;
	u32 unknown5;
	u32 unknown6;
	u32 unknown7;
	u32 unknown8;
	u32 unknown9;
	u32 unknown10;
	u32 unknown11;
};

struct fan_control_params {
	u32 version;
	u32 selector;
	u32 unknown0;
	u32 request;
	u32 index_or_restore;
	struct fan_control_entry entries[FAN_CONTROL_ENTRY_COUNT];
};

static u32 q16_to_percent(u32 q16)
{
	u32 percent;

	percent = DIV_ROUND_CLOSEST_ULL((u64)q16 * 100, 65536);
	return min(percent, 100U);
}

static u32 percent_to_q16(u32 percent)
{
	return DIV_ROUND_CLOSEST(percent * 65536U, 100U);
}

static long percent_to_pwm(u32 percent)
{
	return DIV_ROUND_CLOSEST(percent * 255U, 100U);
}

static u32 pwm_to_percent(long pwm)
{
	return DIV_ROUND_CLOSEST((u32)pwm * 100U, 255U);
}

static bool status_entry_present(const struct fan_status_entry *entry)
{
	return entry->flags || entry->min_percent_q16 || entry->max_percent_q16;
}

static bool status_entry_limits(const struct fan_status_entry *entry,
				u32 *min_percent, u32 *max_percent)
{
	u32 min_value;
	u32 max_value;

	if (!status_entry_present(entry))
		return false;

	min_value = q16_to_percent(entry->min_percent_q16);
	max_value = q16_to_percent(entry->max_percent_q16);
	if (min_value > max_value)
		return false;

	*min_percent = min_value;
	*max_percent = max_value;
	return true;
}

static bool control_entry_present(const struct fan_control_entry *entry)
{
	return entry->flags || entry->target_percent_q16 || entry->manual;
}

static int control_read_all(struct nvhwmon_gpu *gpu,
			    struct fan_control_params *control,
			    struct fan_status_params *status)
{
	u32 rm_status;

	BUILD_BUG_ON(sizeof(*control) != FAN_CONTROL_PARAMS_SIZE);
	BUILD_BUG_ON(sizeof(*status) != FAN_STATUS_PARAMS_SIZE);

	memset(control, 0, sizeof(*control));
	memset(status, 0, sizeof(*status));

	control->request = FAN_CONTROL_GET_ALL;
	rm_status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				       CTRL_CMD_CLIENT_FAN_COOLERS_GET_CONTROL,
				       control, sizeof(*control));
	if (rm_status != NVOS_STATUS_SUCCESS)
		return nvhwmon_rm_status_to_errno(rm_status);

	status->selector = 1;
	rm_status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				       CTRL_CMD_CLIENT_FAN_COOLERS_GET_STATUS,
				       status, sizeof(*status));
	if (rm_status != NVOS_STATUS_SUCCESS)
		return nvhwmon_rm_status_to_errno(rm_status);

	return 0;
}

static void control_apply(struct nvhwmon_gpu *gpu,
			  const struct fan_control_params *control,
			  const struct fan_status_params *status)
{
	bool global_status_present;
	u32 global_max_percent = 0;
	u32 global_min_percent = 0;
	u32 fan;

	/*
	 * Status entry 0 can describe global fan limits. Per-cooler entries
	 * override those limits when populated.
	 */
	global_status_present = status_entry_limits(&status->entries[0],
						    &global_min_percent,
						    &global_max_percent);

	for (fan = 0; fan < gpu->fan_count; fan++) {
		struct nvhwmon_fan *state = &gpu->fans[fan];
			const struct fan_control_entry *control_entry;
		bool status_present = global_status_present;
		u32 max_percent = global_max_percent;
		u32 min_percent = global_min_percent;
		u8 rm_index = state->rm_index;

		if (rm_index >= ARRAY_SIZE(control->entries)) {
			state->pwm_native = false;
			continue;
		}

		if (rm_index < ARRAY_SIZE(status->entries) &&
		    status_entry_limits(&status->entries[rm_index],
					&min_percent, &max_percent))
			status_present = true;

		control_entry = &control->entries[rm_index];
		state->pwm_native =
		    status_present &&
		    control_entry_present(control_entry);
		if (!state->pwm_native)
			continue;

		state->pwm_manual = control_entry->manual != 0;
		if (control_entry->target_percent_q16)
			state->target_percent =
				q16_to_percent(control_entry->target_percent_q16);
		else
			state->target_percent = min_percent;

		state->min_percent = min_percent;
		state->max_percent = max_percent;
	}

	gpu->fan_control_valid = true;
	gpu->fan_control_expires =
		jiffies + msecs_to_jiffies(FAN_CONTROL_CACHE_MS);
}

static int control_refresh(struct nvhwmon_gpu *gpu, bool force)
{
	struct fan_control_params control;
	struct fan_status_params status;
	int ret;

	if (!force && gpu->fan_control_valid &&
	    time_before(jiffies, gpu->fan_control_expires))
		return 0;

	ret = control_read_all(gpu, &control, &status);
	if (ret)
		return ret;

	control_apply(gpu, &control, &status);
	return 0;
}

static void control_invalidate(struct nvhwmon_gpu *gpu)
{
	gpu->fan_control_valid = false;
	gpu->fan_control_expires = 0;
}

static int control_restore_default(struct nvhwmon_gpu *gpu, u32 channel);

static void fan_set_present(struct nvhwmon_gpu *gpu, u8 channel, u32 rm_index)
{
	struct nvhwmon_fan *state = &gpu->fans[channel];

	state->rm_index = rm_index;
	scnprintf(state->label, sizeof(state->label), "fan%u", channel);
}

static int control_set_manual(struct nvhwmon_gpu *gpu, u32 channel, u32 percent)
{
	struct fan_control_params params = { 0 };
	bool restore_on_set_failure;
	u8 rm_index;
	u32 status;

	BUILD_BUG_ON(sizeof(params) != FAN_CONTROL_PARAMS_SIZE);

	if (channel >= gpu->fan_count)
		return -EINVAL;

	rm_index = gpu->fans[channel].rm_index;
	if (rm_index >= ARRAY_SIZE(params.entries))
		return -EINVAL;

	params.request = FAN_CONTROL_GET_ALL;
	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    CTRL_CMD_CLIENT_FAN_COOLERS_GET_CONTROL,
				    &params, sizeof(params));
	if (status != NVOS_STATUS_SUCCESS)
		return nvhwmon_rm_status_to_errno(status);

	if (!control_entry_present(&params.entries[rm_index]))
		return -ENODEV;

	/*
	 * If a write fails after touching manual state, try to put the cooler
	 * back into automatic control before reporting the error.
	 */
	restore_on_set_failure = gpu->fans[channel].manual_touched ||
				 !params.entries[rm_index].manual;
	params.entries[rm_index].manual = 1;
	params.entries[rm_index].target_percent_q16 =
		percent_to_q16(percent);

	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    CTRL_CMD_CLIENT_FAN_COOLERS_SET_CONTROL,
				    &params, sizeof(params));
	if (status != NVOS_STATUS_SUCCESS) {
		if (restore_on_set_failure) {
			gpu->fans[channel].manual_touched = true;
			control_restore_default(gpu, channel);
		}
		return nvhwmon_rm_status_to_errno(status);
	}

	gpu->fans[channel].pwm_manual = true;
	gpu->fans[channel].manual_touched = true;
	gpu->fans[channel].target_percent = percent;
	control_invalidate(gpu);
	return 0;
}

static int control_restore_default(struct nvhwmon_gpu *gpu, u32 channel)
{
	struct fan_control_params params = { 0 };
	u8 rm_index;
	u32 status;

	BUILD_BUG_ON(sizeof(params) != FAN_CONTROL_PARAMS_SIZE);

	if (channel >= gpu->fan_count)
		return -EINVAL;

	rm_index = gpu->fans[channel].rm_index;
	if (rm_index >= FAN_CONTROL_ENTRY_COUNT)
		return -EINVAL;

	params.request = rm_index + 1;
	params.index_or_restore = FAN_CONTROL_RESTORE_DEFAULT;
	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    CTRL_CMD_CLIENT_FAN_COOLERS_GET_CONTROL,
				    &params, sizeof(params));
	if (status != NVOS_STATUS_SUCCESS)
		return nvhwmon_rm_status_to_errno(status);

	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    CTRL_CMD_CLIENT_FAN_COOLERS_SET_CONTROL,
				    &params, sizeof(params));
	if (status != NVOS_STATUS_SUCCESS)
		return nvhwmon_rm_status_to_errno(status);

	gpu->fans[channel].pwm_manual = false;
	gpu->fans[channel].manual_touched = false;
	control_invalidate(gpu);
	return 0;
}

static void control_probe(struct nvhwmon_gpu *gpu)
{
	struct fan_control_params control;
	struct fan_status_params status;
	u32 fan;
	u8 count = 0;

	if (control_read_all(gpu, &control, &status))
		return;

	for (fan = 0; fan < ARRAY_SIZE(control.entries); fan++) {
		bool control_present =
		    control_entry_present(&control.entries[fan]);
		bool status_present =
		    fan > 0 && fan < ARRAY_SIZE(status.entries) &&
		    status_entry_present(&status.entries[fan]);

		/*
		 * Status entry 0 can be global limits, so it does not prove
		 * cooler 0 exists unless its control entry is also populated.
		 */
		if (!control_present && !status_present)
			continue;

		fan_set_present(gpu, count, fan);
		count++;

		if (count == NVHWMON_MAX_FANS)
			break;
	}

	gpu->fan_count = count;
	if (gpu->fan_count)
		control_apply(gpu, &control, &status);
}

void nvhwmon_fan_probe(struct nvhwmon_gpu *gpu)
{
	struct fan_info_params params = { 0 };
	u32 status;
	u32 fan;
	u8 count = 0;

	BUILD_BUG_ON(sizeof(params) != FAN_INFO_PARAMS_SIZE);

	/* Empty cooler entries are skipped; rm_index preserves the RM slot id. */
	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    CTRL_CMD_CLIENT_FAN_COOLERS_GET_INFO,
				    &params, sizeof(params));
	if (status != NVOS_STATUS_SUCCESS) {
		control_probe(gpu);
		return;
	}

	for (fan = 0; fan < ARRAY_SIZE(params.entries); fan++) {
		if (!memchr_inv(&params.entries[fan], 0,
				sizeof(params.entries[fan])))
			continue;

		fan_set_present(gpu, count, fan);
		gpu->fans[count].max_rpm = params.entries[fan].max_rpm;
		count++;

		if (count == NVHWMON_MAX_FANS)
			break;
	}

	gpu->fan_count = count;

	if (gpu->fan_count)
		control_refresh(gpu, true);
	else
		control_probe(gpu);
}

/* Called on teardown and on failed writes that may have changed fan mode. */
void nvhwmon_fan_restore_all(struct nvhwmon_gpu *gpu)
{
	u32 fan;

	mutex_lock(&gpu->lock);

	for (fan = 0; fan < gpu->fan_count; fan++) {
		int ret;

		if (!gpu->fans[fan].manual_touched)
			continue;

		ret = control_restore_default(gpu, fan);
		if (ret)
			dev_warn(gpu->parent,
				 "%s: failed to restore fan %u automatic control: %d\n",
				 NVHWMON_DRIVER_NAME, fan, ret);
	}

	mutex_unlock(&gpu->lock);
}

static void restore_after_failure(struct nvhwmon_gpu *gpu, u32 channel)
{
	int ret;

	if (channel >= gpu->fan_count || !gpu->fans[channel].manual_touched)
		return;

	ret = control_restore_default(gpu, channel);
	if (ret)
		dev_warn(gpu->parent,
			 "%s: failed to restore fan %u after fan-control failure: %d\n",
			 NVHWMON_DRIVER_NAME, channel, ret);
}

bool nvhwmon_fan_has_max_rpm(const struct nvhwmon_gpu *gpu, u32 channel)
{
	if (channel >= gpu->fan_count)
		return false;

	return gpu->fans[channel].max_rpm != 0;
}

int nvhwmon_fan_read_max_rpm(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	if (channel >= gpu->fan_count || !gpu->fans[channel].max_rpm)
		return -EINVAL;

	*val = gpu->fans[channel].max_rpm;
	return 0;
}

bool nvhwmon_fan_has_pwm(const struct nvhwmon_gpu *gpu, u32 channel)
{
	if (channel >= gpu->fan_count)
		return false;

	return gpu->fans[channel].pwm_native;
}

int nvhwmon_fan_read_pwm(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	int ret;

	if (channel >= gpu->fan_count || !gpu->fans[channel].pwm_native)
		return -EINVAL;

	ret = control_refresh(gpu, false);
	if (ret)
		return ret;

	*val = percent_to_pwm(gpu->fans[channel].target_percent);
	return 0;
}

int nvhwmon_fan_write_pwm(struct nvhwmon_gpu *gpu, u32 channel, long val)
{
	struct nvhwmon_fan *fan;
	u32 percent;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (channel >= gpu->fan_count || !gpu->fans[channel].pwm_native)
		return -EINVAL;

	if (val < 0 || val > 255)
		return -EINVAL;

	ret = control_refresh(gpu, false);
	if (ret) {
		restore_after_failure(gpu, channel);
		return ret;
	}

	fan = &gpu->fans[channel];
	percent = pwm_to_percent(val);
	if (percent < fan->min_percent || percent > fan->max_percent)
		return -EINVAL;

	ret = control_set_manual(gpu, channel, percent);
	if (ret)
		restore_after_failure(gpu, channel);

	return ret;
}

int nvhwmon_fan_read_pwm_enable(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	int ret;

	if (channel >= gpu->fan_count || !gpu->fans[channel].pwm_native)
		return -EINVAL;

	ret = control_refresh(gpu, false);
	if (ret)
		return ret;

	*val = gpu->fans[channel].pwm_manual ? FAN_CONTROL_MANUAL
					     : FAN_CONTROL_AUTO;
	return 0;
}

int nvhwmon_fan_write_pwm_enable(struct nvhwmon_gpu *gpu, u32 channel, long val)
{
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (channel >= gpu->fan_count || !gpu->fans[channel].pwm_native)
		return -EINVAL;

	if (val == FAN_CONTROL_AUTO)
		return control_restore_default(gpu, channel);

	if (val != FAN_CONTROL_MANUAL)
		return -EINVAL;

	ret = control_refresh(gpu, false);
	if (ret) {
		restore_after_failure(gpu, channel);
		return ret;
	}

	ret = control_set_manual(gpu, channel,
				 gpu->fans[channel].target_percent);
	if (ret)
		restore_after_failure(gpu, channel);

	return ret;
}

int nvhwmon_fan_read_pwm_mode(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	if (channel >= gpu->fan_count || !gpu->fans[channel].pwm_native)
		return -EINVAL;

	*val = FAN_CONTROL_PWM_MODE;
	return 0;
}

int nvhwmon_fan_write_pwm_mode(struct nvhwmon_gpu *gpu, u32 channel, long val)
{
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (channel >= gpu->fan_count || !gpu->fans[channel].pwm_native)
		return -EINVAL;

	if (val != FAN_CONTROL_PWM_MODE)
		return -EINVAL;

	return 0;
}
