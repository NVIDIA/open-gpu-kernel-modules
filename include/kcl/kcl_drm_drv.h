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
#ifndef __KCL_KCL_DRM_DRV_H__
#define __KCL_KCL_DRM_DRV_H__

#include <drm/drm_device.h>
#include <drm/drm_drv.h>

#ifndef HAVE_DRM_DEV_PUT
static inline void drm_dev_get(struct drm_device *dev)
{
	drm_dev_ref(dev);
}

static inline void drm_dev_put(struct drm_device *dev)
{
	return drm_dev_unref(dev);
}
#endif

#ifndef HAVE_DRM_DEV_ENTER
/* Copied from include/drm/drm_drv.h*/

bool drm_dev_enter(struct drm_device *dev, int *idx);
void drm_dev_exit(int idx);

#ifndef HAVE_DRM_DEV_IS_UNPLUGGED
/**
 * drm_dev_is_unplugged - is a DRM device unplugged
 * @dev: DRM device
 *
 * This function can be called to check whether a hotpluggable is unplugged.
 * Unplugging itself is singalled through drm_dev_unplug(). If a device is
 * unplugged, these two functions guarantee that any store before calling
 * drm_dev_unplug() is visible to callers of this function after it completes
 *
 * WARNING: This function fundamentally races against drm_dev_unplug(). It is
 * recommended that drivers instead use the underlying drm_dev_enter() and
 * drm_dev_exit() function pairs.
 */
static inline bool drm_dev_is_unplugged(struct drm_device *dev)
{
        int idx;

        if (drm_dev_enter(dev, &idx)) {
                drm_dev_exit(idx);
                return false;
        }

        return true;
}
#endif /* HAVE_DRM_DEV_IS_UNPLUGGED */
#endif /* HAVE_DRM_DEV_ENTER */

#ifndef HAVE_DRM_DRV_USES_ATOMIC_MODESET
static inline bool drm_drv_uses_atomic_modeset(struct drm_device *dev)
{
	return drm_core_check_feature(dev, DRIVER_ATOMIC) ||
		(dev->mode_config.funcs && dev->mode_config.funcs->atomic_commit != NULL);
}
#endif /* HAVE_DRM_DRV_USES_ATOMIC_MODESET */

#endif
