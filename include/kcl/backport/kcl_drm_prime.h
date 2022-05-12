/*
 * Copyright © 2012 Red Hat
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * Copyright (c) 2009-2010, Code Aurora Forum.
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
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *      Dave Airlie <airlied@redhat.com>
 *      Rob Clark <rob.clark@linaro.org>
 *
 */

// Copied from include/drm/drm_prime.h
#ifndef _KCL_BACKPORT_KCL__DRM_PRIME_H__H_
#define _KCL_BACKPORT_KCL__DRM_PRIME_H__H_

#ifdef HAVE_DRM_DRMP_H
#include <drm/drmP.h>
#else
#include <drm/drm_prime.h>
#endif

#ifndef HAVE_DRM_PRIME_PAGES_TO_SG_3ARGS
static inline
struct sg_table *_kcl_drm_prime_pages_to_sg(struct drm_device *dev,
				       struct page **pages, unsigned int nr_pages)
{
	pr_warn_once("legacy kernel with drm_prime_pages_to_sg() ignore segment size limits, which is buggy\n");
	return drm_prime_pages_to_sg(pages, nr_pages);
}
#define drm_prime_pages_to_sg _kcl_drm_prime_pages_to_sg
#endif

#endif
