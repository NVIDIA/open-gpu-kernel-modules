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
#ifndef __AMDGPU_BACKPORT_KCL_AMDGPU_DRM_DRV_H__
#define __AMDGPU_BACKPORT_KCL_AMDGPU_DRM_DRV_H__

#include <drm/drm_drv.h>

/* Copied from v5.7-rc1-343-gb0b5849e0cc0 include/drm/drm_drv.h */
#ifndef devm_drm_dev_alloc
#define AMDKCL_DEVM_DRM_DEV_ALLOC 1
void *__devm_drm_dev_alloc(struct device *parent, struct drm_driver *driver,
						   size_t size, size_t offset);
#define devm_drm_dev_alloc(parent, driver, type, member) \
	((type *) __devm_drm_dev_alloc(parent, driver, sizeof(type), \
				       offsetof(type, member)))

void amdkcl_drm_dev_release(struct drm_device *ddev);
#endif

#endif
