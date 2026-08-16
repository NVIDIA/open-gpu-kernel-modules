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
#include <linux/string.h>

#include "hwmon-rm.h"
#include "hwmon-thermal.h"

#define THERMAL_CACHE_MS 1000
#define THERMAL_ERROR_RETRY_MS 100
#define THERMAL_TARGET_SLOTS 5

/* Thermal-system queries batch small instructions into one RM control call. */
static int execute(struct nvhwmon_gpu *gpu,
		   NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS *params)
{
	u32 status;

	params->clientAPIVersion = THERMAL_SYSTEM_API_VER;
	params->clientAPIRevision = THERMAL_SYSTEM_API_REV;
	params->clientInstructionSizeOf =
		sizeof(NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION);

	status = nvhwmon_rm_control(gpu->h_client, gpu->h_subdevice,
				    NV2080_CTRL_CMD_THERMAL_SYSTEM_EXECUTE_V2,
				    params, sizeof(*params));
	if (status != NVOS_STATUS_SUCCESS)
		return nvhwmon_rm_status_to_errno(status);

	return 0;
}

static bool
instruction_ok(const NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION *instruction)
{
	return instruction->executed &&
	       instruction->result == NVOS_STATUS_SUCCESS;
}

static int get_sensor_count(struct nvhwmon_gpu *gpu, u32 *count)
{
	NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS params = { 0 };
	NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION *instruction;
	int ret;

	params.instructionListSize = 1;
	instruction = &params.instructionList[0];
	instruction->opcode =
		NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSORS_AVAILABLE_OPCODE;

	ret = execute(gpu, &params);
	if (ret)
		return ret;

	if (!instruction_ok(instruction))
		return -ENODATA;

	*count = instruction->operands.getInfoSensorsAvailable.availableSensors;
	return 0;
}

static const char *target_name(u32 target_type)
{
	switch (target_type) {
	case NV2080_CTRL_THERMAL_SYSTEM_TARGET_GPU:
		return "gpu_hw";
	case NV2080_CTRL_THERMAL_SYSTEM_TARGET_MEMORY:
		return "memory_hw";
	case NV2080_CTRL_THERMAL_SYSTEM_TARGET_POWER_SUPPLY:
		return "power_supply_hw";
	case NV2080_CTRL_THERMAL_SYSTEM_TARGET_BOARD:
		return "board_hw";
	default:
		return "thermal_hw";
	}
}

static u32 target_slot(u32 target_type)
{
	switch (target_type) {
	case NV2080_CTRL_THERMAL_SYSTEM_TARGET_GPU:
		return 0;
	case NV2080_CTRL_THERMAL_SYSTEM_TARGET_MEMORY:
		return 1;
	case NV2080_CTRL_THERMAL_SYSTEM_TARGET_POWER_SUPPLY:
		return 2;
	case NV2080_CTRL_THERMAL_SYSTEM_TARGET_BOARD:
		return 3;
	default:
		return 4;
	}
}

static void set_label(struct nvhwmon_thermal_sensor *sensor, u32 target_seen)
{
	const char *base = target_name(sensor->target_type);

	if (target_seen)
		scnprintf(sensor->label, sizeof(sensor->label), "%s%u", base,
			  target_seen + 1);
	else
		strscpy(sensor->label, base, sizeof(sensor->label));
}

static void probe_sensor(struct nvhwmon_gpu *gpu, u32 sensor_index,
			 u32 *target_counts)
{
	NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS params = { 0 };
	struct nvhwmon_thermal_sensor *sensor;
	NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION *instruction;
	u32 target_index;
	u32 slot;
	int ret;

	if (gpu->thermal_count >= NVHWMON_THERMAL_MAX_SENSORS)
		return;

	/* Target index is required for stable labels; reading range is optional. */
	params.executeFlags =
		NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_FLAGS_IGNORE_FAIL;
	params.instructionListSize = 2;

	instruction = &params.instructionList[0];
	instruction->opcode =
		NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_TARGET_OPCODE;
	instruction->operands.getInfoSensorTarget.sensorIndex = sensor_index;

	instruction = &params.instructionList[1];
	instruction->opcode =
		NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_SENSOR_READING_RANGE_OPCODE;
	instruction->operands.getInfoSensorReadingRange.sensorIndex =
		sensor_index;

	ret = execute(gpu, &params);
	if (ret)
		return;

	if (!instruction_ok(&params.instructionList[0]))
		return;

	sensor = &gpu->thermal_sensors[gpu->thermal_count];
	memset(sensor, 0, sizeof(*sensor));
	sensor->sensor_index = sensor_index;
	target_index = params.instructionList[0]
			       .operands.getInfoSensorTarget.targetIndex;

	if (instruction_ok(&params.instructionList[1])) {
		sensor->min_valid = true;
		sensor->max_valid = true;
		sensor->min_c =
			params.instructionList[1]
				.operands.getInfoSensorReadingRange.minimum;
		sensor->max_c =
			params.instructionList[1]
				.operands.getInfoSensorReadingRange.maximum;
	}

	memset(&params, 0, sizeof(params));
	params.executeFlags =
		NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_FLAGS_IGNORE_FAIL;
	params.instructionListSize = 1;

	instruction = &params.instructionList[0];
	instruction->opcode =
		NV2080_CTRL_THERMAL_SYSTEM_GET_INFO_TARGET_TYPE_OPCODE;
	instruction->operands.getInfoTargetType.targetIndex = target_index;

	/* Keep the sensor visible even if target type is unavailable. */
	ret = execute(gpu, &params);
	if (!ret && instruction_ok(&params.instructionList[0]))
		sensor->target_type = params.instructionList[0]
					      .operands.getInfoTargetType.type;
	else
		sensor->target_type = NV2080_CTRL_THERMAL_SYSTEM_TARGET_UNKNOWN;

	slot = target_slot(sensor->target_type);
	set_label(sensor, target_counts[slot]);
	target_counts[slot]++;
	gpu->thermal_count++;
}

void nvhwmon_thermal_probe(struct nvhwmon_gpu *gpu)
{
	u32 target_counts[THERMAL_TARGET_SLOTS] = { 0 };
	u32 count;
	u32 sensor;
	int ret;

	BUILD_BUG_ON(NVHWMON_THERMAL_MAX_SENSORS >
		     NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION_MAX_COUNT);

	ret = get_sensor_count(gpu, &count);
	if (ret)
		return;

	count = min_t(u32, count, NVHWMON_THERMAL_MAX_SENSORS);
	for (sensor = 0; sensor < count; sensor++)
		probe_sensor(gpu, sensor, target_counts);
}

static unsigned long cache_expires(bool retry_soon)
{
	u32 cache_ms = retry_soon ? THERMAL_ERROR_RETRY_MS :
				    THERMAL_CACHE_MS;

	return jiffies + msecs_to_jiffies(cache_ms);
}

static void refresh(struct nvhwmon_gpu *gpu)
{
	NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_V2_PARAMS params = { 0 };
	bool retry_soon = false;
	u32 channel;
	int ret;

	if (gpu->thermal_expires && time_before(jiffies, gpu->thermal_expires))
		return;

	/* Read all probed sensors in one batch and retry sooner on partial data. */
	params.executeFlags =
		NV2080_CTRL_THERMAL_SYSTEM_EXECUTE_FLAGS_IGNORE_FAIL;
	params.instructionListSize = gpu->thermal_count;

	for (channel = 0; channel < gpu->thermal_count; channel++) {
		NV2080_CTRL_THERMAL_SYSTEM_INSTRUCTION *instruction;

		instruction = &params.instructionList[channel];
		instruction->opcode =
			NV2080_CTRL_THERMAL_SYSTEM_GET_STATUS_SENSOR_READING_OPCODE;
		instruction->operands.getStatusSensorReading.sensorIndex =
			gpu->thermal_sensors[channel].sensor_index;
	}

	ret = execute(gpu, &params);
	if (ret) {
		for (channel = 0; channel < gpu->thermal_count; channel++)
			gpu->thermal_sensors[channel].input_valid = false;
		gpu->thermal_expires = cache_expires(true);
		return;
	}

	for (channel = 0; channel < gpu->thermal_count; channel++) {
		struct nvhwmon_thermal_sensor *sensor;

		sensor = &gpu->thermal_sensors[channel];
		if (instruction_ok(&params.instructionList[channel])) {
			sensor->input_valid = true;
			sensor->input_c =
				params.instructionList[channel]
					.operands.getStatusSensorReading.value;
		} else {
			sensor->input_valid = false;
			retry_soon = true;
		}
	}

	gpu->thermal_expires = cache_expires(retry_soon);
}

bool nvhwmon_thermal_has_sensor(const struct nvhwmon_gpu *gpu, u32 channel)
{
	return channel < gpu->thermal_count;
}

bool nvhwmon_thermal_has_rated_min(const struct nvhwmon_gpu *gpu, u32 channel)
{
	return nvhwmon_thermal_has_sensor(gpu, channel) &&
	       gpu->thermal_sensors[channel].min_valid;
}

bool nvhwmon_thermal_has_rated_max(const struct nvhwmon_gpu *gpu, u32 channel)
{
	return nvhwmon_thermal_has_sensor(gpu, channel) &&
	       gpu->thermal_sensors[channel].max_valid;
}

int nvhwmon_thermal_read_temp(struct nvhwmon_gpu *gpu, u32 channel, long *val)
{
	if (!nvhwmon_thermal_has_sensor(gpu, channel))
		return -EINVAL;

	refresh(gpu);
	if (!gpu->thermal_sensors[channel].input_valid)
		return -ENODATA;

	*val = (long)gpu->thermal_sensors[channel].input_c * 1000;
	return 0;
}

int nvhwmon_thermal_read_rated_min(struct nvhwmon_gpu *gpu, u32 channel,
				   long *val)
{
	if (!nvhwmon_thermal_has_rated_min(gpu, channel))
		return -EINVAL;

	*val = (long)gpu->thermal_sensors[channel].min_c * 1000;
	return 0;
}

int nvhwmon_thermal_read_rated_max(struct nvhwmon_gpu *gpu, u32 channel,
				   long *val)
{
	if (!nvhwmon_thermal_has_rated_max(gpu, channel))
		return -EINVAL;

	*val = (long)gpu->thermal_sensors[channel].max_c * 1000;
	return 0;
}

const char *nvhwmon_thermal_label(const struct nvhwmon_gpu *gpu, u32 channel)
{
	if (!nvhwmon_thermal_has_sensor(gpu, channel))
		return "unknown";

	return gpu->thermal_sensors[channel].label;
}
