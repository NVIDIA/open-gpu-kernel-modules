/*
 * GEM Graphics Execution Manager Driver Interfaces
 *
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * Copyright (c) 2009-2010, Code Aurora Forum.
 * All rights reserved.
 * Copyright © 2014 Intel Corporation
 *   Daniel Vetter <daniel.vetter@ffwll.ch>
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

#ifndef __KCL_BACKPORT_KCL_DRM_GEM_H__
#define __KCL_BACKPORT_KCL_DRM_GEM_H__

#include <kcl/kcl_drm_gem.h>

#if !defined(HAVE_DRM_GEM_OBJECT_PUT_LOCKED)
#if defined(HAVE_DRM_GEM_OBJECT_PUT_UNLOCKED)
#define drm_gem_object_put _kcl_drm_gem_object_put
#endif
#endif

#ifndef HAVE_DRM_GEM_OBJECT_LOOKUP_2ARGS
static inline struct drm_gem_object *
_kcl_drm_gem_object_lookup(struct drm_file *filp, u32 handle)
{
	return drm_gem_object_lookup(filp->minor->dev, filp, handle);
}
#define drm_gem_object_lookup _kcl_drm_gem_object_lookup
#endif

#endif
