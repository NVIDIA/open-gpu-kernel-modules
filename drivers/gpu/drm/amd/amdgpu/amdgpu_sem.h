/*
 * Copyright 2016 Advanced Micro Devices, Inc.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Chunming Zhou <david1.zhou@amd.com>
 *
 */


#ifndef _LINUX_AMDGPU_SEM_H
#define _LINUX_AMDGPU_SEM_H

#include <linux/types.h>
#include <linux/kref.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/spinlock.h>

struct amdgpu_sem_core {
	struct file		*file;
	struct kref		kref;
	struct dma_fence            *fence;
	struct mutex	lock;
};

struct amdgpu_sem_dep {
	struct dma_fence		*fence;
	struct list_head	list;
};

struct amdgpu_sem {
	struct amdgpu_sem_core	*base;
	struct kref		kref;
	struct list_head        list;
};

void amdgpu_sem_put(struct amdgpu_sem *sem);

int amdgpu_sem_ioctl(struct drm_device *dev, void *data,
		     struct drm_file *filp);

int amdgpu_sem_add_cs(struct amdgpu_ctx *ctx, struct drm_sched_entity *entity,
		     struct amdgpu_sync *sync);

void amdgpu_sem_destroy(struct amdgpu_fpriv *fpriv, u32 handle);

#endif /* _LINUX_AMDGPU_SEM_H */
