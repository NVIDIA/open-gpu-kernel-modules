/*
 * Copyright © 2017 Red Hat
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
 *
 */

#ifndef __KCL_BACKPORT_KCL_DRM_SYNCOBJ_H__
#define __KCL_BACKPORT_KCL_DRM_SYNCOBJ_H__

#ifdef HAVE_CHUNK_ID_SYNOBJ_IN_OUT
#include <drm/drm_syncobj.h>

static inline
int _kcl_drm_syncobj_find_fence(struct drm_file *file_private,
						u32 handle, u64 point, u64 flags,
						struct dma_fence **fence)
{
#if defined(HAVE_DRM_SYNCOBJ_FIND_FENCE)
#if defined(HAVE_DRM_SYNCOBJ_FIND_FENCE_5ARGS)
	return drm_syncobj_find_fence(file_private, handle, point, flags, fence);
#elif defined(HAVE_DRM_SYNCOBJ_FIND_FENCE_4ARGS)
	return drm_syncobj_find_fence(file_private, handle, point, fence);
#else
	return drm_syncobj_find_fence(file_private, handle, fence);
#endif /* HAVE_DRM_SYNCOBJ_FIND_FENCE_5ARGS */
#elif defined(HAVE_DRM_SYNCOBJ_FENCE_GET)
	return drm_syncobj_fence_get(file_private, handle, fence);
#endif /* HAVE_DRM_SYNCOBJ_FIND_FENCE */
}
#define drm_syncobj_find_fence _kcl_drm_syncobj_find_fence

#endif /* HAVE_CHUNK_ID_SYNOBJ_IN_OUT */
#endif
