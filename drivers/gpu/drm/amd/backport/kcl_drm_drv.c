/*
 * Created: Fri Jan 19 10:48:35 2001 by faith@acm.org
 *
 * Copyright 2001 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Author Rickard E. (Rik) Faith <faith@valinux.com>
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
 * PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <drm/drm_drv.h>
#include "amdgpu.h"

#ifdef AMDKCL_DEVM_DRM_DEV_ALLOC
/* Copied from v5.7-rc1-343-gb0b5849e0cc0 drivers/gpu/drm/drm_drv.c and modified for KCL */
void *__devm_drm_dev_alloc(struct device *parent, struct drm_driver *driver,
			   size_t size, size_t offset)
{
	void *container;
	struct drm_device *drm;
	int ret;

	container = kzalloc(size, GFP_KERNEL);
	if (!container)
		return ERR_PTR(-ENOMEM);

#ifdef HAVE_DRM_DRIVER_RELEASE
	drm = container + offset;
	ret = drm_dev_init(drm, driver, parent);
	if (ret) {
		drm_dev_put(drm);
		return ERR_PTR(ret);
	}
#ifdef HAVE_DRM_DRM_MANAGED_H
	drmm_add_final_kfree(drm, container);
#endif
#else
	drm = drm_dev_alloc(driver, parent);
	if (IS_ERR(drm))
		return PTR_ERR(drm);
	((struct amdgpu_device*)container)->ddev = drm;
#endif
	drm->dev_private = container;
	return container;
}

void amdkcl_drm_dev_release(struct drm_device *ddev)
{
#ifndef HAVE_DRM_DRIVER_RELEASE
	if (ddev) {
		kfree(drm_to_adev(ddev));
		ddev->dev_private = NULL;
	}
#endif
	drm_dev_put(ddev);
}

#endif
