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
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/string.h>

#include "hwmon-rm.h"

/* Private handle namespace for objects allocated by this in-tree hwmon path. */
#define RM_HANDLE_BASE 0x5a000000U
#define RM_HANDLE_STRIDE 0x00000100U
#define RM_HANDLE_DEVICE 0x00000001U
#define RM_HANDLE_SUBDEVICE 0x00000002U
#define RM_HANDLE_RUSD 0x00000003U

static nvidia_modeset_rm_ops_t rm_ops;

static NvHandle rm_handle(const struct nvhwmon_gpu *gpu, u32 slot)
{
	return RM_HANDLE_BASE + gpu->index * RM_HANDLE_STRIDE +
	       slot;
}

NvHandle nvhwmon_rm_rusd_handle(const struct nvhwmon_gpu *gpu)
{
	return rm_handle(gpu, RM_HANDLE_RUSD);
}

int nvhwmon_rm_status_to_errno(NvU32 status)
{
	if (status == NVOS_STATUS_SUCCESS)
		return 0;

	if (status == NV_ERR_NOT_SUPPORTED)
		return -EOPNOTSUPP;

	if (status == NV_ERR_INVALID_ARGUMENT ||
	    status == NV_ERR_INVALID_PARAM_STRUCT)
		return -EINVAL;

	if (status == NV_ERR_INSUFFICIENT_PERMISSIONS)
		return -EPERM;

	if (status == NV_ERR_NO_MEMORY)
		return -ENOMEM;

	return -EIO;
}

int nvhwmon_rm_bind(void)
{
	NV_STATUS status;

	memset(&rm_ops, 0, sizeof(rm_ops));
	rm_ops.version_string = NV_VERSION_STRING;

	status = nvidia_get_rm_ops(&rm_ops);
	if (status != NV_OK) {
		memset(&rm_ops, 0, sizeof(rm_ops));
		return -EINVAL;
	}

	if (!rm_ops.alloc_stack || !rm_ops.free_stack ||
	    !rm_ops.enumerate_gpus || !rm_ops.open_gpu ||
	    !rm_ops.close_gpu || !rm_ops.op) {
		memset(&rm_ops, 0, sizeof(rm_ops));
		return -ENODEV;
	}

	return 0;
}

void nvhwmon_rm_unbind(void)
{
	memset(&rm_ops, 0, sizeof(rm_ops));
}

u32 nvhwmon_rm_enumerate_gpus(nv_gpu_info_t *gpu_info)
{
	if (!rm_ops.enumerate_gpus)
		return 0;

	return rm_ops.enumerate_gpus(gpu_info);
}

/* RMAPI transport failure is separate from the op-specific status field. */
static NvU32 rm_call(void *ops)
{
	nvidia_modeset_stack_ptr stack = NULL;

	if (rm_ops.alloc_stack(&stack) != 0)
		return NV_ERR_NO_MEMORY;

	rm_ops.op(stack, ops);
	rm_ops.free_stack(stack);

	return NVOS_STATUS_SUCCESS;
}

/* This increments the NVIDIA device usage count until nvhwmon_rm_close_gpu(). */
int nvhwmon_rm_open_gpu(struct nvhwmon_gpu *gpu)
{
	nvidia_modeset_stack_ptr stack = NULL;
	int ret;

	ret = rm_ops.alloc_stack(&stack);
	if (ret != 0)
		return ret;

	ret = rm_ops.open_gpu(gpu->gpu_id, stack, NV_FALSE);
	rm_ops.free_stack(stack);
	if (ret != 0)
		return ret;

	gpu->opened = true;
	return 0;
}

/* Must run before PCI remove checks usage_count, otherwise removal can wait. */
void nvhwmon_rm_close_gpu(struct nvhwmon_gpu *gpu)
{
	nvidia_modeset_stack_ptr stack = NULL;

	if (!gpu->opened)
		return;

	if (rm_ops.alloc_stack(&stack) == 0) {
		rm_ops.close_gpu(gpu->gpu_id, stack, NV_FALSE);
		rm_ops.free_stack(stack);
		gpu->opened = false;
	} else {
		pr_err("%s: failed to allocate NVIDIA stack while closing GPU 0x%x\n",
		       NVHWMON_DRIVER_NAME, gpu->gpu_id);
		/*
		 * close_gpu() requires an RM stack. If allocation fails here, the
		 * device reference cannot be released, so keep local state from
		 * advertising a future close that can no longer be guaranteed.
		 */
		gpu->opened = false;
	}
}

NvU32 nvhwmon_rm_alloc(NvHandle h_client, NvHandle h_parent, NvHandle h_object,
		       NvU32 h_class, void *params, NvU32 params_size)
{
	nvidia_kernel_rmapi_ops_t ops = { 0 };

	ops.op = NV04_ALLOC;
	ops.params.alloc.hRoot = h_client;
	ops.params.alloc.hObjectParent = h_parent;
	ops.params.alloc.hObjectNew = h_object;
	ops.params.alloc.hClass = h_class;
	ops.params.alloc.pAllocParms = NV_PTR_TO_NvP64(params);
	ops.params.alloc.paramsSize = params_size;

	if (rm_call(&ops) != NVOS_STATUS_SUCCESS)
		return NV_ERR_NO_MEMORY;

	return ops.params.alloc.status;
}

NvU32 nvhwmon_rm_free(NvHandle h_client, NvHandle h_parent, NvHandle h_object)
{
	nvidia_kernel_rmapi_ops_t ops = { 0 };

	ops.op = NV01_FREE;
	ops.params.free.hRoot = h_client;
	ops.params.free.hObjectParent = h_parent;
	ops.params.free.hObjectOld = h_object;

	if (rm_call(&ops) != NVOS_STATUS_SUCCESS)
		return NV_ERR_NO_MEMORY;

	return ops.params.free.status;
}

NvU32 nvhwmon_rm_control(NvHandle h_client, NvHandle h_object, NvU32 cmd,
			 void *params, NvU32 params_size)
{
	nvidia_kernel_rmapi_ops_t ops = { 0 };

	ops.op = NV04_CONTROL;
	ops.params.control.hClient = h_client;
	ops.params.control.hObject = h_object;
	ops.params.control.cmd = cmd;
	ops.params.control.params = NV_PTR_TO_NvP64(params);
	ops.params.control.paramsSize = params_size;

	if (rm_call(&ops) != NVOS_STATUS_SUCCESS)
		return NV_ERR_NO_MEMORY;

	return ops.params.control.status;
}

NvU32 nvhwmon_rm_map_memory(NvHandle h_client, NvHandle h_device,
			    NvHandle h_memory, NvU64 offset, NvU64 length,
			    void **linear_address, NvU32 flags)
{
	nvidia_kernel_rmapi_ops_t ops = { 0 };

	*linear_address = NULL;

	ops.op = NV04_MAP_MEMORY;
	ops.params.mapMemory.hClient = h_client;
	ops.params.mapMemory.hDevice = h_device;
	ops.params.mapMemory.hMemory = h_memory;
	ops.params.mapMemory.offset = offset;
	ops.params.mapMemory.length = length;
	ops.params.mapMemory.flags = flags;

	if (rm_call(&ops) != NVOS_STATUS_SUCCESS)
		return NV_ERR_NO_MEMORY;

	if (ops.params.mapMemory.status == NVOS_STATUS_SUCCESS)
		*linear_address =
			NvP64_VALUE(ops.params.mapMemory.pLinearAddress);

	return ops.params.mapMemory.status;
}

NvU32 nvhwmon_rm_unmap_memory(NvHandle h_client, NvHandle h_device,
			      NvHandle h_memory, const void *linear_address,
			      NvU32 flags)
{
	nvidia_kernel_rmapi_ops_t ops = { 0 };

	ops.op = NV04_UNMAP_MEMORY;
	ops.params.unmapMemory.hClient = h_client;
	ops.params.unmapMemory.hDevice = h_device;
	ops.params.unmapMemory.hMemory = h_memory;
	ops.params.unmapMemory.pLinearAddress = NV_PTR_TO_NvP64(linear_address);
	ops.params.unmapMemory.flags = flags;

	if (rm_call(&ops) != NVOS_STATUS_SUCCESS)
		return NV_ERR_NO_MEMORY;

	return ops.params.unmapMemory.status;
}

int nvhwmon_rm_alloc_gpu_objects(struct nvhwmon_gpu *gpu)
{
	NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS id_info = { 0 };
	NV0080_ALLOC_PARAMETERS device_params = { 0 };
	NV2080_ALLOC_PARAMETERS subdevice_params = { 0 };
	NvU32 status;

	/*
	 * Client/device/subdevice handles are the common parent chain for all
	 * telemetry controls and RUSD mapping below.
	 */
	gpu->h_client = NV01_NULL_OBJECT;
	gpu->h_device = NV01_NULL_OBJECT;
	gpu->h_subdevice = NV01_NULL_OBJECT;
	gpu->h_rusd = NV01_NULL_OBJECT;

	status = nvhwmon_rm_alloc(NV01_NULL_OBJECT, NV01_NULL_OBJECT,
				  NV01_NULL_OBJECT, NV01_ROOT, &gpu->h_client,
				  sizeof(gpu->h_client));
	if (status != NVOS_STATUS_SUCCESS)
		return nvhwmon_rm_status_to_errno(status);
	if (gpu->h_client == NV01_NULL_OBJECT)
		return -ENODEV;

	id_info.gpuId = gpu->gpu_id;
	status = nvhwmon_rm_control(gpu->h_client, gpu->h_client,
				    NV0000_CTRL_CMD_GPU_GET_ID_INFO_V2,
				    &id_info, sizeof(id_info));
	if (status != NVOS_STATUS_SUCCESS)
		goto fail;

	gpu->h_device = rm_handle(gpu, RM_HANDLE_DEVICE);
	device_params.deviceId = id_info.deviceInstance;
	device_params.hClientShare = gpu->h_client;

	status = nvhwmon_rm_alloc(gpu->h_client, gpu->h_client, gpu->h_device,
				  NV01_DEVICE_0, &device_params,
				  sizeof(device_params));
	if (status != NVOS_STATUS_SUCCESS)
		goto fail;

	gpu->h_subdevice = rm_handle(gpu, RM_HANDLE_SUBDEVICE);
	subdevice_params.subDeviceId = id_info.subDeviceInstance;

	status = nvhwmon_rm_alloc(gpu->h_client, gpu->h_device,
				  gpu->h_subdevice, NV20_SUBDEVICE_0,
				  &subdevice_params, sizeof(subdevice_params));
	if (status != NVOS_STATUS_SUCCESS)
		goto fail;

	return 0;

fail:
	nvhwmon_rm_free_gpu_objects(gpu);
	return nvhwmon_rm_status_to_errno(status);
}

/* Free children before parents; each handle is nulled so cleanup is idempotent. */
void nvhwmon_rm_free_gpu_objects(struct nvhwmon_gpu *gpu)
{
	if (gpu->h_subdevice != NV01_NULL_OBJECT) {
		nvhwmon_rm_free(gpu->h_client, gpu->h_device, gpu->h_subdevice);
			gpu->h_subdevice = NV01_NULL_OBJECT;
	}

	if (gpu->h_device != NV01_NULL_OBJECT) {
		nvhwmon_rm_free(gpu->h_client, gpu->h_client, gpu->h_device);
			gpu->h_device = NV01_NULL_OBJECT;
	}

	if (gpu->h_client != NV01_NULL_OBJECT) {
		nvhwmon_rm_free(gpu->h_client, NV01_NULL_OBJECT, gpu->h_client);
			gpu->h_client = NV01_NULL_OBJECT;
	}
}
