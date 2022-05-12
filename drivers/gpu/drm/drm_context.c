/*
 * Legacy: Generic DRM Contexts
 *
 * Copyright 1999, 2000 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Author: Rickard E. (Rik) Faith <faith@valinux.com>
 * Author: Gareth Hughes <gareth@valinux.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <linux/slab.h>
#include <linux/uaccess.h>

#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_print.h>

#include "drm_legacy.h"

struct drm_ctx_list {
	struct list_head head;
	drm_context_t handle;
	struct drm_file *tag;
};

/******************************************************************/
/** \name Context bitmap support */
/*@{*/

/*
 * Free a handle from the context bitmap.
 *
 * \param dev DRM device.
 * \param ctx_handle context handle.
 *
 * Clears the bit specified by \p ctx_handle in drm_device::ctx_bitmap and the entry
 * in drm_device::ctx_idr, while holding the drm_device::struct_mutex
 * lock.
 */
void drm_legacy_ctxbitmap_free(struct drm_device * dev, int ctx_handle)
{
	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return;

	mutex_lock(&dev->struct_mutex);
	idr_remove(&dev->ctx_idr, ctx_handle);
	mutex_unlock(&dev->struct_mutex);
}

/*
 * Context bitmap allocation.
 *
 * \param dev DRM device.
 * \return (non-negative) context handle on success or a negative number on failure.
 *
 * Allocate a new idr from drm_device::ctx_idr while holding the
 * drm_device::struct_mutex lock.
 */
static int drm_legacy_ctxbitmap_next(struct drm_device * dev)
{
	int ret;

	mutex_lock(&dev->struct_mutex);
	ret = idr_alloc(&dev->ctx_idr, NULL, DRM_RESERVED_CONTEXTS, 0,
			GFP_KERNEL);
	mutex_unlock(&dev->struct_mutex);
	return ret;
}

/*
 * Context bitmap initialization.
 *
 * \param dev DRM device.
 *
 * Initialise the drm_device::ctx_idr
 */
void drm_legacy_ctxbitmap_init(struct drm_device * dev)
{
	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return;

	idr_init(&dev->ctx_idr);
}

/*
 * Context bitmap cleanup.
 *
 * \param dev DRM device.
 *
 * Free all idr members using drm_ctx_sarea_free helper function
 * while holding the drm_device::struct_mutex lock.
 */
void drm_legacy_ctxbitmap_cleanup(struct drm_device * dev)
{
	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return;

	mutex_lock(&dev->struct_mutex);
	idr_destroy(&dev->ctx_idr);
	mutex_unlock(&dev->struct_mutex);
}

/**
 * drm_legacy_ctxbitmap_flush() - Flush all contexts owned by a file
 * @dev: DRM device to operate on
 * @file: Open file to flush contexts for
 *
 * This iterates over all contexts on @dev and drops them if they're owned by
 * @file. Note that after this call returns, new contexts might be added if
 * the file is still alive.
 */
void drm_legacy_ctxbitmap_flush(struct drm_device *dev, struct drm_file *file)
{
	struct drm_ctx_list *pos, *tmp;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return;

	mutex_lock(&dev->ctxlist_mutex);

	list_for_each_entry_safe(pos, tmp, &dev->ctxlist, head) {
		if (pos->tag == file &&
		    pos->handle != DRM_KERNEL_CONTEXT) {
			if (dev->driver->context_dtor)
				dev->driver->context_dtor(dev, pos->handle);

			drm_legacy_ctxbitmap_free(dev, pos->handle);
			list_del(&pos->head);
			kfree(pos);
		}
	}

	mutex_unlock(&dev->ctxlist_mutex);
}

/*@}*/

/******************************************************************/
/** \name Per Context SAREA Support */
/*@{*/

/*
 * Get per-context SAREA.
 *
 * \param inode device inode.
 * \param file_priv DRM file private.
 * \param cmd command.
 * \param arg user argument pointing to a drm_ctx_priv_map structure.
 * \return zero on success or a negative number on failure.
 *
 * Gets the map from drm_device::ctx_idr with the handle specified and
 * returns its handle.
 */
int drm_legacy_getsareactx(struct drm_device *dev, void *data,
			   struct drm_file *file_priv)
{
	struct drm_ctx_priv_map *request = data;
	struct drm_local_map *map;
	struct drm_map_list *_entry;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	mutex_lock(&dev->struct_mutex);

	map = idr_find(&dev->ctx_idr, request->ctx_id);
	if (!map) {
		mutex_unlock(&dev->struct_mutex);
		return -EINVAL;
	}

	request->handle = NULL;
	list_for_each_entry(_entry, &dev->maplist, head) {
		if (_entry->map == map) {
			request->handle =
			    (void *)(unsigned long)_entry->user_token;
			break;
		}
	}

	mutex_unlock(&dev->struct_mutex);

	if (request->handle == NULL)
		return -EINVAL;

	return 0;
}

/*
 * Set per-context SAREA.
 *
 * \param inode device inode.
 * \param file_priv DRM file private.
 * \param cmd command.
 * \param arg user argument pointing to a drm_ctx_priv_map structure.
 * \return zero on success or a negative number on failure.
 *
 * Searches the mapping specified in \p arg and update the entry in
 * drm_device::ctx_idr with it.
 */
int drm_legacy_setsareactx(struct drm_device *dev, void *data,
			   struct drm_file *file_priv)
{
	struct drm_ctx_priv_map *request = data;
	struct drm_local_map *map = NULL;
	struct drm_map_list *r_list = NULL;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	mutex_lock(&dev->struct_mutex);
	list_for_each_entry(r_list, &dev->maplist, head) {
		if (r_list->map
		    && r_list->user_token == (unsigned long) request->handle)
			goto found;
	}
      bad:
	mutex_unlock(&dev->struct_mutex);
	return -EINVAL;

      found:
	map = r_list->map;
	if (!map)
		goto bad;

	if (IS_ERR(idr_replace(&dev->ctx_idr, map, request->ctx_id)))
		goto bad;

	mutex_unlock(&dev->struct_mutex);

	return 0;
}

/*@}*/

/******************************************************************/
/** \name The actual DRM context handling routines */
/*@{*/

/*
 * Switch context.
 *
 * \param dev DRM device.
 * \param old old context handle.
 * \param new new context handle.
 * \return zero on success or a negative number on failure.
 *
 * Attempt to set drm_device::context_flag.
 */
static int drm_context_switch(struct drm_device * dev, int old, int new)
{
	if (test_and_set_bit(0, &dev->context_flag)) {
		DRM_ERROR("Reentering -- FIXME\n");
		return -EBUSY;
	}

	DRM_DEBUG("Context switch from %d to %d\n", old, new);

	if (new == dev->last_context) {
		clear_bit(0, &dev->context_flag);
		return 0;
	}

	return 0;
}

/*
 * Complete context switch.
 *
 * \param dev DRM device.
 * \param new new context handle.
 * \return zero on success or a negative number on failure.
 *
 * Updates drm_device::last_context and drm_device::last_switch. Verifies the
 * hardware lock is held, clears the drm_device::context_flag and wakes up
 * drm_device::context_wait.
 */
static int drm_context_switch_complete(struct drm_device *dev,
				       struct drm_file *file_priv, int new)
{
	dev->last_context = new;	/* PRE/POST: This is the _only_ writer. */

	if (!_DRM_LOCK_IS_HELD(file_priv->master->lock.hw_lock->lock)) {
		DRM_ERROR("Lock isn't held after context switch\n");
	}

	/* If a context switch is ever initiated
	   when the kernel holds the lock, release
	   that lock here.
	 */
	clear_bit(0, &dev->context_flag);

	return 0;
}

/*
 * Reserve contexts.
 *
 * \param inode device inode.
 * \param file_priv DRM file private.
 * \param cmd command.
 * \param arg user argument pointing to a drm_ctx_res structure.
 * \return zero on success or a negative number on failure.
 */
int drm_legacy_resctx(struct drm_device *dev, void *data,
		      struct drm_file *file_priv)
{
	struct drm_ctx_res *res = data;
	struct drm_ctx ctx;
	int i;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	if (res->count >= DRM_RESERVED_CONTEXTS) {
		memset(&ctx, 0, sizeof(ctx));
		for (i = 0; i < DRM_RESERVED_CONTEXTS; i++) {
			ctx.handle = i;
			if (copy_to_user(&res->contexts[i], &ctx, sizeof(ctx)))
				return -EFAULT;
		}
	}
	res->count = DRM_RESERVED_CONTEXTS;

	return 0;
}

/*
 * Add context.
 *
 * \param inode device inode.
 * \param file_priv DRM file private.
 * \param cmd command.
 * \param arg user argument pointing to a drm_ctx structure.
 * \return zero on success or a negative number on failure.
 *
 * Get a new handle for the context and copy to userspace.
 */
int drm_legacy_addctx(struct drm_device *dev, void *data,
		      struct drm_file *file_priv)
{
	struct drm_ctx_list *ctx_entry;
	struct drm_ctx *ctx = data;
	int tmp_handle;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	tmp_handle = drm_legacy_ctxbitmap_next(dev);
	if (tmp_handle == DRM_KERNEL_CONTEXT) {
		/* Skip kernel's context and get a new one. */
		tmp_handle = drm_legacy_ctxbitmap_next(dev);
	}
	DRM_DEBUG("%d\n", tmp_handle);
	if (tmp_handle < 0) {
		DRM_DEBUG("Not enough free contexts.\n");
		/* Should this return -EBUSY instead? */
		return tmp_handle;
	}

	ctx->handle = tmp_handle;

	ctx_entry = kmalloc(sizeof(*ctx_entry), GFP_KERNEL);
	if (!ctx_entry) {
		DRM_DEBUG("out of memory\n");
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&ctx_entry->head);
	ctx_entry->handle = ctx->handle;
	ctx_entry->tag = file_priv;

	mutex_lock(&dev->ctxlist_mutex);
	list_add(&ctx_entry->head, &dev->ctxlist);
	mutex_unlock(&dev->ctxlist_mutex);

	return 0;
}

/*
 * Get context.
 *
 * \param inode device inode.
 * \param file_priv DRM file private.
 * \param cmd command.
 * \param arg user argument pointing to a drm_ctx structure.
 * \return zero on success or a negative number on failure.
 */
int drm_legacy_getctx(struct drm_device *dev, void *data,
		      struct drm_file *file_priv)
{
	struct drm_ctx *ctx = data;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	/* This is 0, because we don't handle any context flags */
	ctx->flags = 0;

	return 0;
}

/*
 * Switch context.
 *
 * \param inode device inode.
 * \param file_priv DRM file private.
 * \param cmd command.
 * \param arg user argument pointing to a drm_ctx structure.
 * \return zero on success or a negative number on failure.
 *
 * Calls context_switch().
 */
int drm_legacy_switchctx(struct drm_device *dev, void *data,
			 struct drm_file *file_priv)
{
	struct drm_ctx *ctx = data;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	DRM_DEBUG("%d\n", ctx->handle);
	return drm_context_switch(dev, dev->last_context, ctx->handle);
}

/*
 * New context.
 *
 * \param inode device inode.
 * \param file_priv DRM file private.
 * \param cmd command.
 * \param arg user argument pointing to a drm_ctx structure.
 * \return zero on success or a negative number on failure.
 *
 * Calls context_switch_complete().
 */
int drm_legacy_newctx(struct drm_device *dev, void *data,
		      struct drm_file *file_priv)
{
	struct drm_ctx *ctx = data;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	DRM_DEBUG("%d\n", ctx->handle);
	drm_context_switch_complete(dev, file_priv, ctx->handle);

	return 0;
}

/*
 * Remove context.
 *
 * \param inode device inode.
 * \param file_priv DRM file private.
 * \param cmd command.
 * \param arg user argument pointing to a drm_ctx structure.
 * \return zero on success or a negative number on failure.
 *
 * If not the special kernel context, calls ctxbitmap_free() to free the specified context.
 */
int drm_legacy_rmctx(struct drm_device *dev, void *data,
		     struct drm_file *file_priv)
{
	struct drm_ctx *ctx = data;

	if (!drm_core_check_feature(dev, DRIVER_KMS_LEGACY_CONTEXT) &&
	    !drm_core_check_feature(dev, DRIVER_LEGACY))
		return -EOPNOTSUPP;

	DRM_DEBUG("%d\n", ctx->handle);
	if (ctx->handle != DRM_KERNEL_CONTEXT) {
		if (dev->driver->context_dtor)
			dev->driver->context_dtor(dev, ctx->handle);
		drm_legacy_ctxbitmap_free(dev, ctx->handle);
	}

	mutex_lock(&dev->ctxlist_mutex);
	if (!list_empty(&dev->ctxlist)) {
		struct drm_ctx_list *pos, *n;

		list_for_each_entry_safe(pos, n, &dev->ctxlist, head) {
			if (pos->handle == ctx->handle) {
				list_del(&pos->head);
				kfree(pos);
			}
		}
	}
	mutex_unlock(&dev->ctxlist_mutex);

	return 0;
}

/*@}*/
