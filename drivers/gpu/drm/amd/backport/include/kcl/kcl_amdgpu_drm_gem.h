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
#ifndef __AMDGPU_BACKPORT_KCL_AMDGPU_DRM_GEM_H__
#define __AMDGPU_BACKPORT_KCL_AMDGPU_DRM_GEM_H__

int _kcl_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma);

static inline int amdkcl_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma) {
	return _kcl_drm_gem_mmap(filp, vma);
}


int _kcl_drm_gem_prime_mmap(struct drm_gem_object *obj, struct vm_area_struct *vma);

static inline int amdkcl_drm_gem_prime_mmap(struct drm_gem_object *obj, struct vm_area_struct *vma) {
	return _kcl_drm_gem_prime_mmap(obj, vma);
}

#endif
