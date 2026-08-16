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

#ifndef NVHWMON_RM_H
#define NVHWMON_RM_H

#include "hwmon-main.h"

/* Bind to RM callbacks exported inside nvidia.ko and clear them on shutdown. */
int nvhwmon_rm_bind(void);
void nvhwmon_rm_unbind(void);

u32 nvhwmon_rm_enumerate_gpus(nv_gpu_info_t *gpu_info);
/* Open/close pins the underlying NVIDIA device lifetime. */
int nvhwmon_rm_open_gpu(struct nvhwmon_gpu *gpu);
void nvhwmon_rm_close_gpu(struct nvhwmon_gpu *gpu);

int nvhwmon_rm_alloc_gpu_objects(struct nvhwmon_gpu *gpu);
void nvhwmon_rm_free_gpu_objects(struct nvhwmon_gpu *gpu);
NvHandle nvhwmon_rm_rusd_handle(const struct nvhwmon_gpu *gpu);

NvU32 nvhwmon_rm_alloc(NvHandle h_client, NvHandle h_parent, NvHandle h_object,
		       NvU32 h_class, void *params, NvU32 params_size);
NvU32 nvhwmon_rm_free(NvHandle h_client, NvHandle h_parent, NvHandle h_object);
NvU32 nvhwmon_rm_control(NvHandle h_client, NvHandle h_object, NvU32 cmd,
			 void *params, NvU32 params_size);
NvU32 nvhwmon_rm_map_memory(NvHandle h_client, NvHandle h_device,
			    NvHandle h_memory, NvU64 offset, NvU64 length,
			    void **linear_address, NvU32 flags);
NvU32 nvhwmon_rm_unmap_memory(NvHandle h_client, NvHandle h_device,
			      NvHandle h_memory, const void *linear_address,
			      NvU32 flags);

int nvhwmon_rm_status_to_errno(NvU32 status);

#endif
