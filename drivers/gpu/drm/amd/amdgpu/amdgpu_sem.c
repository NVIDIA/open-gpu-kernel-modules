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
 * Authors:
 *    Chunming Zhou <david1.zhou@amd.com>
 */
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/seq_file.h>
#include <linux/export.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/anon_inodes.h>
#include <drm/drm_file.h>
#include <drm/drm_device.h>
#include "amdgpu.h"
#include "amdgpu_sem.h"

#define to_amdgpu_ctx_entity(e)	\
	container_of((e), struct amdgpu_ctx_entity, entity)

static int amdgpu_sem_entity_add(struct amdgpu_fpriv *fpriv,
				struct drm_amdgpu_sem_in *in,
				struct amdgpu_sem *sem);

static void amdgpu_sem_core_free(struct kref *kref)
{
	struct amdgpu_sem_core *core = container_of(
		kref, struct amdgpu_sem_core, kref);

	dma_fence_put(core->fence);
	mutex_destroy(&core->lock);
	kfree(core);
}

static void amdgpu_sem_free(struct kref *kref)
{
	struct amdgpu_sem *sem = container_of(
		kref, struct amdgpu_sem, kref);

	kref_put(&sem->base->kref, amdgpu_sem_core_free);
	kfree(sem);
}

static inline void amdgpu_sem_get(struct amdgpu_sem *sem)
{
	if (sem)
		kref_get(&sem->kref);
}

void amdgpu_sem_put(struct amdgpu_sem *sem)
{
	if (sem)
		kref_put(&sem->kref, amdgpu_sem_free);
}

static int amdgpu_sem_release(struct inode *inode, struct file *file)
{
	struct amdgpu_sem_core *core = file->private_data;

	/* set the core->file to null if file got released */
	mutex_lock(&core->lock);
	core->file = NULL;
	mutex_unlock(&core->lock);

	kref_put(&core->kref, amdgpu_sem_core_free);
	return 0;
}

static unsigned int amdgpu_sem_poll(struct file *file, poll_table *wait)
{
	return 0;
}

static long amdgpu_sem_file_ioctl(struct file *file, unsigned int cmd,
				   unsigned long arg)
{
	return 0;
}

static const struct file_operations amdgpu_sem_fops = {
	.release = amdgpu_sem_release,
	.poll = amdgpu_sem_poll,
	.unlocked_ioctl = amdgpu_sem_file_ioctl,
	.compat_ioctl = amdgpu_sem_file_ioctl,
};


static inline struct amdgpu_sem *amdgpu_sem_lookup(struct amdgpu_fpriv *fpriv, u32 handle)
{
	struct amdgpu_sem *sem;

	spin_lock(&fpriv->sem_handles_lock);

	/* Check if we currently have a reference on the object */
	sem = idr_find(&fpriv->sem_handles, handle);
	amdgpu_sem_get(sem);

	spin_unlock(&fpriv->sem_handles_lock);

	return sem;
}

static struct amdgpu_sem_core *amdgpu_sem_core_alloc(void)
{
	struct amdgpu_sem_core *core;

	core = kzalloc(sizeof(*core), GFP_KERNEL);
	if (!core)
		return NULL;

	kref_init(&core->kref);
	mutex_init(&core->lock);
	return core;
}

static struct amdgpu_sem *amdgpu_sem_alloc(void)
{
	struct amdgpu_sem *sem;

	sem = kzalloc(sizeof(*sem), GFP_KERNEL);
	if (!sem)
		return NULL;

	kref_init(&sem->kref);
	INIT_LIST_HEAD(&sem->list);

	return sem;
}

static int amdgpu_sem_create(struct amdgpu_fpriv *fpriv, u32 *handle)
{
	struct amdgpu_sem *sem;
	struct amdgpu_sem_core *core;
	int ret;

	sem = amdgpu_sem_alloc();
	core = amdgpu_sem_core_alloc();
	if (!sem || !core) {
		kfree(sem);
		kfree(core);
		return -ENOMEM;
	}

	sem->base = core;

	idr_preload(GFP_KERNEL);
	spin_lock(&fpriv->sem_handles_lock);

	ret = idr_alloc(&fpriv->sem_handles, sem, 1, 0, GFP_NOWAIT);

	spin_unlock(&fpriv->sem_handles_lock);
	idr_preload_end();

	if (ret < 0)
		return ret;

	*handle = ret;
	return 0;
}

static int amdgpu_sem_signal(struct amdgpu_fpriv *fpriv,
				u32 handle, struct dma_fence *fence)
{
	struct amdgpu_sem *sem;
	struct amdgpu_sem_core *core;

	sem = amdgpu_sem_lookup(fpriv, handle);
	if (!sem)
		return -EINVAL;

	core = sem->base;
	mutex_lock(&core->lock);
	dma_fence_put(core->fence);
	core->fence = dma_fence_get(fence);
	mutex_unlock(&core->lock);

	amdgpu_sem_put(sem);
	return 0;
}

static int amdgpu_sem_wait(struct amdgpu_fpriv *fpriv,
			  struct drm_amdgpu_sem_in *in)
{
	struct amdgpu_sem *sem;
	int ret;

	sem = amdgpu_sem_lookup(fpriv, in->handle);
	if (!sem)
		return -EINVAL;

	ret = amdgpu_sem_entity_add(fpriv, in, sem);
	amdgpu_sem_put(sem);

	return ret;
}

static int amdgpu_sem_import(struct amdgpu_fpriv *fpriv,
				       int fd, u32 *handle)
{
	struct file *file = fget(fd);
	struct amdgpu_sem *sem;
	struct amdgpu_sem_core *core;
	int ret;

	if (!file)
		return -EINVAL;

	core = file->private_data;
	if (!core) {
		fput(file);
		return -EINVAL;
	}

	kref_get(&core->kref);
	sem = amdgpu_sem_alloc();
	if (!sem) {
		ret = -ENOMEM;
		goto err_sem;
	}

	sem->base = core;

	idr_preload(GFP_KERNEL);
	spin_lock(&fpriv->sem_handles_lock);

	ret = idr_alloc(&fpriv->sem_handles, sem, 1, 0, GFP_NOWAIT);

	spin_unlock(&fpriv->sem_handles_lock);
	idr_preload_end();

	if (ret < 0)
		goto err_out;

	*handle = ret;
	fput(file);
	return 0;
err_sem:
	kref_put(&core->kref, amdgpu_sem_core_free);
err_out:
	amdgpu_sem_put(sem);
	fput(file);
	return ret;

}

static int amdgpu_sem_export(struct amdgpu_fpriv *fpriv,
				       u32 handle, int *fd)
{
	struct amdgpu_sem *sem;
	struct amdgpu_sem_core *core;
	int ret;

	sem = amdgpu_sem_lookup(fpriv, handle);
	if (!sem)
		return -EINVAL;

	core = sem->base;
	kref_get(&core->kref);
	mutex_lock(&core->lock);
	if (!core->file) {
		core->file = anon_inode_getfile("sem_file",
					       &amdgpu_sem_fops,
					       core, 0);
		if (IS_ERR(core->file)) {
			mutex_unlock(&core->lock);
			ret = -ENOMEM;
			goto err_put_sem;
		}
	} else {
		get_file(core->file);
	}
	mutex_unlock(&core->lock);

	ret = get_unused_fd_flags(O_CLOEXEC);
	if (ret < 0)
		goto err_put_file;

	fd_install(ret, core->file);

	*fd = ret;
	amdgpu_sem_put(sem);

	return 0;

err_put_file:
	fput(core->file);
err_put_sem:
	kref_put(&core->kref, amdgpu_sem_core_free);
	amdgpu_sem_put(sem);
	return ret;
}

void amdgpu_sem_destroy(struct amdgpu_fpriv *fpriv, u32 handle)
{
	struct amdgpu_sem *sem = amdgpu_sem_lookup(fpriv, handle);
	if (!sem)
		return;

	spin_lock(&fpriv->sem_handles_lock);
	idr_remove(&fpriv->sem_handles, handle);
	spin_unlock(&fpriv->sem_handles_lock);

	kref_put(&sem->kref, amdgpu_sem_free);
	kref_put(&sem->kref, amdgpu_sem_free);
}

static struct dma_fence *amdgpu_sem_get_fence(struct amdgpu_fpriv *fpriv,
					 struct drm_amdgpu_sem_in *in)
{
	struct drm_sched_entity *entity;
	struct amdgpu_ctx *ctx;
	struct dma_fence *fence;
	uint32_t ctx_id, ip_type, ip_instance, ring;
	int r;

	ctx_id = in->ctx_id;
	ip_type = in->ip_type;
	ip_instance = in->ip_instance;
	ring = in->ring;
	ctx = amdgpu_ctx_get(fpriv, ctx_id);
	if (!ctx)
		return NULL;
	r = amdgpu_ctx_get_entity(ctx, ip_type,
			       ip_instance, ring, &entity);
	if (r) {
		amdgpu_ctx_put(ctx);
		return NULL;
	}
	/* get the last fence of this entity */
	fence = amdgpu_ctx_get_fence(ctx, entity, in->seq);
	amdgpu_ctx_put(ctx);

	return fence;
}

static int amdgpu_sem_entity_add(struct amdgpu_fpriv *fpriv,
				struct drm_amdgpu_sem_in *in,
				struct amdgpu_sem *sem)
{
	struct amdgpu_ctx *ctx;
	struct amdgpu_sem_dep *dep;
	struct drm_sched_entity *entity;
	struct amdgpu_ctx_entity *centity;
	uint32_t ctx_id, ip_type, ip_instance, ring;
	int r;

	ctx_id = in->ctx_id;
	ip_type = in->ip_type;
	ip_instance = in->ip_instance;
	ring = in->ring;
	ctx = amdgpu_ctx_get(fpriv, ctx_id);
	if (!ctx)
		return -EINVAL;
	r = amdgpu_ctx_get_entity(ctx, ip_type,
			         ip_instance, ring, &entity);
	if (r)
		goto err;

	dep = kzalloc(sizeof(*dep), GFP_KERNEL);
	if (!dep)
		goto err;

	INIT_LIST_HEAD(&dep->list);
	dep->fence = dma_fence_get(sem->base->fence);

	centity = to_amdgpu_ctx_entity(entity);
	mutex_lock(&centity->sem_lock);
	list_add(&dep->list, &centity->sem_dep_list);
	mutex_unlock(&centity->sem_lock);

err:
	amdgpu_ctx_put(ctx);
	return r;
}

int amdgpu_sem_add_cs(struct amdgpu_ctx *ctx, struct drm_sched_entity *entity,
		     struct amdgpu_sync *sync)
{
	struct amdgpu_sem_dep *dep, *tmp;
	struct amdgpu_ctx_entity *centity = to_amdgpu_ctx_entity(entity);
	int r = 0;

	if (list_empty(&centity->sem_dep_list))
		return 0;

	mutex_lock(&centity->sem_lock);
	list_for_each_entry_safe(dep, tmp, &centity->sem_dep_list,
				 list) {
		r = amdgpu_sync_fence(sync, dep->fence);
		if (r)
			goto err;
		dma_fence_put(dep->fence);
		list_del_init(&dep->list);
		kfree(dep);
	}
err:
	mutex_unlock(&centity->sem_lock);
	return r;
}

int amdgpu_sem_ioctl(struct drm_device *dev, void *data,
		     struct drm_file *filp)
{
	union drm_amdgpu_sem *args = data;
	struct amdgpu_fpriv *fpriv = filp->driver_priv;
	struct dma_fence *fence;
	int r = 0;

	switch (args->in.op) {
	case AMDGPU_SEM_OP_CREATE_SEM:
		r = amdgpu_sem_create(fpriv, &args->out.handle);
		break;
	case AMDGPU_SEM_OP_WAIT_SEM:
		r = amdgpu_sem_wait(fpriv, &args->in);
		break;
	case AMDGPU_SEM_OP_SIGNAL_SEM:
		fence = amdgpu_sem_get_fence(fpriv, &args->in);
		if (IS_ERR(fence)) {
			r = PTR_ERR(fence);
			return r;
		}
		r = amdgpu_sem_signal(fpriv, args->in.handle, fence);
		dma_fence_put(fence);
		break;
	case AMDGPU_SEM_OP_IMPORT_SEM:
		r = amdgpu_sem_import(fpriv, args->in.handle, &args->out.handle);
		break;
	case AMDGPU_SEM_OP_EXPORT_SEM:
		r = amdgpu_sem_export(fpriv, args->in.handle, &args->out.fd);
		break;
	case AMDGPU_SEM_OP_DESTROY_SEM:
		amdgpu_sem_destroy(fpriv, args->in.handle);
		break;
	default:
		r = -EINVAL;
		break;
	}

	return r;
}
