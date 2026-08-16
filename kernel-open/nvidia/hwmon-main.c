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

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include "hwmon-main.h"
#include "hwmon-fan.h"
#include "hwmon-device.h"
#include "hwmon-rm.h"
#include "hwmon-rusd.h"
#include "hwmon-temp-limit.h"
#include "hwmon-thermal.h"

static LIST_HEAD(gpus);
static DEFINE_MUTEX(gpus_lock);
static bool active;

/* The list is keyed by RM GPU id; index is only for local RM handle slots. */
static struct nvhwmon_gpu *gpu_find_locked(u32 gpu_id)
{
	struct nvhwmon_gpu *gpu;

	list_for_each_entry(gpu, &gpus, node) {
		if (gpu->gpu_id == gpu_id)
			return gpu;
	}

	return NULL;
}

static int gpu_index_locked(void)
{
	bool used[NV_MAX_GPUS] = { 0 };
	struct nvhwmon_gpu *gpu;
	u32 i;

	list_for_each_entry(gpu, &gpus, node) {
		if (gpu->index < NV_MAX_GPUS)
			used[gpu->index] = true;
	}

	for (i = 0; i < NV_MAX_GPUS; i++) {
		if (!used[i])
			return i;
	}

	return -ENOSPC;
}

/*
 * Unregister sysfs first so no new hwmon callbacks can enter while fan state
 * and RM objects are being torn down.
 */
static void gpu_destroy(struct nvhwmon_gpu *gpu)
{
	if (!list_empty(&gpu->node))
		list_del_init(&gpu->node);

	nvhwmon_unregister_device(gpu);
	nvhwmon_fan_restore_all(gpu);
	nvhwmon_rusd_fini(gpu);
	nvhwmon_rm_free_gpu_objects(gpu);
	nvhwmon_rm_close_gpu(gpu);
	mutex_destroy(&gpu->lock);
	kfree(gpu);
}

static int gpu_create(const nv_gpu_info_t *info, u32 index,
		      struct nvhwmon_gpu **gpu_out)
{
	struct nvhwmon_gpu *gpu;
	int ret;

	gpu = kzalloc(sizeof(*gpu), GFP_KERNEL);
	if (!gpu)
		return -ENOMEM;

	INIT_LIST_HEAD(&gpu->node);
	mutex_init(&gpu->lock);
	gpu->index = index;
	gpu->gpu_id = info->gpu_id;
	gpu->parent = info->os_device_ptr;
	if (!gpu->parent) {
		ret = -ENODEV;
		goto fail;
	}

	/* Pins the NVIDIA device until destroy releases it before PCI removal. */
	ret = nvhwmon_rm_open_gpu(gpu);
	if (ret)
		goto fail;

	ret = nvhwmon_rm_alloc_gpu_objects(gpu);
	if (ret)
		goto fail;

	nvhwmon_fan_probe(gpu);
	nvhwmon_thermal_probe(gpu);
	nvhwmon_temp_limit_probe(gpu);

	ret = nvhwmon_rusd_init(gpu);
	if (ret)
		goto fail;

	ret = nvhwmon_register_device(gpu);
	if (ret)
		goto fail;

	*gpu_out = gpu;
	return 0;

fail:
	nvhwmon_unregister_device(gpu);
	nvhwmon_fan_restore_all(gpu);
	nvhwmon_rusd_fini(gpu);
	nvhwmon_rm_free_gpu_objects(gpu);
	nvhwmon_rm_close_gpu(gpu);
	mutex_destroy(&gpu->lock);
	kfree(gpu);
	return ret;
}

static int gpu_add_locked(const nv_gpu_info_t *info)
{
	struct nvhwmon_gpu *gpu;
	int index;
	int ret;

	if (!active)
		return -ENODEV;

	if (!info || !info->os_device_ptr)
		return -ENODEV;

	if (gpu_find_locked(info->gpu_id))
		return -EEXIST;

	index = gpu_index_locked();
	if (index < 0)
		return index;

	ret = gpu_create(info, index, &gpu);
	if (ret)
		return ret;

	list_add_tail(&gpu->node, &gpus);
	return 0;
}

/* Probe/remove hooks may race with module exit, so both share gpus_lock. */
void nvhwmon_driver_gpu_add(const nv_gpu_info_t *info)
{
	mutex_lock(&gpus_lock);
	gpu_add_locked(info);
	mutex_unlock(&gpus_lock);
}

void nvhwmon_driver_gpu_remove(NvU32 gpu_id)
{
	struct nvhwmon_gpu *gpu;

	mutex_lock(&gpus_lock);
	gpu = gpu_find_locked(gpu_id);
	if (gpu)
		gpu_destroy(gpu);
	mutex_unlock(&gpus_lock);
}

static void driver_shutdown(void)
{
	struct nvhwmon_gpu *gpu;

	mutex_lock(&gpus_lock);
	/* Blocks late hotplug adds before RM ops are unbound below. */
	active = false;
	while (!list_empty(&gpus)) {
		gpu = list_first_entry(&gpus, struct nvhwmon_gpu, node);
		gpu_destroy(gpu);
	}
	mutex_unlock(&gpus_lock);

	nvhwmon_rm_unbind();
}

int __init nvhwmon_driver_init(void)
{
	nv_gpu_info_t *gpu_info;
	u32 count;
	u32 i;
	int ret;

	ret = nvhwmon_rm_bind();
	if (ret)
		return ret;

	mutex_lock(&gpus_lock);
	active = true;
	mutex_unlock(&gpus_lock);

	gpu_info = kcalloc(NV_MAX_GPUS, sizeof(*gpu_info), GFP_KERNEL);
	if (!gpu_info) {
		ret = -ENOMEM;
		goto fail;
	}

	count = nvhwmon_rm_enumerate_gpus(gpu_info);
	if (!count) {
		ret = -ENODEV;
		goto fail_info;
	}
	if (count > NV_MAX_GPUS)
		count = NV_MAX_GPUS;

	for (i = 0; i < count; i++) {
		mutex_lock(&gpus_lock);
		gpu_add_locked(&gpu_info[i]);
		mutex_unlock(&gpus_lock);
	}

	kfree(gpu_info);

	mutex_lock(&gpus_lock);
	ret = list_empty(&gpus) ? -ENODEV : 0;
	mutex_unlock(&gpus_lock);
	if (ret)
		goto fail;

	return 0;

fail_info:
	kfree(gpu_info);
fail:
	driver_shutdown();
	return ret;
}

void __exit nvhwmon_driver_exit(void)
{
	driver_shutdown();
}
