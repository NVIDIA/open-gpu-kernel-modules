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

#ifndef HAVE_DRM_DEV_ENTER
#include <linux/srcu.h>
#include <kcl/kcl_drm_drv.h>

DEFINE_STATIC_SRCU(drm_unplug_srcu);

/**
 * drm_dev_enter - Enter device critical section
 * @dev: DRM device
 * @idx: Pointer to index that will be passed to the matching drm_dev_exit()
 *
 * This function marks and protects the beginning of a section that should not
 * be entered after the device has been unplugged. The section end is marked
 * with drm_dev_exit(). Calls to this function can be nested.
 *
 * Returns:
 * True if it is OK to enter the section, false otherwise.
 */
bool drm_dev_enter(struct drm_device *dev, int *idx)
{
        *idx = srcu_read_lock(&drm_unplug_srcu);

        if (atomic_read(&dev->unplugged)) {
                srcu_read_unlock(&drm_unplug_srcu, *idx);
                return false;
        }

        return true;
}
EXPORT_SYMBOL(drm_dev_enter);

/**
 * drm_dev_exit - Exit device critical section
 * @idx: index returned from drm_dev_enter()
 *
 * This function marks the end of a section that should not be entered after
 * the device has been unplugged.
 */
void drm_dev_exit(int idx)
{
        srcu_read_unlock(&drm_unplug_srcu, idx);
}
EXPORT_SYMBOL(drm_dev_exit);

#endif /* HAVE_DRM_DEV_ENTER */
