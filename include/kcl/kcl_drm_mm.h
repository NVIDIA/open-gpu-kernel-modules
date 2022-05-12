/**************************************************************************
 *
 * Copyright 2006-2008 Tungsten Graphics, Inc., Cedar Park, TX. USA.
 * Copyright 2016 Intel Corporation
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 **************************************************************************/
/*
 * Authors:
 * Thomas Hellstrom <thomas-at-tungstengraphics-dot-com>
 */
#ifndef _KCL_KCL_DRM_MM_H_H_
#define _KCL_KCL_DRM_MM_H_H_
#include <drm/drm_mm.h>

#ifndef HAVE_DRM_MM_INSERT_MODE
/* Copied from 4e64e5539d15 include/drm/drm_mm.h */
enum drm_mm_insert_mode {
	DRM_MM_INSERT_BEST = 0,
	DRM_MM_INSERT_LOW,
	DRM_MM_INSERT_HIGH,
	DRM_MM_INSERT_EVICT,
};

static inline
int _kcl_drm_mm_insert_node_in_range(struct drm_mm * const mm,
				struct drm_mm_node * const node,
				u64 size, u64 alignment,
				unsigned long color,
				u64 range_start, u64 range_end,
				enum drm_mm_insert_mode mode)
{
	enum drm_mm_search_flags sflags = DRM_MM_SEARCH_BEST;
	enum drm_mm_allocator_flags aflags = DRM_MM_CREATE_DEFAULT;

	if (mode == DRM_MM_INSERT_HIGH) {
		sflags = DRM_MM_SEARCH_BELOW;
		aflags = DRM_MM_CREATE_TOP;
	}

	return drm_mm_insert_node_in_range_generic(mm, node, size,
		alignment, color, range_start, range_end,
		sflags, aflags);
}
#endif /* HAVE_DRM_MM_INSERT_MODE */

#endif
