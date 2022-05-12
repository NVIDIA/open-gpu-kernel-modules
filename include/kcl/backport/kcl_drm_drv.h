/*
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * Copyright (c) 2009-2010, Code Aurora Forum.
 * Copyright 2016 Intel Corp.
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

#ifndef __KCL_BACKPORT_KCL_DRM_DRV_H__
#define __KCL_BACKPORT_KCL_DRM_DRV_H__

#include <kcl/kcl_drm_drv.h>

#ifdef HAVE_DRM_DEV_UNPLUG
/*
 * v5.1-rc5-1150-gbd53280ef042 drm/drv: Fix incorrect resolution of merge conflict
 * v5.1-rc2-5-g3f04e0a6cfeb drm: Fix drm_release() and device unplug
 */
#if DRM_VERSION_CODE < DRM_VERSION(5, 2, 0)
static inline
void _kcl_drm_dev_unplug(struct drm_device *dev)
{
	unsigned int prev, post;

	drm_dev_get(dev);

	prev = kref_read(&dev->ref);
	drm_dev_unplug(dev);
	post = kref_read(&dev->ref);

	if (prev == post)
		drm_dev_put(dev);
}
#define drm_dev_unplug _kcl_drm_dev_unplug
#endif
#endif

#endif
